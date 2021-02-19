#ifndef BLIB_CONTAINERS_DYNAMICOBJECTPOOL_HPP
#define BLIB_CONTAINERS_DYNAMICOBJECTPOOL_HPP

#include <vector>

namespace bl
{
/**
 * @brief Basic object pool that may vary in size. Similar to FastEraseVector in that erase is O(1),
 *        however objects in the pool maintain their indices in the DynamicObjectPool. Object slots
 *        are marked free on erase(), reused on add(), and the pool grows in size if no slot is
 *        when add() is called
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
         * @brief Returns a reference to the underlying object. Undefined behavior if invalid
         *
         */
        EType& operator*();

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

    private:
        IteratorType(std::vector<Entry>& pool, long long int i);

        std::vector<Entry>& pool;
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
     */
    void add(const T& obj);

    /**
     * @brief Removes the given iterator from the pool and marks the object slot for resuse. All
     *        iterators remain valid except for the one removed
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
     * @brief Returns whether or not the pool is empty
     *
     */
    bool empty() const;

private:
    struct Entry {
        bool alive;
        T data;
        int next;

        Entry(const T& data)
        : alive(true)
        , data(data)
        , next(-1) {}
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
void DynamicObjectPool<T>::add(const T& obj) {
    ++trackedSize;
    if (next >= 0) {
        pool[next].alive = true;
        pool[next].data  = obj;
        next             = pool[next].next;
    }
    else {
        pool.push_back({obj});
    }
}

template<typename T>
void DynamicObjectPool<T>::erase(const Iterator& i) {
    --trackedSize;
    pool[i.i].alive = false;
    pool[i.i].next  = -1;
    if (next >= 0) { pool[i.i].next = next; }
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
bool DynamicObjectPool<T>::empty() const {
    return trackedSize == 0;
}

template<typename T>
template<typename ET>
DynamicObjectPool<T>::IteratorType<ET>::IteratorType(std::vector<DynamicObjectPool<T>::Entry>& pool,
                                                     long long int i)
: pool(pool)
, i(i) {}

template<typename T>
template<typename ET>
ET& DynamicObjectPool<T>::IteratorType<ET>::operator*() {
    return pool[i].data;
}

template<typename T>
template<typename ET>
DynamicObjectPool<T>::IteratorType<ET>& DynamicObjectPool<T>::IteratorType<ET>::operator++() {
    ++i;
    while (i < pool.size() && !pool[i].alive) { ++i; }
    if (i == pool.size() - 1 && !pool[i].alive) ++i;
    return *this;
}

template<typename T>
template<typename ET>
DynamicObjectPool<T>::IteratorType<ET>& DynamicObjectPool<T>::IteratorType<ET>::operator--() {
    --i;
    while (i > 0 && !pool[i].alive) { --i; }
    return *this;
}

template<typename T>
template<typename ET>
DynamicObjectPool<T>::IteratorType<ET> DynamicObjectPool<T>::IteratorType<ET>::operator+(
    int d) const {
    IteratorType<ET> it = *this;
    for (int j = 0; j < d; ++j) { ++it; }
    return it;
}

template<typename T>
template<typename ET>
DynamicObjectPool<T>::IteratorType<ET> DynamicObjectPool<T>::IteratorType<ET>::operator-(
    int d) const {
    IteratorType<ET> it = *this;
    for (int j = 0; j < d; ++j) { --it; }
    return it;
}

template<typename T>
template<typename ET>
bool DynamicObjectPool<T>::IteratorType<ET>::operator==(const IteratorType<ET>& right) const {
    return &pool == &right.pool && i == right.i;
}

template<typename T>
template<typename ET>
bool DynamicObjectPool<T>::IteratorType<ET>::operator!=(const IteratorType<ET>& right) const {
    return &pool != &right.pool || i != right.i;
}

} // namespace bl

#endif
