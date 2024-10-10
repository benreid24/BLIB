#ifndef BLIB_PARTICLES_RENDERER_HPP
#define BLIB_PARTICLES_RENDERER_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Particles/Link.hpp>
#include <BLIB/Particles/RenderComponentMap.hpp>
#include <BLIB/Particles/RenderConfigMap.hpp>
#include <BLIB/Render/Scenes/CodeScene.hpp>
#include <cstddef>

namespace bl
{
namespace engine
{
class World;
class Engine;
} // namespace engine

namespace pcl
{
/**
 * @brief Default renderer implementation for particle systems. If you use the default all you need
 *        to do is specialize RenderConfig for your particle type to specify the pipeline and
 *        transparency settings to render with.
 *
 * @tparam T The type of particle to render
 * @ingroup Particles
 */
template<typename T>
class Renderer {
public:
    using TComponent                           = typename RenderComponentMap<T>::TComponent;
    static constexpr std::uint32_t PipelineId  = RenderConfigMap<T>::PipelineId;
    static constexpr bool ContainsTransparency = RenderConfigMap<T>::ContainsTransparency;
    using TGlobalsPayload                      = typename RenderConfigMap<T>::GlobalShaderPayload;

    static_assert(PipelineId != priv::PipelineNotSet,
                  "Specialize RenderConfigMap and specify pipeline id and transparency");

    /**
     * @brief Creates the default renderer
     */
    Renderer();

    /**
     * @brief Called once after being constructed
     *
     * @param world The world to create the particle system entity in
     */
    void init(engine::World& world);

    /**
     * @brief Called when the particle manager should be rendered in the given scene
     *
     * @param scene The scene to add to
     */
    void addToScene(rc::Scene* scene);

    /**
     * @brief Called when the particles should be removed from its current scene
     */
    void removeFromScene();

    /**
     * @brief Called when the particle data changes
     *
     * @param particles The current beginning particle
     * @param length The number of particles
     */
    void notifyData(T* particles, std::size_t length);

    /**
     * @brief Returns the global shader payload being sent to the shaders
     */
    TGlobalsPayload& getGlobals() { return globals; }

    /**
     * @brief Access the ECS entity being used to render the particle system
     */
    ecs::Entity getEntity() const { return entity; }

    /**
     * @brief Returns the renderable component. Only valid after addToScene is called
     */
    TComponent* getComponent();

    /**
     * @brief Returns the link component. Only valid after addToScene is called
     */
    Link<T>* getLink();

    /**
     * @brief Manually issue the required commands to draw this object. Must be called within the
     *        context of a CodeScene
     *
     * @param ctx The CodeScene rendering context
     */
    void draw(rc::scene::CodeScene::RenderContext& ctx);

private:
    engine::World* world;
    void* system;
    ecs::Entity entity;
    TComponent* component;
    Link<T>* link;
    TGlobalsPayload globals;
};

} // namespace pcl
} // namespace bl

#endif
