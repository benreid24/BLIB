#include <BLIB/Components/Orientation3D.hpp>

#include <BLIB/Render/Config/Constants.hpp>
#include <glm/gtx/quaternion.hpp>

namespace bl
{
namespace com
{
Orientation3D::Orientation3D(float yaw, float pitch, float roll) {
    setYawAndPitch(yaw, pitch);
    setRoll(roll);
}

Orientation3D::Orientation3D(const glm::vec3& dir, float r) {
    setFaceDirection(dir);
    setRoll(r);
}

Orientation3D::Orientation3D(const glm::vec3& lookAt, const glm::vec3& from, float r) {
    setFaceDirection(glm::normalize(lookAt - from));
    setRoll(r);
}

void Orientation3D::lookAt(const glm::vec3& pos, const glm::vec3& from) {
    setFaceDirection(glm::normalize(pos - from));
}

void Orientation3D::setFaceDirection(const glm::vec3& dir) {
    const glm::vec3 direction = glm::normalize(dir);

    pitch = glm::degrees(std::asin(glm::clamp(direction.y, -1.f, 1.f)));
    yaw   = glm::degrees(std::atan2(direction.x, -direction.z));

    updateRotation();
}

void Orientation3D::setForwardDirection(const glm::vec3& forwardDir, const glm::vec3& upDir) {
    const glm::vec3 direction = glm::normalize(forwardDir);
    const glm::vec3 right     = glm::normalize(glm::cross(direction, upDir));
    const glm::vec3 up        = glm::cross(right, direction);

    glm::mat4 rotationMatrix(1.f);
    rotationMatrix[0] = glm::vec4(right, 0.f);
    rotationMatrix[1] = glm::vec4(up, 0.f);
    rotationMatrix[2] = glm::vec4(-direction, 0.f);

    setRotationQuaternion(glm::quat_cast(rotationMatrix));
}

glm::vec3 Orientation3D::getForwardDir() const {
    return cachedRotation * rc::cfg::Constants::ForwardDirection;
}

glm::vec3 Orientation3D::getRightDir() const {
    return cachedRotation * rc::cfg::Constants::RightDirection;
}

glm::vec3 Orientation3D::getUpDir() const {
    return cachedRotation * rc::cfg::Constants::UpDirection;
}

void Orientation3D::setYawAndPitch(float yaw, float pitch) {
    this->yaw   = yaw;
    this->pitch = pitch;
    updateRotation();
}

void Orientation3D::setPitch(float pitch) {
    this->pitch = pitch;
    updateRotation();
}

void Orientation3D::applyPitch(float offset) {
    pitch += offset;
    updateRotation();
}

void Orientation3D::setYaw(float yaw) {
    this->yaw = yaw;
    updateRotation();
}

void Orientation3D::applyYaw(float offset) {
    yaw += offset;
    updateRotation();
}

void Orientation3D::setRoll(float roll) {
    this->roll = roll;
    updateRotation();
}

void Orientation3D::applyRoll(float offset) {
    roll += offset;
    updateRotation();
}

void Orientation3D::setEulerAngles(float yaw, float pitch, float roll) {
    this->yaw   = yaw;
    this->pitch = pitch;
    this->roll  = roll;
    updateRotation();
}

glm::quat Orientation3D::getRotationQuaternion() const { return cachedRotation; }

void Orientation3D::setRotationQuaternion(const glm::quat& quat) {
    const glm::quat normalized = glm::normalize(quat);
    const glm::mat4 matrix     = glm::mat4_cast(normalized);

    pitch = glm::degrees(std::asin(glm::clamp(-matrix[2][1], -1.f, 1.f)));

    const float cosPitch = std::cos(glm::radians(pitch));

    if (std::abs(cosPitch) > 0.0001f) {
        yaw  = glm::degrees(std::atan2(matrix[2][0], -matrix[2][2]));
        roll = glm::degrees(std::atan2(-matrix[0][1], matrix[1][1]));
    }
    else {
        yaw  = glm::degrees(std::atan2(matrix[0][2], matrix[0][0]));
        roll = 0.f;
    }

    cachedRotation = normalized;
}

void Orientation3D::updateRotation() {
    const glm::quat yawRotation =
        glm::angleAxis(glm::radians(-yaw), rc::cfg::Constants::UpDirection);
    const glm::quat pitchRotation =
        glm::angleAxis(glm::radians(pitch), rc::cfg::Constants::RightDirection);
    const glm::quat rollRotation =
        glm::angleAxis(glm::radians(roll), rc::cfg::Constants::ForwardDirection);

    cachedRotation = yawRotation * pitchRotation * rollRotation;
}

} // namespace com
} // namespace bl
