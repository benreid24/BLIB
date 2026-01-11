#ifndef BLIB_SIGNALS_TABLE_HPP
#define BLIB_SIGNALS_TABLE_HPP

#include <BLIB/Signals/Channel.hpp>
#include <string>

namespace bl
{
namespace sig
{
/**
 * @brief Global signal channel table that allows for central management and access to channels
 *
 * @ingroup Signals
 */
class Table {
public:
    /**
     * @brief Creates a new channel with the given key. Returns an existing channel on collision
     *
     * @param key The key of the channel to create
     * @return A reference to the created or existing channel
     */
    static Channel& createChannel(const std::string& key);

    /**
     * @brief Creates a new channel with the given key. Returns an existing channel on collision
     *
     * @param key The key of the channel to create
     * @return A reference to the created or existing channel
     */
    static Channel& createChannel(void* key);

    /**
     * @brief Registers an existing channel with the given key, replacing any existing channel.
     *        Performs no lifetime management of the channel, care must be taken to call
     *        removeChannel if the registered channel goes out of scope
     *
     * @param key The key to register the channel with
     * @param channel The channel to register
     */
    static void registerChannel(const std::string& key, Channel& channel);

    /**
     * @brief Registers an existing channel with the given key, replacing any existing channel.
     *        Performs no lifetime management of the channel, care must be taken to call
     *        removeChannel if the registered channel goes out of scope
     *
     * @param key The key to register the channel with
     * @param channel The channel to register
     */
    static void registerChannel(void* key, Channel& channel);

    /**
     * @brief Fetches or creates a channel with the given key
     *
     * @param key The key of the channel to fetch or create
     * @return A reference to the channel with the given key
     */
    static Channel& getChannel(const std::string& key);

    /**
     * @brief Fetches or creates a channel with the given key
     *
     * @param key The key of the channel to fetch or create
     * @return A reference to the channel with the given key
     */
    static Channel& getChannel(void* key);

    /**
     * @brief Removes the channel with the given key. If the table owns the channel then it will be
     *        destroyed, disconnecting all emitters and listeners.
     *
     * @param key The key of the channel to remove
     */
    static void removeChannel(const std::string& key);

    /**
     * @brief Removes the channel with the given key. If the table owns the channel then it will be
     *        destroyed, disconnecting all emitters and listeners.
     *
     * @param key The key of the channel to remove
     */
    static void removeChannel(void* key);

    /**
     * @brief Returns a reference to the global channel
     */
    static Channel& getGlobalChannel();
};

} // namespace sig
} // namespace bl

#endif
