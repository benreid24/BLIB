#ifndef BLIB_CONTAINERS_OBJECTWRAPPER_HPP
#define BLIB_CONTAINERS_OBJECTWRAPPER_HPP

#include <utility>

namespace bl
{
namespace container
{
/**
 * @brief Simple wrapper around single objects allowing for delayed construction, memory reuse, and
 *        destruction at any time. Note that space is allocated inline with the object and will
 *        therefor be wherever ObjectWrapper is allocated (stack or heap). Because of this, moving
 *        from another ObjectWrapper will invoke T's move constructor instead of swapping memory
 *
 * @tparam T The type of object to wrap
 */
template<typename T>
class ObjectWrapper {
public:
    /**
     * @brief Creates an empty wrapper with no value
     *
     */
    ObjectWrapper();

    /**
     * @brief Creates a wrapper and copies the value from copy, if any
     *
     * @param copy The wrapper to copy
     */
    ObjectWrapper(const ObjectWrapper& copy);

    /**
     * @brief Creates the wrapper with the given object value, copied in
     *
     * @param obj The object to copy in
     */
    ObjectWrapper(const T& obj);

    /**
     * @brief Assumes ownership of the object inside of copy, if any
     *
     * @param copy The wrapper to move objects from
     */
    ObjectWrapper(ObjectWrapper&& copy);

    /**
     * @brief Constructs the internal object by moving from the given value
     *
     * @param obj The object to move into the wrapper
     */
    ObjectWrapper(T&& obj);

    /**
     * @brief Constructs the wrapped object inplace
     *
     * @tparam TArgs The constructor argument types for T
     * @param args The constructor arguments for T
     */
    template<typename... TArgs>
    ObjectWrapper(TArgs... args);

    /**
     * @brief Destroys thw wrapped object, if any
     *
     */
    ~ObjectWrapper();

    /**
     * @brief Copies the given object wrapper and its wrapped object, if any
     *
     * @param copy The wrapper to copy
     * @return ObjectWrapper& A reference to this wrapper
     */
    ObjectWrapper& operator=(const ObjectWrapper& copy);

    /**
     * @brief Moves from the given object wrapper, taking its object if any
     *
     * @param copy The wrapper to move from
     * @return ObjectWrapper& A reference to this object
     */
    ObjectWrapper& operator=(ObjectWrapper&& copy);

    /**
     * @brief If no object is contained then a new one is copy constructed inplace. If an object is
     *        already contained then its assignment operator is invoked
     *
     * @param obj The value to copy into the wrapper
     * @return ObjectWrapper& A reference to this wrapper
     */
    ObjectWrapper& operator=(const T& obj);

    /**
     * @brief If no object is contained then a new one is move constructed inplace. If an object is
     *        already contained then its move operator is invoked
     *
     * @param obj The value to move into the wrapper
     * @return ObjectWrapper& A reference to this wrapper
     */
    ObjectWrapper& operator=(T&& obj);

    /**
     * @brief If no object is contained then a new one is copy constructed inplace. If an object is
     *        already contained then its assignment operator is invoked
     *
     * @param obj The value to copy into the wrapper
     */
    void assign(const T& value);

    /**
     * @brief If no object is contained then a new one is move constructed inplace. If an object is
     *        already contained then its move operator is invoked
     *
     * @param obj The value to move into the wrapper
     */
    void move(T&& obj);

    /**
     * @brief Constructs a new object inplace in the wrapper. If an object is already stored it is
     *        destructed first
     *
     * @tparam TArgs The argument types for T's constructor
     * @param args The arguments to pass to T's constructor
     */
    template<typename... TArgs>
    void emplace(TArgs... args);

    /**
     * @brief Destroyes the contained ojbect, if any
     *
     */
    void destroy();

    /**
     * @brief Returns a reference to the contained object
     *
     */
    T& get();

    /**
     * @brief Returns a reference to the contained object
     *
     */
    const T& get() const;

    /**
     * @brief Returns a pointer to the contained object
     *
     */
    T* ptr();

    /**
     * @brief Returns a pointer to the contained object
     *
     */
    const T* ptr() const;

    /**
     * @brief Returns whether or not the wrapper currently contains an object
     *
     * @return true if any object is in the wrapper, false if empty
     */
    bool hasValue() const;

private:
    bool _alive;
    char buf[sizeof(T)];
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
ObjectWrapper<T>::ObjectWrapper()
: _alive(false) {}

template<typename T>
ObjectWrapper<T>::ObjectWrapper(const ObjectWrapper& copy)
: ObjectWrapper() {
    *this = copy;
}

template<typename T>
ObjectWrapper<T>::ObjectWrapper(const T& val)
: ObjectWrapper() {
    assign(val);
}

template<typename T>
ObjectWrapper<T>::ObjectWrapper(ObjectWrapper&& mv)
: ObjectWrapper() {
    *this = std::forward<ObjectWrapper<T>>(mv);
}

template<typename T>
ObjectWrapper<T>::ObjectWrapper(T&& mv)
: ObjectWrapper() {
    move(std::forward<T>(mv));
}

template<typename T>
template<typename... TArgs>
ObjectWrapper<T>::ObjectWrapper(TArgs... args)
: ObjectWrapper() {
    emplace<TArgs...>(args...);
}

template<typename T>
ObjectWrapper<T>& ObjectWrapper<T>::operator=(const ObjectWrapper& copy) {
    if (copy.hasValue()) { assign(copy.get()); }
    else {
        destroy();
    }
    return *this;
}

template<typename T>
ObjectWrapper<T>& ObjectWrapper<T>::operator=(const T& val) {
    assign(val);
    return *this;
}

template<typename T>
ObjectWrapper<T>& ObjectWrapper<T>::operator=(ObjectWrapper&& mv) {
    if (mv.hasValue()) {
        move(std::forward<T>(mv.get()));
        mv.destroy();
    }
    else {
        destroy();
    }
    return *this;
}

template<typename T>
ObjectWrapper<T>& ObjectWrapper<T>::operator=(T&& val) {
    move(std::forward<T>(val));
    return *this;
}

template<typename T>
ObjectWrapper<T>::~ObjectWrapper() {
    destroy();
}

template<typename T>
void ObjectWrapper<T>::assign(const T& val) {
    if (hasValue()) { get() = val; }
    else {
        T* nodiscard = new (ptr()) T(val);
        (void)nodiscard;
        _alive = true;
    }
}

template<typename T>
void ObjectWrapper<T>::move(T&& val) {
    if (hasValue()) { get() = std::forward<T>(val); }
    else {
        T* nodiscard = new (ptr()) T(std::forward<T>(val));
        (void)nodiscard;
        _alive = true;
    }
}

template<typename T>
template<typename... TArgs>
void ObjectWrapper<T>::emplace(TArgs... args) {
    destroy();
    T* nodiscard = new (ptr()) T(args...);
    (void)nodiscard;
    _alive = true;
}

template<typename T>
void ObjectWrapper<T>::destroy() {
    if (hasValue()) {
        get().~T();
        _alive = false;
    }
}

template<typename T>
T& ObjectWrapper<T>::get() {
    return *ptr();
}

template<typename T>
const T& ObjectWrapper<T>::get() const {
    return *ptr();
}

template<typename T>
T* ObjectWrapper<T>::ptr() {
    return static_cast<T*>(static_cast<void*>(buf));
}

template<typename T>
const T* ObjectWrapper<T>::ptr() const {
    return static_cast<const T*>(static_cast<const void*>(buf));
}

template<typename T>
bool ObjectWrapper<T>::hasValue() const {
    return _alive;
}

} // namespace container
} // namespace bl

#endif
