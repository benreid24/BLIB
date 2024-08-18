#ifndef BLIB_RENDER_COMPONENTS_DRAWABLEBASE_HPP
#define BLIB_RENDER_COMPONENTS_DRAWABLEBASE_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Components/SceneObjectRef.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <BLIB/Render/UpdateSpeed.hpp>
#include <cstdint>
#include <limits>

namespace bl
{
namespace com
{
class Rendered;
}
namespace ecs
{
class Registry;
}

namespace rc
{
class Scene;
namespace scene
{
class SceneSync;
}

namespace rcom
{
/**
 * @brief Base class for drawable scene components for the ECS. Provides common members
 *
 * @ingroup Renderer
 */
struct DrawableBase {
    static constexpr std::uint32_t PipelineNotSet = std::numeric_limits<std::uint32_t>::max();

    /**
     * @brief Sets reasonable defaults
     */
    DrawableBase();

    /**
     * @brief Performs some cleanup
     */
    ~DrawableBase();

    /**
     * @brief Sets the pipeline to render with. Re-buckets if already in a scene
     *
     * @param pipeline The new pipeline to render with
     */
    void setPipeline(std::uint32_t pipeline);

    /**
     * @brief Adds this component to the given scene
     *
     * @param ecs The engine ECS registry instance
     * @param entity The id of the entity that this component belongs to
     * @param scene The scene to add to
     * @param updateSpeed The expected frequency of changes to descriptors for this entity
     */
    void addToScene(ecs::Registry& ecs, ecs::Entity entity, Scene* scene, UpdateSpeed updateSpeed);

    /**
     * @brief Adds this component to the given scene with a custom pipeline
     *
     * @param ecs The engine ECS registry instance
     * @param entity The id of the entity that this component belongs to
     * @param scene The scene to add to
     * @param updateSpeed The expected frequency of changes to descriptors for this entity
     * @param pipeline The pipeline id to render with
     */
    void addToSceneWithPipeline(ecs::Registry& ecs, ecs::Entity entity, Scene* scene,
                                UpdateSpeed updateSpeed, std::uint32_t pipeline);

    /**
     * @brief Removes this component from its current scene
     *
     * @param ecs The engine ECS registry instance
     * @param entity The id of the entity that this component belongs to
     */
    void removeFromScene(ecs::Registry& ecs, ecs::Entity entity);

    /**
     * @brief Helper method to sync this component's draw parameters to the scene it is in
     */
    void syncDrawParamsToScene();

    /**
     * @brief Sets whether or not the object should be rendered
     *
     * @param hide True to skip rendering, false to render
     */
    void setHidden(bool hide);

    /**
     * @brief Returns whether or not the entity is hidden
     */
    bool isHidden() const { return hidden; }

    /**
     * @brief Returns the current scene object reference for this component
     */
    const SceneObjectRef& getSceneRef() const { return sceneRef; }

    /**
     * @brief Returns the current pipeline used to render this component
     */
    std::uint32_t getCurrentPipeline() const { return pipeline; }

    /**
     * @brief Returns whether the rendered component has transparency
     */
    bool getContainsTransparency() const { return containsTransparency; }

    /**
     * @brief Set whether the rendered component has transparency
     *
     * @param transparent True if there is transparency, false if opaque
     */
    void setContainsTransparency(bool transparent);

    /**
     * @brief Returns the current draw parameters for this component
     */
    const prim::DrawParameters& getDrawParameters() const { return drawParams; }

    /**
     * @brief Returns the current draw parameters for this component for editing
     */
    prim::DrawParameters& getDrawParametersForEdit() { return drawParams; }

    /**
     * @brief Triggers the owning scene to re-bucket this object. Call when transparency changes
     */
    void rebucket();

    /**
     * @brief Derived components should return the default pipeline to use in regular scenes
     */
    virtual std::uint32_t getDefaultScenePipelineId() const = 0;

    /**
     * @brief Derived components should return the default pipeline to use in regular scenes
     */
    virtual std::uint32_t getDefaultOverlayPipelineId() const = 0;

protected:
    std::uint32_t pipeline;
    prim::DrawParameters drawParams;
    bool containsTransparency;

private:
    com::Rendered* renderComponent;
    SceneObjectRef sceneRef;
    bool hidden;

    friend class rc::Scene;
    friend class rc::scene::SceneSync;
};

} // namespace rcom
} // namespace rc
} // namespace bl

#endif
