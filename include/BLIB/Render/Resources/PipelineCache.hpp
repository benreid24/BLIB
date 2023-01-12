#ifndef BLIB_RENDER_RESOURCES_PIPELINECACHE_HPP
#define BLIB_RENDER_RESOURCES_PIPELINECACHE_HPP

#include <BLIB/Render/Renderer/Pipeline.hpp>
#include <cstdint>
#include <unordered_map>

namespace bl
{
namespace render
{
class Renderer;

/**
 * @brief Basic cache to own and manage pipelines. Owned by the renderer
 *
 * @ingroup Renderer
 */
class PipelineCache {
public:
    /**
     * @brief Creates a new pipeline in the cache. Id should be unique
     *
     * @param pipelineId The unique id of the pipeline to create
     * @param params The parameters to create the pipeline with
     * @return Pipeline& The newly created pipeline
     */
    Pipeline& createPipline(std::uint32_t pipelineId, PipelineParameters&& params);

    /**
     * @brief Returns the pipeline with the given id. Throws an exception if not found. Pipelines
     *        should be created at application start
     *
     * @param pipelineId The id of the pipeline to fetch
     * @return Pipeline& The pipeline with the given id
     */
    Pipeline& getPipeline(std::uint32_t pipelineId);

private:
    Renderer& renderer;
    std::unordered_map<std::uint32_t, Pipeline> cache;

    PipelineCache(Renderer& renderer);
    void cleanup();

    friend class Renderer;
};

} // namespace render
} // namespace bl

#endif
