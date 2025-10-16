#ifndef BLIB_RENDER_SCENES_CODESCENEOBJECT_HPP
#define BLIB_RENDER_SCENES_CODESCENEOBJECT_HPP

#include <BLIB/Render/Descriptors/InstanceTable.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <array>
#include <cstdint>

namespace bl
{
namespace rc
{
namespace scene
{
/**
 * @brief Special scene object for code driven scenes
 *
 * @ingroup Renderer
 */
struct CodeSceneObject : public SceneObject {
    /**
     * @brief Creates the code scene object
     */
    CodeSceneObject() = default;

    mat::MaterialPipeline* pipeline;
    ds::InstanceTable descriptors;
};
} // namespace scene
} // namespace rc
} // namespace bl

#endif
