#ifndef BLIB_UTIL_REFCOUNTED_HPP
#define BLIB_UTIL_REFCOUNTED_HPP

#include <list>
#include <memory>
#include <type_traits>

namespace bl
{
namespace util
{
template<typename T>
class RefPool;

namespace priv
{
template<typename T>
struct TypeDeducer {};

template<typename T>
struct Payload {
    RefPool<T>* owner;
    std::unique_ptr<T> value;
    unsigned int refCount;

    template<typename U, typename... TArgs>
    Payload(RefPool<T>* owner, TypeDeducer<U>, TArgs&&... args)
    : owner(owner)
    , refCount(0)
    , value(std::make_unique<U>(std::forward<TArgs>(args)...)) {}
};

} // namespace priv

/**
 * @brief Utility for lightweight ref counting of resources
 *
 * @tparam T The type of resource being ref counted
 * @ingroup Util
 */
template<typename T>
class Ref {
public:
    /**
     * @brief Creates an invalid ref
     */
    Ref()
    : value(nullptr)
    , payload(nullptr) {}

    /**
     * @brief Initializes this ref from another
     *
     * @param other The ref to copy
     */
    Ref(const Ref& other)
    : value(other.value)
    , payload(other.payload) {
        increment();
    }

    /**
     * @brief Takes over ownership from another ref
     *
     * @param other The ref to assume ownership from
     */
    Ref(Ref&& other)
    : value(other.value)
    , payload(other.payload) {
        other.value   = nullptr;
        other.payload = nullptr;
    }

    /**
     * @brief Releases the ref and possibly the resource if the ref count is zero
     */
    ~Ref() { decrement(); }

    /**
     * @brief Initializes this ref from another
     *
     * @param other The ref to copy
     * @return A reference to this object
     */
    Ref& operator=(const Ref& other) {
        decrement();
        value   = other.value;
        payload = other.payload;
        if (value) { increment(); }
        return *this;
    }

    /**
     * @brief Takes over ownership from another ref
     *
     * @param other The ref to assume ownership from
     * @return A reference to this object
     */
    Ref& operator=(Ref&& other) {
        decrement();
        value         = other.value;
        payload       = other.payload;
        other.value   = nullptr;
        other.payload = nullptr;
        return *this;
    }

    /**
     * @brief Access the underlying resource value
     */
    T& operator*() { return *value; }

    /**
     * @brief Access the underlying resource value
     */
    const T& operator*() const { return *value; }

    /**
     * @brief Access the underlying resource value
     */
    T* operator->() { return value; }

    /**
     * @brief Access the underlying resource value
     */
    const T* operator->() const { return value; }

    /**
     * @brief Access the underlying resource value
     */
    T* get() { return value; }

    /**
     * @brief Access the underlying resource value
     */
    const T* get() const { return value; }

    /**
     * @brief Returns the number of refs pointing to the underlying resource
     */
    unsigned int refCount() const { return payload->refCount; }

    /**
     * @brief Returns whether this ref points to a resource or not
     */
    bool isValid() const { return value != nullptr; }

    /**
     * @brief Returns whether this ref points to a resource or not
     */
    operator bool() const { return value != nullptr; }

    /**
     * @brief Releases this handle on the underlying resource
     */
    void release() {
        if (value) {
            decrement();
            value   = nullptr;
            payload = nullptr;
        }
    }

private:
    T* value;
    priv::Payload<T>* payload;

    Ref(priv::Payload<T>* payload)
    : value(payload->value.get())
    , payload(payload) {
        increment();
    }

    void increment();
    void decrement();

    friend class RefPool<T>;
};

/**
 * @brief Utility to manage a pool of ref counted resources
 *
 * @tparam T The type of resource being managed
 * @tparam TInject Optional arguments to inject into each resource construction
 * @ingroup Util
 */
template<typename T>
class RefPool {
public:
    /**
     * @brief Creates a new resource
     *
     * @tparam ...TArgs Argument types to the resources constructor
     * @param ...args Arguments to the resources constructor
     * @return A ref to the newly created resource
     */
    template<typename... TArgs>
    Ref<T> emplace(TArgs&&... args);

    /**
     * @brief Creates a new resource from a derived type
     *
     * @tparam TDerived The derived type to create
     * @tparam ...TArgs Argument types to the resources constructor
     * @param ...args Arguments to the resources constructor
     * @return A ref to the newly created resource
     */
    template<typename TDerived, typename... TArgs>
    Ref<T> emplaceDerived(TArgs&&... args);

    /**
     * @brief Clears all managed resources, ignoring ref counts
     */
    void clear();

private:
    std::list<priv::Payload<T>> storage;

    void release(priv::Payload<T>* value);

    friend class Ref<T>;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
void Ref<T>::increment() {
    ++payload->refCount;
}

template<typename T>
void Ref<T>::decrement() {
    if (value) {
        --payload->refCount;
        if (payload->refCount == 0) { payload->owner->release(payload); }
    }
}

template<typename T>
inline void RefPool<T>::clear() {}

template<typename T>
void RefPool<T>::release(priv::Payload<T>* value) {
    for (auto it = storage.begin(); it != storage.end(); ++it) {
        if (&*it == value) {
            storage.erase(it);
            return;
        }
    }
}

template<typename T>
template<typename... TArgs>
inline Ref<T> RefPool<T>::emplace(TArgs&&... args) {
    auto& created =
        storage.emplace_back(this, priv::TypeDeducer<T>(), std::forward<TArgs>(args)...);
    return Ref<T>(&created);
}

template<typename T>
template<typename TDerived, typename... TArgs>
inline Ref<T> RefPool<T>::emplaceDerived(TArgs&&... args) {
    static_assert(std::is_base_of_v<T, TDerived>, "TDerived must derive from T");

    auto& created =
        storage.emplace_back(this, priv::TypeDeducer<TDerived>(), std::forward<TArgs>(args)...);
    return Ref<T>(&created);
}

} // namespace util
} // namespace bl

#endif
