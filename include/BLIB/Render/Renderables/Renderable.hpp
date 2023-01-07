#ifndef BLIB_RENDER_RENDERABLES_RENDERABLE_HPP
#define BLIB_RENDER_RENDERABLES_RENDERABLE_HPP

#include <BLIB/Render/Uniforms/PushConstants.hpp>
#include <cstdint>
#include <glad/vulkan.h>
#include <mutex>

namespace bl
{
namespace render
{
class PipelineInstance;
struct Object;
class Scene;

class Renderable {
public:
    Renderable(Renderable&& copy);

    virtual ~Renderable();

    void addToScene(Scene& scene, std::uint32_t renderPassId, std::uint32_t pipelineId);

    void addToScene(PipelineInstance& pipeline);

    void removeFromScene();

    void setHidden(bool hidden);

protected:
    Renderable();

    void markPCDirty();

    void setTextureId(std::uint32_t textureId);

    void setMaterialId(std::uint32_t materialId);

    // TODO - transform helpers

private:
    PipelineInstance* owner;
    std::mutex mutex;
    Object* object;
    PushConstants frameData;

    void syncPC();

    Renderable(const Renderable&) = delete;
    friend class PipelineInstance;
};

} // namespace render
} // namespace bl

#endif
