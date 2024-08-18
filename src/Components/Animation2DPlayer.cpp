#include <BLIB/Components/Animation2DPlayer.hpp>

namespace bl
{
namespace com
{
Animation2DPlayer::Animation2DPlayer(const resource::Ref<gfx::a2d::AnimationData>& a, bool s,
                                     bool p, bool l)
: forSlideshow(s)
, animation(a)
, currentState(0)
, currentFrame(0)
, isPlaying(p)
, forceLoop(l)
, loop(true)
, frameTime(0.f)
, playerIndex(InvalidIndex)
, framePayload() {}

void Animation2DPlayer::play(bool restart) {
    forceLoop = false;
    if (restart) { stop(); }
    isPlaying = true;
}

void Animation2DPlayer::playLooping(bool l, bool restart) {
    if (restart) { stop(); }
    forceLoop = true;
    loop      = l;
    isPlaying = true;
}

void Animation2DPlayer::pause() { isPlaying = false; }

void Animation2DPlayer::stop() {
    isPlaying    = false;
    frameTime    = 0.f;
    currentFrame = animation->getFrameForState(currentState);
    if (framePayload.valid()) { *framePayload = currentFrame; }
}

void Animation2DPlayer::setState(std::size_t state, bool p) {
    if (currentState != state) {
        frameTime    = 0.f;
        currentState = state;
        currentFrame = animation->getFrameForState(currentState);
        if (framePayload.valid()) { *framePayload = currentFrame; }
    }
    if (p) { play(); }
}

void Animation2DPlayer::update(float dt) {
    if (isPlaying) {
        if (animation && animation->frameCount() > 0) {
            frameTime += dt;
            while (frameTime >= animation->getFrameLength(currentFrame)) {
                const std::size_t ogFrame = currentFrame;
                frameTime -= animation->getFrameLength(currentFrame);
                currentFrame = animation->getNextFrame(currentFrame);

                // if we restarted and we shouldn't loop then stop
                if (ogFrame > currentFrame) {
                    if (!isLooping()) {
                        stop();
                        break;
                    }
                }

                if (framePayload.valid()) { *framePayload = currentFrame; }
            }
        }
        else { isPlaying = false; }
    }
}

bool Animation2DPlayer::isLooping() const {
    return forceLoop ? loop : animation ? animation->isLooping() : false;
}

} // namespace com
} // namespace bl
