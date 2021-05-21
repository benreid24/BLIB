#include <BLIB/Media/Graphics/Flashing.hpp>

namespace bl
{
namespace gfx
{
Flashing::Flashing(sf::Drawable& component, float on, float off)
: component(component)
, onPeriod(on)
, offPeriod(off)
, time(0.f)
, visible(true) {}

void Flashing::update(float dt) {
    time += dt;
    if (visible) {
        if (time >= onPeriod) {
            time -= onPeriod;
            visible = false;
        }
    }
    else {
        if (time >= offPeriod) {
            time -= offPeriod;
            visible = true;
        }
    }
}

void Flashing::render(sf::RenderTarget& target, sf::RenderStates states, float lag) const {
    const float t = time + lag;
    if (visible && t < onPeriod || !visible && t >= offPeriod) { target.draw(component, states); }
}

} // namespace gfx
} // namespace bl
