#ifndef BLIB_RENDER_COMPONENTS_DRAWABLEBASE_HPP
#define BLIB_RENDER_COMPONENTS_DRAWABLEBASE_HPP

#include <BLIB/Render/Components/SceneObjectRef.hpp>
#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <cstdint>

namespace bl
{
namespace render
{
namespace com
{
/**
 * @brief Base class for drawable scene components for the ECS. Provides common members
 *
 * @ingroup Renderer
 */
struct DrawableBase {
    SceneObjectRef sceneRef;
    prim::DrawParameters drawParams;

    /**
     * @brief Helper method to sync this component's draw parameters to the scene it is in
     */
    void syncToScene() { sceneRef.object->drawParams = drawParams; }
};

} // namespace com
} // namespace render
} // namespace bl

#endif
