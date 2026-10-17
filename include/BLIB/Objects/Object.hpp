#ifndef BLIB_OBJECTS_OBJECT_HPP
#define BLIB_OBJECTS_OBJECT_HPP

#include <BLIB/ECS/EntityBacked.hpp>

namespace bl
{
namespace obj
{
/**
 * @brief Base class for all engine objects
 *
 * @ingroup Objects
 */
class Object : public ecs::EntityBacked {
public:
    /**
     * @brief Destroys the object
     */
    virtual ~Object() = default;

    /**
     * @brief Move assigns the object
     *
     * @param move The object to move from
     * @return A reference to this object
     */
    Object& operator=(Object&& move) = default;
};

} // namespace obj
} // namespace bl

#endif
