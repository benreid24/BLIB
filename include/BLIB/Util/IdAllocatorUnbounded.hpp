#ifndef BLIB_UTIL_IDALLOCATORUNBOUNDED_HPP
#define BLIB_UTIL_IDALLOCATORUNBOUNDED_HPP

#include <algorithm>
#include <vector>

namespace bl
{
namespace util
{
/**
 * @brief Similar to IdAllocator, but with no max id restriction
 *
 * @tparam T The id type to allocate
 * @ingroup Util
 */
template<typename T>
class IdAllocatorUnbounded {
public:
    /**
     * @brief Creates a new id allocator
     */
    IdAllocatorUnbounded();

    /**
     * @brief Creates a new id allocator and pre-allocates some memory
     *
     * @param capacityHint Id capacity to pre-allocate
     */
    IdAllocatorUnbounded(std::size_t capacityHint);

    /**
     * @brief Allocates a new id from the allocator
     *
     * @return The new id
     */
    T allocate();

    /**
     * @brief Releases the given id back into the pool. Double-release is an error
     *
     * @param id The id to release
     */
    void release(T id);

    /**
     * @brief Releases all ids
     */
    void releaseAll();

    /**
     * @brief Returns whether or not the given id is allocated
     *
     * @param id The id to check
     * @return True if the id is allocated, false if free
     */
    constexpr bool isAllocated(T id) const;

    /**
     * @brief Returns the capacity of the id pool
     */
    constexpr T poolSize() const;

    /**
     * @brief Returns essentially maxId+1 for iteration
     */
    constexpr T endId() const;

private:
    T nextId;
    std::vector<bool> allocMap;
    std::vector<T> freeStack;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
IdAllocatorUnbounded<T>::IdAllocatorUnbounded()
: nextId(0) {}

template<typename T>
IdAllocatorUnbounded<T>::IdAllocatorUnbounded(std::size_t capacityHint)
: IdAllocatorUnbounded() {
    allocMap.resize(capacityHint, false);
    freeStack.reserve(capacityHint);
}

template<typename T>
T IdAllocatorUnbounded<T>::allocate() {
    T id;
    if (freeStack.empty()) {
        id = nextId++;
        allocMap.resize(static_cast<std::size_t>(id) + 1, false);
    }
    else {
        id = freeStack.back();
        freeStack.pop_back();
    }
    allocMap[id] = true;
    return id;
}

template<typename T>
void IdAllocatorUnbounded<T>::release(T id) {
    allocMap[id] = false;
    const auto it =
        std::lower_bound(freeStack.begin(), freeStack.end(), id, std::cmp_greater<T, T>);
    freeStack.emplace(it, id);
}

template<typename T>
void IdAllocatorUnbounded<T>::releaseAll() {
    nextId = 0;
    std::fill(allocMap.begin(), allocMap.end(), false);
    freeStack.clear();
}

template<typename T>
constexpr bool IdAllocatorUnbounded<T>::isAllocated(T id) const {
    return allocMap[id];
}

template<typename T>
constexpr T IdAllocatorUnbounded<T>::poolSize() const {
    return static_cast<T>(allocMap.size());
}

template<typename T>
constexpr T IdAllocatorUnbounded<T>::endId() const {
    return nextId;
}

} // namespace util
} // namespace bl

#endif
