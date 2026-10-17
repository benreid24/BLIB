#include <BLIB/Objects/Skybox.hpp>

namespace bl
{
namespace obj
{
void Skybox::create(engine::World& world, as::TypedRef<asi::CubemapPayload> asset) {
    rc::res::TextureRef cubemap = world.engine().renderer().texturePool().getOrCreateCubemap(
        asset, rc::vk::SamplerOptions::Type::FilteredEdgeClamped);
    rc::res::MaterialRef material =
        world.engine().renderer().materialPool().getOrCreateFromTexture(cubemap);
    cube.create(world, 1.0f, material, rc::cfg::MaterialPipelineIds::Skybox);
    cube.component().setContainsTransparency(true); // always forward render
}

void Skybox::addToScene(rc::Scene* scene) { cube.addToScene(scene, rc::UpdateSpeed::Static); }

void Skybox::removeFromScene() { cube.removeFromScene(); }

void Skybox::draw(rc::scene::CodeScene::RenderContext& ctx) { cube.draw(ctx); }

} // namespace obj
} // namespace bl
