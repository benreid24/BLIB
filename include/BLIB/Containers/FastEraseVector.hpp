#ifndef BLIB_CONTAINERS_FASTERASEVECTOR_HPP
#define BLIB_CONTAINERS_FASTERASEVECTOR_HPP

#include <vector>

namespace bl
{
/**
 * @brief A thin reskin of std::vector, this vector replaces the O(n) erase() methods of vector
 *        with a constant time implementation. It works by copying the last item into the
 *        position being erased, then popping the last item. This vector works best for items
 *        that are trivial to copy
 *
 * @tparam T Type of object to store
 * @tparam Alloc Allocator type to use, defaults to std::allocator<T>
 * @ingroup Containers
 */
template<class T, class Alloc = std::allocator<T>>
class FastEraseVector : public std::vector<T, Alloc> {
public:
    using iterator = typename std::vector<T, Alloc>::iterator;

    /**
     * @brief Erase a single element using an iterator
     *
     * @param position Element to erase
     */
    void erase(iterator position);

    /**
     * @brief Erase a single element using an index
     *
     * @param position Element to erase
     */
    void erase(std::size_t index);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<class T, class Alloc>
void FastEraseVector<T, Alloc>::erase(iterator position) {
    *position = std::vector<T, Alloc>::back();
    std::vector<T, Alloc>::pop_back();
}

template<class T, class Alloc>
void FastEraseVector<T, Alloc>::erase(std::size_t i) {
    (*this)[i] = std::vector<T, Alloc>::back();
    std::vector<T, Alloc>::pop_back();
}

} // namespace bl

#endif
