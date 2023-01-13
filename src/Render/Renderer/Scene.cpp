#include <BLIB/Render/Renderer/Scene.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
using RpId = Config::RenderPassIds;

Scene::Scene(Renderer& r)
: renderer(r)
, objects(container::ObjectPool<Object>::GrowthPolicy::ExpandBuffer, 256)
, shadowPass(renderer, RpId::Shadow)
, opaquePass(renderer, RpId::Opaque)
, transparencyPass(renderer, RpId::Transparent)
, postFxPass(renderer, RpId::PostFX)
, overlayPass(renderer, RpId::Overlay) {
    renderPasses[RpId::Shadow]      = &shadowPass;
    renderPasses[RpId::Opaque]      = &opaquePass;
    renderPasses[RpId::Transparent] = &transparencyPass;
    renderPasses[RpId::PostFX]      = &postFxPass;
    renderPasses[RpId::Overlay]     = &overlayPass;
    toRemove.reserve(64);
}

Object::Handle Scene::createAndAddObject(Renderable* owner) {
    std::unique_lock lock(mutex);
    return objects.getStableRef(objects.emplace(owner));
}

void Scene::removeObject(const Object::Handle& obj) {
    std::unique_lock lock(eraseMutex);
    toRemove.emplace_back(obj);
}

void Scene::recordRenderCommands() {
    // TODO
}

void Scene::performRemovals() {
    std::unique_lock lock(eraseMutex);
    for (Object::Handle& obj : toRemove) {
        obj.erase();
    }
    toRemove.clear();
}

} // namespace render
} // namespace bl
