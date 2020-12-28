#ifndef BLIB_UTIL_TIMER_HPP
#define BLIB_UTIL_TIMER_HPP

#include <BENG/Util/NonCopyable.hpp>
#include <SFML/System.hpp>

namespace bg
{
/**
 * @brief Global class for tracking time. Implemented over the top of sf::Clock
 *        to allow for pausing
 *
 * @ingroup Util
 */
class Timer : private NonCopyable {
public:
    /**
     * @brief Returns the global timer object
     */
    static Timer& get();

    /**
     * @brief Returns the time elapsed in seconds
     */
    float timeElapsedSeconds() const;

    /**
     * @brief Returns the time elapsed in milliseconds
     */
    unsigned long int timeElapsedMilliseconds() const;

    /**
     * @brief Returns the elapsed time
     */
    sf::Time timeElapsed() const;

    /**
     * @brief Returns the raw elapsed time. Does not compensate for pausing
     */
    sf::Time timeElapsedRaw() const;

    /**
     * @brief Pauses the timer
     */
    void pause();

    /**
     * @brief Resumes the timer
     */
    void resume();

private:
    Timer();

    sf::Clock clock;
    sf::Time timePaused, offset;
    bool paused;
};

} // namespace bg

#endif
