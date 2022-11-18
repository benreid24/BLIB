#ifndef BLIB_UTIL_IDALLOCATOR_HPP
#define BLIB_UTIL_IDALLOCATOR_HPP

#include <BLIB/Containers/RingQueue.hpp>
#include <type_traits>

namespace bl
{
namespace util
{
/**
 * @brief Utility to allocate integral ids in some range [0, max). Allows ids to be reused as well
 *        as tracks the current highest id to allow for tighter iteration over the id range. Ids are
 *        allocated in linear order
 *
 * @tparam T The integral type to use for ids
 * @ingroup Util
 */
template<typename T>
class IdAllocator {
public:
    static_assert(std::is_integral_v<T>, "IdAllocator only works with integer types");

    /**
     * @brief Construct a new Id Allocator. Ids will be in range [0, totalIdCount)
     *
     * @param totalIdCount The total number of ids
     */
    IdAllocator(std::size_t totalIdCount);

    /**
     * @brief Returns the current highest id that has been allocated
     *
     * @return constexpr T The highest id currently in use
     */
    constexpr T highestId() const;

    /**
     * @brief Returns a bitmap that can be used to query a specific id's allocation status
     *
     * @return constexpr const std::vector<bool>& Map of true=allocated false=free
     */
    constexpr const std::vector<bool>& allocatedIds() const;

    /**
     * @brief Returns whether or not any ids are available to be allocated. It is undefined behavior
     *        to call allocate() if this returns false
     *
     * @return True if at least one id can be allocated, false if all ids are in use
     */
    bool available() const;

    /**
     * @brief Allocates an id for use. Undefined behavior if available() returns false
     *
     * @return T The newly allocated id
     */
    T allocate();

    /**
     * @brief Marks the given id as being available for use again
     *
     * @param id The id to mark as free
     */
    void release(T id);

    /**
     * @brief Clears the state of the entire allocator and marks all ids as free
     *
     */
    void releaseAll();

private:
    T nextId;
    T maxId;
    std::vector<bool> usedIds;
    container::RingQueue<T> freeIds;
};

template<typename T>
IdAllocator<T>::IdAllocator(std::size_t maxIdCount)
: nextId(0)
, maxId(0)
, usedIds(maxIdCount, false)
, freeIds(maxIdCount) {}

template<typename T>
constexpr T IdAllocator<T>::highestId() const {
    return maxId;
}

template<typename T>
constexpr const std::vector<bool>& IdAllocator<T>::allocatedIds() const {
    return usedIds;
}

template<typename T>
bool IdAllocator<T>::available() const {
    return nextId < freeIds.capacity() || !freeIds.empty();
}

template<typename T>
T IdAllocator<T>::allocate() {
    T id = nextId;
    if (!freeIds.empty()) {
        id = freeIds.front();
        freeIds.pop();
    }
    else if (nextId < freeIds.capacity()) {
        ++nextId;
    }

    maxId       = std::max(maxId, id);
    usedIds[id] = true;
    return id;
}

template<typename T>
void IdAllocator<T>::release(T id) {
    freeIds.push(id);
    usedIds[id] = false;
    if (id == maxId) {
        do { --maxId; } while (!usedIds[maxId]);
    }
}

template<typename T>
void IdAllocator<T>::releaseAll() {
    freeIds.clear();
    nextId = 0;
    maxId  = 0;
    usedIds.clear();
    usedIds.resize(freeIds.capacity(), false);
}

} // namespace util
} // namespace bl

#endif
