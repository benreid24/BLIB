#ifndef BLIB_GRAPHICS_BATCHICON_HPP
#define BLIB_GRAPHICS_BATCHICON_HPP

#include <BLIB/Graphics/Shapes2D/BatchShape2D.hpp>
#include <BLIB/Graphics/Shapes2D/IconBase.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief A renderable icon comprised of vertices. Can be batched with other shapes
 *
 * @ingroup Graphics
 */
class BatchIcon : public s2d::IconBase<s2d::BatchShape2D> {
public:
    /// The various types of icons
    using Type = IconBase::Type;

    /**
     * @brief Creates an icon of the given type
     *
     * @param type The type of icon to create
     * @param size The size of the icon
     */
    BatchIcon(Type type, const glm::vec2& size = {16.f, 16.f});

    /**
     * @brief Creates the icon entity and renderer resources
     *
     * @param engine The game engine instance
     * @param owner The set of batched shapes to be a part of
     */
    void create(engine::Engine& engine, BatchedShapes2D& owner);
};

} // namespace gfx
} // namespace bl

#endif
