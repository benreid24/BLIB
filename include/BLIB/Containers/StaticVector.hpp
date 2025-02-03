#ifndef BLIB_CONTAINERS_STATICVECTOR_HPP
#define BLIB_CONTAINERS_STATICVECTOR_HPP

#include <BLIB/Containers/ObjectWrapper.hpp>
#include <array>
#include <stdexcept>

namespace bl
{
namespace ctr
{
/**
 * @brief Wrapper over std::array that provides a vector-like interface with a fixed capacity
 *
 * @tparam T The type of elements to store
 * @tparam N The maximum number of elements to store
 * @ingroup Containers
 */
template<typename T, std::size_t N>
class StaticVector {
public:
    /**
     * @brief Creates an empty StaticVector
     */
    StaticVector();

    /**
     * @brief Copies from another vector
     *
     * @param copy The vector to copy from
     */
    StaticVector(const StaticVector& copy);

    /**
     * @brief Moves from another vector
     *
     * @param copy The vector to move from
     */
    StaticVector(StaticVector&& copy);

    /**
     * @brief Destroys the StaticVector
     */
    ~StaticVector();

    /**
     * @brief Append a value to the end of the vector
     *
     * @param value The value to append
     */
    void push_back(const T& value);

    /**
     * @brief Appends a value to the end of the vector
     *
     * @param value The value to append
     */
    void push_back(T&& value);

    /**
     * @brief Constructs a new element in place at the end of the vector
     *
     * @tparam ...TArgs The types of the arguments to pass to the constructor
     * @param ...args The arguments to pass to the constructor
     */
    template<typename... TArgs>
    void emplace_back(TArgs&&... args);

    /**
     * @brief Removes the last element from the vector
     */
    void pop_back();

    /**
     * @brief Erases the element at the given index
     *
     * @param index The index of the element to erase
     */
    void erase(std::size_t index);

    /**
     * @brief Resizes the vector to the given size
     *
     * @param newSize The new size of the vector
     */
    void resize(std::size_t newSize);

    /**
     * @brief Returns a reference to the element at the given index
     *
     * @param index The index of the element to access
     * @return A reference to the element at the given index
     */
    T& operator[](std::size_t index);

    /**
     * @brief Returns a reference to the element at the given index
     *
     * @param index The index of the element to access
     * @return A reference to the element at the given index
     */
    const T& operator[](std::size_t index) const;

    /**
     * @brief Returns the current number of elements in the vector
     */
    std::size_t size() const;

    /**
     * @brief Returns the maximum number of elements that can be stored in the vector
     */
    std::size_t capacity() const;

    /**
     * @brief Clears all elements from the vector
     */
    void clear();

    /**
     * @brief Returns whether the vector is empty or not
     */
    bool empty() const;

    /**
     * @brief Returns a reference to the last element in the vector
     */
    T& back();

    /**
     * @brief Returns a reference to the last element in the vector
     */
    const T& back() const;

    /**
     * @brief Returns a reference to the first element in the vector
     */
    T& front();

    /**
     * @brief Returns a reference to the first element in the vector
     */
    const T& front() const;

    /**
     * @brief Returns a pointer to the first element in the vector
     */
    T* data();

    /**
     * @brief Returns a pointer to the first element in the vector
     */
    const T* data() const;

    /**
     * @brief Returns a pointer to the first element in the vector
     */
    T* begin();

    /**
     * @brief Returns a pointer to the first element in the vector
     */
    const T* begin() const;

    /**
     * @brief Returns a pointer to the last element in the vector
     */
    T* end();

    /**
     * @brief Returns a pointer to the first element in the vector
     */
    const T* end() const;

private:
    std::array<ObjectWrapper<T>, N> storage;
    std::size_t used;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, std::size_t N>
StaticVector<T, N>::StaticVector()
: used(0) {}

template<typename T, std::size_t N>
StaticVector<T, N>::StaticVector(const StaticVector& copy)
: used(copy.used) {
    for (std::size_t i = 0; i < used; ++i) { storage[i].emplace(copy.storage[i].get()); }
}

template<typename T, std::size_t N>
StaticVector<T, N>::StaticVector(StaticVector&& copy)
: used(copy.used) {
    for (std::size_t i = 0; i < used; ++i) { storage[i].emplace(copy.storage[i].getRValue()); }
}

template<typename T, std::size_t N>
StaticVector<T, N>::~StaticVector() {
    clear();
}

template<typename T, std::size_t N>
void StaticVector<T, N>::push_back(const T& value) {
#ifdef BLIB_DEBUG
    if (used >= N) { throw std::runtime_error("StaticVector is full"); }
#endif

    storage[used++].emplace(value);
}

template<typename T, std::size_t N>
void StaticVector<T, N>::push_back(T&& value) {
#ifdef BLIB_DEBUG
    if (used >= N) { throw std::runtime_error("StaticVector is full"); }
#endif

    storage[used++].emplace(std::forward<T>(value));
}

template<typename T, std::size_t N>
template<typename... TArgs>
void StaticVector<T, N>::emplace_back(TArgs&&... args) {
#ifdef BLIB_DEBUG
    if (used >= N) { throw std::runtime_error("StaticVector is full"); }
#endif

    storage[used++].emplace(std::forward<TArgs>(args)...);
}

template<typename T, std::size_t N>
void StaticVector<T, N>::pop_back() {
#ifdef BLIB_DEBUG
    if (used == 0) { throw std::runtime_error("StaticVector is empty"); }
#endif

    storage[--used].destroy();
}

template<typename T, std::size_t N>
void StaticVector<T, N>::erase(std::size_t index) {
#ifdef BLIB_DEBUG
    if (index >= used) { throw std::runtime_error("Index out of bounds"); }
#endif

    if (index == used - 1) { pop_back(); }
    else {
        for (std::size_t i = index; i < used - 1; ++i) {
            storage[i].destroy();
            storage[i].emplace(std::move(storage[i + 1].get()));
        }
        --used;
    }
}

template<typename T, std::size_t N>
void StaticVector<T, N>::resize(std::size_t newSize) {
    if (newSize > used) {
        for (std::size_t i = used; i < newSize; ++i) { storage[i].emplace(); }
    }
    else {
        for (std::size_t i = newSize; i < used; ++i) { storage[i].destroy(); }
    }
    used = newSize;
}

template<typename T, std::size_t N>
T& StaticVector<T, N>::operator[](std::size_t index) {
    return storage[index].get();
}

template<typename T, std::size_t N>
const T& StaticVector<T, N>::operator[](std::size_t index) const {
    return storage[index].get();
}

template<typename T, std::size_t N>
std::size_t StaticVector<T, N>::size() const {
    return used;
}

template<typename T, std::size_t N>
std::size_t StaticVector<T, N>::capacity() const {
    return N;
}

template<typename T, std::size_t N>
void StaticVector<T, N>::clear() {
    for (std::size_t i = 0; i < used; ++i) { storage[i].destroy(); }
    used = 0;
}

template<typename T, std::size_t N>
bool StaticVector<T, N>::empty() const {
    return used == 0;
}

template<typename T, std::size_t N>
T& StaticVector<T, N>::back() {
    return storage[used - 1].get();
}

template<typename T, std::size_t N>
const T& StaticVector<T, N>::back() const {
    return storage[used - 1].get();
}

template<typename T, std::size_t N>
T& StaticVector<T, N>::front() {
    return storage[0].get();
}

template<typename T, std::size_t N>
const T& StaticVector<T, N>::front() const {
    return storage[0].get();
}

template<typename T, std::size_t N>
T* StaticVector<T, N>::data() {
    return &storage[0].get();
}

template<typename T, std::size_t N>
const T* StaticVector<T, N>::data() const {
    return &storage[0].get();
}

template<typename T, std::size_t N>
T* StaticVector<T, N>::begin() {
    return &storage[0].get();
}

template<typename T, std::size_t N>
const T* StaticVector<T, N>::begin() const {
    return &storage[0].get();
}

template<typename T, std::size_t N>
T* StaticVector<T, N>::end() {
    return (&storage[used - 1].get()) + 1;
}

template<typename T, std::size_t N>
const T* StaticVector<T, N>::end() const {
    return (&storage[used - 1].get()) + 1;
}

} // namespace ctr
} // namespace bl

#endif
