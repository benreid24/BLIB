#include <BLIB/Engine.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <BLIB/Math.hpp>
#include <BLIB/Render/Cameras/3D/Affectors/CameraShake.hpp>
#include <BLIB/Render/Cameras/3D/Camera3D.hpp>
#include <BLIB/Render/Cameras/3D/Controllers/OrbiterController.hpp>

class DemoState : public bl::engine::State {
public:
    DemoState()
    : State(bl::engine::StateMask::All) {}

    virtual ~DemoState() = default;

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        time = 0.f;

        bl::render::r3d::Camera3D* camera =
            engine.renderer().getObserver().pushCamera<bl::render::r3d::Camera3D>(
                glm::vec3{0.f, 1.f, 1.f}, glm::vec3{0.f, 0.f, 0.f}, 75.f);
        camera->setController<bl::render::r3d::OrbiterController>(
            glm::vec3{0.f, 0.f, 0.f}, 4.f, glm::vec3{0.3f, 1.f, 0.1f}, 2.f, 4.f);

        bl::render::Observer& o = engine.renderer().addObserver();
        player2Cam              = o.pushCamera<bl::render::r3d::Camera3D>(
            glm::vec3{0.f, 0.5f, 2.f}, glm::vec3{0.f, 0.f, 0.f}, 75.f);
        player2Cam->addAffector<bl::render::r3d::CameraShake>(0.1f, 7.f);

        bl::render::Scene* scene = engine.renderer().getObserver().pushScene(10, 10);
        o.pushScene(scene);
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
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
