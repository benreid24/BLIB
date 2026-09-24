#include <BLIB/Cameras/3D/Controllers/FreeController.hpp>

#include <BLIB/Cameras/3D/Camera3D.hpp>
#include <BLIB/Math/Constants.hpp>

namespace bl
{
namespace cam
{
namespace c3d
{
namespace
{
constexpr float TwoPi        = bl::math::Pi * 2.f;
constexpr float HalfPi       = bl::math::Pi * 0.5f;
constexpr float SafeHalfPi   = HalfPi - 0.001f;
constexpr float StoppedSpeed = 0.01f;
} // namespace

FreeController::FreeController(float maxSpeed, float acceleration, float timeToStop)
: velocity(0.f)
, appliedMovement(0.f)
, appliedRotation(0.f)
, dampening(std::pow(StoppedSpeed, 1.f / timeToStop))
, acceleration(acceleration)
, maxSpeed(maxSpeed) {}

void FreeController::applyYaw(float factor) { appliedRotation.y = factor; }

void FreeController::applyPitch(float factor) { appliedRotation.x = factor; }

void FreeController::applyRoll(float factor) { appliedRotation.z = factor; }

void FreeController::moveForward(float factor) { appliedMovement.z = factor; }

void FreeController::moveRight(float factor) { appliedMovement.x = factor; }

void FreeController::moveUp(float factor) { appliedMovement.y = factor; }

void FreeController::update(float dt) {
    glm::vec3 forward = camera().getOrientation().getFaceDirection();
    float yaw         = std::atan2(forward.x, forward.z);
    float pitch       = std::asin(glm::clamp(-forward.y, -1.f, 1.f));
    float roll        = camera().getOrientation().getRoll();

    // orientation
    yaw = std::fmod(yaw + appliedRotation.y, TwoPi);

    pitch += appliedRotation.x;
    if (pitch > SafeHalfPi) { pitch = SafeHalfPi; }
    else if (pitch < -SafeHalfPi) { pitch = -SafeHalfPi; }

    roll += appliedRotation.z;
    if (roll > math::Pi) { roll -= TwoPi; }
    else if (roll < -math::Pi) { roll += TwoPi; }

    forward.x = std::cos(pitch) * std::sin(yaw);
    forward.y = -std::sin(pitch);
    forward.z = std::cos(pitch) * std::cos(yaw);

    camera().getOrientationForChange().setFaceDirection(forward);
    camera().getOrientationForChange().setRoll(roll);
    appliedRotation = glm::vec3(0.f);

    // velocity
    const auto applyVelocity = [this, dt](float& v, float& applied) {
        if (applied != 0.f) {
            v += applied * acceleration * dt;
            if (maxSpeed > 0.f && std::abs(v) > maxSpeed) { v = std::copysign(maxSpeed, v); }
        }
        else { v *= std::pow(dampening, dt); }
        applied = 0.f;
    };
    applyVelocity(velocity.x, appliedMovement.x);
    applyVelocity(velocity.y, appliedMovement.y);
    applyVelocity(velocity.z, appliedMovement.z);

    // position
    const glm::vec3 right   = glm::normalize(glm::cross(forward, glm::vec3(0.f, 1.f, 0.f)));
    const glm::vec3 up      = glm::normalize(glm::cross(right, forward));
    glm::vec3 pos           = camera().getPosition();
    glm::vec3 worldVelocity = right * velocity.x + up * velocity.y + forward * velocity.z;
    pos += worldVelocity * dt;
    camera().setPosition(pos);
}

} // namespace c3d
} // namespace cam
} // namespace bl
