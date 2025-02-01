#ifndef BLIB_RENDER_RESOURCES_MATERIALPIPELINECACHE_HPP
#define BLIB_RENDER_RESOURCES_MATERIALPIPELINECACHE_HPP

#include <BLIB/Render/Materials/MaterialPipeline.hpp>

namespace bl
{
namespace rc
{
class Renderer;

namespace res
{
/**
 * @brief Stores all material pipelines
 *
 * @ingroup Renderer
 */
class MaterialPipelineCache {
public:
    /// The starting id for dynamically created material pipelines
    static constexpr std::uint32_t DynamicMaterialPipelineIdStart = 10000;

    /**
     * @brief Creates a new material pipeline with the given id and parameters
     *
     * @param pipelineId The id of the pipeline to create
     * @param settings The settings to create the pipeline with
     * @return A reference to the newly created pipeline
     */
    mat::MaterialPipeline& createPipeline(std::uint32_t pipelineId,
                                          mat::MaterialPipelineSettings&& settings);

    /**
     * @brief Creates or returns an existing pipeline with the given settings
     *
     * @param settings The settings to search for or create with
     * @return A reference to a pipeline with the given settings
     */
    mat::MaterialPipeline& getOrCreatePipeline(mat::MaterialPipelineSettings&& settings);

    /**
     * @brief Fetches the pipeline with the given id
     *
     * @param pipelineId The id of the pipeline to fetch
     * @return A reference to the pipeline with the given id
     */
    mat::MaterialPipeline& getPipeline(std::uint32_t pipelineId);

    /**
     * @brief Returns whether the pipeline with the given id exists
     *
     * @param pipelineId The id to search for
     * @return True if the pipeline exists, false otherwise
     */
    bool pipelineExists(std::uint32_t pipelineId) const;

private:
    Renderer& renderer;
    std::unordered_map<std::uint32_t, mat::MaterialPipeline> pipelines;
    std::uint32_t nextId;

    MaterialPipelineCache(Renderer& renderer);
    void createBuiltins();

    friend class Renderer;
};

} // namespace res
} // namespace rc
} // namespace bl

#endif
