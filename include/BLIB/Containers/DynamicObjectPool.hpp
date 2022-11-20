#ifndef BLIB_CONTAINERS_DYNAMICOBJECTPOOL_HPP
#define BLIB_CONTAINERS_DYNAMICOBJECTPOOL_HPP

#include <BLIB/Containers/ObjectWrapper.hpp>
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
    class Entry;

public:
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
        EType& operator*();

        /**
         * @brief Returns a pointer to the underlying object. Undefined behavior if invalid
         *
         */
        EType* operator->();

        /**
         * @brief Returns a const reference to the underlying object. Undefined behavior if invalid
         *
         */
        const EType& operator*() const;

        /**
         * @brief Returns a const pointer to the underlying object. Undefined behavior if invalid
         *
         */
        const EType* operator->() const;

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
        IteratorType(Entry* e, Entry* end);

        Entry* pos;
        Entry* end;

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
    Iterator emplace(TArgs&&... args);

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
            slot.payload.emplace(data);
        }

        Entry(T&& data)
        : _alive(true) {
            slot.payload.emplace(std::forward<T>(data));
        }

        template<typename... TArgs>
        Entry(TArgs&&... args)
        : _alive(true) {
            slot.payload.emplace(std::forward<TArgs>(args)...);
        }

        Entry(Entry&& copy)
        : _alive(copy._alive) {
            if (copy._alive) {
                slot.payload.emplace(std::forward<T>(copy.slot.payload.getRValue()));
            }
        }

        ~Entry() {
            if (_alive) destroy();
        }

        template<typename... TArgs>
        void emplace(TArgs&&... args) {
            if (_alive) { slot.payload.destroy(); }
            else {
                _alive = true;
            }
            slot.payload.emplace(std::forward<TArgs>(args)...);
        }

        void destroy() {
            slot.payload.destroy();
            _alive = false;
        }

        constexpr bool alive() const { return _alive; }

        constexpr T* cast() { return &slot.payload.get(); }

        constexpr long& next() { return slot.next; }

    private:
        bool _alive;
        union {
            ObjectWrapper<T> payload;
            long next;
        } slot;
    };

    std::vector<Entry> pool;
    std::size_t trackedSize;
    long next;

    constexpr Entry* endPointer() { return &pool.back() + 1; }
    constexpr const Entry* endPointer() const { return &pool.back() + 1; }
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
        Entry* e = &pool[next];
        next     = e->next();
        e->emplace(obj);
        return {e, endPointer()};
    }
    else {
        pool.emplace_back(obj);
        return {&pool.back(), endPointer()};
    }
}

template<typename T>
typename DynamicObjectPool<T>::Iterator DynamicObjectPool<T>::add(T&& obj) {
    ++trackedSize;
    if (next >= 0) {
        Entry* e = &pool[next];
        next     = e->next();
        e->emplace(std::forward<T>(obj));
        return {e, endPointer()};
    }
    else {
        pool.emplace_back(std::forward<T>(obj));
        return {&pool.back(), endPointer()};
    }
}

template<typename T>
template<typename... TArgs>
typename DynamicObjectPool<T>::Iterator DynamicObjectPool<T>::emplace(TArgs&&... args) {
    ++trackedSize;
    if (next >= 0) {
        Entry* e = &pool[next];
        next     = e->next();
        e->emplace(std::forward<TArgs>(args)...);
        return {e, endPointer()};
    }
    else {
        pool.emplace_back(std::forward<TArgs>(args)...);
        return {&pool.back(), endPointer()};
    }
}

template<typename T>
void DynamicObjectPool<T>::erase(const Iterator& i) {
    --trackedSize;
    i.pos->destroy();
    i.pos->next() = -1;
    if (next >= 0) { i.pos->next() = next; }
    next = i.pos - &pool.front();
}

template<typename T>
typename DynamicObjectPool<T>::Iterator DynamicObjectPool<T>::begin() {
    return {&pool.front(), endPointer()};
}

template<typename T>
typename DynamicObjectPool<T>::ConstIterator DynamicObjectPool<T>::begin() const {
    return {&pool.front(), endPointer()};
}

template<typename T>
typename DynamicObjectPool<T>::Iterator DynamicObjectPool<T>::end() {
    return {endPointer(), endPointer()};
}

template<typename T>
typename DynamicObjectPool<T>::ConstIterator DynamicObjectPool<T>::end() const {
    return {endPointer(), endPointer()};
}

template<typename T>
std::size_t DynamicObjectPool<T>::size() const {
    return trackedSize;
}

template<typename T>
std::size_t DynamicObjectPool<T>::capacity() const {
    return pool.capacity();
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
    for (Entry& entry : pool) {
        if (entry.alive()) { entry.destroy(); }
        else {
            entry.next() = -1;
        }
    }
    pool.clear();
    next = -1;
    if (s) {
        std::vector<Entry> newPool;
        std::swap(newPool, pool);
    }
}

template<typename T>
void DynamicObjectPool<T>::shrink() {
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
DynamicObjectPool<T>::IteratorType<ET>::IteratorType(DynamicObjectPool<T>::Entry* e,
                                                     DynamicObjectPool<T>::Entry* end)
: pos(e)
, end(end) {}

template<typename T>
template<typename ET>
ET& DynamicObjectPool<T>::IteratorType<ET>::operator*() {
    return *pos->cast();
}

template<typename T>
template<typename ET>
ET* DynamicObjectPool<T>::IteratorType<ET>::operator->() {
    return pos->cast();
}

template<typename T>
template<typename ET>
const ET& DynamicObjectPool<T>::IteratorType<ET>::operator*() const {
    return *pos->cast();
}

template<typename T>
template<typename ET>
const ET* DynamicObjectPool<T>::IteratorType<ET>::operator->() const {
    return pos->cast();
}

template<typename T>
template<typename ET>
typename DynamicObjectPool<T>::template IteratorType<ET>&
DynamicObjectPool<T>::IteratorType<ET>::operator++() {
    do { ++pos; } while (pos != end && !pos->alive());
    return *this;
}

template<typename T>
template<typename ET>
typename DynamicObjectPool<T>::template IteratorType<ET>&
DynamicObjectPool<T>::IteratorType<ET>::operator++(int) {
    const auto ov = *this;
    do { ++pos; } while (pos != end && !pos->alive());
    return ov;
}

template<typename T>
template<typename ET>
bool DynamicObjectPool<T>::IteratorType<ET>::operator==(const IteratorType<ET>& right) const {
    return pos == right.pos;
}

template<typename T>
template<typename ET>
bool DynamicObjectPool<T>::IteratorType<ET>::operator!=(const IteratorType<ET>& right) const {
    return pos != right.pos;
}

} // namespace container
} // namespace bl

#endif
