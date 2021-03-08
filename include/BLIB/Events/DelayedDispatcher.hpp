#ifndef BLIB_EVENTS_DELAYEDDISPATCHER_HPP
#define BLIB_EVENTS_DELAYEDDISPATCHER_HPP

#include <BLIB/Containers/Any.hpp>
#include <BLIB/Events/Dispatcher.hpp>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace bl
{
/// Collection of classes providing different types of publisher/consumer event queues
namespace event
{
/**
 * @brief A wrapper around Dispatcher for delayed event dispatches. Events are queued up
 *        as dispatch is called. The queue may then be drained manually or using a builtin
 *        background thread.
 *
 * @ingroup Events
 *
 */
class DelayedDispatcher {
public:
    /**
     * @brief Creates a new delayed dispatcher with the given underlying. If multithread is true
     *        then a background thread is started which drains the event queue each time an event
     *        comes in with at least 15 milliseconds in between drains. If not multithreaded then
     *        drain() must be manually called
     *
     * @param dispatcher The underlying dispatcher to send events into
     * @param multithread True to drain in the background, false for manual draining
     */
    DelayedDispatcher(Dispatcher& dispatcher, bool multithread = true);

    /**
     * @brief Stops the background thread if running and drains the queue one last time
     *
     */
    ~DelayedDispatcher();

    /**
     * @brief Dispatches the given event to each listener that is subscribed to that type of event
     *
     * @tparam T The type of event to dispatch
     * @param event The event to dispatch
     */
    template<typename T>
    void dispatch(const T& event);

    /**
     * @brief Drains the queue of events by dispatching them through the underlying dispatcher.
     *        While threadsafe, this only needs to be called when not running with the background
     *        thread
     *
     */
    void drain();

private:
    struct Dispatch {
        virtual ~Dispatch()                                 = default;
        virtual void dispatch(Dispatcher& dispatcher) const = 0;
    };

    template<typename T>
    class TypedDispatch : public Dispatch {
    public:
        TypedDispatch(const T& v);
        virtual ~TypedDispatch() = default;

        virtual void dispatch(Dispatcher& dispatcher) const override;

    private:
        const T event;
    };

    Dispatcher& underlying;
    std::mutex mutex;
    std::condition_variable cvar;
    std::atomic_bool running;
    std::thread runner;
    std::vector<container::Any<32>> events;

    void background();
    void drainAll();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
void DelayedDispatcher::dispatch(const T& e) {
    std::unique_lock lock(mutex);
    events.emplace_back(TypedDispatch<T>(e));
    cvar.notify_all();
}

template<typename T>
DelayedDispatcher::TypedDispatch<T>::TypedDispatch(const T& e)
: event(e) {}

template<typename T>
void DelayedDispatcher::TypedDispatch<T>::dispatch(Dispatcher& underlying) const {
    underlying.dispatch(event);
}

} // namespace event
} // namespace bl

#endif
