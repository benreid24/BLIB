#ifndef BLIB_RENDER_UTIL_REF_HPP
#define BLIB_RENDER_UTIL_REF_HPP

#include <vector>

namespace bl
{
namespace render
{
/**
 * @brief Utility struct to provide a stable iterator into a vector where indices do not change
 *
 * @tparam T The type contained within the vector to index into
 * @ingroup Renderer
 */
template<typename T>
class Ref {
public:
    /// @brief Deleted
    Ref() = delete;

    /**
     * @brief Construct a new from a vector and index
     *
     * @param storage The vector to index into
     * @param index The index to refer to
     */
    Ref(std::vector<T>& storage, std::size_t index);

    /// @brief Copy the Ref
    Ref(const Ref&) = default;

    /// @brief Copy the Ref
    Ref(Ref&&) = default;

    /// @brief Copy the Ref
    Ref& operator=(const Ref&) = default;

    /// @brief Copy the Ref
    Ref& operator=(Ref&&) = default;

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
     * @brief Returns the index of this Ref
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
Ref<T>::Ref(std::vector<T>& s, std::size_t i)
: storage(&s)
, i(i) {}

template<typename T>
constexpr T& Ref<T>::operator*() {
    return (*storage)[i];
}

template<typename T>
constexpr const T& Ref<T>::operator*() const {
    return (*storage)[i];
}

template<typename T>
constexpr T* Ref<T>::operator->() {
    return &(*storage)[i];
}

template<typename T>
constexpr const T* Ref<T>::operator->() const {
    return &(*storage)[i];
}

template<typename T>
constexpr std::size_t Ref<T>::index() const {
    return i;
}

} // namespace render
} // namespace bl

#endif
