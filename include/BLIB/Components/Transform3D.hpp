#ifndef BLIB_COMPONENTS_TRANSFORM3D_HPP
#define BLIB_COMPONENTS_TRANSFORM3D_HPP

#include <BLIB/Components/Orientation3D.hpp>
#include <BLIB/ECS/Traits/ParentAwareVersioned.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Render/Components/DescriptorComponentBase.hpp>
#include <BLIB/Render/Config/Constants.hpp>
#include <BLIB/Render/Descriptors/Builtin/Transform3DPayload.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Representation of a full 3d transform. Includes position, orientation, and scale
 *
 * @ingroup Components
 */
class Transform3D
: public rc::rcom::DescriptorComponentBase<Transform3D, rc::dsi::Transform3DPayload>
, public ecs::trait::ParentAwareVersioned<Transform3D> {
public:
    /**
     * @brief Creates a new transform with no scaling and sane defaults
     */
    Transform3D();

    /**
     * @brief Copies the transform from another
     *
     * @param copy The transform to copy
     * @return A reference to this transform
     */
    Transform3D& operator=(const Transform3D& copy);

    /**
     * @brief Sets the position of the transform
     *
     * @param pos The position in 3d space
     */
    void setPosition(const glm::vec3& pos);

    /**
     * @brief Translates the transform
     *
     * @param offset Amount to change the position by
     */
    void move(const glm::vec3& offset);

    /**
     * @brief Returns the position of the transform
     */
    const glm::vec3& getPosition() const;

    /**
     * @brief Computes and returns the global position of the transform
     */
    glm::vec3 getGlobalPosition() const;

    /**
     * @brief Returns the rotation quaternion of the transform
     */
    const glm::quat& getRotation() const;

    /**
     * @brief Computes and returns the global rotation of the transform
     */
    glm::quat getGlobalRotation() const;

    /**
     * @brief Returns the forward direction unit vector in world space
     */
    glm::vec3 getGlobalForwardDir() const;

    /**
     * @brief Returns the right direction unit vector in world space
     */
    glm::vec3 getGlobalRightDir() const;

    /**
     * @brief Returns the up direction unit vector in world space
     */
    glm::vec3 getGlobalUpDir() const;

    /**
     * @brief Sets the scale of the transform
     *
     * @param scale The scale factors to set
     */
    void setScale(const glm::vec3& scale);

    /**
     * @brief Multiplies the current scale of the transform by the given factors
     *
     * @param factors The factors to multiply the current scale with
     */
    void scale(const glm::vec3& factors);

    /**
     * @brief Returns the current scale factors of the transform
     */
    const glm::vec3& getScale() const;

    /**
     * @brief Returns the local orientation of the transform and marks the transform as needing to
     *        refresh the transform matrix. Use this method to when modifying the orientation
     */
    Orientation3D& getOrientationForChange();

    /**
     * @brief Returns the local orientation of the transform
     */
    const Orientation3D& getOrientation() const { return orientation; }

    /**
     * @brief Sets the transform matrix directly
     *
     * @param transform The transform matrix to assign
     */
    void setTransform(const glm::mat4& transform);

    /**
     * @brief Computes the transform and populates the given transform matrix
     *
     * @param dest The payload to populate
     */
    virtual void refreshDescriptor(rc::dsi::Transform3DPayload& dest) override;

    /**
     * @brief Returns the local transform matrix of this transform
     */
    glm::mat4 getLocalTransform() const;

    /**
     * @brief Returns the global transform matrix of this transform
     */
    glm::mat4 getGlobalTransform() const;

    /**
     * @brief Transforms the given point by this transform
     *
     * @param src The point to transform
     * @return The transformed point
     */
    glm::vec3 transformPoint(const glm::vec3& src) const;

    /**
     * @brief Returns whether the transform has been marked dirty and has not been refreshed yet
     */
    bool isDirty() const;

    /**
     * @brief Manually set the parent transform. Called by the ECS but can be used manually
     *
     * @param parent The parent transform to set
     */
    void setParent(Transform3D& parent);

private:
    glm::vec3 position;
    Orientation3D orientation;
    glm::vec3 scaleFactors;

    void makeDirty();

    friend struct refl::ReflectedObject<Transform3D>;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline const glm::vec3& Transform3D::getPosition() const { return position; }

inline const glm::quat& Transform3D::getRotation() const {
    return orientation.getRotationQuaternion();
}

inline const glm::vec3& Transform3D::getScale() const { return scaleFactors; }

} // namespace com
namespace refl
{
template<>
struct ReflectedObject<com::Transform3D> {
    inline static const auto spec = makeSpec<com::Transform3D>(
        "Transform3D", memberList(defineMember(1, "position", &com::Transform3D::position),
                                  defineMember(2, "orientation", &com::Transform3D::orientation),
                                  defineMember(3, "scale", &com::Transform3D::scaleFactors)));
};

} // namespace refl
} // namespace bl

#endif
