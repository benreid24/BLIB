#ifndef BLIB_RENDER_VULKAN_DESCRIPTORPOOLREQUIREMENTS_HPP
#define BLIB_RENDER_VULKAN_DESCRIPTORPOOLREQUIREMENTS_HPP

#include <cstdint>
#include <glad/vulkan.h>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace render
{
/**
 * @brief Utility struct to gather requirements from various renderer subsystems to determine the
 *        specific requirements and size of the descriptor pool that is needed
 *
 * @ingroup Renderer
 */
class DescriptorPoolRequirements {
public:
    /**
     * @brief Construct a new DescriptorPoolRequirements with no requirements
     *
     */
    DescriptorPoolRequirements();

    /**
     * @brief Adds the given number of sets to the required count of descriptor sets to be created
     *
     * @param setCount The amount of set that a subsystem will allocate
     */
    void addSets(std::uint32_t setCount);

    /**
     * @brief Adds a specific number of descriptors of a certain type that a subsystem will create
     *
     * @param type The type of descriptor that will be created
     * @param count The amount of the given descriptor that will be created
     */
    void registerDescriptorType(VkDescriptorType type, std::uint32_t count);

    /**
     * @brief Collates the collected requirements and returns the parameters to create the
     *        descriptor pool
     *
     * @return VkDescriptorPoolCreateInfo The parameters to use to create the descriptor pool
     */
    VkDescriptorPoolCreateInfo buildRequirements() const;

private:
    std::uint32_t maxSets;
    std::vector<VkDescriptorPoolSize> descriptorCounts;
};

} // namespace render
} // namespace bl

#endif
