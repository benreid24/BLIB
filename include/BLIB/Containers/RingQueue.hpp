#ifndef BLIB_CONTAINERS_RINGQUEUE_HPP
#define BLIB_CONTAINERS_RINGQUEUE_HPP

#include <BLIB/Containers/ObjectWrapper.hpp>
#include <utility>
#include <vector>

namespace bl
{
namespace container
{
/**
 * @brief Queue that uses a fixed size buffer internally to maintain contiguous storage and avoid
 *        allocations. The queue cannot hold more elements than it's capacity
 *
 * @tparam T The type of object to hold in the queue
 * @ingroup Containers
 */
template<typename T>
class RingQueue {
public:
    /**
     * @brief Construct a new Ring Queue with the given size. Allocates all memory here
     *
     * @param size The maximum size the queue can hold
     */
    RingQueue(std::size_t size);

    /**
     * @brief Push a new value into the queue. May fail if the queue is full
     *
     * @param val The value to push into the queue
     * @return True if the element could be added, false if the queue is full
     */
    bool push(const T& val);

    /**
     * @brief Push a new value into the queue. May fail if the queue is full
     *
     * @param val The value to push into the queue
     * @return True if the element could be added, false if the queue is full
     */
    bool push(T&& val);

    /**
     * @brief Constructs a new value at the front of the queue
     *
     * @tparam TArgs The argument types to construct with
     * @param args The arguments to construct with
     * @return True if the element could be added, false if the queue is full
     */
    template<typename... TArgs>
    bool emplace(TArgs&&... args);

    /**
     * @brief Removes the front element from the queue
     *
     */
    void pop();

    /**
     * @brief Returns the front element in the queue
     *
     * @return T& The front element
     */
    T& front();

    /**
     * @brief Returns the front element in the queue
     *
     * @return T& The front element
     */
    const T& front() const;

    /**
     * @brief Returns the back element in the queue
     *
     * @return T& The back element
     */
    T& back();

    /**
     * @brief Returns the back element in the queue
     *
     * @return T& The back element
     */
    const T& back() const;

    /**
     * @brief Returns the maximum size of the queue
     *
     * @return std::size_t The number of elements the queue can hold
     */
    std::size_t capacity() const;

    /**
     * @brief Returns the number of elements in the queue
     *
     * @return std::size_t The number of elements currently contained
     */
    std::size_t size() const;

    /**
     * @brief Returns whether or not the queue has any elements
     *
     * @return True if the size is at least 1, false otherwise
     */
    bool empty() const;

    /**
     * @brief Removes all elements from the queue
     *
     */
    void clear();

private:
    using Buffer        = std::vector<container::ObjectWrapper<T>>;
    using Iterator      = typename Buffer::iterator;
    using ConstIterator = typename Buffer::const_iterator;

    Buffer ring;
    Iterator frontIter;
    Iterator backIter;
    bool notEmpty;

    Iterator incIter(Iterator it);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
RingQueue<T>::RingQueue(std::size_t cap)
: ring(cap)
, frontIter(ring.begin())
, backIter(ring.begin())
, notEmpty(false) {}

template<typename T>
bool RingQueue<T>::push(const T& val) {
    if (notEmpty && backIter == frontIter) return false;
    notEmpty          = true;
    const Iterator nb = incIter(backIter);
    backIter->emplace(val);
    backIter = nb;
    return true;
}

template<typename T>
bool RingQueue<T>::push(T&& val) {
    if (notEmpty && backIter == frontIter) return false;
    notEmpty          = true;
    const Iterator nb = incIter(backIter);
    backIter->emplace(val);
    backIter = nb;
    return true;
}

template<typename T>
template<typename... TArgs>
bool RingQueue<T>::emplace(TArgs&&... args) {
    if (notEmpty && backIter == frontIter) return false;
    notEmpty          = true;
    const Iterator nb = incIter(backIter);
    backIter->emplace(std::forward<TArgs>(args)...);
    backIter = nb;
    return true;
}

template<typename T>
void RingQueue<T>::pop() {
    frontIter->destroy();
    frontIter = incIter(frontIter);
    notEmpty  = frontIter != backIter;
}

template<typename T>
T& RingQueue<T>::front() {
    return frontIter->get();
}

template<typename T>
const T& RingQueue<T>::front() const {
    return frontIter->get();
}

template<typename T>
T& RingQueue<T>::back() {
    Iterator it = backIter != ring.begin() ? backIter - 1 : ring.end() - 1;
    return it->get();
}

template<typename T>
const T& RingQueue<T>::back() const {
    ConstIterator it = backIter != ring.begin() ? backIter - 1 : ring.end() - 1;
    return it->get();
}

template<typename T>
std::size_t RingQueue<T>::capacity() const {
    return ring.size();
}

template<typename T>
std::size_t RingQueue<T>::size() const {
    if (frontIter > backIter) { return (ring.end() - frontIter) + (backIter - ring.begin()); }
    else if (frontIter == backIter) {
        return notEmpty ? capacity() : 0;
    }
    else {
        return backIter - frontIter;
    }
}

template<typename T>
bool RingQueue<T>::empty() const {
    return !notEmpty;
}

template<typename T>
void RingQueue<T>::clear() {
    while (frontIter != backIter) {
        frontIter->destroy();
        frontIter = incIter(frontIter);
    }
    notEmpty = false;
}

template<typename T>
typename RingQueue<T>::Iterator RingQueue<T>::incIter(Iterator it) {
    ++it;
    if (it == ring.end()) return ring.begin();
    return it;
}

} // namespace container
} // namespace bl

#endif
