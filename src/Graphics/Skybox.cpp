#include <BLIB/Graphics/Skybox.hpp>

namespace bl
{
namespace gfx
{
void Skybox::create(engine::World& world, const std::string& right, const std::string& left,
                    const std::string& top, const std::string& bottom, const std::string& back,
                    const std::string& front) {
    rc::res::TextureRef cubemap =
        world.engine().renderer().texturePool().createCubemap(right,
                                                              left,
                                                              top,
                                                              bottom,
                                                              back,
                                                              front,
                                                              rc::vk::TextureFormat::SRGBA32Bit,
                                                              rc::vk::Sampler::FilteredEdgeClamped);
    rc::res::MaterialRef material =
        world.engine().renderer().materialPool().getOrCreateFromTexture(cubemap);
    cube.create(world, 1.0f, material, rc::cfg::MaterialPipelineIds::Skybox);
    cube.component().setContainsTransparency(true); // always forward render
}

void Skybox::addToScene(rc::Scene* scene) { cube.addToScene(scene, rc::UpdateSpeed::Static); }

void Skybox::removeFromScene() { cube.removeFromScene(); }

void Skybox::draw(rc::scene::CodeScene::RenderContext& ctx) { cube.draw(ctx); }

} // namespace gfx
} // namespace bl
