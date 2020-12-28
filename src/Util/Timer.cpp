#include <BENG/Util/Timer.hpp>

namespace bg
{
Timer::Timer()
: paused(false) {}

Timer& Timer::get() {
    static Timer timer;
    return timer;
}

sf::Time Timer::timeElapsedRaw() const { return clock.getElapsedTime(); }

sf::Time Timer::timeElapsed() const {
    sf::Time ret = clock.getElapsedTime() - offset;
    if (paused) ret -= (clock.getElapsedTime() - timePaused);
    return ret;
}

float Timer::timeElapsedSeconds() const { return timeElapsed().asSeconds(); }

unsigned long int Timer::timeElapsedMilliseconds() const {
    return timeElapsed().asMilliseconds();
}

void Timer::pause() {
    paused     = true;
    timePaused = clock.getElapsedTime();
}

void Timer::resume() {
    paused = false;
    offset += clock.getElapsedTime() - timePaused;
}

} // namespace bg