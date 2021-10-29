#include <BLIB/Media/Graphics/Animation.hpp>

namespace bl
{
/// Collection of graphical utilities and functionality
namespace gfx
{
Animation::Animation()
: data(nullptr)
, isPlaying(false)
, frame(0)
, frameTime(0.f)
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
    if (!isPlaying || restart) {
        frameTime = 0.f;
        frame     = 0;
    }
    isPlaying = true;
}

bool Animation::playing() const {
    if (data == nullptr || !isPlaying) return false;

    const bool isLoop = loopOverride ? loop : data->isLooping();
    if (!isLoop) return frame < data->frames.size() - 1 || frameTime <= data->lengths.back();
    return true;
}

bool Animation::finished() const { return !playing(); }

void Animation::stop() {
    isPlaying = false;
    frameTime = 0.f;
    frame     = 0;
}

void Animation::update(float dt) {
    if (isPlaying) {
        frameTime += dt;
        if (frameTime > data->lengths[frame]) {
            frameTime -= data->lengths[frame];
            ++frame;
            if (frame >= data->frames.size()) {
                frame = 0;
                if (!data->isLooping() && !(loopOverride && loop)) {
                    isPlaying = false;
                    frameTime = 0.f;
                }
            }
        }
    }
}

void Animation::render(sf::RenderTarget& target, float lag, sf::RenderStates states) {
    if (data) {
        const float t = isPlaying ? frameTime + lag : 0.f;
        const unsigned int i =
            (t > data->lengths[frame]) ? (frame + 1) % data->frames.size() : frame;
        data->render(target, states, position, scale, rotation, centerOrigin, i);
    }
}

void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (data) { data->render(target, states, position, scale, rotation, centerOrigin, frame); }
}

} // namespace gfx
} // namespace bl
