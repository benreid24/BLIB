#ifndef BLIB_RENDER_DRAWABLES_COMPONENTS_TRANSFORMABLE_HPP
#define BLIB_RENDER_DRAWABLES_COMPONENTS_TRANSFORMABLE_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Transforms/2D/Transform2D.hpp>

namespace bl
{
namespace render
{
namespace draw
{
/// Base component classes for Drawable objects
namespace com
{
/**
 * @brief Base component class for Drawables that require a 2d transform
 *
 * @ingroup Renderer
 */
class Transformable {
public:
    /**
     * @brief Does nothing
     */
    Transformable() = default;

    /**
     * @brief Returns the transform. Must only be called after create()
     */
    t2d::Transform2D& getTransform();

    /**
     * @brief Returns the transform. Must only be called after create()
     */
    const t2d::Transform2D& getTransform() const;

protected:
    /**
     * @brief Creates the transform in the ECS
     *
     * @tparam ...TArgs Argument types to the transform constructor
     * @param registry The ECS registry instance
     * @param entity The entity id
     * @param ...args Arguments to the transform constructor
     */
    template<typename... TArgs>
    void create(ecs::Registry& registry, ecs::Entity entity, TArgs&&... args);

private:
    ecs::StableHandle<t2d::Transform2D> handle;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline t2d::Transform2D& Transformable::getTransform() { return handle.get(); }

inline const t2d::Transform2D& Transformable::getTransform() const { return handle.get(); }

template<typename... TArgs>
inline void Transformable::create(ecs::Registry& registry, ecs::Entity entity, TArgs&&... args) {
    registry.emplaceComponent<t2d::Transform2D>(entity, std::forward<TArgs>(args)...);
    handle.assign(registry, entity);
}

} // namespace com
} // namespace draw
} // namespace render
} // namespace bl

#endif
