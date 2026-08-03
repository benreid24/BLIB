#ifndef BLIB_CONTAINERS_PRIORITYQUEUE_HPP
#define BLIB_CONTAINERS_PRIORITYQUEUE_HPP

#include <BLIB/Containers/InstrusiveLinkedList.hpp>
#include <algorithm>
#include <list>
#include <type_traits>
#include <vector>

namespace bl
{
namespace ctr
{
namespace detail
{
template<typename T>
struct PriorityQueueNode : public IntrusiveLinkedListNode {
    T value;
    std::uint64_t priority;

    PriorityQueueNode(const T& value, std::uint64_t priority)
    : value(value)
    , priority(priority) {}

    template<typename... TArgs>
    PriorityQueueNode(TArgs&&... args, std::uint64_t priority)
    : value(std::forward<TArgs>(args)...)
    , priority(priority) {}
};

template<typename T>
using PriorityQueueStorage = IntrusiveLinkedList<PriorityQueueNode<T>>;
} // namespace detail

template<typename T, typename TPriority>
class PriorityQueue;

/**
 * @brief Reference to a node in the priority queue. Allows the value to be reordered if priority
 *        changes via the reposition() method
 *
 * @tparam T The type of value being stored
 * @tparam TPriority The priority function
 * @ingroup Containers
 */
template<typename T, typename TPriority>
class PriorityQueueReference {
public:
    /**
     * @brief Creates an empty reference
     */
    PriorityQueueReference();

    /**
     * @brief Copies the reference from another
     */
    PriorityQueueReference(PriorityQueueReference&& move) = default;

    /**
     * @brief Copies the reference from another
     */
    PriorityQueueReference(const PriorityQueueReference& copy) = default;

    /**
     * @brief Copies the reference from another
     */
    PriorityQueueReference& operator=(const PriorityQueueReference& copy) = default;

    /**
     * @brief Copies the reference from another
     */
    PriorityQueueReference& operator=(PriorityQueueReference&& move) = default;

    /**
     * @brief Tests whether this reference is equal to another
     *
     * @param ref The other reference to compare with
     * @return True if the references point to the same node, false otherwise
     */
    bool operator==(const PriorityQueueReference& ref) const;

    /**
     * @brief Tests whether this reference is not equal to another
     *
     * @param ref The other reference to compare with
     * @return True if the references point to different nodes, false otherwise
     */
    bool operator!=(const PriorityQueueReference& ref) const;

    /**
     * @brief Re-evaluates the node priority and repositions it in the queue
     */
    void reposition();

    /**
     * @brief Access the underlying value being referenced
     */
    T* operator->();

    /**
     * @brief Access the underlying value being referenced
     */
    const T* operator->() const;

    /**
     * @brief Access the underlying value being referenced
     */
    T& operator*();

    /**
     * @brief Access the underlying value being referenced
     */
    const T& operator*() const;

private:
    PriorityQueue<T, TPriority>* ctr;
    IntrusiveLinkedListNode* node;

    PriorityQueueReference(PriorityQueue<T, TPriority>* ctr, IntrusiveLinkedListNode* node);

    friend class PriorityQueue<T, TPriority>;
};

/**
 * @brief Re-orderable priority queue that allows for repositioning of elements
 *
 * @tparam T The object type to store
 * @tparam TPriority The priority function. Signature should be (const T&) -> std::uint64_t. Lower
 *                   values are higher priority
 * @ingroup Containers
 */
template<typename T, typename TPriority>
class PriorityQueue {
public:
    static_assert(std::is_object_v<T>, "T must be an object type");

    using TNode      = detail::PriorityQueueNode<T>;
    using TStorage   = detail::PriorityQueueStorage<T>;
    using TReference = PriorityQueueReference<T, TPriority>;

    /**
     * @brief Creates an empty queue
     */
    PriorityQueue();

    /**
     * @brief Creates a copy of another queue
     */
    PriorityQueue(const PriorityQueue& copy) = default;

    /**
     * @brief Move constructor that transfers ownership of resources from another PriorityQueue
     *
     * @param move The source PriorityQueue object to move from
     */
    PriorityQueue(PriorityQueue&& move) = default;

    /**
     * @brief Destructor that releases resources used by the PriorityQueue
     */
    ~PriorityQueue() = default;

    /**
     * @brief Copy assignment operator that copies the contents of another queue
     *
     * @param copy The source queue to copy from
     * @return A reference to this object
     */
    PriorityQueue& operator=(const PriorityQueue& copy) = default;

    /**
     * @brief Move assignment operator that transfers ownership of resources from another queue
     *
     * @param move The source queue to move from
     * @return A reference to this object
     */
    PriorityQueue& operator=(PriorityQueue&& move) = default;

    /**
     * @brief Returns the value at the front of the queue
     */
    T& front();

    /**
     * @brief Returns the value at the front of the queue
     */
    const T& front() const;

    /**
     * @brief Removes the top element from the queue
     */
    void pop();

    /**
     * @brief Adds a new value to the queue
     *
     * @param value The value to add
     * @return A reference to the newly added value
     */
    TReference push(const T& value);

    /**
     * @brief Constructs a new value in place in the queue
     *
     * @tparam TArgs The types of the arguments to forward to the constructor
     * @param args The arguments to forward to the constructor
     * @return A reference to the newly constructed value
     */
    template<typename... TArgs>
    TReference emplace(TArgs&&... args);

    /**
     * @brief Returns whether the queue is empty
     */
    bool empty() const;

    /**
     * @brief Returns the number of elements in the queue
     */
    std::size_t size() const;

private:
    struct Bucket {
        std::uint64_t priority;
        TStorage elements;

        Bucket(std::uint64_t priority)
        : priority(priority) {}

        template<typename... TArgs>
        TStorage::Iterator add(TArgs&&... args) {
            return elements.emplace(elements.end(), std::forward<TArgs>(args)..., priority);
        }

        void remove(TStorage::Iterator iter) { elements.erase(iter); }
    };

    std::size_t numElements;
    std::vector<Bucket> buckets;
    TPriority priorityFunction;

    std::vector<Bucket>::iterator findOrCreateBucket(std::uint64_t priority);
    TStorage::Iterator reposition(TStorage::Iterator iter);

    friend class PriorityQueueReference<T, TPriority>;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, typename TPriority>
PriorityQueue<T, TPriority>::PriorityQueue()
: numElements(0)
, priorityFunction() {}

template<typename T, typename TPriority>
T& PriorityQueue<T, TPriority>::front() {
    // stored in reverse order
    return buckets.back().elements.front().value;
}

template<typename T, typename TPriority>
const T& PriorityQueue<T, TPriority>::front() const {
    // stored in reverse order
    return buckets.back().elements.front().value;
}

template<typename T, typename TPriority>
void PriorityQueue<T, TPriority>::pop() {
    --numElements;
    buckets.back().elements.pop_front();
    if (buckets.back().elements.empty()) { buckets.pop_back(); }
}

template<typename T, typename TPriority>
typename PriorityQueue<T, TPriority>::TReference PriorityQueue<T, TPriority>::push(const T& value) {
    ++numElements;

    const std::uint64_t priority = priorityFunction(value);
    const auto bucketIt          = findOrCreateBucket(priority);
    const auto it                = bucketIt->add(value);
    return PriorityQueueReference(this, it.getNode());
}

template<typename T, typename TPriority>
template<typename... TArgs>
typename PriorityQueue<T, TPriority>::TReference PriorityQueue<T, TPriority>::emplace(
    TArgs&&... args) {
    ++numElements;

    // construct in a temp list so that we can splice into the correct list
    TStorage temp;
    temp.emplace_back(std::forward<TArgs>(args)..., 0);
    const std::uint64_t priority = priorityFunction(temp.back().value);
    temp.back().priority         = priority;

    const auto bucketIt = findOrCreateBucket(priority);
    bucketIt->elements.splice(bucketIt->elements.end(), temp);
    return PriorityQueueReference(this, (--bucketIt->elements.end()).getNode());
}

template<typename T, typename TPriority>
bool PriorityQueue<T, TPriority>::empty() const {
    return buckets.empty();
}

template<typename T, typename TPriority>
std::size_t PriorityQueue<T, TPriority>::size() const {
    return numElements;
}

template<typename T, typename TPriority>
std::vector<typename PriorityQueue<T, TPriority>::Bucket>::iterator
PriorityQueue<T, TPriority>::findOrCreateBucket(std::uint64_t priority) {
    auto bucketIt = std::lower_bound(
        buckets.begin(), buckets.end(), priority, [](const Bucket& b, std::uint64_t p) {
            return b.priority < p; // TODO - reverse?
        });
    if (bucketIt == buckets.end() || bucketIt->priority != priority) {
        bucketIt = buckets.emplace(bucketIt, priority); // TODO - insert after?
    }
    return bucketIt;
}

template<typename T, typename TPriority>
PriorityQueue<T, TPriority>::TStorage::Iterator PriorityQueue<T, TPriority>::reposition(
    TStorage::Iterator iter) {
    const std::uint64_t priority = priorityFunction(iter->value);
    if (priority == iter->priority) { return iter; }

    const auto newBucketIt = findOrCreateBucket(priority);
    const auto oldBucketIt = std::lower_bound(
        buckets.begin(), buckets.end(), iter->priority, [](const Bucket& b, std::uint64_t p) {
            return b.priority < p; // TODO - reverse?
        });
    if (oldBucketIt == buckets.end() || oldBucketIt->priority != iter->priority) {
        // element not found, error condition. Need to erase new bucket if empty
        if (newBucketIt->elements.empty()) { buckets.erase(newBucketIt); }
        return iter;
    }

    newBucketIt->elements.splice(newBucketIt->elements.end(), oldBucketIt->elements, iter);
    iter->priority = newBucketIt->priority;
    auto resultIt  = --newBucketIt->elements.end();
    if (oldBucketIt->elements.empty()) { buckets.erase(oldBucketIt); }

    return resultIt;
}

template<typename T, typename TPriority>
PriorityQueueReference<T, TPriority>::PriorityQueueReference()
: ctr(nullptr)
, node(nullptr) {}

template<typename T, typename TPriority>
PriorityQueueReference<T, TPriority>::PriorityQueueReference(PriorityQueue<T, TPriority>* ctr,
                                                             IntrusiveLinkedListNode* node)
: ctr(ctr)
, node(node) {}

template<typename T, typename TPriority>
bool PriorityQueueReference<T, TPriority>::operator==(const PriorityQueueReference& ref) const {
    return node == ref.node;
}

template<typename T, typename TPriority>
bool PriorityQueueReference<T, TPriority>::operator!=(const PriorityQueueReference& ref) const {
    return node != ref.node;
}

template<typename T, typename TPriority>
T* PriorityQueueReference<T, TPriority>::operator->() {
    return &static_cast<detail::PriorityQueueNode<T>&>(*node).value;
}

template<typename T, typename TPriority>
const T* PriorityQueueReference<T, TPriority>::operator->() const {
    return &static_cast<const detail::PriorityQueueNode<T>&>(*node).value;
}

template<typename T, typename TPriority>
T& PriorityQueueReference<T, TPriority>::operator*() {
    return static_cast<detail::PriorityQueueNode<T>&>(*node).value;
}

template<typename T, typename TPriority>
const T& PriorityQueueReference<T, TPriority>::operator*() const {
    return static_cast<detail::PriorityQueueNode<T>&>(*node).value;
}

template<typename T, typename TPriority>
void PriorityQueueReference<T, TPriority>::reposition() {
    ctr->reposition(detail::PriorityQueueStorage<T>::Iterator(node));
}

} // namespace ctr
} // namespace bl

#endif
