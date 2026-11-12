#ifndef BLIB_COMPONENTS_ORIENTATION3D_HPP
#define BLIB_COMPONENTS_ORIENTATION3D_HPP

#include <BLIB/Reflection/ReflectedObject.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Angle-axis representation of a 3d orientation
 *
 * @ingroup Components
 */
class Orientation3D {
public:
    /**
     * @brief Creates a new orientation from the given Euler angles
     *
     * @param yaw The yaw of the orientation in degrees
     * @param pitch The pitch of the orientation in degrees
     * @param roll The roll of the orientation in degrees
     */
    Orientation3D(float yaw = 0.f, float pitch = 0.f, float roll = 0.f);

    /**
     * @brief Creates a new orientation from an axis and roll angle
     *
     * @param faceDir The axis to orient towards
     * @param roll The roll angle around the axis in degrees
     */
    Orientation3D(const glm::vec3& faceDir, float roll);

    /**
     * @brief Creates a new orientation looking at a given position from another position
     *
     * @param lookAt The position to look at
     * @param from The position to look from
     * @param roll The roll around the resulting axis
     */
    Orientation3D(const glm::vec3& lookAt, const glm::vec3& from, float roll = 0.f);

    /**
     * @brief Construct directly from the rotation quaternion
     *
     * @param rotation The quaternion representing the orientation
     */
    Orientation3D(const glm::quat& rotation);

    /**
     * @brief Orients the orientation to look at the given position from its current position
     *
     * @param lookAt The position to look at
     * @param from The position to look from
     */
    void lookAt(const glm::vec3& lookAt, const glm::vec3& from);

    /**
     * @brief Sets the direction the orientation is facing towards
     *
     * @param A unit vector to orient towards
     */
    void setFaceDirection(const glm ::vec3& faceDir);

    /**
     * @brief Sets the forward direction vector with a custom up direction as well
     *
     * @param forwardDir The forward unit vector
     * @param upDir The local up direction unit vector
     */
    void setForwardDirection(const glm::vec3& forwardDir, const glm::vec3& upDir);

    /**
     * @brief Sets the pitch angle of the orientation
     *
     * @param pitch The pitch of the orientation in degrees
     */
    void setPitch(float pitch);

    /**
     * @brief Adds the given offset to the pitch of the orientation
     *
     * @param offset Offset to apply in degrees
     */
    void applyPitch(float offset);

    /**
     * @brief Returns the pitch of the orientation in degrees
     */
    float getPitch() const { return pitch; }

    /**
     * @brief Sets the yaw angle of the orientation
     *
     * @param yaw The yaw of the orientation in degrees
     */
    void setYaw(float yaw);

    /**
     * @brief Adds the given offset to the yaw of the orientation
     *
     * @param offset Offset to apply in degrees
     */
    void applyYaw(float offset);

    /**
     * @brief Returns the yaw of the orientation in degrees
     */
    float getYaw() const { return yaw; }

    /**
     * @brief Sets both the yaw and pitch of the orientation. Call this if both are changing as it's
     * more efficient than setting the yaw and pitch individually
     *
     * @param yaw The yaw of the orientation in degrees
     * @param pitch The pitch of the orientation in degrees
     */
    void setYawAndPitch(float yaw, float pitch);

    /**
     * @brief Returns the roll of the orientation in degrees
     */
    float getRoll() const { return roll; }

    /**
     * @brief Sets the roll of the orientation
     *
     * @param roll The roll of the orientation in degrees
     */
    void setRoll(float roll);

    /**
     * @brief Adds the given roll to the roll angle of the orientation
     *
     * @param roll The amount to roll in degrees
     */
    void applyRoll(float roll);

    /**
     * @brief Sets the yaw, pitch, and roll of the orientation
     *
     * @param yaw The yaw of the orientation in degrees
     * @param pitch The pitch of the orientation in degrees
     * @param roll The roll of the orientation in degrees
     */
    void setEulerAngles(float yaw, float pitch, float roll);

    /**
     * @brief Returns the underlying quaternion representing this orientation
     */
    glm::quat getRotationQuaternion() const;

    /**
     * @brief Direct set the rotation quaternion of the orientation
     *
     * @param quat The quaternion to set the rotation to
     */
    void setRotationQuaternion(const glm::quat& quat);

    /**
     * @brief Returns the forward direction unit vector local to this orientation
     */
    glm::vec3 getForwardDir() const;

    /**
     * @brief Returns the right direction unit vector local to this orientation
     */
    glm::vec3 getRightDir() const;

    /**
     * @brief Returns the up direction unit vector local to this orientation
     */
    glm::vec3 getUpDir() const;

private:
    float yaw;
    float roll;
    float pitch;
    glm::quat cachedRotation;

    void updateRotation();

    friend struct refl::ReflectedObject<Orientation3D>;
};

} // namespace com

namespace refl
{
template<>
struct ReflectedObject<com::Orientation3D> {
    inline static const auto spec = makeSpec<com::Orientation3D>(
        "Orientation3D", memberList(defineMember(1, "yaw", &com::Orientation3D::yaw),
                                    defineMember(2, "pitch", &com::Orientation3D::pitch),
                                    defineMember(3, "roll", &com::Orientation3D::roll)));
};
} // namespace refl
} // namespace bl

#endif
