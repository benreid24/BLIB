#ifndef BLIB_ALLOCATORS_VECTOREDPOOLS_HPP
#define BLIB_ALLOCATORS_VECTOREDPOOLS_HPP

#include <BLIB/Util/RangeAllocator.hpp>
#include <list>

namespace bl
{
namespace alloc
{
/**
 * @brief Paged memory allocator that allocates pages that are double the previous max size page as
 *        necessary. Does not release empty pages until destructed
 *
 * @tparam T The element type to allocate
 * @tparam TAlloc The underlying allocator type to use for page allocations
 * @ingroup Allocators
 */
template<typename T, template<typename U> typename TAlloc = std::allocator>
class VectoredPools {
public:
    static constexpr std::size_t AlignedSize = sizeof(T);

    using value_type     = T;
    using size_type      = std::size_t;
    using different_type = std::ptrdiff_t;

    /**
     * @brief Creates the allocator
     * @param initialPageSize The number of elements to fit in the first page
     */
    VectoredPools(std::size_t initialPageSize = 64);

    /**
     * @brief Copy assignment is not valid
     */
    VectoredPools(const VectoredPools&) = delete;

    /**
     * @brief Move assignment
     */
    VectoredPools(VectoredPools&&) = default;

    /**
     * @brief Releases all pages back to the underlyinng allocator
     */
    ~VectoredPools() = default;

    /**
     * @brief Copy assignment is not valid
     */
    VectoredPools& operator=(const VectoredPools&) = delete;

    /**
     * @brief Move assignment
     */
    VectoredPools& operator=(VectoredPools&&) = default;

    /**
     * @brief Allocates memory for new objects
     *
     * @param count The number of objects to allocate memory for
     * @return A pointer to the allocated memory
     */
    T* allocate(std::size_t count);

    /**
     * @brief Marks the given allocation as available for future allocations
     *
     * @param ptr Pointer to the memory to release
     * @param count The number of elements in the block to release
     */
    void deallocate(T* const ptr, std::size_t count);

    /**
     * @brief Tests whether this allocator is the same as another
     *
     * @param other The allocator to test against
     * @return True if they are the same instance, false otherwise
     */
    bool operator==(const VectoredPools& other) const;

private:
    struct Page {
        TAlloc<T>& alloc;
        const std::size_t elementCount;
        void* pool;
        util::RangeAllocator<std::size_t> freeSlots;

        Page(TAlloc<T>& alloc, std::size_t count)
        : alloc(alloc)
        , elementCount(count)
        , freeSlots(count) {
            pool = alloc.allocate(count);
        }

        ~Page() { alloc.deallocate(static_cast<T*>(pool), elementCount); }

        T* allocate(std::size_t count) {
            const auto range = freeSlots.alloc(count);
            if (range.size != count) { return nullptr; }
            return static_cast<T*>(
                static_cast<void*>(static_cast<char*>(pool) + range.start * AlignedSize));
        }

        bool contains(T* const ptr) const {
            return (ptr - static_cast<const T*>(pool)) < elementCount;
        }

        void release(T* const ptr, std::size_t count) {
            const std::size_t diff = static_cast<char*>(static_cast<void*>(ptr)) - pool;
            const std::size_t i    = diff / AlignedSize;
            freeSlots.release({i, count});
        }
    };

    TAlloc<T> alloc;
    std::list<Page, TAlloc<Page>> pages;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, template<typename U> typename TAlloc>
VectoredPools<T, TAlloc>::VectoredPools(std::size_t initialPageSize) {
    pages.emplace_back(initialPageSize);
}

template<typename T, template<typename U> typename TAlloc>
T* VectoredPools<T, TAlloc>::allocate(std::size_t count) {
    for (auto& page : pages) {
        T* result = page.allocate(count);
        if (result) { return result; }
    }

    pages.emplace_back(alloc, std::max(pages.back().elementCount * 2, count));
    return pages.back().allocate(count);
}

template<typename T, template<typename U> typename TAlloc>
void VectoredPools<T, TAlloc>::deallocate(T* const ptr, std::size_t count) {
    for (auto& page : pages) {
        if (page.contains(ptr)) {
            page.release(ptr, count);
            return;
        }
    }
}

template<typename T, template<typename U> typename TAlloc>
bool VectoredPools<T, TAlloc>::operator==(const VectoredPools& other) const {
    return this == &other;
}

} // namespace alloc
} // namespace bl

namespace std
{
template<typename T, template<typename U> typename TAlloc>
struct allocator_traits<bl::alloc::VectoredPools<T, TAlloc>> {
    using allocator_type = bl::alloc::VectoredPools<T, TAlloc>;
    using value_type     = T;

    using pointer            = value_type*;
    using const_pointer      = const value_type*;
    using void_pointer       = void*;
    using const_void_pointer = const void*;

    using size_type       = size_t;
    using difference_type = ptrdiff_t;

    using propagate_on_container_copy_assignment = false_type;
    using propagate_on_container_move_assignment = true_type;
    using propagate_on_container_swap            = true_type;
    using is_always_equal                        = false_type;

    template<class _Other>
    using rebind_alloc = allocator<_Other>;

    template<class _Other>
    using rebind_traits = allocator_traits<allocator<_Other>>;
};

} // namespace std

#endif
