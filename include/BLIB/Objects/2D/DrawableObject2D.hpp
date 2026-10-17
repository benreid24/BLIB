#ifndef BLIB_OBJECTS_DRAWABLEOBJECT2D_HPP
#define BLIB_OBJECTS_DRAWABLEOBJECT2D_HPP

#include <BLIB/Objects/Components/OverlayScalable.hpp>
#include <BLIB/Objects/DrawableObject.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
/**
 * @brief Base class for 2d drawable objects
 *
 * @tparam TCom The drawable component type
 * @ingroup Objects
 */
template<typename TCom>
class DrawableObject2D
: public DrawableObject<TCom>
, public bcom::OverlayScalable {
public:
    /**
     * @brief Destroys the object
     */
    virtual ~DrawableObject2D() = default;

    /**
     * @brief Helper method to set the scale so that the entity is a certain size
     *
     * @param size The size to scale to
     */
    virtual void scaleToSize(const glm::vec2& size) = 0;

    /**
     * @brief Does nothing. Should be overriden by any objects that intend to use overlay scaling
     */
    virtual void ensureLocalSizeUpdated() override {}

protected:
    /**
     * @brief Creates the ECS entity and drawable component. Will destroy the prior instance if any
     *
     * @tparam ...TArgs Argument types to the component's constructor
     * @param world The world to create the object in
     * @param ...args Arguments to the component's constructor
     */
    template<typename... TArgs>
    void create(engine::World& world, TArgs&&... args) {
        DrawableObject<TCom>::create(world, std::forward<TArgs>(args)...);
        bcom::OverlayScalable::create(world.engine(), this->entity());
    }

    /**
     * @brief Creates the ECS entity and drawable component using specific material parameters. Will
     *        destroy the prior instance if any
     *
     * @tparam ...TArgs Argument types to the component's constructor
     * @param world The world to create the object in
     * @param materialPipelineId The material pipeline id to render with
     * @param ...args Arguments to the component's constructor
     */
    template<typename... TArgs>
    void createWithMaterial(engine::World& world, std::uint32_t materialPipelineId,
                            TArgs&&... args) {
        DrawableObject<TCom>::createWithMaterial(
            world, materialPipelineId, std::forward<TArgs>(args)...);
        OverlayScalable::create(world.engine(), this->entity());
    }

    /**
     * @brief Creates the ECS entity and drawable component using specific material parameters. Will
     *        destroy the prior instance if any
     *
     * @tparam ...TArgs Argument types to the component's constructor
     * @param world The world to create the object in
     * @param materialPipelineId The material pipeline id to render with
     * @param material The material to use
     * @param ...args Arguments to the component's constructor
     */
    template<typename... TArgs>
    void createWithMaterial(engine::World& world, std::uint32_t materialPipelineId,
                            const bl::rc::res::MaterialRef& material, TArgs&&... args) {
        DrawableObject<TCom>::createWithMaterial(
            world, materialPipelineId, material, std::forward<TArgs>(args)...);
        OverlayScalable::create(world.engine(), this->entity());
    }
};

} // namespace o2d
} // namespace obj
} // namespace bl

#endif
