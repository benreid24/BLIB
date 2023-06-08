#include <BLIB/Engine.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <BLIB/Math.hpp>
#include <BLIB/Render/Cameras/2D/Camera2D.hpp>
#include <BLIB/Render/Cameras/3D/Affectors/CameraShake.hpp>
#include <BLIB/Render/Cameras/3D/Camera3D.hpp>
#include <BLIB/Render/Cameras/3D/Controllers/OrbiterController.hpp>
#include <BLIB/Render/Components/Mesh.hpp>
#include <BLIB/Render/Components/Texture.hpp>
#include <BLIB/Render/Systems/BuiltinDrawableSystems.hpp>
#include <BLIB/Transforms.hpp>

namespace
{
const std::vector<bl::render::prim::Vertex> Vertices = {
    bl::render::prim::Vertex{-0.5f, -0.5f, 0.f, 0.0f, 1.f},
    bl::render::prim::Vertex{0.5f, -0.5f, 0.f, 1.0f, 1.f},
    bl::render::prim::Vertex{0.5f, 0.5f, 0.f, 1.0f, 0.f},
    bl::render::prim::Vertex{-0.5f, 0.5f, 0.f, 0.0f, 0.f},

    bl::render::prim::Vertex{1.f, 0.f, 0.5f, 0.0f, 0.f},
    bl::render::prim::Vertex{1.f, 0.1f, -0.5f, 0.0f, 1.f},
    bl::render::prim::Vertex{0.75f, 0.7f, 0.5f, 1.0f, 1.f}};
const std::vector<std::uint32_t> Indices = {0, 1, 2, 2, 3, 0, 4, 5, 6};
} // namespace

class DemoState
: public bl::engine::State
, bl::event::Listener<sf::Event> {
public:
    DemoState()
    : State(bl::engine::StateMask::All) {}

    virtual ~DemoState() = default;

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        time     = 0.f;
        renderer = &engine.renderer();

        // load textures
        texture =
            engine.renderer().texturePool().getOrLoadTexture("Resources/Textures/texture.png");
        messageBoxTxtr =
            engine.renderer().texturePool().getOrLoadTexture("Resources/Textures/messageBox.png");

        // get first observer and set background color
        bl::render::Observer& p1 = engine.renderer().getObserver(0);
        p1.setClearColor({0.f, 0.f, 1.f});

        // create 2d scene and camera for observer 1
        bl::render::Scene* scene = p1.pushScene<bl::render::scene::BasicScene>(10, 10);
        auto* p1cam =
            p1.setCamera<bl::render::c2d::Camera2D>(sf::FloatRect{0.f, 0.f, 1920.f, 1080.f * 0.5f});
        p1cam->setNearAndFarPlanes(-100000.f, 100000.f);
        p1cam->setRotation(15.f);

        // create sprite in scene
        spriteEntity   = engine.ecs().createEntity();
        spritePosition = engine.ecs().emplaceComponent<bl::t2d::Transform2D>(spriteEntity);
        engine.ecs().emplaceComponent<bl::render::com::Sprite>(
            spriteEntity, engine.renderer(), texture);
        engine.systems().getSystem<bl::render::sys::SpriteSystem>().addToScene(
            spriteEntity, scene, bl::render::UpdateSpeed::Dynamic);
        spritePosition->setPosition({1920.f * 0.5f, 1080.f * 0.25f});
        spritePosition->setScale({100.f / texture->sizeF.x, 100.f / texture->sizeF.y});
        spritePosition->setOrigin(texture->sizeF * 0.5f);

        // create 3d scene for observer 2
        bl::render::Observer& p2 = engine.renderer().addObserver();
        scene                    = p2.pushScene<bl::render::scene::BasicScene>(10, 10);

        // create camera for observer 2
        p2.setClearColor({0.f, 1.f, 0.f});
        bl::render::c3d::Camera3D* player2Cam = p2.setCamera<bl::render::c3d::Camera3D>(
            glm::vec3{0.f, 0.5f, 2.f}, glm::vec3{0.f, 0.f, 0.f}, 75.f);
        player2Cam->setController<bl::render::c3d::OrbiterController>(
            glm::vec3{0.f, 0.f, 0.f}, 4.f, glm::vec3{0.3f, 1.f, 0.1f}, 2.f, 4.f);
        player2Cam->addAffector<bl::render::c3d::CameraShake>(0.1f, 7.f);

        // get handle to mesh system
        bl::render::sys::MeshSystem& meshSystem =
            engine.systems().getSystem<bl::render::sys::MeshSystem>();

        // create object in scene
        meshEntity = engine.ecs().createEntity();
        engine.ecs().emplaceComponent<bl::t3d::Transform3D>(meshEntity);
        engine.ecs().emplaceComponent<bl::render::com::Texture>(meshEntity, texture.id());
        bl::render::com::Mesh* mesh =
            engine.ecs().emplaceComponent<bl::render::com::Mesh>(meshEntity);
        mesh->create(engine.renderer().vulkanState(), Vertices.size(), Indices.size());
        mesh->vertices = Vertices;
        mesh->indices  = Indices;
        mesh->gpuBuffer.sendToGPU();
        meshSystem.addToScene(meshEntity, scene, bl::render::UpdateSpeed::Static);

        // create overlay and add sprite for observer 2
        bl::render::Overlay* overlay = p2.getOrCreateSceneOverlay(10, 10);
        const auto ent               = engine.ecs().createEntity();
        auto* entPos                 = engine.ecs().emplaceComponent<bl::t2d::Transform2D>(ent);
        engine.ecs().emplaceComponent<bl::render::com::Sprite>(
            ent, engine.renderer(), messageBoxTxtr);
        const float scale = 0.2f / messageBoxTxtr->sizeF.y;
        entPos->setPosition({0.5f, 0.90f});
        entPos->setScale({scale, scale});
        entPos->setOrigin(messageBoxTxtr->sizeF * 0.5f);
        engine.systems().getSystem<bl::render::sys::SpriteSystem>().addToOverlay(
            ent, overlay, bl::render::UpdateSpeed::Dynamic);

        // subscribe to window events
        bl::event::Dispatcher::subscribe(this);
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        bl::event::Dispatcher::unsubscribe(this);
        texture.release();
        engine.renderer().texturePool().releaseUnused();
        engine.renderer().getObserver().popScene();
        engine.renderer().removeObserver(1);
        engine.ecs().destroyAllEntities();
    }

    virtual void update(bl::engine::Engine&, float dt) override {
        time += dt;
        if (time > 1.f) {
            time = 0.f;
            BL_LOG_INFO << "(" << spritePosition->getPosition().x << ", "
                        << spritePosition->getPosition().y << ")";
        }

        spritePosition->rotate(180.f * dt);
    }

    virtual void render(bl::engine::Engine&, float) override {
        // deprecated
    }

private:
    bl::render::Renderer* renderer;
    bl::ecs::Entity spriteEntity;
    bl::t2d::Transform2D* spritePosition;
    bl::ecs::Entity meshEntity;
    bl::render::res::TextureRef texture;
    bl::render::res::TextureRef messageBoxTxtr;
    float time;

    virtual void observe(const sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
            case sf::Keyboard::Z:
                renderer->setSplitscreenDirection(
                    bl::render::Renderer::SplitscreenDirection::LeftAndRight);
                break;
            case sf::Keyboard::X:
                renderer->setSplitscreenDirection(
                    bl::render::Renderer::SplitscreenDirection::TopAndBottom);
                break;

            case sf::Keyboard::Up:
                spritePosition->move({0.f, -10.f});
                break;
            case sf::Keyboard::Right:
                spritePosition->move({10.f, 0.f});
                break;
            case sf::Keyboard::Down:
                spritePosition->move({0.f, 10.f});
                break;
            case sf::Keyboard::Left:
                spritePosition->move({-10.f, 0.f});
                break;
            }
        }
    }
};

int main() {
    const bl::engine::Settings engineSettings = bl::engine::Settings().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(1920, 1080, 32))
            .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
            .withTitle("Renderer Demo")
            .withIcon("vulkan.png")
            .withLetterBoxOnResize(true));
    bl::engine::Engine engine(engineSettings);

    engine.run(std::make_shared<DemoState>());

    return 0;
}
