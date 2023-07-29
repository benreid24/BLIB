#include <BLIB/Components/Animation2DPlayer.hpp>

namespace bl
{
namespace com
{
Animation2DPlayer::Animation2DPlayer()
: currentState(0)
, currentFrame(0)
, isPlaying(false)
, forceLoop(false)
, frameTime(0.f)
, framePayload(nullptr) {}

Animation2DPlayer::Animation2DPlayer(const resource::Ref<gfx::a2d::AnimationData>& a, bool p,
                                     bool l)
: Animation2DPlayer() {
    animation = a;
    isPlaying = p;
    forceLoop = l;
}

void Animation2DPlayer::setAnimation(const resource::Ref<gfx::a2d::AnimationData>& a) {
    if (framePayload) { throw std::runtime_error("setAnimation called on player that is in use"); }

    animation    = a;
    isPlaying    = false;
    forceLoop    = false;
    currentState = 0;
    currentFrame = 0;
    frameTime    = 0.f;
}

void Animation2DPlayer::play(bool restart) {
    if (restart) { stop(); }
    isPlaying = true;
}

void Animation2DPlayer::playLooping() {
    stop();
    forceLoop = true;
    isPlaying = true;
}

void Animation2DPlayer::pause() { isPlaying = false; }

void Animation2DPlayer::stop() {
    isPlaying    = false;
    frameTime    = 0.f;
    currentFrame = animation->getFrameForState(currentState);
    if (framePayload) { *framePayload = currentFrame; }
}

void Animation2DPlayer::setState(std::size_t state, bool p) {
    currentState = state;
    stop();
    if (p) { play(); }
}

void Animation2DPlayer::update(float dt) {
    if (isPlaying && animation) {
        frameTime += dt;
        while (frameTime >= animation->getFrameLength(currentFrame)) {
            const std::size_t ogFrame = currentFrame;
            frameTime -= animation->getFrameLength(currentFrame);
            currentFrame = animation->getNextFrame(currentFrame);

            // we restarted, stop if we shouldn't loop
            if (ogFrame > currentFrame) {
                if (!forceLoop && !animation->isLooping()) { stop(); }
            }
        }
        if (framePayload) { *framePayload = currentFrame; }
    }
}

} // namespace com
} // namespace bl
