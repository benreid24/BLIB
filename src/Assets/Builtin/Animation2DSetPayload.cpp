#include <BLIB/Assets/Builtin/Animation2DSetPayload.hpp>

namespace bl
{
namespace asi
{
namespace
{
Animation2DPayload::Frame emptyFrame{.length = 1.f};
}

Animation2DSetPayload::Animation2DSetPayload(const as::Payload::ConstructContext& ctx)
: Payload(ctx)
, animations(ctx.repo, *this, "animations") {}

as::TypedRef<ImagePayload> Animation2DSetPayload::getSpritesheet() const {
    if (animations.getSize() == 0) {
        BL_LOG_ERROR << "Accessing spritesheet of empty animation set";
        return {};
    }
    return animations.get(0).getSpritesheet();
}

bool Animation2DSetPayload::isLooping() const {
    if (animations.getSize() == 0) { return false; }
    return animations.get(0).isLooping();
}

float Animation2DSetPayload::getLength() const {
    if (animations.getSize() == 0) { return 0.f; }
    return animations.get(0).getDuration();
}

bool Animation2DSetPayload::isSlideshow() const {
    if (animations.getSize() == 0) { return false; }
    return animations.get(0).isSlideshow();
}

bool Animation2DSetPayload::shardsAreCentered() const {
    if (animations.getSize() == 0) { return false; }
    return animations.get(0).shardsAreCentered();
}

std::size_t Animation2DSetPayload::frameCount() const { return totalFrames; }

const Animation2DPayload::Frame& Animation2DSetPayload::getFrame(std::size_t frameIndex) const {
    if (animations.getSize() == 0) {
        BL_LOG_ERROR << "Attempted to get frame from empty animation set";
        return emptyFrame;
    }
    if (frameIndex >= totalFrames) {
        BL_LOG_ERROR << "Attempted to get frame index " << frameIndex << " from animation set with "
                     << totalFrames << " frames";
        return emptyFrame;
    }

    for (unsigned int i = 0; i < stateOffsets.size(); ++i) {
        auto& frames = animations.get(i).getFrames();
        if (frameIndex < frames.size()) { return frames[frameIndex]; }
        frameIndex -= frames.size();
    }

    BL_LOG_ERROR << "Failed to get frame index " << frameIndex << " from animation set with "
                 << totalFrames << " frames";
    return emptyFrame;
}

const glm::vec2& Animation2DSetPayload::getFrameSize(std::size_t i) const {
    return getFrame(i).size;
}

float Animation2DSetPayload::getFrameLength(std::size_t i) const { return getFrame(i).length; }

glm::vec2 Animation2DSetPayload::getMaxSize() const {
    glm::vec2 max(0, 0);
    for (unsigned int i = 0; i < totalFrames; ++i) {
        const glm::vec2 s = getFrame(i).size;
        max.x             = std::max(max.x, s.x);
        max.y             = std::max(max.y, s.y);
    }
    return max;
}

std::size_t Animation2DSetPayload::getNextFrame(std::size_t current) const {
    if (animations.getSize() == 0) {
        BL_LOG_ERROR << "Attempted to get next frame from empty animation set";
        return 0;
    }
    if (current >= totalFrames) {
        BL_LOG_ERROR << "Attempted to get next frame index " << current
                     << " from animation set with " << totalFrames << " frames";
        return 0;
    }
    for (unsigned int i = 0; i < stateOffsets.size(); ++i) {
        auto& frames = animations.get(i).getFrames();
        if (current < frames.size()) {
            const std::size_t localNext = (current + 1) % frames.size();
            return stateOffsets[i] + localNext;
        }
        current -= frames.size();
    }
    BL_LOG_ERROR << "Failed to get next frame index " << current << " from animation set with "
                 << totalFrames << " frames";
    return 0;
}

std::size_t Animation2DSetPayload::getFrameForState(std::size_t state) const {
    if (state >= stateOffsets.size()) {
        BL_LOG_ERROR << "Attempted to get frame for state index " << state
                     << " from animation set with " << stateOffsets.size() << " states";
        return 0;
    }
    return stateOffsets[state];
}

std::size_t Animation2DSetPayload::getStateFromFrame(std::size_t frame) const {
    if (frame >= totalFrames) {
        BL_LOG_ERROR << "Attempted to get state for frame index " << frame
                     << " from animation set with " << totalFrames << " frames";
        return 0;
    }
    for (unsigned int i = 0; i < stateOffsets.size(); ++i) {
        if (frame < stateOffsets[i]) { return i - 1; }
    }
    return stateOffsets.size() - 1;
}

void Animation2DSetPayload::computeDerivedState() {
    totalFrames = 0;
    stateOffsets.clear();
    stateOffsets.reserve(animations.getSize());
    for (unsigned int i = 0; i < animations.getSize(); ++i) {
        stateOffsets.push_back(totalFrames);
        totalFrames += animations.get(i).getFrames().size();
    }
}

bool Animation2DSetPayload::addState(std::size_t stateIndex,
                                     as::TypedRef<Animation2DPayload> stateAnim,
                                     bool enforceSlideshow) {
    if (stateIndex != animations.getSize()) {
        BL_LOG_ERROR << "State index " << stateIndex << " does not match expected index "
                     << animations.getSize();
        return false;
    }

    if (!stateAnim.isValid()) {
        BL_LOG_ERROR << "Attempted to add invalid animation ref as state index " << stateIndex;
        return false;
    }

    // validate compatible
    if (stateAnim->getFrames().empty()) {
        BL_LOG_ERROR << "Animation combine failed: State animation is empty";
        return false;
    }

    if (animations.get(0).getSpritesheetId() != stateAnim->getSpritesheetId()) {
        BL_LOG_ERROR << "Animation combine failed: Spritesheets are different";
        return false;
    }

    for (const auto& frame : animations.get(0).getFrames()) {
        if (frame.shards.size() != animations.get(0).getFrames().front().shards.size()) {
            BL_LOG_ERROR
                << "Animation combine failed: Base animation has inconsistent shard counts";
            return false;
        }
    }

    for (const auto& frame : stateAnim->getFrames()) {
        if (frame.shards.size() != stateAnim->getFrames().front().shards.size()) {
            BL_LOG_ERROR << "Animation combine failed: State animation has different shard "
                            "counts than base";
            return false;
        }
    }

    if (enforceSlideshow) {
        if (animations.get(0).isSlideshow()) {
            if (!stateAnim->isSlideshow()) {
                BL_LOG_ERROR << "Animation combine failed: State animation was not Slideshow";
                return false;
            }
        }
    }

    if (!animations.addDependency(stateAnim.getUUID())) {
        BL_LOG_ERROR << "Failed to add state index " << stateIndex << " to animation set with "
                     << animations.getSize() << " states";
        return false;
    }
    computeDerivedState();
    return true;
}

} // namespace asi
} // namespace bl
