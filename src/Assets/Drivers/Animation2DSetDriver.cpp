#include <BLIB/Assets/Drivers/Animation2DSetDriver.hpp>

namespace bl
{
namespace asi
{
bool Animation2DSetDriver::doCreate(const as::CreateContext& ctx, Animation2DSetPayload& payload) {
    const Animation2DSetPayload::CreateData* data =
        ctx.getCustomDataAsMaybe<Animation2DSetPayload::CreateData>();
    as::TypedRef<Animation2DPayload> baseAnim;
    if (!data) {
        if (ctx.getCustomData().getPath().empty()) {
            BL_LOG_ERROR << "No create data or path provided for animation set asset creation";
            return false;
        }
        baseAnim = ctx.getRepository().getAssetFromSourcePath<Animation2DPayload>(
            ctx.getCustomData().getPath());
        if (!baseAnim) {
            BL_LOG_ERROR
                << "Failed to find base animation from path for animation set asset creation: "
                << ctx.getCustomData().getPath();
            return false;
        }
    }
    else { baseAnim = data->baseAnimation; }

    if (!baseAnim) {
        if (data->debugCreateData.has_value()) {
            auto animation = ctx.getRepository().createAsset<Animation2DPayload>("", *data);
            if (!animation) {
                BL_LOG_ERROR << "Failed to create base animation from debug data for animation set "
                                "asset creation";
                return false;
            }
            baseAnim = animation;
        }
        else {
            BL_LOG_ERROR << "No base animation provided for animation set asset creation";
            return false;
        }
    }

    if (!payload.animations.addDependency(baseAnim.getUUID())) {
        BL_LOG_ERROR << "Failed to set base animation for new animation set asset";
        return false;
    }
    payload.computeDerivedState();

    if (data) {
        for (const auto& anim : data->additionalAnimations) {
            if (!payload.addState(payload.animations.getSize(), anim, data->enforceSlideshow)) {
                BL_LOG_ERROR << "Failed to add additional animation to new animation set asset";
                return false;
            }
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
