#ifndef BLIB_COMPONENTS_ORIENTATION3D_HPP
#define BLIB_COMPONENTS_ORIENTATION3D_HPP

#include <glm/glm.hpp>

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
     * @brief Returns a normalized direction vector that the orientation is facing
     */
    const glm::vec3& getFaceDirection() const;

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
    float getPitch() const;

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
    float getYaw() const;

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
    float getRoll() const;

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

private:
    glm::vec3 faceDir;
    float roll;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline float Orientation3D::getRoll() const { return roll; }

inline const glm::vec3& Orientation3D::getFaceDirection() const { return faceDir; }

} // namespace com
} // namespace bl

#endif
