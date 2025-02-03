#ifndef BLIB_CONTAINERS_REFPOOL_HPP
#define BLIB_CONTAINERS_REFPOOL_HPP

#include <list>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace bl
{
namespace ctr
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
 * @tparam TDerived Derived type of T that this ref points to
 * @ingroup Containers
 */
template<typename T, typename TDerived = T>
class Ref {
    static_assert(std::is_base_of_v<T, TDerived>, "TDervied must derive from T");

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
     * @tparam TOther The derived type of the other ref
     * @param other The ref to copy
     */
    template<typename TOther>
    Ref(const Ref<T, TOther>& other)
    : Ref() {
        *this = other;
    }

    /**
     * @brief Initializes this ref from another
     *
     * @tparam TOther The derived type of the other ref
     * @param other The ref to copy
     */
    Ref(const Ref& other)
    : Ref() {
        *this = other;
    }

    /**
     * @brief Takes over ownership from another ref
     *
     * @param other The ref to assume ownership from
     */
    template<typename TOther>
    Ref(Ref<T, TOther>&& other)
    : Ref() {
        *this = std::forward<Ref<T, TOther>>(other);
    }

    /**
     * @brief Takes over ownership from another ref
     *
     * @param other The ref to assume ownership from
     */
    Ref(Ref&& other)
    : Ref() {
        *this = std::forward<Ref>(other);
    }

    /**
     * @brief Releases the ref and possibly the resource if the ref count is zero
     */
    ~Ref() { release(); }

    /**
     * @brief Initializes this ref from another
     *
     * @param other The ref to copy
     * @return A reference to this object
     */
    Ref& operator=(const Ref& other) {
        decrement();

        payload = other.payload;
        value   = other.value;

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

        payload = other.payload;
        value   = other.value;

        other.value   = nullptr;
        other.payload = nullptr;
        return *this;
    }

    /**
     * @brief Initializes this ref from another
     *
     * @param other The ref to copy
     * @return A reference to this object
     */
    template<typename TOther>
    Ref& operator=(const Ref<T, TOther>& other) {
        decrement();

        payload = other.payload;
        if constexpr (std::is_base_of_v<TDerived, TOther>) { value = other.value; }
        else {
            value = dynamic_cast<TDerived*>(other.value);
            if (other.value && !value) { throw std::runtime_error("Invalid ref assignment"); }
        }

        if (value) { increment(); }
        return *this;
    }

    /**
     * @brief Takes over ownership from another ref
     *
     * @param other The ref to assume ownership from
     * @return A reference to this object
     */
    template<typename TOther>
    Ref& operator=(Ref<T, TOther>&& other) {
        decrement();

        payload = other.payload;
        if constexpr (std::is_base_of_v<TDerived, TOther>) { value = other.value; }
        else {
            value = dynamic_cast<TDerived*>(other.value);
            if (other.value && !value) { throw std::runtime_error("Invalid ref assignment"); }
        }

        other.value   = nullptr;
        other.payload = nullptr;
        return *this;
    }

    /**
     * @brief Access the underlying resource value
     */
    TDerived& operator*() { return *value; }

    /**
     * @brief Access the underlying resource value
     */
    const TDerived& operator*() const { return *value; }

    /**
     * @brief Access the underlying resource value
     */
    TDerived* operator->() { return value; }

    /**
     * @brief Access the underlying resource value
     */
    const TDerived* operator->() const { return value; }

    /**
     * @brief Access the underlying resource value
     */
    TDerived* get() { return value; }

    /**
     * @brief Access the underlying resource value
     */
    const TDerived* get() const { return value; }

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
    TDerived* value;
    priv::Payload<T>* payload;

    Ref(priv::Payload<T>* payload)
    : value(static_cast<TDerived*>(payload->value.get()))
    , payload(payload) {
        increment();
    }

    void increment();
    void decrement();

    template<typename U, typename V>
    friend class Ref;
    friend class RefPool<T>;
};

/**
 * @brief Utility to manage a pool of ref counted resources
 *
 * @tparam T The type of resource being managed
 * @tparam TInject Optional arguments to inject into each resource construction
 * @ingroup Containers
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
    Ref<T, TDerived> emplaceDerived(TArgs&&... args);

    /**
     * @brief Clears all managed resources, ignoring ref counts
     */
    void clear();

private:
    std::list<priv::Payload<T>> storage;

    void release(priv::Payload<T>* value);

    template<typename U, typename V>
    friend class Ref;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, typename TDerived>
void Ref<T, TDerived>::increment() {
    ++payload->refCount;
}

template<typename T, typename TDerived>
void Ref<T, TDerived>::decrement() {
    if (value) {
        --payload->refCount;
        if (payload->refCount == 0) { payload->owner->release(payload); }
    }
}

template<typename T>
inline void RefPool<T>::clear() {
    storage.clear();
}

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
inline Ref<T, TDerived> RefPool<T>::emplaceDerived(TArgs&&... args) {
    static_assert(std::is_base_of_v<T, TDerived>, "TDerived must derive from T");

    auto& created =
        storage.emplace_back(this, priv::TypeDeducer<TDerived>(), std::forward<TArgs>(args)...);
    return Ref<T, TDerived>(&created);
}

} // namespace ctr
} // namespace bl

#endif
