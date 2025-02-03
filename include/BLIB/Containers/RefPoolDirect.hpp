#ifndef BLIB_CONTAINERS_REFPOOLDIRECT_HPP
#define BLIB_CONTAINERS_REFPOOLDIRECT_HPP

#include <list>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace bl
{
namespace ctr
{
template<typename T>
class RefPoolDirect;

template<typename T>
class RefDirect;

namespace priv
{

template<typename T>
struct PayloadDirect {
    RefPoolDirect<T>* owner;
    T value;
    unsigned int refCount;

    template<typename... TArgs>
    PayloadDirect(RefPoolDirect<T>* owner, TArgs&&... args)
    : owner(owner)
    , refCount(0)
    , value(std::forward<TArgs>(args)...) {}

    RefDirect<T> makeRef();
};

} // namespace priv

/**
 * @brief Utility for lightweight ref counting of resources
 *
 * @tparam T The type of resource being ref counted
 * @ingroup Containers
 */
template<typename T>
class RefDirect {
public:
    /**
     * @brief Creates an invalid ref
     */
    RefDirect()
    : payload(nullptr) {}

    /**
     * @brief Initializes this ref from another
     *
     * @tparam TOther The derived type of the other ref
     * @param other The ref to copy
     */
    RefDirect(const RefDirect& other)
    : RefDirect() {
        *this = other;
    }

    /**
     * @brief Takes over ownership from another ref
     *
     * @param other The ref to assume ownership from
     */
    RefDirect(RefDirect&& other)
    : RefDirect() {
        *this = std::forward<RefDirect>(other);
    }

    /**
     * @brief Releases the ref and possibly the resource if the ref count is zero
     */
    ~RefDirect() { release(); }

    /**
     * @brief Initializes this ref from another
     *
     * @param other The ref to copy
     * @return A reference to this object
     */
    RefDirect& operator=(const RefDirect& other) {
        decrement();
        payload = other.payload;
        if (payload) { increment(); }
        return *this;
    }

    /**
     * @brief Takes over ownership from another ref
     *
     * @param other The ref to assume ownership from
     * @return A reference to this object
     */
    RefDirect& operator=(RefDirect&& other) {
        decrement();
        payload       = other.payload;
        other.payload = nullptr;
        return *this;
    }

    /**
     * @brief Access the underlying resource value
     */
    T& operator*() { return payload->value; }

    /**
     * @brief Access the underlying resource value
     */
    const T& operator*() const { return payload->value; }

    /**
     * @brief Access the underlying resource value
     */
    T* operator->() { return &payload->value; }

    /**
     * @brief Access the underlying resource value
     */
    const T* operator->() const { return &payload->value; }

    /**
     * @brief Access the underlying resource value
     */
    T* get() { return &payload->value; }

    /**
     * @brief Access the underlying resource value
     */
    const T* get() const { return &payload->value; }

    /**
     * @brief Returns the number of refs pointing to the underlying resource
     */
    unsigned int refCount() const { return payload->refCount; }

    /**
     * @brief Returns whether this ref points to a resource or not
     */
    bool isValid() const { return payload != nullptr; }

    /**
     * @brief Returns whether this ref points to a resource or not
     */
    operator bool() const { return payload != nullptr; }

    /**
     * @brief Releases this handle on the underlying resource
     */
    void release() {
        if (payload) {
            decrement();
            payload = nullptr;
        }
    }

private:
    priv::PayloadDirect<T>* payload;

    RefDirect(priv::PayloadDirect<T>* payload)
    : payload(payload) {
        increment();
    }

    void increment();
    void decrement();

    friend class RefPoolDirect<T>;
    friend struct priv::PayloadDirect<T>;
};

/**
 * @brief Utility to manage a pool of ref counted resources
 *
 * @tparam T The type of resource being managed
 * @tparam TInject Optional arguments to inject into each resource construction
 * @ingroup Containers
 */
template<typename T>
class RefPoolDirect {
public:
    /**
     * @brief Basic iterator for the ref pool
     */
    class Iterator {
    public:
        /**
         * @brief Increments the iterator to the next value
         *
         * @return A reference to this object
         */
        Iterator& operator++() {
            ++it;
            return *this;
        }

        /**
         * @brief Increments the iterator to the next value and returns the previous value
         *
         * @param N/A
         * @return The value before the increment
         */
        Iterator operator++(int) {
            Iterator copy = *this;
            ++it;
            return copy;
        }

        /**
         * @brief Tests whether two iterators are equal
         *
         * @param other The iterator to test against
         * @return True if not equal, false otherwise
         */
        bool operator!=(const Iterator& other) const { return it != other.it; }

        /**
         * @brief Tests whether two iterators are equal
         *
         * @param other The iterator to test against
         * @return True if equal, false otherwise
         */
        bool operator==(const Iterator& other) const { return it == other.it; }

        /**
         * @brief Returns the underlying value
         */
        T& operator*() { return it->value; }

        /**
         * @brief Returns the underlying value
         */
        T* operator->() { return &it->value; }

        /**
         * @brief Create a ref to the value this iterator points to
         *
         * @return A newly created ref
         */
        RefDirect<T> makeRef() { return it->makeRef(); }

    private:
        typename std::list<priv::PayloadDirect<T>>::iterator it;

        Iterator(typename std::list<priv::PayloadDirect<T>>::iterator it)
        : it(it) {}

        friend class RefPoolDirect<T>;
    };

    /**
     * @brief Creates a new resource
     *
     * @tparam ...TArgs Argument types to the resources constructor
     * @param ...args Arguments to the resources constructor
     * @return A ref to the newly created resource
     */
    template<typename... TArgs>
    RefDirect<T> emplace(TArgs&&... args);

    /**
     * @brief Returns an iterator to the first value in the pool
     */
    Iterator begin() { return Iterator(storage.begin()); }

    /**
     * @brief Returns an iterator to the end of the pool
     */
    Iterator end() { return Iterator(storage.end()); }

    /**
     * @brief Clears all managed resources, ignoring ref counts
     */
    void clear();

private:
    std::list<priv::PayloadDirect<T>> storage;

    void release(priv::PayloadDirect<T>* value);

    friend class RefDirect<T>;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
void RefDirect<T>::increment() {
    ++payload->refCount;
}

template<typename T>
void RefDirect<T>::decrement() {
    if (payload) {
        --payload->refCount;
        if (payload->refCount == 0) { payload->owner->release(payload); }
    }
}

template<typename T>
void RefPoolDirect<T>::clear() {
    storage.clear();
}

template<typename T>
void RefPoolDirect<T>::release(priv::PayloadDirect<T>* value) {
    for (auto it = storage.begin(); it != storage.end(); ++it) {
        if (&*it == value) {
            storage.erase(it);
            return;
        }
    }
}

template<typename T>
template<typename... TArgs>
RefDirect<T> RefPoolDirect<T>::emplace(TArgs&&... args) {
    auto& created = storage.emplace_back(this, std::forward<TArgs>(args)...);
    return RefDirect<T>(&created);
}

namespace priv
{
template<typename T>
RefDirect<T> PayloadDirect<T>::makeRef() {
    return RefDirect<T>(this);
}
} // namespace priv

} // namespace ctr
} // namespace bl

#endif
