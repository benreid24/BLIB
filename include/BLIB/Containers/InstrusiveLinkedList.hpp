#ifndef BLIB_CONTAINERS_INTRUSIVELINKEDLIST_HPP
#define BLIB_CONTAINERS_INTRUSIVELINKEDLIST_HPP

#include <type_traits>

namespace bl
{
namespace ctr
{
template<typename T>
class IntrusiveLinkedList;

/**
 * @brief Base class for types that are to be stored in IntrusiveLinkedList. Must be inherited by
 *        the type to be stored in the list
 *
 * @ingroup Containers
 */
class IntrusiveLinkedListNode {
public:
    /**
     * @brief Initializes the node
     */
    IntrusiveLinkedListNode()
    : next(nullptr)
    , prev(nullptr) {}

private:
    IntrusiveLinkedListNode* next;
    IntrusiveLinkedListNode* prev;

    template<typename T>
    friend class IntrusiveLinkedList;
};

/**
 * @brief Special type of linked list where each allocated list node is the payload object itself.
 *        Payload types must inherit from IntrusiveLinkedListNode
 *
 * @tparam T The payload type. Must inherit from IntrusiveLinkedListNode
 * @ingroup Containers
 */
template<typename T>
class IntrusiveLinkedList {
    static_assert(std::is_base_of<IntrusiveLinkedListNode, T>::value,
                  "T must derive from IntrusiveLinkedListNode");

public:
    /**
     * @brief Iterator for the intrusive linked list
     */
    class Iterator {
    public:
        /**
         * @brief Creates an empty iterator
         */
        Iterator();

        /**
         * @brief Creates an iterator for the given node
         *
         * @param node The object to create the iterator for
         */
        explicit Iterator(IntrusiveLinkedListNode* node);

        /**
         * @brief Creates the iterator from another iterator
         *
         * @param copy The iterator to copy
         */
        Iterator(const Iterator& copy);

        /**
         * @brief Creates the iterator for the given node and list container
         *
         * @param list The containing list. Useful if decrementing the end iterator
         * @param node The node to point to. Can be nullptr for the end iterator
         */
        Iterator(IntrusiveLinkedList* list, IntrusiveLinkedListNode* node);

        /**
         * @brief Copies the iterator from another
         *
         * @param copy The iterator to copy from
         * @return A reference to this iterator
         */
        Iterator& operator=(const Iterator& copy);

        /**
         * @brief Access the underlying value pointed to by the iterator
         */
        T& operator*();

        /**
         * @brief Access the underlying value pointed to by the iterator
         */
        const T& operator*() const;

        /**
         * @brief Access the underlying value pointed to by the iterator
         */
        T* operator->();

        /**
         * @brief Access the underlying value pointed to by the iterator
         */
        const T* operator->() const;

        /**
         * @brief Increments the iterator
         */
        Iterator& operator++();

        /**
         * @brief Increments the iterator
         */
        Iterator operator++(int);

        /**
         * @brief Increments the iterator
         */
        Iterator& operator--();

        /**
         * @brief Increments the iterator
         */
        Iterator operator--(int);

        /**
         * @brief Compares the iterator with another
         * @param other The iterator to compare with
         * @return True if the iterators point to the same node, false otherwise
         */
        bool operator==(const Iterator& other) const;

        /**
         * @brief Compares the iterator with another
         * @param other The iterator to compare with
         * @return True if the iterators point to different nodes, false otherwise
         */
        bool operator!=(const Iterator& other) const;

    private:
        IntrusiveLinkedList* list;
        IntrusiveLinkedListNode* node;

        friend class IntrusiveLinkedList;
    };

    /**
     * @brief Creates an empty intrusive linked list
     */
    IntrusiveLinkedList();

    /**
     * @brief Creates a copy of another intrusive linked list
     *
     * @param copy The list to copy
     */
    IntrusiveLinkedList(const IntrusiveLinkedList& copy);

    /**
     * @brief Creates a new intrusive linked list by moving another list into this one
     *
     * @param move The list to adopt nodes from
     */
    IntrusiveLinkedList(IntrusiveLinkedList&& move);

    /**
     * @brief Releases all storage
     */
    ~IntrusiveLinkedList();

    /**
     * @brief Copy assignment operator for the intrusive linked list
     *
     * @param copy The intrusive linked list to copy from
     * @return A reference to this object
     */
    IntrusiveLinkedList& operator=(const IntrusiveLinkedList& copy);

    /**
     * @brief Move assignment operator that adopts nodes from another list
     *
     * @param move The list to move from.
     * @return A reference to this object
     */
    IntrusiveLinkedList& operator=(IntrusiveLinkedList&& move);

    /**
     * @brief Inserts a new node at the front of the list
     *
     * @param value The value to insert
     * @return An iterator pointing to the newly inserted node
     */
    Iterator push_front(const T& value);

    /**
     * @brief Inserts a new node at the back of the list
     *
     * @param value The value to insert
     * @return An iterator pointing to the newly inserted node
     */
    Iterator push_back(const T& value);

    /**
     * @brief Constructs a new node at the front of the list
     *
     * @tparam TArgs The types of the arguments to forward to the constructor
     * @param args The arguments to forward to the constructor
     * @return An iterator pointing to the newly constructed node
     */
    template<typename... TArgs>
    Iterator emplace_front(TArgs&&... args);

    /**
     * @brief Constructs a new node at the back of the list
     *
     * @tparam TArgs The types of the arguments to forward to the constructor
     * @param args The arguments to forward to the constructor
     * @return An iterator pointing to the newly constructed node
     */
    template<typename... TArgs>
    Iterator emplace_back(TArgs&&... args);

    /**
     * @brief Inserts a new node before the given iterator
     *
     * @param iter The iterator to insert before
     * @param value The value to insert
     * @return An iterator pointing to the newly inserted node
     */
    Iterator insert(Iterator iter, const T& value);

    /**
     * @brief Constructs a new node before the given iterator
     *
     * @tparam TArgs The types of the arguments to forward to the constructor
     * @param iter The iterator to insert before
     * @param args The arguments to forward to the constructor
     * @return An iterator pointing to the newly constructed node
     */
    template<typename... TArgs>
    Iterator emplace(Iterator iter, TArgs&&... args);

    /**
     * @brief Erases the node pointed to by the given iterator
     *
     * @param iter The iterator pointing to the node to erase
     * @return An iterator pointing to the node after the erased node
     */
    Iterator erase(Iterator iter);

    /**
     * @brief Splices all nodes from another list into this list before the given iterator
     *
     * @param iter The iterator to splice before
     * @param other The other list to splice from. This list will be empty after the operation
     */
    void splice(Iterator iter, IntrusiveLinkedList<T>& other);

    /**
     * @brief Splices a single node from another list into this list before the given iterator
     *
     * @param iter The iterator to splice before
     * @param other The other list to splice from
     * @param node The iterator pointing to the node in the other list to splice
     */
    void splice(Iterator iter, IntrusiveLinkedList<T>& other, Iterator node);

    /**
     * @brief Removes all nodes from the list
     */
    void clear();

    /**
     * @brief Returns an iterator to the first node
     */
    Iterator begin();

    /**
     * @brief Returns an iterator to the end of the list (one past the last node)
     */
    Iterator end();

    /**
     * @brief Returns the first value in the list
     */
    T& front();

    /**
     * @brief Returns the first value in the list
     */
    const T& front() const;

    /**
     * @brief Returns the last value in the list
     */
    T& back();

    /**
     * @brief Returns the last value in the list
     */
    const T& back() const;

    /**
     * @brief Returns whether the list is empty
     */
    bool empty() const;

    /**
     * @brief Returns the number of nodes in the list
     */
    std::size_t size() const;

private:
    IntrusiveLinkedListNode* head;
    IntrusiveLinkedListNode* tail;
    std::size_t listSize;

    friend class IntrusiveLinkedListNode;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
IntrusiveLinkedList<T>::IntrusiveLinkedList()
: head(nullptr)
, tail(nullptr)
, listSize(0) {}

template<typename T>
IntrusiveLinkedList<T>::IntrusiveLinkedList(const IntrusiveLinkedList& copy)
: IntrusiveLinkedList() {
    *this = copy;
}

template<typename T>
IntrusiveLinkedList<T>::IntrusiveLinkedList(IntrusiveLinkedList&& move)
: IntrusiveLinkedList() {
    *this = std::move(move);
}

template<typename T>
IntrusiveLinkedList<T>::~IntrusiveLinkedList() {
    clear();
}

template<typename T>
IntrusiveLinkedList<T>& IntrusiveLinkedList<T>::operator=(const IntrusiveLinkedList& copy) {
    clear();
    if (!copy.empty()) {
        for (IntrusiveLinkedListNode* it = copy.head; it != nullptr; it = it->next) {
            push_back(*static_cast<T*>(it));
        }
    }
    return *this;
}

template<typename T>
IntrusiveLinkedList<T>& IntrusiveLinkedList<T>::operator=(IntrusiveLinkedList&& move) {
    listSize      = move.listSize;
    head          = move.head;
    tail          = move.tail;
    move.listSize = 0;
    move.head     = nullptr;
    move.tail     = nullptr;
    return *this;
}

template<typename T>
typename IntrusiveLinkedList<T>::Iterator IntrusiveLinkedList<T>::push_front(const T& value) {
    IntrusiveLinkedListNode* newNode = new T(value);
    if (head == nullptr) {
        head = newNode;
        tail = newNode;
    }
    else {
        newNode->next = head;
        head->prev    = newNode;
        head          = newNode;
    }
    ++listSize;
    return Iterator(this, newNode);
}

template<typename T>
typename IntrusiveLinkedList<T>::Iterator IntrusiveLinkedList<T>::push_back(const T& value) {
    IntrusiveLinkedListNode* newNode = new T(value);
    if (tail == nullptr) {
        head = newNode;
        tail = newNode;
    }
    else {
        newNode->prev = tail;
        tail->next    = newNode;
        tail          = newNode;
    }
    ++listSize;
    return Iterator(this, newNode);
}

template<typename T>
typename IntrusiveLinkedList<T>::Iterator IntrusiveLinkedList<T>::insert(Iterator iter,
                                                                         const T& value) {
    IntrusiveLinkedListNode* newNode = new T(value);
    newNode->next                    = iter.node;
    if (iter.node != nullptr) {
        newNode->prev   = iter.node->prev;
        iter.node->prev = newNode;
        if (iter.node == head) { head = newNode; }
        else { newNode->prev->next = newNode; }
    }
    else {
        newNode->prev = tail;
        if (tail != nullptr) { tail->next = newNode; }
        else { head = newNode; }
        tail = newNode;
    }
    ++listSize;
    return Iterator(this, newNode);
}

template<typename T>
template<typename... TArgs>
typename IntrusiveLinkedList<T>::Iterator IntrusiveLinkedList<T>::emplace_front(TArgs&&... args) {
    IntrusiveLinkedListNode* newNode = new T(std::forward<TArgs>(args)...);
    if (head == nullptr) {
        head = newNode;
        tail = newNode;
    }
    else {
        newNode->next = head;
        head->prev    = newNode;
        head          = newNode;
    }
    ++listSize;
    return Iterator(this, newNode);
}

template<typename T>
template<typename... TArgs>
typename IntrusiveLinkedList<T>::Iterator IntrusiveLinkedList<T>::emplace_back(TArgs&&... args) {
    IntrusiveLinkedListNode* newNode = new T(std::forward<TArgs>(args)...);
    if (tail == nullptr) {
        head = newNode;
        tail = newNode;
    }
    else {
        newNode->prev = tail;
        tail->next    = newNode;
        tail          = newNode;
    }
    ++listSize;
    return Iterator(this, newNode);
}

template<typename T>
template<typename... TArgs>
typename IntrusiveLinkedList<T>::Iterator IntrusiveLinkedList<T>::emplace(Iterator iter,
                                                                          TArgs&&... args) {
    IntrusiveLinkedListNode* newNode = new T(std::forward<TArgs>(args)...);
    newNode->next                    = iter.node;
    if (iter.node != nullptr) {
        newNode->prev   = iter.node->prev;
        iter.node->prev = newNode;
        if (iter.node == head) { head = newNode; }
        else { newNode->prev->next = newNode; }
    }
    else {
        newNode->prev = tail;
        if (tail != nullptr) { tail->next = newNode; }
        else { head = newNode; }
        tail = newNode;
    }
    ++listSize;
    return Iterator(this, newNode);
}

template<typename T>
typename IntrusiveLinkedList<T>::Iterator IntrusiveLinkedList<T>::erase(Iterator iter) {
    IntrusiveLinkedListNode* next = iter.node->next;
    if (iter.node->prev) { iter.node->prev->next = iter.node->next; }
    else { head = iter.node->next; }
    if (iter.node->next) { iter.node->next->prev = iter.node->prev; }
    else { tail = iter.node->prev; }
    delete static_cast<T*>(iter.node);
    --listSize;
    return Iterator(this, next);
}

template<typename T>
void IntrusiveLinkedList<T>::splice(Iterator iter, IntrusiveLinkedList<T>& other) {
    IntrusiveLinkedListNode* it   = other.head;
    IntrusiveLinkedListNode* prev = iter.node ? iter.node->prev : nullptr;
    while (it != nullptr) {
        IntrusiveLinkedListNode* nextIt = it->next;

        it->next = iter.node;
        if (it->next == nullptr) { tail = it; }
        if (prev == nullptr) {
            if (tail == nullptr) { tail = it; }
            head     = it;
            it->prev = nullptr;
        }
        else {
            prev->next = it;
            it->prev   = prev;
            prev       = it;
        }
        iter.node->prev = it;
        ++listSize;
        it = nextIt;
    }

    other.head     = nullptr;
    other.tail     = nullptr;
    other.listSize = 0;
}

template<typename T>
void IntrusiveLinkedList<T>::splice(Iterator iter, IntrusiveLinkedList<T>& other, Iterator node) {
    --other.listSize;
    if (other.tail == node.node) {
        if (other.head != node.node) { other.tail = node.node->prev; }
        else { other.tail = nullptr; }
    }
    if (other.head == node.node) {
        if (other.tail != node.node) { other.head = node.node->next; }
        else { other.head = nullptr; }
    }

    if (node.node->next != nullptr) { node.node->next->prev = node.node->prev; }
    if (node.node->prev != nullptr) { node.node->prev->next = node.node->next; }

    node.node->next = iter.node;
    if (iter.node) {
        IntrusiveLinkedListNode* prevNode = iter.node->prev;
        node.node->prev                   = prevNode;
        iter.node->prev                   = node.node;
        if (prevNode) { prevNode->next = node.node; }
        else { head = node.node; }
    }
    else {
        if (tail) { tail->next = node.node; }
        else { head = node.node; }
        node.node->prev = tail;
        tail            = node.node;
    }
    ++listSize;
}

template<typename T>
void IntrusiveLinkedList<T>::clear() {
    while (head != nullptr) {
        IntrusiveLinkedListNode* next = head->next;
        delete static_cast<T*>(head);
        head = next;
    }
    tail     = nullptr;
    listSize = 0;
}

template<typename T>
typename IntrusiveLinkedList<T>::Iterator IntrusiveLinkedList<T>::begin() {
    return Iterator(this, head);
}

template<typename T>
typename IntrusiveLinkedList<T>::Iterator IntrusiveLinkedList<T>::end() {
    return Iterator(this, nullptr);
}

template<typename T>
T& IntrusiveLinkedList<T>::front() {
    return *static_cast<T*>(head);
}

template<typename T>
const T& IntrusiveLinkedList<T>::front() const {
    return *static_cast<T*>(head);
}

template<typename T>
T& IntrusiveLinkedList<T>::back() {
    return *static_cast<T*>(tail);
}

template<typename T>
const T& IntrusiveLinkedList<T>::back() const {
    return *static_cast<T*>(tail);
}

template<typename T>
bool IntrusiveLinkedList<T>::empty() const {
    return listSize == 0;
}

template<typename T>
std::size_t IntrusiveLinkedList<T>::size() const {
    return listSize;
}

template<typename T>
IntrusiveLinkedList<T>::Iterator::Iterator()
: list(nullptr)
, node(nullptr) {}

template<typename T>
IntrusiveLinkedList<T>::Iterator::Iterator(IntrusiveLinkedListNode* node)
: list(nullptr)
, node(node) {}

template<typename T>
IntrusiveLinkedList<T>::Iterator::Iterator(const Iterator& copy)
: list(copy.list)
, node(copy.node) {}

template<typename T>
IntrusiveLinkedList<T>::Iterator::Iterator(IntrusiveLinkedList* list, IntrusiveLinkedListNode* node)
: list(list)
, node(node) {}

template<typename T>
typename IntrusiveLinkedList<T>::Iterator& IntrusiveLinkedList<T>::Iterator::operator=(
    const Iterator& copy) {
    list = copy.list;
    node = copy.node;
    return *this;
}

template<typename T>
T& IntrusiveLinkedList<T>::Iterator::operator*() {
    return *static_cast<T*>(node);
}

template<typename T>
const T& IntrusiveLinkedList<T>::Iterator::operator*() const {
    return *static_cast<T*>(node);
}

template<typename T>
T* IntrusiveLinkedList<T>::Iterator::operator->() {
    return static_cast<T*>(node);
}

template<typename T>
const T* IntrusiveLinkedList<T>::Iterator::operator->() const {
    return static_cast<T*>(node);
}

template<typename T>
typename IntrusiveLinkedList<T>::Iterator& IntrusiveLinkedList<T>::Iterator::operator++() {
    if (node != nullptr) { node = node->next; }
    return *this;
}

template<typename T>
typename IntrusiveLinkedList<T>::Iterator IntrusiveLinkedList<T>::Iterator::operator++(int) {
    Iterator copy = *this;
    ++(*this);
    return copy;
}

template<typename T>
typename IntrusiveLinkedList<T>::Iterator& IntrusiveLinkedList<T>::Iterator::operator--() {
    if (node != nullptr) { node = node->prev; }
    else if (list != nullptr) { node = list->tail; }
    return *this;
}

template<typename T>
typename IntrusiveLinkedList<T>::Iterator IntrusiveLinkedList<T>::Iterator::operator--(int) {
    Iterator copy = *this;
    --(*this);
    return copy;
}

template<typename T>
bool IntrusiveLinkedList<T>::Iterator::operator==(const Iterator& other) const {
    return node == other.node;
}

template<typename T>
bool IntrusiveLinkedList<T>::Iterator::operator!=(const Iterator& other) const {
    return node != other.node;
}

} // namespace ctr
} // namespace bl

#endif
