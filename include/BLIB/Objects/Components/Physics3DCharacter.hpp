#ifndef BLIB_OBJECTS_COMPONENTS_PHYSICS3DCHARACTER_HPP
#define BLIB_OBJECTS_COMPONENTS_PHYSICS3DCHARACTER_HPP

#include <BLIB/Components/Physics3DCharacter.hpp>
#include <BLIB/ECS/EntityBacked.hpp>
#include <BLIB/Objects/Components/Shape3D.hpp>

namespace bl
{
namespace obj
{
namespace bcom
{
/**
 * @brief Base class for objects that may be controllable characters in the 3d physics simulation
 *
 * @ingroup Objects
 */
class Physics3DCharacter
: protected virtual ecs::EntityBacked
, protected virtual Shape3D {
public:
    /**
     * @brief Does nothing
     */
    Physics3DCharacter();

    /**
     * @brief Does nothing
     */
    virtual ~Physics3DCharacter() = default;

    /**
     * @brief Creates a 3d physics character for this object. Mutually exclusive with Physics3D
     *
     * @param settings The character settings to create with
     * @param addToSimulation True to immediately add to the simulation, false to manually add later
     * @param material The physics material to use for the character
     */
    void createPhysicsCharacter(const JPH::CharacterSettings& settings, bool addToSimulation = true,
                                JPH::Ref<JPH::PhysicsMaterial> material = {});

    /**
     * @brief Creates a 3d physics character for this object. Mutually exclusive with Physics3D
     *
     * @param settings The character settings to create with
     * @param shape The shape to use for the character
     * @param addToSimulation True to immediately add to the simulation, false to manually add later
     */
    void createPhysicsCharacter(const JPH::CharacterSettings& settings, JPH::Ref<JPH::Shape> shape,
                                bool addToSimulation = true);

    /**
     * @brief Returns whether this object has a physics character
     */
    bool hasPhysicsCharacter() const;

    /**
     * @brief Returns a handle to the physics character of this object
     */
    com::Physics3DCharacter* getPhysicsCharacter();

private:
    com::Physics3DCharacter* character;
};

} // namespace bcom
} // namespace obj
} // namespace bl

#endif
