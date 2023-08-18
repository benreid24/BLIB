#ifndef BLIB_UTIL_RANGEALLOCATORUNBOUNDED_HPP
#define BLIB_UTIL_RANGEALLOCATORUNBOUNDED_HPP

#include <BLIB/Logging.hpp>
#include <limits>
#include <type_traits>
#include <vector>

namespace bl
{
namespace util
{
template<typename T>
class RangeAllocatorUnbounded {
    static_assert(std::is_integral_v<T>, "T must be an integer type");
    static_assert(std::is_unsigned_v<T>, "T must be unsigned");

public:
    struct Range {
        T start;
        T size;
    };

    struct AllocResult {
        Range range;
        bool poolExpanded;
        T newPoolSize;
    };

    RangeAllocatorUnbounded(T initialCapacity = 0);

    AllocResult alloc(T size);

    void release(Range range);

    void releaseAll();

    constexpr T poolSize() const;

private:
    T totalSize;
    std::vector<Range> freeRanges;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
RangeAllocatorUnbounded<T>::RangeAllocatorUnbounded(T cap)
: totalSize(cap) {
    freeRanges.reserve(std::max(cap / 8, static_cast<T>(16)));
    freeRanges.emplace_back(0, cap);
}

template<typename T>
RangeAllocatorUnbounded<T>::AllocResult RangeAllocatorUnbounded<T>::alloc(T size) {
    // find smallest free range that fits
    Range* allocFrom = nullptr;
    Range* backRange = nullptr;
    for (Range& range : freeRanges) {
        if (range.start + range.size == totalSize) { backRange = &range; }
        if (range.size >= size) {
            if (!allocFrom || allocFrom->size > range.size) { allocFrom = &range; }
        }
    }

    AllocResult result;
    result.range.size  = size;
    result.newPoolSize = totalSize;

    // alloc from range and update range
    if (allocFrom) {
        result.poolExpanded = false;
        result.range.start  = allocFrom->start;
        if (allocFrom->size > size) {
            allocFrom->start += size;
            allocFrom->size -= size;
        }
        else {
            const std::size_t i = allocFrom - freeRanges.data();
            freeRanges.erase(freeRanges.begin() + i);
        }
    }
    // expand pool if no free range large enough
    else {
        result.poolExpanded = true;
        const T credit      = backRange ? backRange->size : 0;
        const T newSize     = std::max(totalSize * 2, totalSize + size - credit);
        const T expandedBy  = newSize - totalSize;
        result.newPoolSize  = newSize;
        if (backRange) {
            backRange->size += expandedBy;
            result.range.start = backRange->start;
            backRange->start += size;
            backRange->size -= size;
        }
        else {
            result.range.start = totalSize;
            if (expandedBy > size) { freeRanges.emplace_back(totalSize + size, expandedBy - size); }
        }
        totalSize = newSize;
    }

    return result;
}

template<typename T>
void RangeAllocatorUnbounded<T>::release(Range range) {
    // find adjacent range(s)
    Range* frontCon = nullptr;
    Range* backCon  = nullptr;
    for (Range& fr : freeRanges) {
        if (fr.start + fr.size == range.start) { frontCon = &fr; }
        else if (range.start + range.size == fr.start) { backCon = &fr; }
    }

    // expand or combine ranges if found
    if (frontCon && backCon) {
        frontCon->size += backCon->size + range.size;
        const std::size_t bi = backCon - freeRanges.data();
        freeRanges.erase(freeRanges.begin() + bi);
    }
    else if (frontCon) { frontCon->size += range.size; }
    else if (backCon) {
        backCon->start -= range.size;
        backCon->size += range.size;
    }
    else { freeRanges.emplace_back(range); }
}

template<typename T>
void RangeAllocatorUnbounded<T>::releaseAll() {
    freeRanges.clear();
    freeRanges.emplace_back(0, totalSize);
}

template<typename T>
constexpr T RangeAllocatorUnbounded<T>::poolSize() const {
    return totalSize;
}

} // namespace util
} // namespace bl

#endif
