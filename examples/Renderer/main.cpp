#include <BLIB/Cameras.hpp>
#include <BLIB/Components.hpp>
#include <BLIB/Engine.hpp>
#include <BLIB/Graphics.hpp>
#include <BLIB/Render.hpp>
#include <BLIB/Systems.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
const std::vector<bl::rc::prim::Vertex> Vertices = {
    bl::rc::prim::Vertex{-0.5f, -0.5f, 0.f, 0.0f, 1.f},
    bl::rc::prim::Vertex{0.5f, -0.5f, 0.f, 1.0f, 1.f},
    bl::rc::prim::Vertex{0.5f, 0.5f, 0.f, 1.0f, 0.f},
    bl::rc::prim::Vertex{-0.5f, 0.5f, 0.f, 0.0f, 0.f},

    bl::rc::prim::Vertex{1.f, 0.f, 0.5f, 0.0f, 0.f},
    bl::rc::prim::Vertex{1.f, 0.1f, -0.5f, 0.0f, 1.f},
    bl::rc::prim::Vertex{0.75f, 0.7f, 0.5f, 1.0f, 1.f}};
const std::vector<std::uint32_t> Indices = {0, 1, 2, 2, 3, 0, 4, 5, 6};
} // namespace

void exportTexture(bl::rc::tfr::TextureExport* te, std::string_view outfile,
                   std::atomic_bool& inProgress) {
    te->wait();
    sf::Image result;
    te->copyImage(result);
    te->release();
    result.saveToFile(std::string(outfile));
    inProgress = false;
}

class DemoState
: public bl::engine::State
, bl::event::Listener<sf::Event> {
public:
    DemoState()
    : State(bl::engine::StateMask::All)
    , fadeout(nullptr)
    , exportInProgress(false) {}

    virtual ~DemoState() = default;

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        this->engine = &engine;
        renderer     = &engine.renderer();

        // load resources
        texture =
            engine.renderer().texturePool().getOrLoadTexture("Resources/Textures/texture.png");
        messageBoxTxtr =
            engine.renderer().texturePool().getOrLoadTexture("Resources/Textures/messageBox.png");
        font = bl::resource::ResourceManager<sf::VulkanFont>::load("Resources/Fonts/font.ttf");

        // get first observer and set background color
        bl::rc::Observer& p1 = engine.renderer().getObserver(0);
        p1.setClearColor({0.f, 0.f, 1.f, 1.f});

        // create 2d scene and camera for observer 1
        bl::rc::SceneRef scene2d = p1.pushScene<bl::rc::scene::Scene2D>();
        auto* p1cam =
            p1.setCamera<bl::cam::Camera2D>(sf::FloatRect{0.f, 0.f, 1920.f, 1080.f * 0.5f});
        p1cam->setRotation(15.f);

        // create sprite in scene
        bl::ecs::Entity spriteEntity = engine.ecs().createEntity();
        spritePosition = engine.ecs().emplaceComponent<bl::com::Transform2D>(spriteEntity);
        engine.ecs().emplaceComponent<bl::com::Texture>(spriteEntity, texture);
        bl::com::Sprite& scom = *engine.ecs().emplaceComponent<bl::com::Sprite>(
            spriteEntity, engine.renderer(), texture);
        scom.addToScene(engine.ecs(), spriteEntity, scene2d, bl::rc::UpdateSpeed::Dynamic);
        spritePosition->setPosition({1920.f * 0.5f, 1080.f * 0.25f});
        spritePosition->setScale({100.f / texture->size().x, 100.f / texture->size().y});
        spritePosition->setOrigin(texture->size() * 0.5f);
        engine.ecs().emplaceComponent<bl::com::Velocity2D>(spriteEntity, glm::vec2{}, 180.f);

        // use SFML-like class to make another
        sprite.create(engine, texture);
        sprite.getTransform().setPosition({1920.f * 0.75f, 1080.f * 0.25f});
        sprite.scaleToSize({150.f, 150.f});
        sprite.getTransform().setOrigin(texture->size() * 0.5f);
        sprite.addToScene(scene2d, bl::rc::UpdateSpeed::Static);

        // create 3d scene for observer 2
        bl::rc::Observer& p2   = engine.addPlayer().getRenderObserver();
        bl::rc::SceneRef scene = p2.pushScene<bl::rc::scene::Scene3D>();

        // create camera for observer 2
        p2.setClearColor({0.f, 1.f, 0.f, 1.f});
        bl::cam::Camera3D* player2Cam = p2.setCamera<bl::cam::Camera3D>(
            glm::vec3{0.f, 0.5f, 2.f}, glm::vec3{0.f, 0.f, 0.f}, 75.f);
        player2Cam->setController<bl::cam::c3d::OrbiterController>(
            glm::vec3{0.f, 0.f, 0.f}, 4.f, glm::vec3{0.3f, 1.f, 0.1f}, 2.f, 4.f);
        player2Cam->addAffector<bl::cam::c3d::CameraShake>(0.1f, 7.f);

        // create object in scene
        meshEntity = engine.ecs().createEntity();
        engine.ecs().emplaceComponent<bl::com::Transform3D>(meshEntity);
        engine.ecs().emplaceComponent<bl::com::Texture>(meshEntity, texture);
        bl::com::Mesh* mesh = engine.ecs().emplaceComponent<bl::com::Mesh>(meshEntity);
        mesh->create(engine.renderer().vulkanState(), Vertices.size(), Indices.size());
        mesh->gpuBuffer.vertices() = Vertices;
        for (auto& v : mesh->gpuBuffer.vertices()) {
            v.texCoord = texture->convertCoord(v.texCoord);
        }
        mesh->gpuBuffer.indices() = Indices;
        mesh->gpuBuffer.queueTransfer(bl::rc::tfr::Transferable::SyncRequirement::Immediate);
        mesh->addToScene(engine.ecs(), meshEntity, scene, bl::rc::UpdateSpeed::Static);

        // create overlay and add sprite for observer 2
        bl::rc::Overlay* overlay = p2.getOrCreateSceneOverlay();
        messageBox.create(engine, messageBoxTxtr);
        messageBox.getTransform().setOrigin(messageBox.getTexture()->size() * 0.5f);
        messageBox.getOverlayScaler().scaleToHeightPercent(0.3f);
        messageBox.getOverlayScaler().positionInParentSpace({0.5f, 0.85f});
        messageBox.setScissorToSelf();
        messageBox.addToScene(overlay, bl::rc::UpdateSpeed::Static);

        // add text to overlay
        text.create(engine, *font, "Text can now be", 64);
        text.addSection("rendered.", 64, {0.f, 0.8f, 0.6f, 1.f}, sf::Text::Italic);
        text.addSection("What a great time to be alive. I wonder if this will wrap properly.", 64);
        text.getOverlayScaler().positionInParentSpace({-0.47f, -0.45f});
        text.getOverlayScaler().scaleToHeightRatio(64.f, 0.19f);
        text.wordWrapToParent(0.9f);
        text.setParent(messageBox);
        text.addToScene(overlay, bl::rc::UpdateSpeed::Static);

        // sanity check children
        sprite2.create(engine, texture);
        sprite2.getOverlayScaler().positionInParentSpace({0.4f, 0.4f});
        sprite2.getOverlayScaler().scaleToHeightPercent(0.1f);
        sprite2.getTransform().setOrigin(texture->size() * 0.5f);
        sprite2.setParent(messageBox);
        sprite2.addToScene(overlay, bl::rc::UpdateSpeed::Static);

        // setup render texture
        renderTexture = engine.renderer().createRenderTexture({128, 128});
        bl::rc::SceneRef rto =
            engine.renderer().scenePool().allocateScene<bl::rc::scene::Scene2D>();
        renderTexture->pushScene(rto);
        renderTexture->setCamera<bl::cam::Camera2D>(sf::FloatRect{0.f, 0.f, 1.f, 1.f});
        renderTexture->setClearColor({0.f, 0.0f, 0.7f, 0.4f});

        renderTextureInnerSprite.create(engine, texture);
        renderTextureInnerSprite.getTransform().setScale(
            {1.f / renderTextureInnerSprite.getLocalSize().x,
             1.f / renderTextureInnerSprite.getLocalSize().y});
        renderTextureInnerSprite.getTransform().setPosition({0.f, 0.f});
        renderTextureInnerSprite.addToScene(rto, bl::rc::UpdateSpeed::Static);

        renderTextureOuterSprite.create(engine, renderTexture->getTexture());
        renderTextureOuterSprite.getOverlayScaler().scaleToHeightPercent(0.15f);
        renderTextureOuterSprite.getOverlayScaler().positionInParentSpace({0.05f, 0.1f});
        renderTextureOuterSprite.addToScene(overlay, bl::rc::UpdateSpeed::Static);

        // setup another render texture that renders our 2d scene
        renderTextureNested = engine.renderer().createRenderTexture({200, 200});
        renderTextureNested->pushScene(scene2d);
        renderTextureNested->setClearColor({0.f, 0.f, 1.f, 1.f});
        renderTextureNested->setCamera<bl::cam::Camera2D>(glm::vec2(1920.f * 0.5f, 1080.f * 0.25f),
                                                          glm::vec2(1200.f, 1200.f));
        renderTextureNestedSprite.create(engine, renderTextureNested->getTexture());
        renderTextureNestedSprite.getOverlayScaler().scaleToHeightPercent(0.4f);
        renderTextureNestedSprite.getOverlayScaler().positionInParentSpace({0.8f, 0.2f});
        renderTextureNestedSprite.addToScene(overlay, bl::rc::UpdateSpeed::Static);

        // subscribe to window events
        bl::event::Dispatcher::subscribe(this);
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        bl::event::Dispatcher::unsubscribe(this);
        texture.release();
        engine.renderer().texturePool().releaseUnused();
        engine.renderer().getObserver().popScene();
        engine.removePlayer(1);
        engine.ecs().destroyAllEntities();
    }

    virtual void update(bl::engine::Engine&, float, float) override {}

private:
    bl::engine::Engine* engine;
    bl::rc::Renderer* renderer;
    bl::gfx::Sprite sprite;
    bl::gfx::Sprite sprite2;
    bl::gfx::Sprite messageBox;
    bl::com::Transform2D* spritePosition;
    bl::ecs::Entity meshEntity;
    bl::rc::res::TextureRef texture;
    bl::rc::res::TextureRef messageBoxTxtr;
    bl::resource::Ref<sf::VulkanFont> font;
    bl::gfx::Text text;
    bl::rc::vk::RenderTexture::Handle renderTexture;
    bl::gfx::Sprite renderTextureInnerSprite;
    bl::gfx::Sprite renderTextureOuterSprite;
    bl::rc::rgi::FadeEffectTask* fadeout;
    bl::rc::vk::RenderTexture::Handle renderTextureNested;
    bl::gfx::Sprite renderTextureNestedSprite;
    std::atomic_bool exportInProgress;

    virtual void observe(const sf::Event& event) override {
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
            case sf::Keyboard::Z:
                renderer->setSplitscreenDirection(
                    bl::rc::Renderer::SplitscreenDirection::LeftAndRight);
                break;
            case sf::Keyboard::X:
                renderer->setSplitscreenDirection(
                    bl::rc::Renderer::SplitscreenDirection::TopAndBottom);
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

            case sf::Keyboard::F:
                fadeout = renderer->getObserver(0)
                              .getRenderGraph()
                              .putUniqueTask<bl::rc::rgi::FadeEffectTask>(2.f);
                fadeout->fadeTo(2.f, 0.f);
                break;
            case sf::Keyboard::G:
                fadeout = renderer->getObserver(0)
                              .getRenderGraph()
                              .putUniqueTask<bl::rc::rgi::FadeEffectTask>(2.f, 0.f, 1.f);
                fadeout->fadeTo(2.f, 1.f);
                break;
            case sf::Keyboard::C:
                renderer->getObserver(0).getRenderGraph().removeTask<bl::rc::rgi::FadeEffectTask>();
                fadeout = nullptr;
                break;

            case sf::Keyboard::S:
            case sf::Keyboard::T:
                if (!exportInProgress) {
                    exportInProgress    = true;
                    const bool isScreen = event.key.code == sf::Keyboard::S;
                    bl::rc::tfr::TextureExport* te =
                        isScreen ?
                            renderer->textureExporter().exportSwapImage() :
                            renderer->textureExporter().exportTexture(renderTexture->getTexture());
                    engine->longRunningThreadpool().queueTask([this, te, isScreen]() {
                        exportTexture(te,
                                      isScreen ? "exportedScreen.png" : "exportedRenderTexture.png",
                                      exportInProgress);
                    });
                }
                break;
            }
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            const glm::vec2 mpos(event.mouseButton.x, event.mouseButton.y);
            const auto ir = text.findCharacterAtWindowPosition(mpos);
            if (ir.found) {
                BL_LOG_INFO << "Clicked: (" << ir.sectionIndex << ", " << ir.characterIndex
                            << ") => '"
                            << static_cast<char>(text.getSection(ir.sectionIndex)
                                                     .getWordWrappedString()[ir.characterIndex])
                            << "'";
            }
            else { BL_LOG_INFO << "Did not click text"; }
        }
        else if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.mouseWheelScroll.delta < 0.f) {
                engine->setTimeScale(engine->getTimeScale() * 0.9f);
            }
            else { engine->setTimeScale(engine->getTimeScale() * 1.1f); }
            BL_LOG_INFO << "Scaling time: " << engine->getTimeScale();
        }
    }
};

int main() {
    bl::cam::OverlayCamera::setOverlayCoordinateSpace(1920.f * 0.5f, 1080.f * 0.5f * 0.5f);

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
