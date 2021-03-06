#ifndef BLIB_EVENTS_DELAYEDEVENTDISPATCHER_HPP
#define BLIB_EVENTS_DELAYEDEVENTDISPATCHER_HPP

#include <BLIB/Events/MultiEventDispatcher.hpp>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace bl
{
/**
 * @brief A wrapper around MultiEventDispatcher for delayed event dispatches. Events are queued up
 *        as dispatch is called. The queue may then be drained manually or using a builtin
 *        background thread.
 *
 * @ingroup Events
 *
 */
class DelayedEventDispatcher {
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
    DelayedEventDispatcher(MultiEventDispatcher& dispatcher, bool multithread = true);

    /**
     * @brief Stops the background thread if running and drains the queue one last time
     *
     */
    ~DelayedEventDispatcher();

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
        virtual ~Dispatch()                                           = default;
        virtual void dispatch(MultiEventDispatcher& dispatcher) const = 0;
    };

    template<typename T>
    class TypedDispatch : public Dispatch {
    public:
        TypedDispatch(const T& v);
        virtual ~TypedDispatch() = default;

        virtual void dispatch(MultiEventDispatcher& dispatcher) const override;

    private:
        const T event;
    };

    MultiEventDispatcher& underlying;
    std::mutex mutex;
    std::condition_variable cvar;
    std::atomic_bool running;
    std::thread runner;
    std::vector<Dispatch*> events;

    void background();
    void drainAll();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
void DelayedEventDispatcher::dispatch(const T& e) {
    std::unique_lock lock(mutex);
    events.emplace_back(new TypedDispatch<T>(e));
    cvar.notify_all();
}

template<typename T>
DelayedEventDispatcher::TypedDispatch<T>::TypedDispatch(const T& e)
: event(e) {}

template<typename T>
void DelayedEventDispatcher::TypedDispatch<T>::dispatch(MultiEventDispatcher& underlying) const {
    underlying.dispatch(event);
}

} // namespace bl

#endif
