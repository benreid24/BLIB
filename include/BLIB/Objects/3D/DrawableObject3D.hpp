#ifndef BLIB_OBJECTS_3D_DRAWABLEOBJECT3D_HPP
#define BLIB_OBJECTS_3D_DRAWABLEOBJECT3D_HPP

#include <BLIB/Objects/Components/Outline3D.hpp>
#include <BLIB/Objects/Components/Transform3D.hpp>
#include <BLIB/Objects/DrawableObject.hpp>

namespace bl
{
namespace obj
{
namespace o3d
{
/**
 * @brief Base class for 3d drawable objects
 *
 * @tparam TCom The drawable component type
 * @ingroup Objects
 */
template<typename TCom>
class DrawableObject3D
: public DrawableObject<TCom>
, public bcom::Transform3D
, public bcom::Outline3D {
public:
    /**
     * @brief Destroys the object
     */
    virtual ~DrawableObject3D() = default;

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
        Transform3D::create(world.engine().ecs(), this->entity());
        Outline3D::init(world.engine().ecs(), this->entity(), &this->component());
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
        Transform3D::create(world.engine().ecs(), this->entity());
        Outline3D::init(world.engine().ecs(), this->entity(), &this->component());
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
        Transform3D::create(world.engine().ecs(), this->entity());
        Outline3D::init(world.engine().ecs(), this->entity(), &this->component());
    }
};

} // namespace o3d
} // namespace obj
} // namespace bl

#endif
