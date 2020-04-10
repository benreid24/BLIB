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
     * @brief Callback function signature for a Signal. It takes 3 parameters: The Action, the
     *        Element id, and the Element group, in that order
     *
     */
    typedef std::function<void(const Action&, const std::string&, const std::string&)>
        TCallback;

    /**
     * @brief When triggered, the Signal will set the given flag to the given value
     *
     * @param var The flag to set
     * @param val The value to set the flag to
     */
    void willSet(bool& var, bool val);

    /**
     * @brief When triggered, the Signal will call the provided callback
     *
     * @param cb The function to call on trigger
     */
    void willCall(TCallback cb);

    /**
     * @brief Clears any set or call action
     *
     */
    void clear();

    /**
     * @brief Triggers the signal. This may be manually done to simulate input
     *
     */
    void operator()();

private:
    Signal();

    std::optional<std::pair<bool*, bool>> setAction;
    std::optional<TCallback> callbackAction;

    friend class Element;
};

} // namespace gui
} // namespace bl

#endif