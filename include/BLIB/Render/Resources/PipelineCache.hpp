#ifndef BLIB_RENDER_RESOURCES_PIPELINECACHE_HPP
#define BLIB_RENDER_RESOURCES_PIPELINECACHE_HPP

#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <cstdint>
#include <unordered_map>

namespace bl
{
namespace rc
{
class Renderer;

namespace res
{
/**
 * @brief Basic cache to own and manage pipelines. Owned by the renderer
 *
 * @ingroup Renderer
 */
class PipelineCache {
public:
    /// The first id to try when creating dynamic pipeline ids
    static constexpr std::uint32_t DynamicPipelineIdStart = 10000;

    /**
     * @brief Creates a new pipeline in the cache. Id should be unique
     *
     * @param pipelineId The unique id of the pipeline to create
     * @param params The parameters to create the pipeline with
     * @return Pipeline& The newly created pipeline
     */
    vk::Pipeline& createPipline(std::uint32_t pipelineId, vk::PipelineParameters&& params);

    /**
     * @brief Creates or returns an existing pipeline using the given parameters and a free id
     *
     * @param params The parameters to get or create the pipeline for
     * @return A pipeline with the given parameters
     */
    vk::Pipeline& getOrCreatePipeline(vk::PipelineParameters&& params);

    /**
     * @brief Returns the pipeline with the given id. Throws an exception if not found. Pipelines
     *        should be created at application start
     *
     * @param pipelineId The id of the pipeline to fetch
     * @return Pipeline& The pipeline with the given id
     */
    vk::Pipeline& getPipeline(std::uint32_t pipelineId);

    /**
     * @brief Checks whether a pipeline with the given id exists
     *
     * @param pipelineId The id to search for
     * @return True if the pipeline exists, false otherwise
     */
    bool pipelineExists(std::uint32_t pipelineId) const;

private:
    Renderer& renderer;
    std::unordered_map<std::uint32_t, vk::Pipeline> cache;
    std::uint32_t nextId;

    PipelineCache(Renderer& renderer);
    void cleanup();
    void createBuiltins();

    friend class bl::rc::Renderer;
};

} // namespace res
} // namespace rc
} // namespace bl

#endif
