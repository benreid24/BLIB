#ifndef BLIB_UTIL_SIGNAL_HPP
#define BLIB_UTIL_SIGNAL_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <functional>
#include <optional>
#include <variant>

namespace bl
{
/**
 * @brief Utility class perform callback actions on events. Can be configured to perform
 * multiple callbacks. Includes a helper method to simply set a boolean variable
 *
 * @tparam CallbackArgs Function signature of callbacks
 * @ingroup Util
 *
 */
template<typename... CallbackArgs>
class Signal : public bl::NonCopyable {
public:
    /**
     * @brief Callback function signature for a Signal
     *
     */
    typedef std::function<void(CallbackArgs...)> Callback;

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
    void operator()(CallbackArgs... args);

private:
    std::vector<std::pair<bool*, bool>> setActions;
    std::vector<Callback> userCallbacks;
    std::vector<Callback> internalCallbacks;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename... CallbackArgs>
void Signal<CallbackArgs...>::willSet(bool& var, bool val) {
    setActions.push_back(std::make_pair(&var, val));
}

template<typename... CallbackArgs>
void Signal<CallbackArgs...>::willCall(Callback cb) {
    userCallbacks.push_back(cb);
}

template<typename... CallbackArgs>
void Signal<CallbackArgs...>::willAlwaysCall(Callback cb) {
    internalCallbacks.push_back(cb);
}

template<typename... CallbackArgs>
void Signal<CallbackArgs...>::clear() {
    setActions.clear();
    userCallbacks.clear();
}

template<typename... CallbackArgs>
void Signal<CallbackArgs...>::operator()(CallbackArgs... args) {
    for (const auto& set : setActions) { *set.first = set.second; }
    for (const auto& cb : userCallbacks) { cb(args...); }
    for (const auto& cb : internalCallbacks) { cb(args...); }
}

} // namespace bl

#endif
