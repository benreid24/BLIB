#ifndef BLIB_RENDER_RESOURCES_COMPUTEPIPELINECACHE_HPP
#define BLIB_RENDER_RESOURCES_COMPUTEPIPELINECACHE_HPP

#include <BLIB/Render/Vulkan/ComputePipeline.hpp>
#include <cstdint>
#include <unordered_map>

namespace bl
{
namespace rc
{
namespace res
{
/**
 * @brief Cache that owns and manages compute pipelines. Owned by the renderer
 *
 * @ingroup Renderer
 */
class ComputePipelineCache {
public:
    /**
     * @brief Creates a new compute pipeline in the cache. Id should be unique
     *
     * @param pipelineId The id of the pipeline to create
     * @param params The parameters to create the pipeline with
     * @return A reference to the newly created pipeline
     */
    vk::ComputePipeline& createPipeline(std::uint32_t pipelineId,
                                        vk::ComputePipelineParameters&& params);

    /**
     * @brief Fetches the pipeline with the given id. Undefined behavior if id is invalid
     *
     * @param pipelineId The id of the pipeline to fetch
     * @return A reference to the pipeline
     */
    vk::ComputePipeline& getPipeline(std::uint32_t pipelineId);

    /**
     * @brief Fetches the pipeline with the given id. Returns nullptr if id is invalid
     *
     * @param pipelineId The id of the pipeline to fetch
     * @return A pointer to the pipeline or nullptr if not found
     */
    vk::ComputePipeline* getPipelineMaybe(std::uint32_t pipelineId);

    /**
     * @brief Returns whether a pipeline with the given id exists
     *
     * @param pipelineId The id to search for
     * @return True if the pipeline exists, false otherwise
     */
    bool pipelineExists(std::uint32_t pipelineId) const;

private:
    Renderer& renderer;
    std::unordered_map<std::uint32_t, vk::ComputePipeline> cache;

    ComputePipelineCache(Renderer& renderer);
    void cleanup();
    void createBuiltins();

    friend class bl::rc::Renderer;
};

} // namespace res
} // namespace rc
} // namespace bl

#endif
