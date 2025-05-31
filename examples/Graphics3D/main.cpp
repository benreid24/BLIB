#include <BLIB/Cameras.hpp>
#include <BLIB/Components.hpp>
#include <BLIB/Engine.hpp>
#include <BLIB/Graphics.hpp>
#include <BLIB/Graphics/BasicModel.hpp>
#include <BLIB/Models.hpp>
#include <BLIB/Render.hpp>
#include <BLIB/Resources.hpp>
#include <BLIB/Systems.hpp>
#include <iostream>

class CameraController
: public bl::cam::CameraController3D
, public bl::event::Listener<sf::Event> {
public:
    static constexpr float DistancePercentPerTick = 0.05f;
    static constexpr float YawPerPixel            = bl::math::Pi / 1920.f;
    static constexpr float PitchPerPixel          = bl::math::Pi / 1080.f;

    CameraController()
    : distance(5.f)
    , yaw(0.f)
    , pitch(bl::math::Pi / 4.f)
    , dragging(false) {}

    virtual ~CameraController() = default;

    virtual void update(float) override {
        const float cosPitch = std::cos(pitch);
        const float x        = std::cos(yaw) * distance * cosPitch;
        const float y        = std::sin(pitch) * distance;
        const float z        = std::sin(yaw) * distance * cosPitch;

        camera().setPosition({x, y, z});
        camera().getOrientationForChange().lookAt(glm::vec3(0.f), camera().getPosition());
    }

    virtual void observe(const sf::Event& event) override {
        switch (event.type) {
        case sf::Event::MouseWheelScrolled:
            distance *= 1.f - event.mouseWheelScroll.delta * DistancePercentPerTick;
            break;

        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Left) {
                dragging = true;
                prevDrag = glm::vec2(event.mouseButton.x, event.mouseButton.y);
            }
            break;

        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Left) { dragging = false; }
            break;

        case sf::Event::MouseMoved:
            if (dragging) {
                const glm::vec2 pos(event.mouseMove.x, event.mouseMove.y);
                const glm::vec2 diff = pos - prevDrag;
                prevDrag             = pos;

                yaw += diff.x * YawPerPixel;
                pitch += diff.y * PitchPerPixel;
            }
            break;
        }
    }

private:
    float distance;
    float yaw;
    float pitch;

    bool dragging;
    glm::vec2 prevDrag;
};

class DemoState : public bl::engine::State {
public:
    DemoState()
    : State(bl::engine::StateMask::All)
    , scene(nullptr) {}

    virtual ~DemoState() = default;

private:
    CameraController* controller;
    bl::rc::scene::Scene3D* scene;
    bl::gfx::Skybox skybox;

    bl::gfx::VertexBuffer3D floor;
    bl::gfx::Cube cube1;
    bl::gfx::Cube cube2;
    bl::gfx::Cube cube3;
    bl::gfx::Cube cube4;
    bl::gfx::Cube cube5;
    bl::gfx::Cube cube6;
    bl::gfx::Cube cube7;

    bl::gfx::BasicModel model1;
    bl::gfx::BasicModel model2;

    bl::gfx::Sphere light1;
    bl::gfx::Sphere light2;
    bl::gfx::Cone light3;

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        // create scene and camera
        auto world =
            engine.getPlayer().enterWorld<bl::engine::BasicWorld<bl::rc::scene::Scene3D>>();
        scene      = static_cast<bl::rc::scene::Scene3D*>(world->scene().get());
        auto& o    = engine.renderer().getObserver();
        auto* cam  = o.setCamera<bl::cam::Camera3D>(glm::vec3(0.f, 3.f, 5.f), glm::vec3(0.f));
        controller = cam->setController<CameraController>();

        auto woodTexture = engine.renderer().texturePool().getOrLoadTexture(
            "Resources/Textures/wood.png", bl::rc::vk::TextureFormat::SRGBA32Bit);
        auto woodMaterial = engine.renderer().materialPool().getOrCreateFromTexture(woodTexture);

        constexpr float FloorSize = 5.f;
        floor.create(*world, 6);
        floor[0].pos      = {-FloorSize, 0.f, -FloorSize};
        floor[0].texCoord = {0.f, 0.f};
        floor[2].pos      = {FloorSize, 0.f, FloorSize};
        floor[2].texCoord = {1.f, 1.f};
        floor[1].pos      = {-FloorSize, 0.f, FloorSize};
        floor[1].texCoord = {0.f, 1.f};
        floor[3].pos      = {-FloorSize, 0.f, -FloorSize};
        floor[3].texCoord = {0.f, 0.f};
        floor[5].pos      = {FloorSize, 0.f, -FloorSize};
        floor[5].texCoord = {1.f, 0.f};
        floor[4].pos      = {FloorSize, 0.f, FloorSize};
        floor[4].texCoord = {1.f, 1.f};
        floor.material().setMaterial(woodMaterial);
        floor.material().setPipeline(bl::rc::Config::MaterialPipelineIds::Mesh3DMaterial);
        floor.commit();
        floor.addToScene(scene, bl::rc::UpdateSpeed::Static);

        skybox.create(*world,
                      "Resources/Textures/skybox/right.jpg",
                      "Resources/Textures/skybox/left.jpg",
                      "Resources/Textures/skybox/top.jpg",
                      "Resources/Textures/skybox/bottom.jpg",
                      "Resources/Textures/skybox/back.jpg",
                      "Resources/Textures/skybox/front.jpg");
        skybox.addToScene(scene);

        auto containerTexture = engine.renderer().texturePool().getOrLoadTexture(
            "Resources/Textures/container.jpg", bl::rc::vk::TextureFormat::SRGBA32Bit);
        auto material = engine.renderer().materialPool().getOrCreateFromTexture(containerTexture);

        auto brickTexture = engine.renderer().texturePool().getOrLoadTexture(
            "Resources/Textures/bricks2.jpg", bl::rc::vk::TextureFormat::SRGBA32Bit);
        auto brickNormal = engine.renderer().texturePool().getOrLoadTexture(
            "Resources/Textures/bricks2_normal.jpg");
        auto brickParallax =
            engine.renderer().texturePool().getOrLoadTexture("Resources/Textures/bricks2_disp.jpg");
        auto brickMaterial = engine.renderer().materialPool().getOrCreateFromNormalAndParallax(
            brickTexture, brickNormal, brickParallax, 0.1f);

        auto toyBoxNormal = engine.renderer().texturePool().getOrLoadTexture(
            "Resources/Textures/toy_box_normal.png");
        auto toyBoxParallax =
            engine.renderer().texturePool().getOrLoadTexture("Resources/Textures/toy_box_disp.png");
        auto toyBoxMaterial = engine.renderer().materialPool().getOrCreateFromNormalAndParallax(
            woodTexture, toyBoxNormal, toyBoxParallax, 0.1f);

        cube1.create(
            *world, 1.f, brickMaterial, bl::rc::Config::MaterialPipelineIds::Mesh3DMaterial);
        cube1.getTransform().setPosition({0.f, 0.501f, 0.f});
        cube1.addToScene(scene, bl::rc::UpdateSpeed::Static);

        cube2.create(
            *world, 1.f, toyBoxMaterial, bl::rc::Config::MaterialPipelineIds::Mesh3DMaterial);
        cube2.getTransform().setPosition({-2.f, 1.501f, 1.f});
        cube2.addToScene(scene, bl::rc::UpdateSpeed::Static);

        cube3.create(*world, 1.f, material, bl::rc::Config::MaterialPipelineIds::Mesh3DMaterial);
        cube3.getTransform().setPosition({2.f, 1.501f, 2.f});
        cube3.addToScene(scene, bl::rc::UpdateSpeed::Static);

        cube4.create(*world, 1.f, material, bl::rc::Config::MaterialPipelineIds::Mesh3DMaterial);
        cube4.getTransform().setPosition({-2.f, 4.501f, -1.f});
        cube4.addToScene(scene, bl::rc::UpdateSpeed::Static);

        auto diffuse = engine.renderer().texturePool().getOrLoadTexture(
            "Resources/Textures/container2.png", bl::rc::vk::TextureFormat::SRGBA32Bit);
        auto specular = engine.renderer().texturePool().getOrLoadTexture(
            "Resources/Textures/container2_specular.png");
        auto material2 =
            engine.renderer().materialPool().getOrCreateFromDiffuseAndSpecular(diffuse, specular);

        cube5.create(*world, 1.f, material2, bl::rc::Config::MaterialPipelineIds::Mesh3DMaterial);
        cube5.getTransform().setPosition({2.f, 1.501f, -2.f});
        cube5.addToScene(scene, bl::rc::UpdateSpeed::Static);

        cube6.create(*world, 1.f, material2, bl::rc::Config::MaterialPipelineIds::Mesh3DMaterial);
        cube6.getTransform().setPosition({-2.f, 0.75f, -2.f});
        cube6.addToScene(scene, bl::rc::UpdateSpeed::Static);

        auto diffuse3 = engine.renderer().texturePool().getOrLoadTexture(
            "Resources/Textures/brickwall.jpg", bl::rc::vk::TextureFormat::SRGBA32Bit);
        auto normal3 = engine.renderer().texturePool().getOrLoadTexture(
            "Resources/Textures/brickwall_normal.jpg");
        auto material3 = engine.renderer().materialPool().create(
            diffuse3,
            diffuse3,
            normal3,
            engine.renderer().materialPool().getDefaultParallaxMap(),
            0.f,
            0.5f);

        cube7.create(*world, 1.f, material3, bl::rc::Config::MaterialPipelineIds::Mesh3DMaterial);
        cube7.getTransform().setPosition({2.f, 1.75f, 0.f});
        cube7.addToScene(scene, bl::rc::UpdateSpeed::Static);

        auto model = bl::resource::ResourceManager<bl::mdl::Model>::load(
            "Resources/Models/backpack/backpack.obj");
        model1.create(*world, model);
        model1.getTransform().setPosition({-1.f, 3.1f, 0.5f});
        model1.getTransform().setScale({0.25f, 0.25f, 0.25f});
        model1.addToScene(scene, bl::rc::UpdateSpeed::Static);

        model2.create(*world, "Resources/Models/Spider/Spider.obj");
        model2.getTransform().setPosition({1.f, 0.1f, 3.5f});
        model2.getTransform().setScale({0.015f, 0.015f, 0.015f});
        model2.addToScene(scene, bl::rc::UpdateSpeed::Static);

        const bl::rc::Color light1Color(sf::Color(80, 180, 255));
        light1.create(*world, 0.2f, 4, {}, bl::rc::Config::MaterialPipelineIds::Mesh3D);
        light1.getTransform().setPosition({1.f, 0.7f, 1.f});
        light1.setColor(light1Color);
        light1.addToScene(scene, bl::rc::UpdateSpeed::Static);
        auto light1Handle        = scene->getLighting().createPointLight();
        light1Handle.get().color = light1Color;
        light1Handle.get().pos   = light1.getTransform().getPosition();

        const bl::rc::Color light2Color(sf::Color::White);
        light2.create(*world, 0.2f, 4, {}, bl::rc::Config::MaterialPipelineIds::Mesh3D);
        light2.getTransform().setPosition({-0.7f, 1.f, -1.6f});
        light2.setColor(light2Color);
        light2.addToScene(scene, bl::rc::UpdateSpeed::Static);
        auto light2Handle        = scene->getLighting().createPointLightWithShadow();
        light2Handle.get().color = light2Color;
        light2Handle.get().pos   = light2.getTransform().getPosition();

        constexpr glm::vec3 light3PointAt = {0.f, 0.5f, .75f};
        const bl::rc::Color light3Color(sf::Color(255, 100, 50));
        light3.create(*world, 0.4f, 0.12f, 4, {}, bl::rc::Config::MaterialPipelineIds::Mesh3D);
        light3.getTransform().setPosition({0.5f, 1.5f, 3.f});
        light3.getTransform().lookAt(light3PointAt);
        light3.setColor(light3Color);
        light3.addToScene(scene, bl::rc::UpdateSpeed::Static);
        auto light3Handle             = scene->getLighting().createSpotlightWithShadow();
        light3Handle.get().getColor() = light3Color;
        light3Handle.get().getAttenuation().constant  = 0.5f;
        light3Handle.get().getAttenuation().linear    = 0.02f;
        light3Handle.get().getAttenuation().quadratic = 0.02f;
        light3Handle.get().setPosition(light3.getTransform().getPosition() +
                                       light3.getTransform().getForwardDir() * 0.2f);
        light3Handle.get().getAttenuation().linear = light3Handle.get().getAttenuation().quadratic =
            0.05f;
        light3Handle.get().pointAt(light3PointAt);

        bl::event::Dispatcher::subscribe(controller);
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        bl::event::Dispatcher::unsubscribe(controller);
        engine.getPlayer().leaveWorld();
    }

    virtual void update(bl::engine::Engine&, float, float) override {
        // noop
    }
};

int main() {
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Debug);
    const bl::engine::Settings engineSettings = bl::engine::Settings().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(1920, 1080, 32))
            .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
            .withTitle("3D Graphics Demo")
            .withLetterBoxOnResize(false));
    bl::engine::Engine engine(engineSettings);

    engine.run(std::make_shared<DemoState>());

    return 0;
}
