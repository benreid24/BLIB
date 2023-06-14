#ifndef BLIB_RENDER_DRAWABLES_COMPONENTS_TRANSFORM2D_HPP
#define BLIB_RENDER_DRAWABLES_COMPONENTS_TRANSFORM2D_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Transforms/2D/Transform2D.hpp>

namespace bl
{
namespace render
{
namespace draw
{
/// Base component classes for Drawable objects
namespace base
{
/**
 * @brief Base component class for Drawables that require a 2d transform
 *
 * @ingroup Renderer
 */
class Transform2D {
public:
    /**
     * @brief Does nothing
     */
    Transform2D() = default;

    /**
     * @brief Returns the transform. Must only be called after create()
     */
    t2d::Transform2D& getTransform();

    /**
     * @brief Returns the transform. Must only be called after create()
     */
    const t2d::Transform2D& getTransform() const;

    /**
     * @brief Scales the transform to take up a percentage of the given overlay width. Maintains
     *        aspect ratio
     *
     * @param ratio Percentage of overlay width to take up, in range [0, 1]
     * @param overlayWidth Width of the overlay
     */
    void scaleWidthToOverlay(float ratio, float overlayWidth = 1.f);

    /**
     * @brief Scales the transform to take up a percentage of the given overlay height. Maintains
     *        aspect ratio
     *
     * @param ratio Percentage of overlay height to take up, in range [0, 1]
     * @param overlayHeight Height of the overlay
     */
    void scaleHeightToOverlay(float ratio, float overlayHeigt = 1.f);

    /**
     * @brief Scales to take up a percentage of overlay width and height. Does not maintain AR
     *
     * @param ratios Width and height percentages in ranges [0, 1]
     * @param overlaySize Width and height of the overlay
     */
    void scaleToOverlay(const glm::vec2& ratios, const glm::vec2& overlaySize = {1.f, 1.f});

    /**
     * @brief Returns the pre-transform size of this entity
     */
    constexpr const glm::vec2& getLocalSize() const;

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

    /**
     * @brief Sets the pre-transform size of this entity. Used for overlay scale helpers
     *
     * @param size The pre-transform size to use
     */
    void setLocalSize(const glm::vec2& size);

private:
    ecs::StableHandle<t2d::Transform2D> handle;
    glm::vec2 localSize;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline t2d::Transform2D& Transform2D::getTransform() { return handle.get(); }

inline const t2d::Transform2D& Transform2D::getTransform() const { return handle.get(); }

template<typename... TArgs>
inline void Transform2D::create(ecs::Registry& registry, ecs::Entity entity, TArgs&&... args) {
    registry.emplaceComponent<t2d::Transform2D>(entity, std::forward<TArgs>(args)...);
    handle.assign(registry, entity);
}

inline constexpr const glm::vec2& Transform2D::getLocalSize() const { return localSize; }

} // namespace base
} // namespace draw
} // namespace render
} // namespace bl

#endif
