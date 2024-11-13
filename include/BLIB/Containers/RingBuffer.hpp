#ifndef BLIB_CONTAINERS_RINGBUFFER_HPP
#define BLIB_CONTAINERS_RINGBUFFER_HPP

#include <BLIB/Containers/ObjectWrapper.hpp>
#include <utility>
#include <vector>

namespace bl
{
namespace ctr
{
/**
 * @brief Simple circular buffer implementation for small objects. Underlying storage is a
 *        std::vector of buffers. Objects are constructed with placement new and destructors
 *        are called when erased, cleared, or overwritten
 *
 * @tparam T Type of object to store
 * @ingroup Containers
 */
template<typename T>
class RingBuffer {
public:
    /**
     * @brief Construct a new Ring Buffer with the given capacity
     *
     * @param capacity Maximum buffer size
     */
    RingBuffer(std::size_t capacity);

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
    std::vector<ObjectWrapper<T>> buffer;
    std::size_t head;
    std::size_t tail;
    std::size_t sz;

    void increaseSize();
    void checkHead();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
RingBuffer<T>::RingBuffer(std::size_t cap)
: buffer(cap)
, head(0)
, tail(0)
, sz(0) {}

template<typename T>
T& RingBuffer<T>::operator[](std::size_t i) {
    return buffer[(head + i) % buffer.size()].get();
}

template<typename T>
const T& RingBuffer<T>::operator[](std::size_t i) const {
    return buffer[(head + i) % buffer.size()].get();
}

template<typename T>
T& RingBuffer<T>::front() {
    return buffer[head].get();
}

template<typename T>
const T& RingBuffer<T>::front() const {
    return buffer[head].get();
}

template<typename T>
T& RingBuffer<T>::back() {
    return buffer[(head + sz - 1) % buffer.size()].get();
}

template<typename T>
const T& RingBuffer<T>::back() const {
    return buffer[(head + sz - 1) % buffer.size()].get();
}

template<typename T>
std::size_t RingBuffer<T>::size() const {
    return sz;
}

template<typename T>
bool RingBuffer<T>::empty() const {
    return sz == 0;
}

template<typename T>
std::size_t RingBuffer<T>::capacity() const {
    return buffer.size();
}

template<typename T>
bool RingBuffer<T>::full() const {
    return size() == capacity();
}

template<typename T>
void RingBuffer<T>::push_back(const T& obj) {
    checkHead();
    buffer[tail].emplace(obj);
    increaseSize();
}

template<typename T>
void RingBuffer<T>::push_back(T&& obj) {
    checkHead();
    buffer[tail].emplace(std::forward<T>(obj));
    increaseSize();
}

template<typename T>
template<typename... TArgs>
void RingBuffer<T>::emplace_back(TArgs&&... args) {
    checkHead();
    buffer[tail].emplace(std::forward<TArgs>(args)...);
    increaseSize();
}

template<typename T>
void RingBuffer<T>::clear() {
    for (unsigned int i = 0; i < size(); ++i) { buffer[(head + i) % buffer.size()].destroy(); }
    head = 0;
    tail = 0;
    sz   = 0;
}

template<typename T>
void RingBuffer<T>::increaseSize() {
    tail = (tail + 1) % buffer.size();
    if (full()) { head = (head + 1) % buffer.size(); }
    else { ++sz; }
}

template<typename T>
void RingBuffer<T>::checkHead() {
    if (sz > 0 && head == tail) { buffer[head].destroy(); }
}

template<typename T>
void RingBuffer<T>::pop_front() {
    if (!empty()) {
        buffer[head].destroy();
        head = (head + 1) % buffer.size();
        --sz;
    }
}

} // namespace ctr
} // namespace bl

#endif
