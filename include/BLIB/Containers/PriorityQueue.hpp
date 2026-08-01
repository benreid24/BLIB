#ifndef BLIB_CONTAINERS_PRIORITYQUEUE_HPP
#define BLIB_CONTAINERS_PRIORITYQUEUE_HPP

#include <BLIB/Allocators/VectoredPools.hpp>
#include <algorithm>
#include <list>
#include <type_traits>
#include <vector>

namespace bl
{
namespace ctr
{
template<typename T, typename TCmp = std::less<T>>
class PriorityQueue {
public:
    static_assert(std::is_object_v<T>, "T must be an object type");

    using TStorage = std::list<T, alloc::VectoredPools<T>>;

    class Reference {
    public:
        Reference();

        Reference(const Reference& copy) = default;

        Reference& operator=(const Reference& copy) = default;

        bool operator==(const Reference& ref) const;

        bool operator!=(const Reference& ref) const;

        void reposition();

        T* operator->();

        const T* operator->() const;

        T& operator*();

        const T& operator*() const;

    private:
        PriorityQueue* ctr;
        TStorage::iterator iter;

        Reference(PriorityQueue* ctr, TStorage::iterator iter);

        friend class PriorityQueue;
    };

    PriorityQueue() = default;

    PriorityQueue(const PriorityQueue& copy) = default;

    PriorityQueue(PriorityQueue&& move) = default;

    ~PriorityQueue() = default;

    PriorityQueue& operator=(const PriorityQueue& copy) = default;

    PriorityQueue& operator=(PriorityQueue&& move) = default;

    T& front();

    const T& front() const;

    void pop();

    Reference push(const T& value);

    template<typename... TArgs>
    Reference emplace(TArgs&&... args);

    bool empty() const;

    std::size_t size() const;

private:
    struct Cmp {
        TCmp cmp;

        bool operator()(const TStorage::iterator& lhs, const T& rhs) const {
            // stored in reverse order
            return cmp(rhs, *lhs);
        }

        bool operator()(const TStorage::iterator& lhs, const TStorage::iterator& rhs) const {
            // stored in reverse order
            return cmp(*rhs, *lhs);
        }

        bool operator()(const T& lhs, const TStorage::iterator& rhs) const {
            // stored in reverse order
            return cmp(*rhs, lhs);
        }
    };

    TStorage queue;
    std::vector<typename TStorage::iterator> iterators;
    Cmp cmp;

    TStorage::iterator reposition(TStorage::iterator iter);

    friend class Reference;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, typename TCmp>
T& PriorityQueue<T, TCmp>::front() {
    // stored in reverse order
    return queue.back();
}

template<typename T, typename TCmp>
const T& PriorityQueue<T, TCmp>::front() const {
    // stored in reverse order
    return queue.back();
}

template<typename T, typename TCmp>
void PriorityQueue<T, TCmp>::pop() {
    /*queue.pop_back();
    std::pop_heap(queue.begin(), queue.end());*/
    queue.pop_back();
    iterators.pop_back();
}

template<typename T, typename TCmp>
PriorityQueue<T, TCmp>::Reference PriorityQueue<T, TCmp>::push(const T& value) {
    const auto newPosIt  = std::lower_bound(iterators.begin(), iterators.end(), value, cmp);
    const auto newPos    = newPosIt != iterators.end() ? *newPosIt : queue.end();
    const auto queueIter = queue.emplace(newPos, value);
    iterators.insert(newPosIt, queueIter);
    /*queue.emplace_back(value);
    std::push_heap(queue.begin(), queue.end(), cmp);*/
    return Reference(this, queueIter);
    // return Reference();
}

template<typename T, typename TCmp>
template<typename... TArgs>
PriorityQueue<T, TCmp>::Reference PriorityQueue<T, TCmp>::emplace(TArgs&&... args) {
    queue.emplace_front(std::forward<TArgs>(args)...);
    // std::push_heap(queue.begin(), queue.end(), cmp);
    return Reference(this, reposition(queue.begin()));
    // return Reference();
}

template<typename T, typename TCmp>
bool PriorityQueue<T, TCmp>::empty() const {
    return queue.empty();
}

template<typename T, typename TCmp>
std::size_t PriorityQueue<T, TCmp>::size() const {
    return queue.size();
}

template<typename T, typename TCmp>
typename PriorityQueue<T, TCmp>::TStorage::iterator PriorityQueue<T, TCmp>::reposition(
    typename TStorage::iterator iter) {
    // TODO - custom linked list to avoid delete and re-insert?
    const auto newPosIt = std::lower_bound(iterators.begin(), iterators.end(), *iter, cmp);
    const auto newPos   = newPosIt != iterators.end() ? *newPosIt : queue.end();
    const auto newIt    = queue.emplace(newPos, std::move(*iter));
    queue.erase(iter);
    // TODO - can we make this non-linear?
    for (auto it = iterators.begin(); it != iterators.end(); ++it) {
        if (*it == iter) {
            iterators.erase(it);
            break;
        }
    }
    return newIt;
    // return iter;
}

template<typename T, typename TCmp>
PriorityQueue<T, TCmp>::Reference::Reference()
: ctr(nullptr) {}

template<typename T, typename TCmp>
PriorityQueue<T, TCmp>::Reference::Reference(PriorityQueue<T, TCmp>* ctr, TStorage::iterator iter)
: ctr(ctr)
, iter(iter) {}

template<typename T, typename TCmp>
bool PriorityQueue<T, TCmp>::Reference::operator==(const Reference& ref) const {
    return ctr == ref.ctr && iter == ref.iter;
}

template<typename T, typename TCmp>
bool PriorityQueue<T, TCmp>::Reference::operator!=(const Reference& ref) const {
    return ctr != ref.ctr && iter != ref.iter;
}

template<typename T, typename TCmp>
T* PriorityQueue<T, TCmp>::Reference::operator->() {
    return &*iter;
}

template<typename T, typename TCmp>
const T* PriorityQueue<T, TCmp>::Reference::operator->() const {
    return &*iter;
}

template<typename T, typename TCmp>
T& PriorityQueue<T, TCmp>::Reference::operator*() {
    return *iter;
}

template<typename T, typename TCmp>
const T& PriorityQueue<T, TCmp>::Reference::operator*() const {
    return *iter;
}

template<typename T, typename TCmp>
void PriorityQueue<T, TCmp>::Reference::reposition() {
    ctr->reposition(iter);
}

} // namespace ctr
} // namespace bl

#endif
