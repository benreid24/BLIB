#ifndef BLIB_UTIL_CACHE_HPP
#define BLIB_UTIL_CACHE_HPP

#include <BLIB/Util/FastQueue.hpp>
#include <forward_list>
#include <memory>
#include <unordered_map>

namespace bl
{
/**
 * @brief Auto-evicting cache with several policy options and programmable watermarks
 *
 * @tparam TKey Key type for the cache
 * @tparam TPayload The type of data to store in the cache
 * @ingroup Util
 */
template<typename TKey, typename TPayload>
class Cache {
public:
    typedef std::shared_ptr<TPayload> Item;

    /**
     * @brief Policy that dictates which items are removed when the high watermark is reached
     *
     */
    enum EvictionPolicy {
        LeastRecentlyAccessed, /// Items that were not read or written recently are purged
        LeastRecentlyModified, /// Items that were not written recently are purged
        FirstInFirstOut        /// Items are purged in the same order they were added in
    };

    /**
     * @brief Creates a new cache with the given eviction policy and watermarks
     *
     * @param policy Eviction policy to use
     * @param lowWatermark Cache size is reduced to lowWatermark when a purge is triggered
     * @param highWatermark Cache is purged when this many items are stored
     */
    Cache(EvictionPolicy policy = LeastRecentlyAccessed, unsigned int lowWatermark = 1000,
          unsigned int highWatermark = 2000);

    /**
     * @brief Add an item to the cache. Will fail if the key is already in use
     *
     * @param key Key of the item to store
     * @param item The item to store
     * @return True if the item was added, false otherwise
     */
    bool add(const TKey& key, const Item& item);

    /**
     * @brief Returns the storage object to allow calling code to take ownership of the stored
     *        item, if found
     *
     * @param key Key of the item to retrieve
     * @return Item The item with the given key, or nullptr if not found
     */
    Item getForOwn(const TKey& key);

    /**
     * @brief Returns a pointer to the given item to be modified, if found
     *
     * @param key Key of the item to modify
     * @return TPayload* Pointer to the given item, nullptr if not found
     */
    TPayload* getForWrite(const TKey& key);

    /**
     * @brief Returns an immutible item for reading, if found
     *
     * @param key Key of the item to read
     * @return const TPayload* Pointer to the item to read, nullptr if not found
     */
    const TPayload* getForRead(const TKey& key) const;

    /**
     * @brief Removes the item from the cache and reports if successful
     *
     * @param key Key of the item to remove
     * @return True if the item was removed, false if not contained
     */
    bool erase(const TKey& key);

private:
    const EvictionPolicy evictionPolicy;
    const unsigned int highWatermark, lowWatermark;

    std::unordered_map<TKey, std::shared_ptr<TPayload>> cache;
    FastQueue<TKey> readQueue;
    FastQueue<TKey> writeQueue;
};

} // namespace bl

#endif
