#ifndef BLIB_RENDER_COMPONENTS_DRAWABLEBASE_HPP
#define BLIB_RENDER_COMPONENTS_DRAWABLEBASE_HPP

#include <BLIB/Render/Components/SceneObjectRef.hpp>
#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <cstdint>
#include <limits>

namespace bl
{
namespace rc
{
namespace rcom
{
/**
 * @brief Base class for drawable scene components for the ECS. Provides common members
 *
 * @ingroup Renderer
 */
struct DrawableBase {
    static constexpr std::uint32_t PipelineNotSet = std::numeric_limits<std::uint32_t>::max();

    SceneObjectRef sceneRef;
    std::uint32_t pipeline;
    prim::DrawParameters drawParams;
    bool containsTransparency;

    /**
     * @brief Sets reasonable defaults
     */
    DrawableBase();

    /**
     * @brief Sets the pipeline to render with. Re-buckets if already in a scene
     *
     * @param pipeline The new pipeline to render with
     */
    void setPipeline(std::uint32_t pipeline);

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
     * @brief Triggers the owning scene to re-bucket this object. Call when transparency changes
     */
    void rebucket();

private:
    bool hidden;

    friend class rc::Scene;
};

} // namespace rcom
} // namespace rc
} // namespace bl

#endif
