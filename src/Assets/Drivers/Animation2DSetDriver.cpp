#include <BLIB/Assets/Drivers/Animation2DSetDriver.hpp>

namespace bl
{
namespace asi
{
bool Animation2DSetDriver::doCreate(const as::CreateContext& ctx, Animation2DSetPayload& payload) {
    const Animation2DSetPayload::CreateData* data =
        ctx.getCustomDataAsMaybe<Animation2DSetPayload::CreateData>();
    if (!data) {
        BL_LOG_ERROR << "No create data provided for animation set asset creation";
        return false;
    }

    if (!payload.animations.addDependency(data->baseAnimation.getUUID())) {
        BL_LOG_ERROR << "Failed to set base animation for new animation set asset";
        return false;
    }
    payload.computeDerivedState();

    for (const auto& anim : data->additionalAnimations) {
        if (!payload.addState(payload.animations.getSize(), anim, data->enforceSlideshow)) {
            BL_LOG_ERROR << "Failed to add additional animation to new animation set asset";
            return false;
        }
    }
    return true;
}

bool Animation2DSetDriver::doRead(const as::ReadContext&, Animation2DSetPayload& payload) {
    payload.computeDerivedState();
    return true;
}

bool Animation2DSetDriver::doWrite(const as::WriteContext&, const Animation2DSetPayload&) {
    return true;
}

} // namespace asi
} // namespace bl
