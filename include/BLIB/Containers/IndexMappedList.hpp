#ifndef BLIB_CONTAINERS_INDEXMAPPEDLIST_HPP
#define BLIB_CONTAINERS_INDEXMAPPEDLIST_HPP

#include <BLIB/Util/IdAllocatorUnbounded.hpp>
#include <limits>
#include <optional>
#include <type_traits>
#include <vector>

namespace bl
{
namespace ctr
{
/**
 * @brief Specialized storage that is like a multimap, but internally is comprised of only a few
 *        vectors. This keeps the storage relatively cache friendly and helps to minimize
 *        fragmentation over time. Chief usage is one-to-many relationship mapping in the ECS
 *
 * @tparam TKey The key type. Must be an unsigned integer
 * @tparam TValue The value type
 * @ingroup Containers
 */
template<typename TKey, typename TValue>
class IndexMappedList {
    static_assert(std::is_unsigned_v<TKey>, "Key must be a 0-based index");

public:
    struct Item;
    struct Iterator;
    struct Range;

    /**
     * @brief Creates a new index mapped list with the given capacity hint
     *
     * @param capacityHint The amount of capacity to reserve
     */
    IndexMappedList(std::size_t capacityHint = 64);

    /**
     * @brief Returns whether or not the given key has any values
     *
     * @param key The key to test
     * @return True if the key has values, false otherwise
     */
    bool hasValues(TKey key) const;

    /**
     * @brief Returns if the given key contains the given value
     *
     * @param key The key to check the values of
     * @param value The specific value to search for
     * @return True if the key has the value, false otherwise
     */
    bool hasValue(TKey key, const TValue& value) const;

    /**
     * @brief Returns the range of values for a given key. May be empty
     *
     * @param key The key to get the values for
     * @return Iterable range of values for the given key
     */
    Range getValues(TKey key);

    /**
     * @brief Removes the specific value for the given key. Safe if the value is not contained
     *
     * @param key The key to remove the value for
     * @param value The value to remove
     */
    void removeValue(TKey key, const TValue& value);

    /**
     * @brief Removes all values for the given key
     *
     * @param key The key to clear from the map
     */
    void remove(TKey key);

    /**
     * @brief Adds the value to the given key
     *
     * @param key The key to add the value for
     * @param value The value to add
     */
    void add(TKey key, TValue value);

    /**
     * @brief Clears all values and keys
     */
    void clear();

    /**
     * @brief Iterator over values for a specific key
     */
    struct Iterator {
        /**
         * @brief Returns whether or not the iterator may be advanced or dereferenced
         */
        bool isValid() const { return current != nullptr; }

        /**
         * @brief Returns whether or not the iterator is the last value in the sequence
         */
        bool hasNext() const { return isValid() && current->next != NoNext; }

        /**
         * @brief Returns an iterator to the next element, leaving this one unchanged
         */
        Iterator next() const {
            Iterator n = *this;
            n.advance();
            return n;
        }

        /**
         * @brief Advances this iterator to the next element
         */
        void advance() { current = hasNext() ? &(*storage)[current->next] : nullptr; }

        /**
         * @brief Advances this iterator to the next element
         */
        Iterator& operator++() {
            advance();
            return *this;
        }

        /**
         * @brief Advances this iterator to the next element and returns the prior value
         */
        Iterator operator++(int) {
            Iterator prev = *this;
            advance();
            return prev;
        }

        /**
         * @brief Access the value the iterator points to
         */
        TValue& operator*() { return current->payload.value(); }

        /**
         * @brief Access the value the iterator points to
         */
        const TValue& operator*() const { return current->payload.value(); }

        /**
         * @brief Access the value the iterator points to
         */
        TValue* operator->() { return &current->payload.value(); }

        /**
         * @brief Access the value the iterator points to
         */
        const TValue* operator->() const { return &current->payload.value(); }

        /**
         * @brief Returns whether or not the given iterator is the same as this one
         */
        bool operator==(const Iterator& right) const { return current == right.current; }

        /**
         * @brief Returns whether or not the given iterator is different than this one
         */
        bool operator!=(const Iterator& right) const { return current != right.current; }

    private:
        std::vector<Item>* storage;
        Item* current;

        Iterator()
        : storage(nullptr)
        , current(nullptr) {}

        Iterator(std::vector<Item>* storage, std::uint32_t i)
        : storage(storage)
        , current(&(*storage)[i]) {}

        friend struct Range;
        friend class IndexMappedList;
    };

    /**
     * @brief Helper struct to allow range-based for loops
     */
    struct Range {
        /**
         * @brief The beginning of the sequence
         */
        Iterator begin() const { return start; }

        /**
         * @brief Returns an invalid sentinel value
         */
        Iterator end() const { return Iterator(); }

    private:
        Range() = default;
        Range(Iterator start)
        : start(start) {}

        Iterator start;

        friend class IndexMappedList;
    };

private:
    static constexpr std::uint32_t NoNext = std::numeric_limits<std::uint32_t>::max();

    std::vector<Item> storage;
    util::IdAllocatorUnbounded<std::uint32_t> freeSet;
    std::vector<std::uint32_t> keyToStartIndex;

    void ensureValueCapacity(std::uint32_t cap);
    void ensureKeyCapacity(std::uint32_t cap);
    void reset(std::uint32_t i);

    struct Item {
        std::optional<TValue> payload;
        std::uint32_t next;

        Item()
        : next(NoNext) {}

        void clear() {
            payload.reset();
            next = NoNext;
        }
    };
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TKey, typename TValue>
IndexMappedList<TKey, TValue>::IndexMappedList(std::size_t capacityHint)
: freeSet(capacityHint) {
    ensureKeyCapacity(capacityHint);
    ensureValueCapacity(capacityHint);
}

template<typename TKey, typename TValue>
bool IndexMappedList<TKey, TValue>::hasValues(TKey key) const {
    return key < keyToStartIndex.size() && keyToStartIndex[key] != NoNext;
}

template<typename TKey, typename TValue>
bool IndexMappedList<TKey, TValue>::hasValue(TKey key, const TValue& value) const {
    if (key >= keyToStartIndex.size()) { return false; }

    std::uint32_t i = keyToStartIndex[key];
    while (i < storage.size()) {
        if (storage[i].payload.value() == value) { return true; }
        i = storage[i].next;
    }
    return false;
}

template<typename TKey, typename TValue>
IndexMappedList<TKey, TValue>::Range IndexMappedList<TKey, TValue>::getValues(TKey key) {
    return {hasValues(key) ? Iterator(&storage, keyToStartIndex[key]) : Iterator()};
}

template<typename TKey, typename TValue>
void IndexMappedList<TKey, TValue>::removeValue(TKey key, const TValue& value) {
    if (key >= keyToStartIndex.size()) { return; }

    std::uint32_t i = keyToStartIndex[key];
    if (i != NoNext) {
        if (storage[i].payload.value() == value) {
            keyToStartIndex[key] = storage[i].next;
            reset(i);
        }
        else if (storage[i].next != NoNext) {
            do {
                std::uint32_t prev = i;
                i                  = storage[i].next;
                if (storage[i].payload.value() == value) {
                    storage[prev].next = storage[i].next;
                    reset(i);
                    break;
                }
            } while (storage[i].next != NoNext);
        }
    }
}

template<typename TKey, typename TValue>
void IndexMappedList<TKey, TValue>::remove(TKey key) {
    if (key >= keyToStartIndex.size()) { return; }

    std::uint32_t i = keyToStartIndex[key];
    if (i != NoNext) {
        keyToStartIndex[key] = NoNext;
        do {
            const std::uint32_t n = storage[i].next;
            reset(i);
            i = n;
        } while (i != NoNext);
    }
}

template<typename TKey, typename TValue>
void IndexMappedList<TKey, TValue>::add(TKey key, TValue value) {
    const std::uint32_t i = freeSet.allocate();
    ensureKeyCapacity(key + 1);
    ensureValueCapacity(i + 1);
    storage[i].payload.emplace(std::forward<TValue>(value));

    std::uint32_t& startIndex = keyToStartIndex[key];
    storage[i].next           = startIndex;
    startIndex                = i;
}

template<typename TKey, typename TValue>
void IndexMappedList<TKey, TValue>::clear() {
    std::fill(keyToStartIndex.begin(), keyToStartIndex.end(), NoNext);
    for (Item& item : storage) { item.clear(); }
    freeSet.releaseAll();
}

template<typename TKey, typename TValue>
inline void IndexMappedList<TKey, TValue>::ensureValueCapacity(std::uint32_t cap) {
    if (storage.size() < cap) { storage.resize(cap); }
}

template<typename TKey, typename TValue>
inline void IndexMappedList<TKey, TValue>::ensureKeyCapacity(std::uint32_t cap) {
    if (keyToStartIndex.size() < cap) { keyToStartIndex.resize(cap, NoNext); }
}

template<typename TKey, typename TValue>
inline void IndexMappedList<TKey, TValue>::reset(std::uint32_t i) {
    storage[i].clear();
    freeSet.release(i);
}

} // namespace ctr
} // namespace bl

#endif
