#ifndef BLIB_RENDER_RESOURCES_PIPELINECACHE_HPP
#define BLIB_RENDER_RESOURCES_PIPELINECACHE_HPP

#include <BLIB/Render/Renderer/Pipeline.hpp>
#include <unordered_map>
#include <cstdint>

namespace bl
{
namespace render
{
class Renderer;

class PipelineCache {
public:
    PipelineCache(Renderer& renderer);

    ~PipelineCache();

    bool createPipline(std::uint32_t pipelineId, PipelineParameters&& params, std::uint32_t renderPassId);

    Pipeline* getPipeline(std::uint32_t pipelineId);

private:
    Renderer& renderer;
    std::unordered_map<std::uint32_t, Pipeline> cache;
};

}
}

#endif
