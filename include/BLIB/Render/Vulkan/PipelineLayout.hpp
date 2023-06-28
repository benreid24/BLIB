#ifndef BLIB_RENDER_VULKAN_PIPELINELAYOUT_HPP
#define BLIB_RENDER_VULKAN_PIPELINELAYOUT_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Util/HashCombine.hpp>
#include <array>
#include <cstdint>
#include <functional>
#include <glad/vulkan.h>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <vector>

namespace bl
{
namespace gfx
{
class Renderer;

namespace vk
{
/**
 * @brief Represents the descriptor layout for pipelines. Allows rendering to be batched by layout
 *        to save on bind calls
 *
 * @ingroup Renderer
 */
class PipelineLayout {
public:
    /**
     * @brief Internal struct to manage layout creation and fetching
     */
    class LayoutParams {
        struct DescriptorSet {
            std::type_index factoryType;
            std::unique_ptr<ds::DescriptorSetFactory> factory;

            DescriptorSet()
            : factoryType(typeid(void)) {}
            DescriptorSet(const DescriptorSet& c)
            : factoryType(c.factoryType) {}
            DescriptorSet(std::type_index tid, std::unique_ptr<ds::DescriptorSetFactory>&& factory)
            : factoryType(tid)
            , factory(std::forward<std::unique_ptr<ds::DescriptorSetFactory>>(factory)) {}
        };

    public:
        LayoutParams()
        : dsCount(0)
        , pcCount(0) {}

        LayoutParams(const LayoutParams& c)
        : descriptorSets(c.descriptorSets)
        , dsCount(c.dsCount)
        , pushConstants(c.pushConstants)
        , pcCount(c.pcCount) {}

        template<typename TFactory, typename... TArgs>
        void addDescriptorSet(TArgs&&... args) {
#ifdef BLIB_DEBUG
            if (dsCount >= 4) { throw std::runtime_error("Exceeded 4 descriptor sets"); }
#endif

            descriptorSets[dsCount].factoryType = typeid(TFactory);
            descriptorSets[dsCount].factory =
                std::make_unique<TFactory>(std::forward<TArgs>(args)...);
            ++dsCount;
        }

        void addPushConstantRange(std::uint32_t offset, std::uint32_t size,
                                  VkShaderStageFlags shaderStages = VK_SHADER_STAGE_ALL_GRAPHICS) {
#ifdef BLIB_DEBUG
            if (pcCount >= 4) { throw std::runtime_error("Exceeded 4 push constant ranges"); }
#endif

            pushConstants[pcCount] = VkPushConstantRange{shaderStages, offset, size};
            ++pcCount;
        }

        bool operator==(const LayoutParams& right) const {
            if (dsCount != right.dsCount || pcCount != right.pcCount) { return false; }

            for (std::uint32_t i = 0; i < dsCount; ++i) {
                if (descriptorSets[i].factoryType != right.descriptorSets[i].factoryType) {
                    return false;
                }
            }

            for (std::uint32_t i = 0; i < pcCount; ++i) {
                if (pushConstants[i].offset != right.pushConstants[i].offset) { return false; }
                if (pushConstants[i].size != right.pushConstants[i].size) { return false; }
                if (pushConstants[i].stageFlags != right.pushConstants[i].stageFlags) {
                    return false;
                }
            }

            return true;
        }

    private:
        std::array<DescriptorSet, 4> descriptorSets;
        std::size_t dsCount;
        std::array<VkPushConstantRange, 4> pushConstants;
        std::size_t pcCount;

        friend struct std::hash<LayoutParams>;
        friend class PipelineLayout;
    };

public:
    /**
     * @brief Creates the new pipeline layout
     *
     * @param renderer The renderer instance
     * @param params The layout parameters
     */
    PipelineLayout(Renderer& renderer, LayoutParams&& params);

    /**
     * @brief Frees Vulkan resources
     */
    ~PipelineLayout();

    /**
     * @brief Returns the Vulkan layout handle
     */
    constexpr VkPipelineLayout rawLayout() const;

    /**
     * @brief Creates descriptor set instances for this layout
     *
     * @param cache Descriptor set cache to use when creating or fetching sets
     * @param descriptors Vector of descriptor sets to populate
     */
    void createDescriptorSets(ds::DescriptorSetInstanceCache& cache,
                              std::vector<ds::DescriptorSetInstance*>& descriptors) const;

    /**
     * @brief Initializes the given array of descriptor sets and returns the number of sets
     *
     * @param cache Descriptor set cache to use when creating or fetching sets
     * @param sets Pointer to an array of descriptor set pointers
     * @return The number of descriptor sets used by this layout
     */
    std::uint32_t initDescriptorSets(ds::DescriptorSetInstanceCache& cache,
                                     ds::DescriptorSetInstance** sets) const;

    /**
     * @brief Updates the existing descriptor sets to the new ones for this layout. Only calls
     *        allocate/release for sets that are different between the old layout and the new
     *
     * @param cache Descriptor set cache to use when creating or fetching sets
     * @param sets The original descriptor sets. Will be updated in place
     * @param descriptorCount The number of descriptor sets in the old layout
     * @param entity The ECS id of the object being updated
     * @param sceneId The object scene id to update
     * @param updateSpeed The descriptor update frequency of the object
     * @return The number of descriptor sets used by this layout
     */
    std::uint32_t updateDescriptorSets(ds::DescriptorSetInstanceCache& cache,
                                       ds::DescriptorSetInstance** sets,
                                       std::uint32_t descriptorCount, ecs::Entity entity,
                                       std::uint32_t sceneId, UpdateSpeed updateSpeed) const;

private:
    Renderer& renderer;
    VkPipelineLayout layout;
    std::array<ds::DescriptorSetFactory*, 4> descriptorSets;
    const std::uint32_t dsCount;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VkPipelineLayout PipelineLayout::rawLayout() const { return layout; }

} // namespace vk
} // namespace gfx
} // namespace bl

namespace std
{
template<>
struct hash<bl::gfx::vk::PipelineLayout::LayoutParams> {
    size_t operator()(const bl::gfx::vk::PipelineLayout::LayoutParams& params) const {
        size_t result = hash<size_t>()(params.descriptorSets.size());
        for (size_t i = 0; i < params.dsCount; ++i) {
            const size_t nh = hash<type_index>()(params.descriptorSets[i].factoryType);
            result          = bl::util::hashCombine(result, nh);
        }
        for (size_t i = 0; i < params.pcCount; ++i) {
            const size_t nh =
                bl::util::hashCombine(hash<std::uint32_t>()(params.pushConstants[i].offset),
                                      hash<std::uint32_t>()(params.pushConstants[i].size));
            result = bl::util::hashCombine(result, nh);
        }
        return result;
    }
};
} // namespace std

#endif
