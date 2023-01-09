#ifndef BLIB_UTIL_VECTORREF_HPP
#define BLIB_UTIL_VECTORREF_HPP

#include <vector>

namespace bl
{
namespace render
{
/**
 * @brief Utility struct to provide a stable iterator into a vector where indices do not change
 *
 * @tparam T The type contained within the vector to index into
 * @ingroup Util
 */
template<typename T>
class VectorRef {
public:
    /// @brief Deleted
    VectorRef() = delete;

    /**
     * @brief Construct a new from a vector and index
     *
     * @param storage The vector to index into
     * @param index The index to refer to
     */
    VectorRef(std::vector<T>& storage, std::size_t index);

    /// @brief Copy the VectorRef
    VectorRef(const VectorRef&) = default;

    /// @brief Copy the VectorRef
    VectorRef(VectorRef&&) = default;

    /// @brief Copy the VectorRef
    VectorRef& operator=(const VectorRef&) = default;

    /// @brief Copy the VectorRef
    VectorRef& operator=(VectorRef&&) = default;

    /**
     * @brief Returns the value from the underlying array
     *
     * @return constexpr T& A reference to the referred-to value
     */
    constexpr T& operator*();

    /**
     * @brief Returns the value from the underlying array
     *
     * @return constexpr T& A reference to the referred-to value
     */
    constexpr const T& operator*() const;

    /**
     * @brief Returns the value from the underlying array
     *
     * @return constexpr T& A pointer to the referred-to value
     */
    constexpr T* operator->();

    /**
     * @brief Returns the value from the underlying array
     *
     * @return constexpr T& A pointer to the referred-to value
     */
    constexpr const T* operator->() const;

    /**
     * @brief Returns the index of this VectorRef
     *
     * @return constexpr std::size_t The index into the underlying vector
     */
    constexpr std::size_t index() const;

private:
    std::vector<T>* storage;
    std::size_t i;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename T>
VectorRef<T>::VectorRef(std::vector<T>& s, std::size_t i)
: storage(&s)
, i(i) {}

template<typename T>
constexpr T& VectorRef<T>::operator*() {
    return (*storage)[i];
}

template<typename T>
constexpr const T& VectorRef<T>::operator*() const {
    return (*storage)[i];
}

template<typename T>
constexpr T* VectorRef<T>::operator->() {
    return &(*storage)[i];
}

template<typename T>
constexpr const T* VectorRef<T>::operator->() const {
    return &(*storage)[i];
}

template<typename T>
constexpr std::size_t VectorRef<T>::index() const {
    return i;
}

} // namespace render
} // namespace bl

#endif
