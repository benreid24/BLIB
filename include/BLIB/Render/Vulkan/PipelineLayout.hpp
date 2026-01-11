#ifndef BLIB_RENDER_VULKAN_PIPELINELAYOUT_HPP
#define BLIB_RENDER_VULKAN_PIPELINELAYOUT_HPP

#include <BLIB/Containers/StaticVector.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Util/HashCombine.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <vector>

namespace bl
{
namespace rc
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
            DescriptorSet(DescriptorSet&& c)
            : factoryType(c.factoryType)
            , factory(std::move(c.factory)) {}
            DescriptorSet(std::type_index tid, std::unique_ptr<ds::DescriptorSetFactory>&& factory)
            : factoryType(tid)
            , factory(std::forward<std::unique_ptr<ds::DescriptorSetFactory>>(factory)) {}
        };

    public:
        LayoutParams() = default;

        LayoutParams(const LayoutParams& c)
        : descriptorSets(c.descriptorSets)
        , pushConstants(c.pushConstants) {}

        LayoutParams(LayoutParams&& c)
        : descriptorSets(std::move(c.descriptorSets))
        , pushConstants(std::move(c.pushConstants)) {}

        template<typename TFactory, typename... TArgs>
        void addDescriptorSet(TArgs&&... args) {
#ifdef BLIB_DEBUG
            if (descriptorSets.size() >= 4) {
                throw std::runtime_error("Exceeded 4 descriptor sets");
            }
#endif

            descriptorSets.emplace_back(typeid(TFactory),
                                        std::make_unique<TFactory>(std::forward<TArgs>(args)...));
        }

        template<typename TFactory, typename... TArgs>
        void replaceDescriptorSet(unsigned int i, TArgs&&... args) {
            descriptorSets[i].factoryType = typeid(TFactory);
            descriptorSets[i].factory = std::make_unique<TFactory>(std::forward<TArgs>(args)...);
        }

        void removeDescriptorSet(unsigned int i) { descriptorSets.erase(i); }

        void addPushConstantRange(std::uint32_t offset, std::uint32_t size,
                                  VkShaderStageFlags shaderStages = VK_SHADER_STAGE_ALL_GRAPHICS) {
#ifdef BLIB_DEBUG
            if (pushConstants.size() >= 4) {
                throw std::runtime_error("Exceeded 4 push constant ranges");
            }
#endif

            pushConstants.push_back(VkPushConstantRange{shaderStages, offset, size});
        }

        bool operator==(const LayoutParams& right) const {
            if (descriptorSets.size() != right.descriptorSets.size() ||
                pushConstants.size() != right.pushConstants.size()) {
                return false;
            }

            for (std::uint32_t i = 0; i < descriptorSets.size(); ++i) {
                if (descriptorSets[i].factoryType != right.descriptorSets[i].factoryType) {
                    return false;
                }
            }

            for (std::uint32_t i = 0; i < pushConstants.size(); ++i) {
                if (pushConstants[i].offset != right.pushConstants[i].offset) { return false; }
                if (pushConstants[i].size != right.pushConstants[i].size) { return false; }
                if (pushConstants[i].stageFlags != right.pushConstants[i].stageFlags) {
                    return false;
                }
            }

            return true;
        }

    private:
        ctr::StaticVector<DescriptorSet, 4> descriptorSets;
        ctr::StaticVector<VkPushConstantRange, 4> pushConstants;

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
    VkPipelineLayout rawLayout() const;

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
     * @brief Returns the number of descriptor sets in this layout
     */
    std::uint32_t getDescriptorSetCount() const { return descriptorSets.size(); }

    /**
     * @brief Returns the descriptor set factory at the given index
     *
     * @param i The index of the descriptor set factory to get
     * @return A pointer to the descriptor set factory
     */
    ds::DescriptorSetFactory* getDescriptorSetFactory(std::uint32_t i) const {
        return descriptorSets[i];
    }

private:
    Renderer& renderer;
    VkPipelineLayout layout;
    ctr::StaticVector<ds::DescriptorSetFactory*, 4> descriptorSets;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline VkPipelineLayout PipelineLayout::rawLayout() const { return layout; }

} // namespace vk
} // namespace rc
} // namespace bl

namespace std
{
template<>
struct hash<bl::rc::vk::PipelineLayout::LayoutParams> {
    size_t operator()(const bl::rc::vk::PipelineLayout::LayoutParams& params) const {
        size_t result = hash<size_t>()(params.descriptorSets.size());
        for (size_t i = 0; i < params.descriptorSets.size(); ++i) {
            const size_t nh = hash<type_index>()(params.descriptorSets[i].factoryType);
            result          = bl::util::hashCombine(result, nh);
        }
        for (size_t i = 0; i < params.pushConstants.size(); ++i) {
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
