#include <BLIB/Engine.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <BLIB/Math.hpp>
#include <BLIB/Render/Cameras/3D/Affectors/CameraShake.hpp>
#include <BLIB/Render/Cameras/3D/Camera3D.hpp>
#include <BLIB/Render/Cameras/3D/Controllers/OrbiterController.hpp>
#include <BLIB/Render/Components/Mesh.hpp>
#include <BLIB/Render/Components/Texture.hpp>
#include <BLIB/Render/Systems/BuiltinDrawableSystems.hpp>
#include <BLIB/Transforms/3D.hpp>

namespace
{
const std::vector<bl::render::prim::Vertex> Vertices = {
    bl::render::prim::Vertex{-0.5f, -0.5f, 1.f, 0.0f, 1.f},
    bl::render::prim::Vertex{0.5f, -0.5f, 1.f, 1.0f, 1.f},
    bl::render::prim::Vertex{0.5f, 0.5f, 1.f, 1.0f, 0.f},
    bl::render::prim::Vertex{-0.5f, 0.5f, 1.f, 0.0f, 0.f},

    bl::render::prim::Vertex{1.f, 0.f, 0.5f, 0.0f, 0.f},
    bl::render::prim::Vertex{1.f, 0.1f, -0.5f, 0.0f, 1.f},
    bl::render::prim::Vertex{0.75f, 0.7f, 0.5f, 1.0f, 1.f}};
const std::vector<std::uint32_t> Indices = {0, 1, 2, 2, 3, 0, 4, 5, 6};
} // namespace

class DemoState : public bl::engine::State {
public:
    DemoState()
    : State(bl::engine::StateMask::All) {}

    virtual ~DemoState() = default;

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        time = 0.f;

        // load texture
        texture =
            engine.renderer().texturePool().getOrLoadTexture("Resources/Textures/texture.png");

        // create camera
        bl::render::r3d::Camera3D* camera =
            engine.renderer().getObserver().pushCamera<bl::render::r3d::Camera3D>(
                glm::vec3{0.f, 1.f, 1.f}, glm::vec3{0.f, 0.f, 0.f}, 75.f);
        camera->setController<bl::render::r3d::OrbiterController>(
            glm::vec3{0.f, 0.f, 0.f}, 4.f, glm::vec3{0.3f, 1.f, 0.1f}, 2.f, 4.f);

        // create observer with camera
        bl::render::Observer& o = engine.renderer().addObserver();
        player2Cam              = o.pushCamera<bl::render::r3d::Camera3D>(
            glm::vec3{0.f, 0.5f, 2.f}, glm::vec3{0.f, 0.f, 0.f}, 75.f);
        player2Cam->addAffector<bl::render::r3d::CameraShake>(0.1f, 7.f);

        // create scene
        bl::render::Scene* scene = engine.renderer().getObserver().pushScene(10, 10);
        o.pushScene(scene);

        // get handle to mesh system
        bl::render::sys::MeshSystem& meshSystem =
            engine.systems().getSystem<bl::render::sys::MeshSystem>();

        // create object in scene
        entity = engine.ecs().createEntity();
        engine.ecs().emplaceComponent<bl::t3d::Transform3D>(entity);
        engine.ecs().emplaceComponent<bl::render::com::Texture>(entity, texture.id());
        bl::render::com::Mesh* mesh = engine.ecs().emplaceComponent<bl::render::com::Mesh>(entity);
        mesh->create(engine.renderer().vulkanState(), Vertices.size(), Indices.size());
        mesh->vertices = Vertices;
        mesh->indices  = Indices;
        mesh->gpuBuffer.sendToGPU();
        // TODO - causes sync/hang issues when added
        // meshSystem.addToScene(entity, scene, bl::render::SceneObject::UpdateSpeed::Static);
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        texture.release();
        engine.renderer().texturePool().releaseUnused();
        engine.renderer().getObserver().popScene();
        engine.renderer().removeObserver(1);
    }

    virtual void update(bl::engine::Engine&, float dt) override {
        time += dt;
        player2Cam->setPosition({0.f, 0.f, 2.f + std::cos(time)});
        player2Cam->getOrientationForChange().applyRoll(90.f * dt);
    }

    virtual void render(bl::engine::Engine& engine, float) override {
        engine.renderer().renderFrame();
    }

private:
    bl::ecs::Entity entity;
    bl::render::TextureRef texture;
    bl::render::r3d::Camera3D* player2Cam;
    float time;
};

int main() {
    const bl::engine::Settings engineSettings = bl::engine::Settings().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(1920, 1080, 32))
            .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
            .withTitle("Renderer Demo")
            .withIcon("vulkan.png"));
    bl::engine::Engine engine(engineSettings);

    engine.run(std::make_shared<DemoState>());

    return 0;
}
