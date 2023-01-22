#include <BLIB/Engine.hpp>
#include <BLIB/Render/Renderables/3D/Mesh.hpp>
#include <glm/gtc/matrix_transform.hpp>

class DemoState : public bl::engine::State {
public:
    DemoState() {}

    virtual ~DemoState() = default;

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        time = 0.f;

        if (mesh.buffer().indices().size() == 0) {
            mesh.buffer().create(engine.renderer().vulkanState(), 4, 6);
            mesh.buffer().vertices().assign<std::initializer_list<float>>(
                {{-0.5f, -0.5f, 0.f, 1.0f, 0.0f, 0.0f},
                 {0.5f, -0.5f, 0.f, 0.0f, 1.0f, 0.0f},
                 {0.5f, 0.5f, 0.f, 0.0f, 0.0f, 1.0f},
                 {-0.5f, 0.5f, 0.f, 1.0f, 1.0f, 1.0f}});
            mesh.buffer().indices().assign({0, 1, 2, 2, 3, 0});
            mesh.buffer().sendToGPU();
            mesh.attachBuffer();
            mesh.setTransform(glm::mat4(1.0f));
        }

        mesh.addToScene(*engine.renderer().testScene);
        mesh.setHidden(false);
    }

    virtual void deactivate(bl::engine::Engine& engine) override { mesh.removeFromScene(); }

    virtual void update(bl::engine::Engine&, float dt) override {
        time += dt;

        mesh.setTransform(
            glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    }

    virtual void render(bl::engine::Engine& engine, float) override {
        engine.renderer().renderFrame();
    }

private:
    bl::render::r3d::Mesh mesh;
    float time;
};

int main() {
    const bl::engine::Settings engineSettings = bl::engine::Settings().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(1920, 1080, 32))
            .withStyle(sf::Style::Close | sf::Style::Titlebar)
            .withTitle("Renderer Demo")
            .withIcon("vulkan.png"));
    bl::engine::Engine engine(engineSettings);

    engine.run(std::make_shared<DemoState>());

    return 0;
}
