#ifndef BLIB_CONTAINERS_OBJECTWRAPPER_HPP
#define BLIB_CONTAINERS_OBJECTWRAPPER_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <utility>

namespace bl
{
namespace ctr
{
/**
 * @brief Thin wrapper over objects that functions similar to std::optional but does not have any
 *        overhead. Does not store the presence state of the object contained. That must be done by
 *        the user. Intended to be used within other data structures that can know if elements are
 *        constructed or not
 *
 * @tparam T The type of object to wrap
 * @ingroup Containers
 */
template<typename T>
class ObjectWrapper {
public:
    /**
     * @brief Construct a new Object Wrapper with no value
     *
     */
    ObjectWrapper() = default;

    /**
     * @brief Copies the raw data from the given object wrapper. Does not call copy constructor of
     *        contained value
     *
     * @param copy The value to copy from
     */
    ObjectWrapper(const ObjectWrapper& copy);

    /**
     * @brief Copies the raw data from the given object wrapper. Does not call move constructor of
     *        contained value
     *
     * @param copy The value to copy from
     */
    ObjectWrapper(ObjectWrapper&& copy);

    /**
     * @brief Constructs the contained value
     *
     * @tparam TArgs The argument types to the contained type's constructor
     * @param tag Dummy tag to disambiguate this constructor
     * @param args The arguments to construct with
     */
    template<typename... TArgs>
    ObjectWrapper(std::in_place_t tag, TArgs&&... args);

    /**
     * @brief Access the wrapped value. Undefined behavior if not constructed
     *
     * @return T& The contained value
     */
    constexpr T& get();

    /**
     * @brief Access the wrapped value. Undefined behavior if not constructed
     *
     * @return T& The contained value
     */
    constexpr const T& get() const;

    /**
     * @brief Returns an RValue reference to the contained object
     *
     * @return constexpr T&& A movable rvalue reference
     */
    constexpr T&& getRValue();

    /**
     * @brief Constructs a new value in place. Does not destruct the prior value if any
     *
     * @tparam TArgs The argument types to the contained type's constructor
     * @param args The arguments to construct with
     */
    template<typename... TArgs>
    void emplace(TArgs&&... args);

    /**
     * @brief Destructs the contained value. Undefined behavior if no contained value
     *
     */
    void destroy();

private:
    union Buffer {
        std::uint8_t dummy;
        T object;

        Buffer()
        : dummy(0) {}
        Buffer(const Buffer& buf)
        : dummy(0) {
            std::memcpy(&object, &buf.object, sizeof(T));
        }
        Buffer(Buffer&& buf)
        : dummy(0) {
            std::memcpy(&object, &buf.object, sizeof(T));
        }
        ~Buffer() {}
    } buffer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
ObjectWrapper<T>::ObjectWrapper(const ObjectWrapper& copy)
: buffer(copy.buffer) {}

template<typename T>
ObjectWrapper<T>::ObjectWrapper(ObjectWrapper&& copy)
: buffer(std::forward<Buffer>(copy.buffer)) {}

template<typename T>
template<typename... TArgs>
ObjectWrapper<T>::ObjectWrapper(std::in_place_t, TArgs&&... args) {
    new (&buffer.object) T(std::forward<TArgs>(args)...);
}

template<typename T>
constexpr T& ObjectWrapper<T>::get() {
    return buffer.object;
}

template<typename T>
constexpr const T& ObjectWrapper<T>::get() const {
    return buffer.object;
}

template<typename T>
constexpr T&& ObjectWrapper<T>::getRValue() {
    return std::move(buffer.object);
}

template<typename T>
template<typename... TArgs>
void ObjectWrapper<T>::emplace(TArgs&&... args) {
    new (&buffer.object) T(std::forward<TArgs>(args)...);
}

template<typename T>
void ObjectWrapper<T>::destroy() {
    buffer.object.~T();
}

} // namespace ctr
} // namespace bl

#endif
