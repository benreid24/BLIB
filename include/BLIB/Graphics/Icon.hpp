#ifndef BLIB_GRAPHICS_ICON_HPP
#define BLIB_GRAPHICS_ICON_HPP

#include <BLIB/Graphics/Shapes2D/IconBase.hpp>
#include <BLIB/Graphics/Shapes2D/SingleShape2D.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief A renderable icon comprised of vertices
 *
 * @ingroup Graphics
 */
class Icon : public s2d::IconBase<s2d::SingleShape2D> {
public:
    /// The various types of icons
    using Type = IconBase::Type;

    /**
     * @brief Creates an icon of the given type
     *
     * @param type The type of icon to create
     * @param size The size of the icon
     */
    Icon(Type type, const glm::vec2& size = {16.f, 16.f});

    /**
     * @brief Creates the icon entity and renderer resources
     *
     * @param engine The game engine instance
     */
    void create(engine::Engine& engine);
};

} // namespace gfx
} // namespace bl

#endif
