#ifndef BLIB_CONTAINERS_FASTQUEUE_HPP
#define BLIB_CONTAINERS_FASTQUEUE_HPP

#include <list>
#include <memory>
#include <unordered_map>

namespace bl
{
/**
 * @brief FIFO Container class similar to std::queue but allows promoting an element back to
 *         the front in O(1) time. Values contained must be unique
 *
 * @tparam T Type of data to store
 * @ingroup Util
 */
template<typename T>
class FastQueue {
public:
    /**
     * @brief Push the given value onto the front of the queue
     *
     * @param value Value to add to the front
     * @return True if the value was added, false if already contained
     */
    bool push_front(const T& value);

    /**
     * @brief Promote the given value back to the front of the queue
     *
     * @param value Value to move from its position to the front
     * @param addIfNotFound True to push the value to the front regardless of already contained
     * @return True if the value is now in the front of the queue, false if not found or added
     */
    bool promote_front(const T& value, bool addIfNotFound = false);

    /**
     * @brief Delete the given value from the queue
     *
     * @param value Value to remove
     * @return True if the value was removed, false if not found
     */
    bool remove(const T& value);

    /**
     * @brief Removes the value in the back of the queue and sets the parameter to the removed
     *        value
     *
     * @param value This takes the value of the back of the list
     * @return True if a value was removed, false if already empty
     */
    bool pop_back(T& value);

    /**
     * @brief Removes the value in the back of the queue
     *
     * @return True if a value was removed, false if already empty
     */
    bool pop_back();

    /**
     * @brief Returns the value contained in the back of the queue
     *
     * @return T Value in the back of the queue
     */
    const T& back() const;

    /**
     * @brief Returns whether or not the queue is empty
     *
     */
    bool empty() const;

    /**
     * @brief Returns the number of elements in the queue
     *
     */
    size_t size() const;

    /**
     * @brief Clears the queue
     *
     */
    void clear();

private:
    std::unordered_map<T, typename std::list<T>::iterator> containedValues;
    std::list<T> queue;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
bool FastQueue<T>::push_front(const T& value) {
    if (containedValues.find(value) == containedValues.end()) {
        queue.emplace_front(value);
        containedValues.emplace(value, queue.begin());
        return true;
    }
    return false;
}

template<typename T>
bool FastQueue<T>::promote_front(const T& value, bool insert) {
    auto it = containedValues.find(value);
    if (it != containedValues.end()) {
        queue.erase(it->second);
        queue.emplace_front(value);
        it->second = queue.begin();
        return true;
    }
    else if (insert) {
        queue.emplace_front(value);
        containedValues.emplace(value, queue.begin());
        return true;
    }
    return false;
}

template<typename T>
bool FastQueue<T>::remove(const T& value) {
    auto it = containedValues.find(value);
    if (it != containedValues.end()) {
        queue.erase(it->second);
        containedValues.erase(it);
        return true;
    }
    return false;
}

template<typename T>
bool FastQueue<T>::pop_back(T& value) {
    if (!queue.empty()) {
        value = queue.back();
        containedValues.erase(value);
        queue.pop_back();
        return true;
    }
    return false;
}

template<typename T>
bool FastQueue<T>::pop_back() {
    if (!queue.empty()) {
        containedValues.erase(queue.back());
        queue.pop_back();
        return true;
    }
    return false;
}

template<typename T>
const T& FastQueue<T>::back() const {
    return queue.back();
}

template<typename T>
bool FastQueue<T>::empty() const {
    return queue.empty();
}

template<typename T>
size_t FastQueue<T>::size() const {
    return queue.size();
}

template<typename T>
void FastQueue<T>::clear() {
    queue.clear();
    containedValues.clear();
}

} // namespace bl

#endif
