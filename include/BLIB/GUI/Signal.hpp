#ifndef BLIB_GUI_SIGNAL_HPP
#define BLIB_GUI_SIGNAL_HPP

#include <BLIB/GUI/Action.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <functional>
#include <optional>
#include <variant>

namespace bl
{
namespace gui
{
class Element;

/**
 * @brief Utility class to represent a signal on a gui Element. Each Element has a Signal for
 *        each Element::Action
 *
 * @ingroup GUI
 *
 */
class Signal : public bl::NonCopyable {
public:
    /**
     * @brief Callback function signature for a Signal
     *
     */
    typedef std::function<void(const Action&, Element*)> Callback;

    /**
     * @brief When triggered, the Signal will set the given flag to the given value
     *
     * @param var The flag to set
     * @param val The value to set the flag to
     */
    void willSet(bool& var, bool val = true);

    /**
     * @brief When triggered, the Signal will call the provided callback
     *
     * @param cb The function to call on trigger
     */
    void willCall(Callback cb);

    /**
     * @brief Set up a callback for always being called. Intended to be used by derived classes
     *        Callbacks set by this are not cleared by clear()
     * 
     * @param cb The method to call when the signal is fired
     */
    void willAlwaysCall(Callback cb);

    /**
     * @brief Clears any set or call actions
     *
     */
    void clear();

    /**
     * @brief Triggers the signal. This may be manually done to simulate input
     *
     * @param action The action that triggered the Singal
     * @param id The Element that triggered the Signal
     */
    void operator()(const Action& action, Element* element);

private:
    Signal() = default;

    std::vector<std::pair<bool*, bool>> setActions;
    std::vector<Callback> userCallbacks;
    std::vector<Callback> internalCallbacks;

    friend class Element;
};

} // namespace gui
} // namespace bl

#endif