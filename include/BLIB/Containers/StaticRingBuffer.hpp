#ifndef BLIB_CONTAINERS_STATICRINGBUFFER_HPP
#define BLIB_CONTAINERS_STATICRINGBUFFER_HPP

#include <BLIB/Containers/ObjectWrapper.hpp>
#include <array>
#include <utility>

namespace bl
{
namespace ctr
{
/**
 * @brief Simple circular buffer implementation for small objects. Underlying storage is a
 *        std::array of buffers. Objects are constructed with placement new and destructors
 *        are called when erased, cleared, or overwritten
 *
 * @tparam T Type of object to store
 * @tparam Capacity The max capacity of the ring buffer
 * @ingroup Containers
 */
template<typename T, std::size_t Capacity>
class StaticRingBuffer {
public:
    /**
     * @brief Construct a new Ring Buffer
     *
     */
    StaticRingBuffer();

    /**
     * @brief Access the element at the given position
     *
     * @param i Position to access
     * @return T& Reference to the element to retrieve. Always valid even if out of bounds
     */
    T& operator[](std::size_t i);

    /**
     * @brief Access the element at the given position
     *
     * @param i Position to access
     * @return T& Const reference to the element to retrieve. Always valid even if out of
     * bounds
     */
    const T& operator[](std::size_t i) const;

    /**
     * @brief Returns a reference to the front object
     *
     */
    T& front();

    /**
     * @brief Returns a reference to the front object
     *
     */
    const T& front() const;

    /**
     * @brief Returns a reference to the back object
     *
     */
    T& back();

    /**
     * @brief Returns a reference to the back object
     *
     */
    const T& back() const;

    /**
     * @brief Returns the capacity of the buffer
     *
     * @return std::size_t The maximum number of elements that may be stored
     */
    std::size_t capacity() const;

    /**
     * @brief Returns the number of elements currently stored
     *
     */
    std::size_t size() const;

    /**
     * @brief Returns whether or not the buffer is empty
     *
     */
    bool empty() const;

    /**
     * @brief Returns true if the buffer is full. Adding more elements to a full buffer will
     *        result in the first elements getting popped off implicitly
     *
     */
    bool full() const;

    /**
     * @brief Appends a new element to the back of the buffer
     *
     * @param obj Element to add
     */
    void push_back(const T& obj);

    /**
     * @brief Appends a new element to the back of the buffer
     *
     * @param obj Element to add
     */
    void push_back(T&& obj);

    /**
     * @brief Construct a new element in place at the end of the buffer
     *
     * @tparam TArgs The types of arguments
     * @param args The arguments to construct with
     */
    template<typename... TArgs>
    void emplace_back(TArgs&&... args);

    /**
     * @brief Removes the element from the front of the buffer. Destructor may not be called
     *        immediately
     *
     */
    void pop_front();

    /**
     * @brief Clears all elements in the buffer
     *
     */
    void clear();

private:
    std::array<ObjectWrapper<T>, Capacity> buffer;
    std::size_t head;
    std::size_t tail;
    std::size_t sz;

    void increaseSize();
    void checkHead();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, std::size_t Capacity>
StaticRingBuffer<T, Capacity>::StaticRingBuffer()
: buffer()
, head(0)
, tail(0)
, sz(0) {}

template<typename T, std::size_t Capacity>
T& StaticRingBuffer<T, Capacity>::operator[](std::size_t i) {
    return buffer[(head + i) % buffer.size()].get();
}

template<typename T, std::size_t Capacity>
const T& StaticRingBuffer<T, Capacity>::operator[](std::size_t i) const {
    return buffer[(head + i) % buffer.size()].get();
}

template<typename T, std::size_t Capacity>
T& StaticRingBuffer<T, Capacity>::front() {
    return buffer[head].get();
}

template<typename T, std::size_t Capacity>
const T& StaticRingBuffer<T, Capacity>::front() const {
    return buffer[head].get();
}

template<typename T, std::size_t Capacity>
T& StaticRingBuffer<T, Capacity>::back() {
    return buffer[(head + sz - 1) % buffer.size()].get();
}

template<typename T, std::size_t Capacity>
const T& StaticRingBuffer<T, Capacity>::back() const {
    return buffer[(head + sz - 1) % buffer.size()].get();
}

template<typename T, std::size_t Capacity>
std::size_t StaticRingBuffer<T, Capacity>::size() const {
    return sz;
}

template<typename T, std::size_t Capacity>
bool StaticRingBuffer<T, Capacity>::empty() const {
    return sz == 0;
}

template<typename T, std::size_t Capacity>
std::size_t StaticRingBuffer<T, Capacity>::capacity() const {
    return buffer.size();
}

template<typename T, std::size_t Capacity>
bool StaticRingBuffer<T, Capacity>::full() const {
    return size() == capacity();
}

template<typename T, std::size_t Capacity>
void StaticRingBuffer<T, Capacity>::push_back(const T& obj) {
    checkHead();
    buffer[tail].emplace(obj);
    increaseSize();
}

template<typename T, std::size_t Capacity>
void StaticRingBuffer<T, Capacity>::push_back(T&& obj) {
    checkHead();
    buffer[tail].emplace(std::forward<T>(obj));
    increaseSize();
}

template<typename T, std::size_t Capacity>
template<typename... TArgs>
void StaticRingBuffer<T, Capacity>::emplace_back(TArgs&&... args) {
    checkHead();
    buffer[tail].emplace(std::forward<TArgs>(args)...);
    increaseSize();
}

template<typename T, std::size_t Capacity>
void StaticRingBuffer<T, Capacity>::clear() {
    for (unsigned int i = 0; i < size(); ++i) { buffer[(head + i) % buffer.size()].destroy(); }
    head = 0;
    tail = 0;
    sz   = 0;
}

template<typename T, std::size_t Capacity>
void StaticRingBuffer<T, Capacity>::increaseSize() {
    tail = (tail + 1) % buffer.size();
    if (full()) { head = (head + 1) % buffer.size(); }
    else { ++sz; }
}

template<typename T, std::size_t Capacity>
void StaticRingBuffer<T, Capacity>::checkHead() {
    if (sz > 0 && head == tail) { buffer[head].destroy(); }
}

template<typename T, std::size_t Capacity>
void StaticRingBuffer<T, Capacity>::pop_front() {
    if (!empty()) {
        buffer[head].destroy();
        head = (head + 1) % buffer.size();
        --sz;
    }
}

} // namespace ctr
} // namespace bl

#endif
