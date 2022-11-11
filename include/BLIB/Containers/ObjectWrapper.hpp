#ifndef BLIB_CONTAINERS_OBJECTWRAPPER_HPP
#define BLIB_CONTAINERS_OBJECTWRAPPER_HPP

#include <cstdint>

namespace bl
{
namespace container
{
/**
 * @brief Thin wrapper over objects that functions similar to std::optional but does not have any
 *        overhead. Does not store the presence state of the object contained. That must be done by
 *        the user. Intended to be used within other datastructures that can know if elements are
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
     * @brief Constructs the contained value
     *
     * @tparam TArgs The argument types to the contained type's constructor
     * @param args The arguments to construct with
     */
    template<typename... TArgs>
    ObjectWrapper(TArgs... args);

    /**
     * @brief Access the wrapped value. Undefined behavior if not constructed
     *
     * @return T& The contained value
     */
    T& get();

    /**
     * @brief Access the wrapped value. Undefined behavior if not constructed
     *
     * @return T& The contained value
     */
    const T& get() const;

    /**
     * @brief Constructs a new value in place. Does not destruct the prior value if any
     *
     * @tparam TArgs The argument types to the contained type's constructor
     * @param args The arguments to construct with
     */
    template<typename... TArgs>
    void emplace(TArgs... args);

    /**
     * @brief Destructs the contained value. Undefined behavior if no contained value
     *
     */
    void destroy();

private:
    std::uint8_t buffer[sizeof(T)];

    T* cast();
    const T* cast() const;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
template<typename... TArgs>
ObjectWrapper<T>::ObjectWrapper(TArgs... args) {
    new (cast()) T(args...);
}

template<typename T>
T& ObjectWrapper<T>::get() {
    return *cast();
}

template<typename T>
const T& ObjectWrapper<T>::get() const {
    return *cast();
}

template<typename T>
template<typename... TArgs>
void ObjectWrapper<T>::emplace(TArgs... args) {
    new (cast()) T(args...);
}

template<typename T>
void ObjectWrapper<T>::destroy() {
    cast()->~T();
}

template<typename T>
T* ObjectWrapper<T>::cast() {
    return static_cast<T*>(static_cast<void*>(buffer));
}

template<typename T>
const T* ObjectWrapper<T>::cast() const {
    return static_cast<const T*>(static_cast<const void*>(buffer));
}

} // namespace container
} // namespace bl

#endif
