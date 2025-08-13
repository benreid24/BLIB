#ifndef BLIB_SIGNALS_CHANNEL_HPP
#define BLIB_SIGNALS_CHANNEL_HPP

#include <BLIB/Containers/FastEraseVector.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Signals/Stream.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <atomic>
#include <mutex>
#include <typeindex>
#include <unordered_map>

namespace bl
{
namespace sig
{
namespace priv
{
class EmitterBase;
class ListenerBase;
} // namespace priv

/**
 * @brief A collection of signal streams. Listeners and emitters can connect to channels to send and
 *        receive signals. Channels can be freely instantiated and used wherever, and can also be
 *        used and managed through the central channel Table
 *
 * @ingroup Signals
 */
class Channel : private util::NonCopyable {
public:
    /**
     * @brief Creates a new channel
     */
    Channel();

    /**
     * @brief Move constructor for the Channel class.
     *
     * @param channel An rvalue reference to another Channel object to move from.
     */
    Channel(Channel&& channel);

    /**
     * @brief Destroys the channel and unsubscribes all emitters and listeners.
     */
    ~Channel();

    /**
     * @brief Takes over another channel
     *
     * @param channel The channel to move from
     * @return A reference to this channel
     */
    Channel& operator=(Channel&& channel);

    /**
     * @brief Returns the stream for the given signal type. Generally should not be used directly
     *
     * @tparam TSignal The type of signal to get the stream for
     * @return A pointer to the stream for the given signal type
     */
    template<typename TSignal>
    Stream<TSignal>* getStream();

    /**
     * @brief Performs any deferred subscriptions or unsubscriptions. Called by the engine
     */
    void syncDeferred();

    /**
     * @brief Clears all handlers and emitters from this channel and all children
     */
    void shutdown();

    /**
     * @brief Returns whether the channel is shutdown. Shutdown channels cannot be subscribed or
     *        connected to
     */
    bool isShutdown() const;

    /**
     * @brief Sets the parent channel. Parenting channels only affects shutdown operations
     *
     * @param channel The parent channel
     */
    void setParent(Channel& channel);

    /**
     * @brief Adds a child channel. Parenting channels only affects shutdown operations
     *
     * @param channel The child channel to add
     */
    void addChild(Channel& channel);

    /**
     * @brief Returns whether the channel has a parent channel
     */
    bool hasParent() const;

    /**
     * @brief Returns the parent channel if there is one. Undefined behavior if there is no parent
     */
    Channel& getParent() const;

private:
    std::mutex mutex;
    std::atomic_bool needDeferSync;
    std::unordered_map<std::type_index, priv::StreamBase*> streams;

    Channel* parent;
    std::vector<Channel*> children;
    std::atomic_bool closed;

    void reparent(Channel* original);
    void shutdownFromParent();

    friend class priv::EmitterBase;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TSignal>
Stream<TSignal>* Channel::getStream() {
    needDeferSync = true;
    const std::type_index key(typeid(TSignal));
    std::unique_lock lock(mutex);
    if (closed) {
        BL_LOG_ERROR << "Cannot get stream for signal type " << key.name()
                     << " from a shutdown channel";
        return nullptr;
    }
    auto it = streams.find(key);
    if (it == streams.end()) { it = streams.try_emplace(key, new Stream<TSignal>()).first; }
    return static_cast<Stream<TSignal>*>(it->second);
}

inline bool Channel::isShutdown() const { return closed.load(std::memory_order_acquire); }

inline bool Channel::hasParent() const { return parent != nullptr; }

inline Channel& Channel::getParent() const {
    if (!hasParent()) {
        BL_LOG_ERROR << "Channel has no parent";
        throw std::runtime_error("Channel has no parent");
    }
    return *parent;
}

} // namespace sig
} // namespace bl

#endif
