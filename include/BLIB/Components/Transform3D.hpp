#ifndef BLIB_COMPONENTS_TRANSFORM3D_HPP
#define BLIB_COMPONENTS_TRANSFORM3D_HPP

#include <BLIB/Render/Components/DescriptorComponentBase.hpp>
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
     * @brief Returns the rotation quaternion of the transform
     */
    const glm::quat& getRotation() const;

    /**
     * @brief Sets the rotation of the transform to the given Euler angles
     *
     * @param eulerAngles The Euler angles to rotate to
     */
    void setRotationEulerAngles(const glm::vec3& eulerAngles);

    /**
     * @brief Rotates the transform about a given axis
     *
     * @param axis The axis to rotate about
     * @param angle The angle to rotate in degrees
     */
    void rotate(const glm::vec3& axis, float angle);

    /**
     * @brief Orients the transform to face the given position
     *
     * @param pos The position to look at
     * @param up The up vector
     */
    void lookAt(const glm::vec3& pos, const glm::vec3& up = {0.f, 1.f, 0.f});

    /**
     * @brief Returns the forward direction unit vector local to this transform
     */
    glm::vec3 getForwardDir() const;

    /**
     * @brief Returns the right direction unit vector local to this transform
     */
    glm::vec3 getRightDir() const;

    /**
     * @brief Returns the up direction unit vector local to this transform
     */
    glm::vec3 getUpDir() const;

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
     * @brief Returns the global transform matrix of this transform, excluding the topmost parent
     */
    glm::mat4 getGlobalTransformExcludingRoot() const;

    /**
     * @brief Transforms the given point by this transform
     *
     * @param src The point to transform
     * @return The transformed point
     */
    glm::vec3 transformPoint(const glm::vec3& src) const;

private:
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scaleFactors;

    void makeDirty();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline const glm::vec3& Transform3D::getPosition() const { return position; }

inline const glm::quat& Transform3D::getRotation() const { return rotation; }

inline const glm::vec3& Transform3D::getScale() const { return scaleFactors; }

} // namespace com
} // namespace bl

#endif
