#ifndef BLIB_INPUT_LISTENER_BOUNDARY_HPP
#define BLIB_INPUT_LISTENER_BOUNDARY_HPP

#include <BLIB/Input/Listener.hpp>

namespace bl
{
namespace input
{
/**
 * @brief Helper class that ignores and blocks all inputs. Use this as a backstop between
 *        menus/states to avoid inputs from leaking back to inactive listeners
 *
 * @ingroup Input
 *
 */
class ListenerBoundary : public Listener {
public:
    /**
     * @brief Destroy the Listener Boundary object
     *
     */
    virtual ~ListenerBoundary() = default;

private:
    virtual bool observe(const Actor&, unsigned int, DispatchType, bool) override { return true; }
};

} // namespace input
} // namespace bl

#endif
