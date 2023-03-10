#ifndef BLIB_RENDER_RENDERER_STAGEPIPELINES_HPP
#define BLIB_RENDER_RENDERER_STAGEPIPELINES_HPP

#include <BLIB/Render/Config.hpp>
#include <array>
#include <cstdint>

namespace bl
{
namespace render
{
namespace scene
{
/**
 * @brief Typedef that represents pipelines used to render an object in each Scene stage it belongs
 *        to. Stages with Config::SceneObjectStage::None are not used for the object
 *
 * @ingroup Renderer
 */
using StagePipelines = std::array<std::uint32_t, Config::SceneObjectStage::Count>;

} // namespace scene
} // namespace render
} // namespace bl

#endif
