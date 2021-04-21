#ifndef BLIB_CONTAINERS_DYNAMICOBJECTPOOL_HPP
#define BLIB_CONTAINERS_DYNAMICOBJECTPOOL_HPP

#include <cstring>
#include <utility>
#include <vector>

namespace bl
{
namespace container
{
/**
 * @brief Basic object pool that may vary in size. Similar to FastEraseVector in that erase is O(1),
 *        however objects in the pool maintain their indices in the DynamicObjectPool. Object slots
 *        are marked free on erase(), reused on add(), and the pool grows in size if no slot is
 *        when add() is called. Objects must implement a default constructor and copy constructor.
 *
 * @tparam T Type of object to store. Works best with small POD objects
 * @ingroup Containers
 */
template<typename T>
class DynamicObjectPool {
    struct Entry;

public:
    /**
     * @brief Iterator for traversing the object pool. Use Iterator or ConstIterator
     *
     * @tparam EType Either T or const T
     */
    template<typename EType>
    struct IteratorType {
        /**
         * @brief Copies from the given iterator
         *
         * @param copy The iterator to copy from
         */
        IteratorType(const IteratorType& copy);

        /**
         * @brief Copies from the given iterator
         *
         * @param copy The iterator to copy from
         * @return IteratorType& A reference to this iterator
         */
        IteratorType& operator=(const IteratorType& copy);

        /**
         * @brief Returns a reference to the underlying object. Undefined behavior if invalid
         *
         */
        EType& operator*();

        /**
         * @brief Returns a reference to the underlying object. Undefined behavior if invalid
         *
         */
        EType* operator->();

        /**
         * @brief Modifies this iterator to point to the next object in the pool
         *
         */
        IteratorType<EType>& operator++();

        /**
         * @brief Modifies this iterator to point to the previous object in the pool
         *
         */
        IteratorType<EType>& operator--();

        /**
         * @brief Basic addition operator. This operation is linear with respect to d
         *
         */
        IteratorType<EType> operator+(int d) const;

        /**
         * @brief Basic subtraction operator. This operation is linear with respect to d
         *
         */
        IteratorType<EType> operator-(int d) const;

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

        // private:
        IteratorType(std::vector<Entry>& pool, long long int i);

        std::vector<Entry>* pool;
        long long int i;

        friend class DynamicObjectPool;
    };

    using Iterator      = IteratorType<T>;
    using ConstIterator = IteratorType<const T>;

    /**
     * @brief Initializes the empty object pool
     *
     */
    DynamicObjectPool();

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
     * @brief Constructs a new object inplace in the pool
     *
     * @tparam TArgs The types of arguments
     * @param args The arguments to construct the new object with
     * @return Iterator Iterator to the newly created object
     */
    template<typename... TArgs>
    Iterator emplace(TArgs... args);

    /**
     * @brief Removes the given iterator from the pool and marks the object slot for resuse. All
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
        : _alive(false) {
            slot.next = -1;
        }

        Entry(const T& data)
        : _alive(true) {
            assign(data);
        }

        Entry(T&& data)
        : _alive(true) {
            move(std::forward<T>(data));
        }

        template<typename... TArgs>
        Entry(TArgs... args)
        : _alive(true) {
            emplace(args...);
        }

        Entry(Entry&& copy)
        : _alive(copy._alive) {
            if (_alive) { new (slot.buf) T(std::move(*copy.cast())); }
            copy._alive = false;
        }

        ~Entry() {
            if (_alive) destroy();
        }

        void assign(const T& v) {
            _alive = true;
            new (cast()) T(v);
        }

        void move(T&& v) {
            _alive = true;
            new (cast()) T(std::forward<T>(v));
        }

        template<typename... TArgs>
        void emplace(TArgs... args) {
            _alive = true;
            new (cast()) T(args...);
        }

        void destroy() {
            cast()->~T();
            _alive = false;
        }

        bool alive() const { return _alive; }

        T* cast() { return static_cast<T*>(static_cast<void*>(slot.buf)); }

        long long int& next() { return slot.next; }

    private:
        bool _alive;
        union {
            char buf[sizeof(T)];
            long long int next;
        } slot;
    };

    std::vector<Entry> pool;
    std::size_t trackedSize;
    long long int next;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
DynamicObjectPool<T>::DynamicObjectPool()
: trackedSize(0)
, next(-1) {}

template<typename T>
typename DynamicObjectPool<T>::Iterator DynamicObjectPool<T>::add(const T& obj) {
    ++trackedSize;
    if (next >= 0) {
        const auto it = next;
        next          = pool[next].next();
        pool[it].assign(obj);
        return {pool, it};
    }
    else {
        pool.emplace_back(obj);
        return {pool, static_cast<long long int>(pool.size() - 1)};
    }
}

template<typename T>
typename DynamicObjectPool<T>::Iterator DynamicObjectPool<T>::add(T&& obj) {
    ++trackedSize;
    if (next >= 0) {
        const auto it = next;
        next          = pool[next].next();
        pool[it].move(std::forward<T>(obj));
        return {pool, it};
    }
    else {
        pool.emplace_back(std::forward<T>(obj));
        return {pool, static_cast<long long int>(pool.size() - 1)};
    }
}

template<typename T>
template<typename... TArgs>
typename DynamicObjectPool<T>::Iterator DynamicObjectPool<T>::emplace(TArgs... args) {
    ++trackedSize;
    if (next >= 0) {
        const auto it = next;
        next          = pool[next].next();
        pool[it].emplace(args...);
        return {pool, it};
    }
    else {
        pool.emplace_back(args...);
        return {pool, static_cast<long long int>(pool.size() - 1)};
    }
}

template<typename T>
void DynamicObjectPool<T>::erase(const Iterator& i) {
    --trackedSize;
    pool[i.i].destroy();
    pool[i.i].next() = -1;
    if (next >= 0) { pool[i.i].next() = next; }
    next = i.i;
}

template<typename T>
typename DynamicObjectPool<T>::Iterator DynamicObjectPool<T>::begin() {
    return {pool, 0};
}

template<typename T>
typename DynamicObjectPool<T>::ConstIterator DynamicObjectPool<T>::begin() const {
    return {pool, 0};
}

template<typename T>
typename DynamicObjectPool<T>::Iterator DynamicObjectPool<T>::end() {
    return {pool, static_cast<long long int>(pool.size())};
}

template<typename T>
typename DynamicObjectPool<T>::ConstIterator DynamicObjectPool<T>::end() const {
    return {pool, static_cast<long long int>(pool.size())};
}

template<typename T>
std::size_t DynamicObjectPool<T>::size() const {
    return trackedSize;
}

template<typename T>
std::size_t DynamicObjectPool<T>::capacity() const {
    return pool.size();
}

template<typename T>
void DynamicObjectPool<T>::reserve(std::size_t cap) {
    pool.reserve(cap);
}

template<typename T>
bool DynamicObjectPool<T>::empty() const {
    return trackedSize == 0;
}

template<typename T>
void DynamicObjectPool<T>::clear(bool s) {
    trackedSize = 0;
    if (s) {
        pool.clear();
        next = -1;
    }
    else {
        for (unsigned int i = 0; i < pool.size(); ++i) {
            Entry& e = pool[i];
            if (e.alive()) {
                e.destroy();
                if (next >= 0) e.next() = next;
                next = i;
            }
        }
    }
}

template<typename T>
void DynamicObjectPool<T>::shrink() {
    std::vector<Entry> newPool;
    newPool.reserve(trackedSize);
    for (unsigned int i = 0; i < pool.size(); ++i) {
        if (pool[i].alive()) { newPool.push_back(std::move(pool[i])); }
    }
    std::swap(pool, newPool);
    next = -1;
}

template<typename T>
template<typename ET>
DynamicObjectPool<T>::IteratorType<ET>::IteratorType(std::vector<DynamicObjectPool<T>::Entry>& pool,
                                                     long long int i)
: pool(&pool)
, i(i) {}

template<typename T>
template<typename ET>
DynamicObjectPool<T>::IteratorType<ET>::IteratorType::IteratorType(const IteratorType& copy)
: pool(copy.pool)
, i(copy.i) {}

template<typename T>
template<typename ET>
typename DynamicObjectPool<T>::template IteratorType<ET>&
DynamicObjectPool<T>::IteratorType<ET>::operator=(const IteratorType& copy) {
    pool = copy.pool;
    i    = copy.i;
    return *this;
}

template<typename T>
template<typename ET>
ET& DynamicObjectPool<T>::IteratorType<ET>::operator*() {
    return *(*pool)[i].cast();
}

template<typename T>
template<typename ET>
ET* DynamicObjectPool<T>::IteratorType<ET>::operator->() {
    return (*pool)[i].cast();
}

template<typename T>
template<typename ET>
typename DynamicObjectPool<T>::template IteratorType<ET>&
DynamicObjectPool<T>::IteratorType<ET>::operator++() {
    ++i;
    while (i < pool->size() && !(*pool)[i].alive()) { ++i; }
    if (i == pool->size() - 1 && !(*pool)[i].alive()) ++i;
    return *this;
}

template<typename T>
template<typename ET>
typename DynamicObjectPool<T>::template IteratorType<ET>&
DynamicObjectPool<T>::IteratorType<ET>::operator--() {
    --i;
    while (i > 0 && !(*pool)[i].alive()) { --i; }
    return *this;
}

template<typename T>
template<typename ET>
typename DynamicObjectPool<T>::template IteratorType<ET>
DynamicObjectPool<T>::IteratorType<ET>::operator+(int d) const {
    IteratorType<ET> it = *this;
    for (int j = 0; j < d; ++j) { ++it; }
    return it;
}

template<typename T>
template<typename ET>
typename DynamicObjectPool<T>::template IteratorType<ET>
DynamicObjectPool<T>::IteratorType<ET>::operator-(int d) const {
    IteratorType<ET> it = *this;
    for (int j = 0; j < d; ++j) { --it; }
    return it;
}

template<typename T>
template<typename ET>
bool DynamicObjectPool<T>::IteratorType<ET>::operator==(const IteratorType<ET>& right) const {
    return pool == right.pool && i == right.i;
}

template<typename T>
template<typename ET>
bool DynamicObjectPool<T>::IteratorType<ET>::operator!=(const IteratorType<ET>& right) const {
    return pool != right.pool || i != right.i;
}

} // namespace container
} // namespace bl

#endif
