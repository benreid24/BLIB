#ifndef BLIB_RESOURCES_STATE_HPP
#define BLIB_RESOURCES_STATE_HPP

namespace bl
{
namespace engine
{
class Engine;
}

namespace resource
{
template<typename T>
class Ref;

/**
 * @brief Private state class that affects resource manager behavior during app exit
 *
 * @ingroup Resources
 */
struct State {
private:
    static bool appExiting;

    template<typename T>
    friend class Ref;
    friend class engine::Engine;
};

} // namespace resource
} // namespace bl

#endif
