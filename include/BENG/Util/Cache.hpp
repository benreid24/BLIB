#ifndef BLIB_UTIL_CACHE_HPP
#define BLIB_UTIL_CACHE_HPP

#include <BENG/Logging.hpp>
#include <BENG/Util/FastQueue.hpp>

#include <forward_list>
#include <memory>
#include <unordered_map>

namespace bg
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
     * @brief Add an item to the cache. Will fail if the key is already in use
     *
     * @param key Key of the item to store
     * @param item The item to store
     * @return True if the item was added, false otherwise
     */
    bool add(const TKey& key, const TPayload& item);

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

    /**
     * @brief Returns whether or not the cache is empty
     *
     */
    bool empty() const;

    /**
     * @brief Returns how many elements are currently contained
     *
     */
    size_t size() const;

    /**
     * @brief Returns an iterator to the first element in the cache
     *
     */
    typename std::unordered_map<TKey, TPayload>::iterator begin();

    /**
     * @brief Returns an iterator to the last element in the cache
     *
     */
    typename std::unordered_map<TKey, TPayload>::iterator end();

private:
    const EvictionPolicy evictionPolicy;
    const unsigned int highWatermark, lowWatermark;

    std::unordered_map<TKey, std::shared_ptr<TPayload>> cache;
    mutable FastQueue<TKey> readQueue;
    FastQueue<TKey> writeQueue;
    FastQueue<TKey> orderQueue;

    void checkEviction();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TKey, typename TPayload>
Cache<TKey, TPayload>::Cache(EvictionPolicy policy, unsigned int lowWatermark,
                             unsigned int highWatermark)
: evictionPolicy(policy)
, lowWatermark(lowWatermark)
, highWatermark(highWatermark) {}

template<typename TKey, typename TPayload>
bool Cache<TKey, TPayload>::add(const TKey& key, const TPayload& value) {
    return add(key, Item(new TPayload(value)));
}

template<typename TKey, typename TPayload>
bool Cache<TKey, TPayload>::add(const TKey& key, const Item& value) {
    if (cache.find(key) == cache.end()) {
        cache.emplace(key, value);
        readQueue.push_front(key);
        writeQueue.push_front(key);
        orderQueue.push_front(key);
        checkEviction();
        return true;
    }
    return false;
}

template<typename TKey, typename TPayload>
typename Cache<TKey, TPayload>::Item Cache<TKey, TPayload>::getForOwn(const TKey& key) {
    auto it = cache.find(key);
    if (it != cache.end) {
        readQueue.promote_front(key);
        writeQueue.promote_front(key);
        return it->second;
    }
}

template<typename TKey, typename TPayload>
const TPayload* Cache<TKey, TPayload>::getForRead(const TKey& key) const {
    auto it = cache.find(key);
    if (it != cache.end()) {
        readQueue.promote_front(key);
        return it->second.get();
    }
    return nullptr;
}

template<typename TKey, typename TPayload>
TPayload* Cache<TKey, TPayload>::getForWrite(const TKey& key) {
    auto it = cache.find(key);
    if (it != cache.end()) {
        readQueue.promote_front(key);
        writeQueue.promote_front(key);
        return it->second.get();
    }
    return nullptr;
}

template<typename TKey, typename TPayload>
bool Cache<TKey, TPayload>::erase(const TKey& key) {
    auto it = cache.find(key);
    if (it != cache.end()) {
        cache.erase(it);
        readQueue.remove(key);
        writeQueue.remove(key);
        orderQueue.remove(key);
        return true;
    }
    return false;
}

template<typename TKey, typename TPayload>
typename std::unordered_map<TKey, TPayload>::iterator Cache<TKey, TPayload>::begin() {
    return cache.begin();
}

template<typename TKey, typename TPayload>
typename std::unordered_map<TKey, TPayload>::iterator Cache<TKey, TPayload>::end() {
    return cache.begin();
}

template<typename TKey, typename TPayload>
bool Cache<TKey, TPayload>::empty() const {
    return cache.empty();
}

template<typename TKey, typename TPayload>
size_t Cache<TKey, TPayload>::size() const {
    return cache.size();
}

template<typename TKey, typename TPayload>
void Cache<TKey, TPayload>::checkEviction() {
    if (cache.size() >= highWatermark) {
        FastQueue<TKey>* queue = nullptr;
        switch (evictionPolicy) {
        case LeastRecentlyAccessed:
            queue = &readQueue;
            break;
        case LeastRecentlyModified:
            queue = &writeQueue;
            break;

        case FirstInFirstOut:
            queue = &orderQueue;
            break;
        default:
            BL_LOG_ERROR << "Invalid eviciton policy in queue: " << evictionPolicy;
            return;
        }

        while (cache.size() > lowWatermark && !queue->empty()) {
            TKey key;
            queue->pop_back(key);
            cache.erase(key);
        }
    }
}

} // namespace bg

#endif
