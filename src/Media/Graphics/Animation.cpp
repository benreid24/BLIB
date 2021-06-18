#include <BLIB/Media/Graphics/Animation.hpp>

#include <cmath>

namespace bl
{
/// Collection of graphical utilities and functionality
namespace gfx
{
Animation::Animation()
: data(nullptr)
, isPlaying(false)
, animTime(0.f)
, centerOrigin(true)
, position(0.f, 0.f)
, scale(1.f, 1.f)
, rotation(0.f)
, loopOverride(false)
, loop(false) {}

Animation::Animation(AnimationData& data)
: Animation() {
    setData(data);
}

void Animation::setData(AnimationData& d) { data = &d; }

AnimationData& Animation::getData() const { return *data; }

void Animation::setIsCentered(bool c) { centerOrigin = c; }

void Animation::setPosition(const sf::Vector2f& pos) { position = pos; }

void Animation::setScale(const sf::Vector2f& s) { scale = s; }

void Animation::setRotation(float r) { rotation = r; }

void Animation::setIsLoop(bool l) {
    loopOverride = true;
    loop         = l;
}

void Animation::resetIsLoop() { loopOverride = false; }

void Animation::play(bool restart) {
    if (!isPlaying || restart) animTime = 0.f;
    isPlaying = true;
}

bool Animation::playing() const {
    if (data == nullptr || !isPlaying) return false;

    const bool isLoop = loopOverride ? loop : data->isLooping();
    if (!isLoop) return animTime <= data->getLength();
    return true;
}

void Animation::stop() {
    isPlaying = false;
    animTime  = 0.f;
}

void Animation::update(float dt) {
    if (isPlaying) {
        animTime += dt;
        if (animTime > data->getLength()) {
            if (data->isLooping() || (loopOverride && loop)) { animTime -= data->getLength(); }
            else {
                isPlaying = false;
                animTime  = 0.f;
            }
        }
    }
}

void Animation::render(sf::RenderTarget& target, float lag, sf::RenderStates states) {
    if (data) {
        const float t = isPlaying ? animTime + lag : 0;
        data->render(
            target, states, t, position, scale, rotation, centerOrigin, loopOverride, loop);
    }
}

void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (data) {
        const float t = isPlaying ? animTime : 0;
        data->render(
            target, states, t, position, scale, rotation, centerOrigin, loopOverride, loop);
    }
}

} // namespace gfx
} // namespace bl
