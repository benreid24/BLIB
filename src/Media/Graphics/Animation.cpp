#include <BLIB/Media/Graphics/Animation.hpp>

namespace bl
{
/// Collection of graphical utilities and functionality
namespace gfx
{
Animation::Animation()
: data(nullptr)
, centerOrigin(true)
, isPlaying(false)
, animTime(0)
, position(0, 0)
, scale(1, 1)
, rotation(0)
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

void Animation::play() {
    isPlaying = true;
    animTime  = 0.f;
}

bool Animation::playing() const {
    if (data == nullptr || !isPlaying) return false;

    const bool isLoop = loopOverride ? loop : data->isLooping();
    if (!isLoop) return animTime <= data->getLength();
    return true;
}

void Animation::stop() { isPlaying = false; }

void Animation::update(float dt) {
    if (isPlaying) { animTime += dt; }
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
