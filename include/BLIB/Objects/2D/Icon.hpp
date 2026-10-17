#ifndef BLIB_OBJECTS_2D_ICON_HPP
#define BLIB_OBJECTS_2D_ICON_HPP

#include <BLIB/Objects/2D/Shapes/IconBase.hpp>
#include <BLIB/Objects/2D/Shapes/SingleShape.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
/**
 * @brief A renderable icon comprised of vertices
 *
 * @ingroup Objects
 */
class Icon : public s2d::IconBase<s2d::SingleShape> {
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
     * @param world The world to create the object in
     */
    void create(engine::World& world);
};

} // namespace o2d
} // namespace obj
} // namespace bl

#endif
