#ifndef BLIB_RENDER_VULKAN_PIPELINEINSTANCE_HPP
#define BLIB_RENDER_VULKAN_PIPELINEINSTANCE_HPP

#include <BLIB/Containers/StaticVector.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>

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
     * @brief Returns the pipeline of this instance. Must be initialized prior to calling
     */
    vk::Pipeline& getPipeline() { return *pipeline; }

    /**
     * @brief Returns the pipeline of this instance. Must be initialized prior to calling
     */
    const vk::Pipeline& getPipeline() const { return *pipeline; }

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
    vk::Pipeline* pipeline;
    ctr::StaticVector<ds::DescriptorSetInstance*, 4> descriptorSets;
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

} // namespace vk
} // namespace rc
} // namespace bl

#endif
