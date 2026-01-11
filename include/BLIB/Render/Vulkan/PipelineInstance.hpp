#ifndef BLIB_RENDER_VULKAN_PIPELINEINSTANCE_HPP
#define BLIB_RENDER_VULKAN_PIPELINEINSTANCE_HPP

#include <BLIB/Containers/StaticVector.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Graph/ExecutionContext.hpp>
#include <BLIB/Render/RenderPhase.hpp>
#include <BLIB/Render/Vulkan/ComputePipeline.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <memory>
#include <variant>
#include <vector>

namespace bl
{
namespace rc
{
namespace scene
{
class SceneRenderContext;
}
namespace ds
{
class DescriptorSetInstance;
class DescriptorSetInstanceCache;
} // namespace ds

namespace vk
{
/**
 * @brief Helper class which represents an instance of a pipeline for a scene. Allows easy binding
 *        of the pipeline and descriptor sets. Intended for use by post processing tasks
 *
 * @ingroup Renderer
 */
class PipelineInstance {
public:
    /**
     * @brief Creates an empty instance
     */
    PipelineInstance();

    /**
     * @brief Initializes the pipeline instance
     *
     * @param pipeline The pipeline to instantiate
     * @param cache The cache to get descriptor sets from
     */
    void init(Pipeline* pipeline, ds::DescriptorSetInstanceCache& cache);

    /**
     * @brief Initializes the pipeline instance
     *
     * @param pipeline The pipeline to instantiate
     * @param cache The cache to get descriptor sets from
     */
    void init(ComputePipeline* pipeline, ds::DescriptorSetInstanceCache& cache);

    /**
     * @brief Returns a pointer to the graphics pipeline of this instance. May be nullptr
     */
    Pipeline* getGraphicsPipeline();

    /**
     * @brief Returns a pointer to the compute pipeline of this instance. May be nullptr
     */
    ComputePipeline* getComputePipeline();

    /**
     * @brief Returns the pipeline of this instance. Must be initialized prior to calling
     */
    const PipelineLayout& getPipelineLayout() const;

    /**
     * @brief Issues commands to bind the pipeline and descriptor sets for rendering
     *
     * @param ctx The current rendering context
     * @param specialization The pipeline specialization to use
     * @param updateFreq The frequency to pass to the descriptor set binds
     */
    void bind(scene::SceneRenderContext& ctx, std::uint32_t specialization = 0,
              UpdateSpeed updateFreq = UpdateSpeed::Dynamic);

    /**
     * @brief Issues commands to bind the pipeline and descriptor sets for rendering. Creates a
     *        scene context from the parameters
     *
     * @param ctx The current render graph execution context
     * @param renderPhase The current render phase
     * @param renderPassId The current render pass id
     * @param viewport The current viewport
     * @param specialization The pipeline specialization to use
     * @param updateFreq The frequency to pass to the descriptor set binds
     */
    void bind(const rg::ExecutionContext& ctx, RenderPhase renderPhase,
              std::uint32_t renderPassId = 0, const VkViewport& viewport = {},
              std::uint32_t specialization = 0, UpdateSpeed updateFreq = UpdateSpeed::Dynamic);

    /**
     * @brief Returns the descriptor set with the given type. Performs a linear search
     *
     * @tparam T The type of the descriptor set to get
     * @return A pointer to the descriptor set. Nullptr if not found
     */
    template<typename T>
    T* getDescriptorSet();

    /**
     * @brief Returns the descriptor set with the given type with an index hint
     *
     * @tparam T The type of the descriptor set to get
     * @param indexHint The index to check first for the given set
     * @return A pointer to the descriptor set. Nullptr if not found
     */
    template<typename T>
    T* getDescriptorSet(std::uint32_t indexHint);

private:
    enum State { Uninitialized, Graphics, Compute } state;
    union {
        Pipeline* gfxPipeline;
        ComputePipeline* computePipeline;
    };
    ctr::StaticVector<ds::DescriptorSetInstance*, 4> descriptorSets;
    std::vector<std::unique_ptr<ds::DescriptorSetInstance>> ownedSets;

    void initDescriptors(ds::DescriptorSetInstanceCache& cache, const PipelineLayout& layout);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
T* PipelineInstance::getDescriptorSet(std::uint32_t indexHint) {
    if (indexHint < descriptorSets.size()) {
        T* set = dynamic_cast<T*>(descriptorSets[indexHint]);
        if (set) { return set; }
    }
    return getDescriptorSet<T>();
}

template<typename T>
T* PipelineInstance::getDescriptorSet() {
    for (ds::DescriptorSetInstance* set : descriptorSets) {
        T* s = dynamic_cast<T*>(set);
        if (s) { return s; }
    }
    return nullptr;
}

inline Pipeline* PipelineInstance::getGraphicsPipeline() { return gfxPipeline; }

inline ComputePipeline* PipelineInstance::getComputePipeline() { return computePipeline; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif
