#include <BLIB/Graphics/Skybox.hpp>

namespace bl
{
namespace gfx
{
void Skybox::create(engine::World& world, as::TypedRef<asi::ImagePayload> right,
                    as::TypedRef<asi::ImagePayload> left, as::TypedRef<asi::ImagePayload> top,
                    as::TypedRef<asi::ImagePayload> bottom, as::TypedRef<asi::ImagePayload> back,
                    as::TypedRef<asi::ImagePayload> front) {
    rc::res::TextureRef cubemap = world.engine().renderer().texturePool().createCubemap(
        right,
        left,
        top,
        bottom,
        back,
        front,
        rc::vk::CommonTextureFormats::SRGBA32Bit,
        rc::vk::SamplerOptions::Type::FilteredEdgeClamped);
    rc::res::MaterialRef material =
        world.engine().renderer().materialPool().getOrCreateFromTexture(cubemap);
    cube.create(world, 1.0f, material, rc::cfg::MaterialPipelineIds::Skybox);
    cube.component().setContainsTransparency(true); // always forward render
}

void Skybox::create(engine::World& world, as::TypedRef<asi::TexturePayload> right,
                    as::TypedRef<asi::TexturePayload> left, as::TypedRef<asi::TexturePayload> top,
                    as::TypedRef<asi::TexturePayload> bottom,
                    as::TypedRef<asi::TexturePayload> back,
                    as::TypedRef<asi::TexturePayload> front) {
    create(world,
           right->image.getRef(),
           left->image.getRef(),
           top->image.getRef(),
           bottom->image.getRef(),
           back->image.getRef(),
           front->image.getRef());
}

void Skybox::addToScene(rc::Scene* scene) { cube.addToScene(scene, rc::UpdateSpeed::Static); }

void Skybox::removeFromScene() { cube.removeFromScene(); }

void Skybox::draw(rc::scene::CodeScene::RenderContext& ctx) { cube.draw(ctx); }

} // namespace gfx
} // namespace bl
