#ifndef BLIB_CONTAINERS_OBJECTPOOL_HPP
#define BLIB_CONTAINERS_OBJECTPOOL_HPP

#include <BLIB/Containers/ObjectWrapper.hpp>
#include <BLIB/Logging.hpp>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <vector>

namespace bl
{
namespace ctr
{
/**
 * @brief Basic object pool that may vary in size. Similar to FastEraseVector in that erase is O(1),
 *        however objects in the pool maintain their indices in the ObjectPool. Object slots
 *        are marked free on erase(), reused on add(), and the pool grows in size if no slot is
 *        when add() is called. Objects must implement a default constructor and copy constructor.
 *
 * @tparam T Type of object to store. Works best with small POD objects
 * @ingroup Containers
 */
template<typename T>
class ObjectPool {
    class Entry;

public:
    /// @brief Policy which dictates how the pool handles adding new objects when it is full
    enum struct GrowthPolicy {
        /// @brief The pool is reallocated  to a larger buffer. Addresses are not stable
        ExpandBuffer,

        /// @brief add() will fail and return end() when the pool is full. Addresses are stable
        FailContinue,

        /// @brief add() will call std::exit() when the pool is full
        FailTerminate
    };

    /**
     * @brief Iterator for traversing the object pool. Use Iterator or ConstIterator. Forward
     *        iteration only
     *
     * @tparam EType Either T or const T
     */
    template<typename EType>
    struct IteratorType {
        /**
         * @brief Creates an empty, invalid iterator
         *
         */
        IteratorType() = default;

        /**
         * @brief Copies from the given iterator
         *
         * @param copy The iterator to copy from
         */
        IteratorType(const IteratorType& copy) = default;

        /**
         * @brief Copies from the given iterator
         *
         * @param copy The iterator to copy from
         * @return IteratorType& A reference to this iterator
         */
        IteratorType& operator=(const IteratorType& copy) = default;

        /**
         * @brief Returns a reference to the underlying object. Undefined behavior if invalid
         *
         */
        constexpr EType& operator*();

        /**
         * @brief Returns a pointer to the underlying object. Undefined behavior if invalid
         *
         */
        constexpr EType* operator->();

        /**
         * @brief Returns a const reference to the underlying object. Undefined behavior if invalid
         *
         */
        constexpr const EType& operator*() const;

        /**
         * @brief Returns a const pointer to the underlying object. Undefined behavior if invalid
         *
         */
        constexpr const EType* operator->() const;

        /**
         * @brief Modifies this iterator to point to the next object in the pool
         *
         */
        IteratorType<EType>& operator++();

        /**
         * @brief Modifies this iterator to point to the next object in the pool
         *
         */
        IteratorType<EType>& operator++(int);

        /**
         * @brief Compares whether two iterators are equal
         *
         */
        bool operator==(const IteratorType<EType>& iterator) const;

        /**
         * @brief Compares whether two iterators are not equal
         *
         */
        bool operator!=(const IteratorType<EType>& iterator) const;

    private:
        using EP = std::conditional_t<std::is_same_v<EType, T>, Entry*, const Entry*>;

        IteratorType(EP e, EP end);

        EP pos;
        EP end;

        friend class ObjectPool;
        friend class FixedRef;
    };

    using Iterator      = IteratorType<T>;
    using ConstIterator = IteratorType<const T>;

    /**
     * @brief Represents a stable reference into the object pool that remains valid even if the pool
     *        is resized. May become invalid if the object pointed to is erased
     *
     */
    class FixedRef {
    public:
        /**
         * @brief Creates an invalid FixedRef
         *
         */
        FixedRef();

        /**
         * @brief Returns the underlying object
         *
         */
        T& operator*();

        /**
         * @brief Returns the underlying object
         *
         */
        const T& operator*() const;

        /**
         * @brief Returns the underlying object
         *
         */
        T* operator->();

        /**
         * @brief Returns the underlying object
         *
         */
        const T* operator->() const;

        /**
         * @brief Erases the underlying object from the pool. Invalidates the reference
         *
         */
        void erase();

        /**
         * @brief Returns whether or not the underlying object still exists. This is always safe to
         *        call if the underlying object pool is still around
         *
         */
        bool valid() const;

        /**
         * @brief Returns the unique id of the underlying object
         *
         */
        std::size_t id() const;

        /**
         * @brief Invalidates this ref entirely. Does not erase the underlying object
         *
         */
        void release();

    private:
        ObjectPool* owner;
        std::size_t index;

        FixedRef(ObjectPool* owner, std::size_t index);

        friend class ObjectPool<T>;
    };

    /**
     * @brief Initializes the object pool with the growth policy and initial capacity. Not that,
     *        unless the growth policy is ExpandBuffer, the pool capacity will always be the given
     *        capacity.
     *
     * @param policy How to handle add() when the pool is full
     * @param capacity The size of buffer to create
     *
     */
    ObjectPool(GrowthPolicy policy, std::size_t capacity);

    /**
     * @brief Adds the given object to the pool. Either reuses an empty object slot or expands the
     *        pool if no empty slots. All iterators remain valid
     *
     * @param obj Object to add
     * @return Iterator Iterator to the newly created object
     */
    Iterator add(const T& obj);

    /**
     * @brief Adds the given object to the pool. Either reuses an empty object slot or expands the
     *        pool if no empty slots. All iterators remain valid
     *
     * @param obj Object to add
     * @return Iterator Iterator to the newly created object
     */
    Iterator add(T&& obj);

    /**
     * @brief Constructs a new object in-place in the pool
     *
     * @tparam TArgs The types of arguments
     * @param args The arguments to construct the new object with
     * @return Iterator Iterator to the newly created object
     */
    template<typename... TArgs>
    Iterator emplace(TArgs&&... args);

    /**
     * @brief Returns a stable reference to an object from its iterator
     *
     * @param it Iterator pointing to the object a reference is needed for
     * @return FixedRef A stable reference to the given object
     */
    FixedRef getStableRef(Iterator it);

    /**
     * @brief Direct accessor used to index into the pool
     *
     * @param id The index of the object to access
     * @return A reference to the object at the given index
     */
    T& getWithId(std::size_t id);

    /**
     * @brief Removes the given iterator from the pool and marks the object slot for reuse. All
     *        iterators remain valid except for the one removed. The erased iterator is partially
     *        invalidated. It may not be dereferenced but it may still be incremented
     *
     * @param i Iterator to erase
     */
    void erase(const Iterator& i);

    /**
     * @brief Returns an iterator to the beginning of the pool
     *
     */
    Iterator begin();

    /**
     * @brief Returns a const iterator to the beginning of the pool
     *
     */
    ConstIterator begin() const;

    /**
     * @brief Returns an iterator to the end of the pool
     *
     */
    Iterator end();

    /**
     * @brief Returns a const iterator to the end of the pool
     *
     */
    ConstIterator end() const;

    /**
     * @brief Returns the size of the pool
     *
     */
    std::size_t size() const;

    /**
     * @brief Returns the capacity of the pool. This is the size plus the amount of unused slots
     *
     */
    std::size_t capacity() const;

    /**
     * @brief Reserve space for the given capacity
     *
     * @param capacity The amount of elements to reserve space for
     */
    void reserve(std::size_t capacity);

    /**
     * @brief Removes all objects from the pool. This invalidates all iterators
     *
     * @param shrinkStorage True to also clear the pool's memory, false to leave it
     *
     */
    void clear(bool shrinkStorage = false);

    /**
     * @brief Shrinks the capacity of the pool down to the size
     *
     */
    void shrink();

    /**
     * @brief Returns whether or not the pool is empty
     *
     */
    bool empty() const;

private:
    class Entry {
    public:
        explicit Entry()
        : _alive(false)
        , _next(-1) {}

        Entry(const T& data)
        : _alive(true) {
            payload.emplace(data);
        }

        Entry(T&& data)
        : _alive(true) {
            payload.emplace(std::forward<T>(data));
        }

        template<typename... TArgs>
        Entry(TArgs&&... args)
        : _alive(true) {
            payload.emplace(std::forward<TArgs>(args)...);
        }

        Entry(Entry&& copy)
        : _alive(copy._alive) {
            if (copy._alive) { payload.emplace(std::forward<T>(copy.payload.getRValue())); }
            else { _next = copy._next; }
        }

        ~Entry() {
            if (_alive) { payload.destroy(); }
        }

        template<typename... TArgs>
        void emplace(TArgs&&... args) {
            if (_alive) { payload.destroy(); }
            else { _alive = true; }
            payload.emplace(std::forward<TArgs>(args)...);
        }

        void destroy() {
            _alive = false;
            payload.destroy();
        }

        constexpr bool alive() const { return _alive; }

        constexpr T& get() { return payload.get(); }

        constexpr long& next() { return _next; }

    private:
        bool _alive;
        union {
            ObjectWrapper<T> payload;
            long _next;
        };
    };

    const GrowthPolicy policy;
    std::vector<Entry> pool;
    std::size_t trackedSize;
    long next;
    long oldest;

    constexpr Entry* endPointer() { return pool.data() + pool.size(); }
    constexpr const Entry* endPointer() const { return pool.data() + pool.size(); }
    Entry* doAdd();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
ObjectPool<T>::ObjectPool(GrowthPolicy pl, std::size_t cap)
: policy(pl)
, trackedSize(0)
, next(-1)
, oldest(-1) {
    pool.reserve(cap);
}

template<typename T>
typename ObjectPool<T>::Iterator ObjectPool<T>::add(const T& obj) {
    Entry* e = doAdd();
    if (e != nullptr) {
        e->emplace(obj);
        return {e, endPointer()};
    }
    return end();
}

template<typename T>
typename ObjectPool<T>::Iterator ObjectPool<T>::add(T&& obj) {
    Entry* e = doAdd();
    if (e != nullptr) {
        e->emplace(std::forward<T>(obj));
        return {e, endPointer()};
    }
    return end();
}

template<typename T>
template<typename... TArgs>
typename ObjectPool<T>::Iterator ObjectPool<T>::emplace(TArgs&&... args) {
    Entry* e = doAdd();
    if (e != nullptr) {
        e->emplace(std::forward<TArgs>(args)...);
        return {e, endPointer()};
    }
    return end();
}

template<typename T>
typename ObjectPool<T>::FixedRef ObjectPool<T>::getStableRef(Iterator it) {
    const std::size_t i = it.pos - pool.data();
    return {this, i};
}

template<typename T>
T& ObjectPool<T>::getWithId(std::size_t i) {
    return pool[i].get();
}

template<typename T>
typename ObjectPool<T>::Entry* ObjectPool<T>::doAdd() {
    ++trackedSize;
    if (next >= 0) {
        Entry* e = &pool[next];
        next     = e->next();
        return e;
    }
    else {
        if (pool.size() < pool.capacity() || policy == GrowthPolicy::ExpandBuffer) {
            pool.emplace_back();
            return &pool.back();
        }
        else {
            switch (policy) {
            case GrowthPolicy::FailContinue:
                BL_LOG_WARN << typeid(T).name() << " pool exceeded capacity (" << capacity()
                            << "). Increase allocation";
                return nullptr;
            case GrowthPolicy::FailTerminate:
                BL_LOG_CRITICAL << typeid(T).name() << " pool exceeded capacity (" << capacity()
                                << "). Increase allocation. Terminating";
                std::exit(1);
                break;
            default:
                // unreachable
                return nullptr;
            }
        }
    }
}

template<typename T>
void ObjectPool<T>::erase(const Iterator& i) {
    --trackedSize;
    i.pos->destroy();
    i.pos->next() = -1;
    if (next >= 0) { i.pos->next() = next; }
    next = i.pos - pool.data();
}

template<typename T>
typename ObjectPool<T>::Iterator ObjectPool<T>::begin() {
    return {pool.data(), endPointer()};
}

template<typename T>
typename ObjectPool<T>::ConstIterator ObjectPool<T>::begin() const {
    return {pool.data(), endPointer()};
}

template<typename T>
typename ObjectPool<T>::Iterator ObjectPool<T>::end() {
    return {endPointer(), endPointer()};
}

template<typename T>
typename ObjectPool<T>::ConstIterator ObjectPool<T>::end() const {
    return {endPointer(), endPointer()};
}

template<typename T>
std::size_t ObjectPool<T>::size() const {
    return trackedSize;
}

template<typename T>
std::size_t ObjectPool<T>::capacity() const {
    return pool.capacity();
}

template<typename T>
void ObjectPool<T>::reserve(std::size_t cap) {
    pool.reserve(cap);
}

template<typename T>
bool ObjectPool<T>::empty() const {
    return trackedSize == 0;
}

template<typename T>
void ObjectPool<T>::clear(bool s) {
    trackedSize = 0;
    for (Entry& entry : pool) {
        if (entry.alive()) { entry.destroy(); }
        else { entry.next() = -1; }
    }
    pool.clear();
    next = -1;
    if (s) {
        std::vector<Entry> newPool;
        std::swap(newPool, pool);
    }
}

template<typename T>
void ObjectPool<T>::shrink() {
    std::vector<Entry> newPool;
    newPool.reserve(trackedSize);
    for (Entry& entry : pool) {
        if (entry.alive()) { newPool.emplace_back(std::move(entry)); }
    }
    std::swap(pool, newPool);
    next = -1;
}

template<typename T>
template<typename ET>
ObjectPool<T>::IteratorType<ET>::IteratorType(EP e, EP end)
: pos(e)
, end(end) {}

template<typename T>
template<typename ET>
constexpr ET& ObjectPool<T>::IteratorType<ET>::operator*() {
    return pos->get();
}

template<typename T>
template<typename ET>
constexpr ET* ObjectPool<T>::IteratorType<ET>::operator->() {
    return &pos->get();
}

template<typename T>
template<typename ET>
constexpr const ET& ObjectPool<T>::IteratorType<ET>::operator*() const {
    return pos->get();
}

template<typename T>
template<typename ET>
constexpr const ET* ObjectPool<T>::IteratorType<ET>::operator->() const {
    return &pos->get();
}

template<typename T>
template<typename ET>
typename ObjectPool<T>::template IteratorType<ET>& ObjectPool<T>::IteratorType<ET>::operator++() {
    do { ++pos; } while (pos != end && !pos->alive());
    return *this;
}

template<typename T>
template<typename ET>
typename ObjectPool<T>::template IteratorType<ET>& ObjectPool<T>::IteratorType<ET>::operator++(
    int) {
    const auto ov = *this;
    do { ++pos; } while (pos != end && !pos->alive());
    return ov;
}

template<typename T>
template<typename ET>
bool ObjectPool<T>::IteratorType<ET>::operator==(const IteratorType<ET>& right) const {
    return pos == right.pos;
}

template<typename T>
template<typename ET>
bool ObjectPool<T>::IteratorType<ET>::operator!=(const IteratorType<ET>& right) const {
    return pos != right.pos;
}

template<typename T>
ObjectPool<T>::FixedRef::FixedRef()
: owner(nullptr)
, index(0) {}

template<typename T>
ObjectPool<T>::FixedRef::FixedRef(ObjectPool<T>* p, std::size_t i)
: owner(p)
, index(i) {}

template<typename T>
T& ObjectPool<T>::FixedRef::operator*() {
    return owner->pool[index].get();
}

template<typename T>
const T& ObjectPool<T>::FixedRef::operator*() const {
    return owner->pool[index].get();
}

template<typename T>
T* ObjectPool<T>::FixedRef::operator->() {
    return &owner->pool[index].get();
}

template<typename T>
const T* ObjectPool<T>::FixedRef::operator->() const {
    return &owner->pool[index].get();
}

template<typename T>
void ObjectPool<T>::FixedRef::erase() {
    owner->erase(ObjectPool<T>::Iterator{owner->pool.data() + index, nullptr});
}

template<typename T>
bool ObjectPool<T>::FixedRef::valid() const {
    return owner && owner->pool[index].alive();
}

template<typename T>
void ObjectPool<T>::FixedRef::release() {
    owner = nullptr;
}

template<typename T>
std::size_t ObjectPool<T>::FixedRef::id() const {
    return index;
}

} // namespace ctr
} // namespace bl

#endif
