#ifndef BLIB_RENDER_VULKAN_DESCRIPTORPOOL_HPP
#define BLIB_RENDER_VULKAN_DESCRIPTORPOOL_HPP

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <cstdint>
#include <list>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace rc
{
namespace vk
{
struct VulkanState;

/**
 * @brief Utility class to manage descriptor set allocation. Maintains a list of descriptor pools
 *        and automatically creates more as needed
 *
 * @ingroup Renderer
 */
class DescriptorPool {
    struct Allocation;

public:
    static constexpr std::size_t BindingTypeCount = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1;

    /**
     * @brief Struct containing the data required to create descriptor sets and layouts
     */
    struct SetBindingInfo {
        SetBindingInfo();

        std::array<VkDescriptorSetLayoutBinding, cfg::Limits::MaxDescriptorBindings> bindings;
        std::uint32_t bindingCount;
    };

    /// Handle to an allocation from the pool
    using AllocationHandle = std::list<Allocation>::iterator;

    /**
     * @brief Creates a descriptor set layout from the set allocation info
     *
     * @param allocInfo Descriptor layout information
     * @return The new descriptor set layout
     */
    VkDescriptorSetLayout createLayout(const SetBindingInfo& allocInfo);

    /**
     * @brief Allocates sets from the pool and returns a handle that can be used to release the
     *        allocated sets once they are finished being used. Synchronization must be performed
     *        externally to ensure that in-use sets are not freed
     *
     * @param layout Layout of the sets to allocate. Must have been created through the pool
     * @param sets Pointer to the set handles to populate
     * @param setCount The number of descriptor sets to allocate
     * @param dedicatedPool Whether or not to use a dedicated descriptor pool for the allocation
     * @return A handle that may be used to release the allocated sets
     */
    AllocationHandle allocate(VkDescriptorSetLayout layout, VkDescriptorSet* sets,
                              std::size_t setCount, bool dedicatedPool = false);

    /**
     * @brief Releases descriptor sets allocated from this pool
     *
     * @param handle Allocation handle of the sets to release
     * @params sets Pointer to the sets to free. Nullptr to use same pointer as allocation
     */
    void release(AllocationHandle handle, const VkDescriptorSet* sets = nullptr);

private:
    struct Subpool {
        Subpool(VulkanState& vulkanState);
        Subpool(VulkanState& vulkanState, const SetBindingInfo& allocInfo, std::size_t setCount);
        ~Subpool();

        bool canAllocate(const SetBindingInfo& allocInfo, std::size_t setCount) const;
        void allocate(const SetBindingInfo& allocInfo, VkDescriptorSetLayout layout,
                      VkDescriptorSet* sets, std::size_t setCount);
        void release(const SetBindingInfo& allocInfo, const VkDescriptorSet* sets,
                     std::size_t setCount);

        VulkanState& vulkanState;
        VkDescriptorPool pool;
        unsigned int freeSets;
        std::array<unsigned int, BindingTypeCount> available;
    };

    struct Allocation {
        std::list<Subpool>::iterator pool;
        bool dedicated;
        VkDescriptorSetLayout layout;
        std::size_t setCount;
        const VkDescriptorSet* sets;

        Allocation(bool ded, VkDescriptorSetLayout ly, std::size_t set, const VkDescriptorSet* sets)
        : dedicated(ded)
        , layout(ly)
        , setCount(set)
        , sets(sets) {}
    };

    VulkanState& vulkanState;
    std::mutex mutex;
    std::unordered_map<VkDescriptorSetLayout, SetBindingInfo> layoutMap;
    std::list<Subpool> pools;
    std::list<Allocation> allocations;

    DescriptorPool(VulkanState& vulkanState);
    void init();
    void cleanup();

    friend struct VulkanState;
};

} // namespace vk
} // namespace rc
} // namespace bl

#endif
