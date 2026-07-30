#ifndef BLIB_CONTAINERS_PRIORITYQUEUE_HPP
#define BLIB_CONTAINERS_PRIORITYQUEUE_HPP

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
        std::vector<T>::iterator iter;

        Reference(PriorityQueue* ctr, std::vector<T>::iterator iter);

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
    std::vector<T> queue;
    TCmp cmp;

    std::vector<T>::iterator reposition(std::vector<T>::iterator iter);

    friend class Reference;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T, typename TCmp>
T& PriorityQueue<T, TCmp>::front() {
    return queue.front();
}

template<typename T, typename TCmp>
const T& PriorityQueue<T, TCmp>::front() const {
    return queue.front();
}

template<typename T, typename TCmp>
void PriorityQueue<T, TCmp>::pop() {
    queue.pop_back();
    std::pop_heap(queue.begin(), queue.end());
}

template<typename T, typename TCmp>
PriorityQueue<T, TCmp>::Reference PriorityQueue<T, TCmp>::push(const T& value) {
    // const auto newPos = std::lower_bound(queue.begin(), queue.end(), value, cmp);
    queue.emplace_back(value);
    std::push_heap(queue.begin(), queue.end(), cmp);
    // return Reference(this, iter);
    return Reference();
}

template<typename T, typename TCmp>
template<typename... TArgs>
PriorityQueue<T, TCmp>::Reference PriorityQueue<T, TCmp>::emplace(TArgs&&... args) {
    queue.emplace_back(std::forward<TArgs>(args)...);
    std::push_heap(queue.begin(), queue.end(), cmp);
    // return Reference(this, reposition(queue.begin()));
    return Reference();
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
std::vector<T>::iterator PriorityQueue<T, TCmp>::reposition(std::vector<T>::iterator iter) {
    /* const auto newPos = std::lower_bound(queue.begin(), queue.end(), *iter, cmp);
     const auto newIt  = queue.emplace(newPos, std::move(*iter));
     queue.erase(iter);
     return newIt;*/
    return iter;
}

template<typename T, typename TCmp>
PriorityQueue<T, TCmp>::Reference::Reference()
: ctr(nullptr) {}

template<typename T, typename TCmp>
PriorityQueue<T, TCmp>::Reference::Reference(PriorityQueue<T, TCmp>* ctr,
                                             std::vector<T>::iterator iter)
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
