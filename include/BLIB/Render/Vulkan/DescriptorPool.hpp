#ifndef BLIB_RENDER_VULKAN_DESCRIPTORPOOL_HPP
#define BLIB_RENDER_VULKAN_DESCRIPTORPOOL_HPP

#include <array>
#include <cstdint>
#include <glad/vulkan.h>
#include <list>

namespace bl
{
namespace render
{
struct VulkanState;

/**
 * @brief Utility class to manage descriptor set allocation. Maintains a list of descriptor pools
 *        and automatically creates more as needed
 *
 * @ingroup Renderer
 */
class DescriptorPool {
    struct Subpool;

public:
    /// Handle to an allocation from the pool. Used when freeing allocated sets
    using AllocationHandle = std::list<Subpool>::reverse_iterator;

    /**
     * @brief Allocates descriptor sets from the pool. Sets allocated with this function are not
     *        able to be released
     *
     * @param createInfos Information containing the set layout parameters
     * @param layouts Layouts of the sets to allocate
     * @param sets Pointer to the set handles to populate
     * @param setCount The number of descriptor sets to allocate
     */
    void allocateForever(const VkDescriptorSetLayoutCreateInfo* createInfos,
                         const VkDescriptorSetLayout* layouts, VkDescriptorSet* sets,
                         std::size_t setCount);

    /**
     * @brief Allocates sets from the pool and returns a handle that can be used to release the
     *        allocated sets once they are finished being used. Synchronization must be performed
     *        externally to ensure that in-use sets are not freed
     *
     * @param createInfos Information containing the set layout parameters
     * @param layouts Layouts of the sets to allocate
     * @param sets Pointer to the set handles to populate
     * @param setCount The number of descriptor sets to allocate
     * @return A handle that may be used to release the allocated sets
     */
    AllocationHandle allocate(const VkDescriptorSetLayoutCreateInfo* createInfos,
                              const VkDescriptorSetLayout* layouts, VkDescriptorSet* sets,
                              std::size_t setCount);

    /**
     * @brief Releases descriptor sets allocated from this pool
     *
     * @param handle Allocation handle of the sets to release
     * @param createInfos Information containing the set layout parameters
     * @param layouts Layouts of the sets to release
     * @param sets The sets to release
     * @param setCount The number of sets to release
     */
    void release(AllocationHandle handle, const VkDescriptorSetLayoutCreateInfo* createInfos,
                 const VkDescriptorSetLayout* layouts, VkDescriptorSet* sets, std::size_t setCount);

private:
    struct Subpool {
        Subpool(VulkanState& vulkanState, bool allowFree);
        ~Subpool();

        bool canAllocate(const VkDescriptorSetLayoutCreateInfo* createInfos,
                         std::size_t setCount) const;
        void allocate(const VkDescriptorSetLayoutCreateInfo* createInfos,
                      const VkDescriptorSetLayout* layouts, VkDescriptorSet* sets,
                      std::size_t setCount);
        void release(const VkDescriptorSetLayoutCreateInfo* createInfos,
                     const VkDescriptorSetLayout* layouts, VkDescriptorSet* sets,
                     std::size_t setCount);
        bool inUse() const;

        VulkanState& vulkanState;
        VkDescriptorPool pool;
        unsigned int freeSets;
        std::array<unsigned int, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1> available;
    };

    VulkanState& vulkanState;
    std::list<Subpool> foreverPools;
    std::list<Subpool> pools;

    DescriptorPool(VulkanState& vulkanState);
    void init();
    void cleanup();
    AllocationHandle doAllocate(std::list<Subpool>& pools,
                                const VkDescriptorSetLayoutCreateInfo* createInfos,
                                const VkDescriptorSetLayout* layouts, VkDescriptorSet* sets,
                                std::size_t setCount);

    friend struct VulkanState;
};

} // namespace render
} // namespace bl

#endif
