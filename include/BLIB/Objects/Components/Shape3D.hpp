#ifndef BLIB_OBJECTS_COMPONENTS_SHAPE3D_HPP
#define BLIB_OBJECTS_COMPONENTS_SHAPE3D_HPP

namespace bl
{
namespace obj
{
namespace bcom
{
/**
 * @brief Base class for Physics3D and Physics3DCharacter. Provides the interface for the shape.
 *        Does not provide anything else
 *
 * @ingroup Objects
 */
class Shape3D {
protected:
    /**
     * @brief Must be implemented by derived classes to define their shape
     *
     * @param material The physics material to use for the shape
     * @return A unique pointer to the created shape
     */
    virtual JPH::Ref<JPH::Shape> createPhysicsShape(JPH::Ref<JPH::PhysicsMaterial> material) = 0;
};

} // namespace bcom
} // namespace obj
} // namespace bl

#endif
