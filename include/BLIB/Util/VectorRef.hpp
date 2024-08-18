#ifndef BLIB_UTIL_VECTORREF_HPP
#define BLIB_UTIL_VECTORREF_HPP

#include <vector>

namespace bl
{
namespace util
{
/**
 * @brief Utility struct to provide a stable iterator into a vector where indices do not change
 *
 * @tparam T The type contained within the vector to index into
 * @tparam TStorage The storage type. Defaults to std::vector. Only requires operator[]
 * @ingroup Util
 */
template<typename T, typename TStorage = std::vector<T>>
class VectorRef {
public:
    /**
     * @brief Creates a null reference
     */
    VectorRef();

    /**
     * @brief Construct a new from a vector and index
     *
     * @param storage The vector to index into
     * @param index The index to refer to
     */
    VectorRef(TStorage& storage, std::size_t index);

    /// @brief Copy the VectorRef
    VectorRef(const VectorRef&) = default;

    /// @brief Copy the VectorRef
    VectorRef(VectorRef&&) = default;

    /// @brief Copy the VectorRef
    VectorRef& operator=(const VectorRef&) = default;

    /// @brief Copy the VectorRef
    VectorRef& operator=(VectorRef&&) = default;

    /**
     * @brief Assigns a new vector and index
     *
     * @param storage The vector to index into
     * @param index The index to refer to
     */
    void assign(TStorage& storage, std::size_t index);

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

    /**
     * @brief Returns whether or not the ref has been assigned
     */
    constexpr bool valid() const;

private:
    TStorage* storage;
    std::size_t i;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename T, typename TStorage>
VectorRef<T, TStorage>::VectorRef()
: storage(nullptr)
, i(0) {}

template<typename T, typename TStorage>
VectorRef<T, TStorage>::VectorRef(TStorage& s, std::size_t i)
: storage(&s)
, i(i) {}

template<typename T, typename TStorage>
void VectorRef<T, TStorage>::assign(TStorage& s, std::size_t j) {
    storage = &s;
    i       = j;
}

template<typename T, typename TStorage>
constexpr T& VectorRef<T, TStorage>::operator*() {
    return (*storage)[i];
}

template<typename T, typename TStorage>
constexpr const T& VectorRef<T, TStorage>::operator*() const {
    return (*storage)[i];
}

template<typename T, typename TStorage>
constexpr T* VectorRef<T, TStorage>::operator->() {
    return &(*storage)[i];
}

template<typename T, typename TStorage>
constexpr const T* VectorRef<T, TStorage>::operator->() const {
    return &(*storage)[i];
}

template<typename T, typename TStorage>
constexpr std::size_t VectorRef<T, TStorage>::index() const {
    return i;
}

template<typename T, typename TStorage>
constexpr bool VectorRef<T, TStorage>::valid() const {
    return storage != nullptr && i < storage->size();
}

} // namespace util
} // namespace bl

#endif
