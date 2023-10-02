#ifndef BLIB_SYSTEMS_BUILTINDRAWABLESYSTEMS_HPP
#define BLIB_SYSTEMS_BUILTINDRAWABLESYSTEMS_HPP

#include <BLIB/Components/Animation2D.hpp>
#include <BLIB/Components/Mesh.hpp>
#include <BLIB/Components/Slideshow.hpp>
#include <BLIB/Components/Sprite.hpp>
#include <BLIB/Components/Text.hpp>
#include <BLIB/Systems/DrawableSystem.hpp>

namespace bl
{
namespace sys
{
/**
 * @brief Engine system for managing entity meshes
 *
 * @ingroup Systems
 */
using MeshSystem = DrawableSystem<com::Mesh>;

/**
 * @brief Engine system for managing entity sprites
 *
 * @ingroup Systems
 */
using SpriteSystem = DrawableSystem<com::Sprite>;

/**
 * @brief Engine system for managing text entities
 *
 * @ingroup Renderer
 */
using TextSystem = DrawableSystem<com::Text>;

/**
 * @brief Engine system for managing slideshow animations
 *
 * @ingroup Renderer
 */
using SlideshowSystem = DrawableSystem<com::Slideshow>;

/**
 * @brief Engine system for managing non-slideshow animations
 *
 * @ingroup Renderer
 */
using Animation2DDrawableSystem = DrawableSystem<com::Animation2D>;

} // namespace sys
} // namespace bl

#endif
