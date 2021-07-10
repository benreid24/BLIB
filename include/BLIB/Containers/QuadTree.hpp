#ifndef BLIB_CONTAINERS_QUADTREE_HPP
#define BLIB_CONTAINERS_QUADTREE_HPP

#include <cstdint>
#include <vector>

namespace bl
{
namespace container
{
/**
 * @brief Fairly basic implementation of a quadtree for spatially partitioning objects. Works most
 *        efficiently if the payload type is trivially movable. Stored objects are keyed on position
 *
 * @tparam TCoord The type of coordinate to use. Must contain members x and y and define operator ==
 * @tparam TPayload The type of payload to store
 * @ingroup Containers
 */
template<typename TCoord, typename TPayload>
class QuadTree {
    class Partition;

public:
    struct Area;

    /**
     * @brief Stores an iterable result set from a QuadTree query
     *
     * @ingroup Containers
     *
     */
    class ResultSet {
    public:
        /**
         * @brief Special iterator for traversing a ResultSet
         *
         * @ingroup Containers
         *
         */
        class Iterator {
        public:
            /**
             * @brief Dereference the iterator to the base value
             *
             * @return TPayload& A reference to the underlying value. Iterator must be valid
             */
            TPayload& operator*();

            /**
             * @brief Returns a pointer to the underlying value
             *
             * @return TPayload* Pointer to underlying payload. Iterator must be valid
             */
            TPayload* operator->();

            /**
             * @brief Returns the position of the underlying object
             *
             * @return const TCoord& The position of the object this iterator points to
             */
            const TCoord& position();

            /**
             * @brief Increments the iterator to the next available value
             *
             */
            void operator++();

            /**
             * @brief Compares this iterator to another
             *
             * @param it The iterator to compare to
             * @return True if equal, false otherwise
             */
            bool operator==(const Iterator& it) const;

            /**
             * @brief Compares this iterator to another
             *
             * @param it The iterator to compare to
             * @return True if not equal, false otherwise
             */
            bool operator!=(const Iterator& it) const;

        private:
            std::vector<Partition*>& leafs;
            std::size_t leafIndex;
            std::size_t valueIndex;

            Iterator(std::vector<Partition*>& leafs, std::size_t leafIndex, std::size_t valueIndex);

            friend class ResultSet;
        };

        /**
         * @brief Returns an iterator to the first item in the set
         *
         * @return Iterator An iterator to the first item in the set
         */
        Iterator begin();

        /**
         * @brief Returns an iterator to the last item in the set
         *
         * @return Iterator An iterator to the last item in the set
         */
        Iterator end();

    private:
        std::vector<Partition*> leafs;

        ResultSet() = default;

        friend class QuadTree;
    };

    /**
     * @brief Creates an empty QuadTree
     *
     */
    QuadTree();

    /**
     * @brief Copies the QuadTree
     *
     * @param copy The tree to copy from
     */
    QuadTree(const QuadTree& copy);

    /**
     * @brief Takes ownership of the objects stored in the given tree
     *
     * @param move The tree to inherit objects and partitions from
     */
    QuadTree(QuadTree&& move);

    /**
     * @brief Cleans up resources
     *
     */
    ~QuadTree();

    /**
     * @brief Set the max number of objects allowed in a single partition. Partitions that exceed
     *        this size are subdivided until each new partition is under the max size
     *
     * @param maxLoad The maximum number of objects allowed in a single quad
     */
    void setMaxLoad(std::size_t maxLoad);

    /**
     * @brief Set the area over which partitioning will occur. QuadTree can handle objects outside
     *        of the given area but is more performant if the area is accurate
     *
     * @param area The spatial region objects will be contained within
     */
    void setIndexedArea(const Area& area);

    /**
     * @brief Adds the given object to the QuadTree at the given position. Objects are keyed on
     *        position and may only be removed if their exact position in the tree is available
     *
     * @param position The position to add the object at
     * @param value The object to add
     */
    void add(const TCoord& position, const TPayload& value);

    /**
     * @brief Adds the given object to the QuadTree at the given position. Objects are keyed on
     *        position and may only be removed if their exact position in the tree is available
     *
     * @param position The position to add the object at
     * @param value The object to add
     */
    void add(const TCoord& position, TPayload&& value);

    /**
     * @brief Updates the position of an object using its old position
     *
     * @param oldPosition The position the object is currently at
     * @param newPosition The position to move the object to
     * @return True if an object was moved, false if no object was moved
     */
    bool updatePosition(const TCoord& oldPosition, const TCoord& newPosition);

    /**
     * @brief Removes an object from the QuadTree by its position
     *
     * @param position The position of the object to remove
     * @return True if an object was removed, false if no object is at the position
     */
    bool remove(const TCoord& position);

    /**
     * @brief Returns an iterable set of all contained objects
     *
     * @return ResultSet A set containing all stored objects
     */
    ResultSet all();

    /**
     * @brief Returns a set of objects contained or nearby the given area
     *
     * @param area The area to search for objects within
     * @return ResultSet A set containing objects near the given area
     */
    ResultSet getInArea(const Area& area);

    /**
     * @brief Returns objects immediately nearby the given position. This returns a single partition
     *        of objects and as such may omit nearby objects if they are in a different partition
     *
     * @param position The position to query for objects at
     * @return ResultSet A set containing objects in the same partition as the given position
     */
    ResultSet getQuad(const TCoord& position);

    /**
     * @brief Removes all stored objects
     *
     */
    void clear();

    /**
     * @brief Helper struct that represents a spatial region
     *
     * @ingroup Containers
     *
     */
    struct Area {
        TCoord position;
        TCoord size;

        /**
         * @brief Creates an empty area
         *
         */
        Area() = default;

        /**
         * @brief Creates an area from the given position and size
         *
         * @param position The top left corner of the area
         * @param size The size of the area
         */
        Area(const TCoord& position, const TCoord& size);

        /**
         * @brief Tests whether or not this area contains the given point
         *
         * @param point The point to check
         * @return True if contained, false if outside
         */
        bool contains(const TCoord& point) const;

        /**
         * @brief Tests whether or not this area intersects the other area
         *
         * @param area The area to test
         * @return True if the areas intersect, false if disjoint
         */
        bool intersects(const Area& area) const;

        /**
         * @brief Returns the top left quadrant of this area
         *
         */
        Area topLeft() const;

        /**
         * @brief Returns the top right quadrant of this area
         *
         */
        Area topRight() const;

        /**
         * @brief Returns the bottom left quadrant of this area
         *
         */
        Area bottomLeft() const;

        /**
         * @brief Returns the bottom right quadrant of this area
         *
         */
        Area bottomRight() const;
    };

private:
    struct Partition {
        Partition* parent;
        Partition* children[4];
        Area area;
        std::vector<std::pair<TCoord, TPayload>> entities;

        Partition(Partition* parent);
        Partition(Partition* parent, const Partition& copy);
        Partition(Partition* parent, const Area& area);
        ~Partition();

        void tryCollapseChildren(std::size_t maxLoad);
        void partitionChildren(std::size_t maxLoad);
        bool isLeaf() const;
        void addSelf(std::vector<Partition*>& results);
    };

    static std::size_t index(const Area& area, const TCoord& size);
    Partition* findLeaf(const TCoord& pos) const;
    void repartition();

    Partition* top;
    std::size_t maxLoad;

    static constexpr std::size_t TopLeft     = 0;
    static constexpr std::size_t BottomLeft  = 1;
    static constexpr std::size_t TopRight    = 2;
    static constexpr std::size_t BottomRight = 3;

    friend class Partition;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::QuadTree()
: maxLoad(10) {
    top = new Partition(nullptr);
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::QuadTree(const QuadTree& copy) {
    top     = new Partition(nullptr, *copy.top);
    maxLoad = copy.maxLoad;
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::QuadTree(QuadTree&& move) {
    top      = move.top;
    maxLoad  = move.maxLoad;
    move.top = new Partition(nullptr);
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::~QuadTree() {
    clear();
}

template<typename TCoord, typename TPayload>
void QuadTree<TCoord, TPayload>::setMaxLoad(std::size_t ml) {
    maxLoad = ml;
    repartition();
}

template<typename TCoord, typename TPayload>
void QuadTree<TCoord, TPayload>::setIndexedArea(const Area& area) {
    top->area = area;
    repartition();
}

template<typename TCoord, typename TPayload>
void QuadTree<TCoord, TPayload>::add(const TCoord& pos, const TPayload& val) {
    Partition* current = findLeaf(pos);
    current->entities.emplace_back(pos, val);
    if (current->entities.size() > maxLoad) current->partitionChildren(maxLoad);
}

template<typename TCoord, typename TPayload>
void QuadTree<TCoord, TPayload>::add(const TCoord& pos, TPayload&& val) {
    Partition* current = findLeaf(pos);
    current->entities.emplace_back(pos, std::forward<TPayload>(val));
    if (current->entities.size() > maxLoad) current->partitionChildren(maxLoad);
}

template<typename TCoord, typename TPayload>
bool QuadTree<TCoord, TPayload>::updatePosition(const TCoord& oldPos, const TCoord& newPos) {
    Partition* p = findLeaf(oldPos);
    for (unsigned int i = 0; i < p->entities.size(); ++i) {
        if (p->entities[i].first == oldPos) {
            TPayload temp(std::forward<TPayload>(p->entities[i].second));
            p->entities.erase(p->entities.begin() + i);
            add(newPos, std::forward<TPayload>(temp));
            return true;
        }
    }
    return false;
}

template<typename TCoord, typename TPayload>
bool QuadTree<TCoord, TPayload>::remove(const TCoord& pos) {
    Partition* p = findLeaf(pos);
    for (unsigned int i = 0; i < p->entities.size(); ++i) {
        if (p->entities[i].first == pos) {
            p->entities.erase(p->entities.begin() + i);
            if (p->parent) p->parent->tryCollapseChildren(maxLoad);
            return true;
        }
    }
    return false;
}

template<typename TCoord, typename TPayload>
typename QuadTree<TCoord, TPayload>::ResultSet QuadTree<TCoord, TPayload>::all() {
    ResultSet result;
    top->addSelf(result.leafs);
    return result;
}

template<typename TCoord, typename TPayload>
typename QuadTree<TCoord, TPayload>::ResultSet QuadTree<TCoord, TPayload>::getQuad(
    const TCoord& pos) {
    ResultSet result;
    Partition* p = findLeaf(pos);
    if (!p->entities.empty()) result.leafs.push_back(p);
    return result;
}

template<typename TCoord, typename TPayload>
typename QuadTree<TCoord, TPayload>::ResultSet QuadTree<TCoord, TPayload>::getInArea(
    const Area& area) {
    ResultSet result;
    result.leafs.reserve(10);

    std::vector<Partition*> toVisit;
    toVisit.reserve(10);
    toVisit.push_back(top);

    while (!toVisit.empty()) {
        Partition* current = toVisit.back();
        toVisit.pop_back();

        if (current->isLeaf()) {
            if (!current->entities.empty()) result.leafs.push_back(current);
        }
        else {
            toVisit.reserve(toVisit.size() + 4);
            for (unsigned int i = 0; i < 4; ++i) {
                if (current->children[i]->area.intersects(area)) {
                    toVisit.push_back(current->children[i]);
                }
            }
        }
    }

    return result;
}

template<typename TCoord, typename TPayload>
void QuadTree<TCoord, TPayload>::clear() {
    Partition* oldTop = top;
    top               = new Partition(nullptr, top->area);
    delete oldTop;
}

template<typename TCoord, typename TPayload>
std::size_t QuadTree<TCoord, TPayload>::index(const Area& area, const TCoord& pos) {
    if (area.position.x + area.size.x / 2 > pos.x) {
        if (area.position.y + area.size.y / 2 > pos.y)
            return TopLeft;
        else
            return BottomLeft;
    }
    else {
        if (area.position.y + area.size.y / 2 > pos.y)
            return TopRight;
        else
            return BottomRight;
    }
}

template<typename TCoord, typename TPayload>
typename QuadTree<TCoord, TPayload>::Partition* QuadTree<TCoord, TPayload>::findLeaf(
    const TCoord& pos) const {
    Partition* current = top;
    while (!current->isLeaf()) { current = current->children[index(current->area, pos)]; }
    return current;
}

template<typename TCoord, typename TPayload>
void QuadTree<TCoord, TPayload>::repartition() {
    ResultSet a       = all();
    Partition* oldTop = top;
    top               = new Partition(nullptr, top->area);
    for (Partition* p : a.leafs) {
        for (const auto& pair : p->entities) { add(pair.first, pair.second); }
    }
    delete oldTop;
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::Partition::Partition(Partition* parent)
: parent(parent) {
    children[0] = nullptr;
    children[1] = nullptr;
    children[2] = nullptr;
    children[3] = nullptr;
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::Partition::Partition(Partition* parent, const Area& area)
: parent(parent)
, area(area) {
    children[0] = nullptr;
    children[1] = nullptr;
    children[2] = nullptr;
    children[3] = nullptr;
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::Partition::Partition(Partition* parent, const Partition& copy)
: parent(parent)
, area(copy.area)
, entities(copy.entities) {
    if (!copy.isLeaf()) {
        children[0] = new Partition(this, *copy.children[0]);
        children[1] = new Partition(this, *copy.children[1]);
        children[2] = new Partition(this, *copy.children[2]);
        children[3] = new Partition(this, *copy.children[3]);
    }
    else {
        children[0] = nullptr;
        children[1] = nullptr;
        children[2] = nullptr;
        children[3] = nullptr;
    }
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::Partition::~Partition() {
    if (!isLeaf()) {
        delete children[0];
        delete children[1];
        delete children[2];
        delete children[3];
    }
}

template<typename TCoord, typename TPayload>
void QuadTree<TCoord, TPayload>::Partition::tryCollapseChildren(std::size_t maxLoad) {
    const std::size_t load = children[0]->entities.size() + children[1]->entities.size() +
                             children[2]->entities.size() + children[3]->entities.size();
    if (load <= maxLoad) {
        entities.reserve(load);
        entities.insert(entities.end(),
                        std::make_move_iterator(children[0]->entities.begin()),
                        std::make_move_iterator(children[0]->entities.end()));
        entities.insert(entities.end(),
                        std::make_move_iterator(children[1]->entities.begin()),
                        std::make_move_iterator(children[1]->entities.end()));
        entities.insert(entities.end(),
                        std::make_move_iterator(children[2]->entities.begin()),
                        std::make_move_iterator(children[2]->entities.end()));
        entities.insert(entities.end(),
                        std::make_move_iterator(children[3]->entities.begin()),
                        std::make_move_iterator(children[3]->entities.end()));
        delete children[0];
        delete children[1];
        delete children[2];
        delete children[3];
        children[0] = nullptr;
        children[1] = nullptr;
        children[2] = nullptr;
        children[3] = nullptr;
        if (parent) parent->tryCollapseChildren(maxLoad);
    }
}

template<typename TCoord, typename TPayload>
void QuadTree<TCoord, TPayload>::Partition::partitionChildren(std::size_t maxLoad) {
    children[TopLeft]     = new Partition(this, area.topLeft());
    children[BottomLeft]  = new Partition(this, area.bottomLeft());
    children[TopRight]    = new Partition(this, area.topRight());
    children[BottomRight] = new Partition(this, area.bottomRight());

    for (auto&& pair : entities) {
        children[QuadTree<TCoord, TPayload>::index(area, pair.first)]->entities.emplace_back(
            std::move(pair));
    }
    entities.clear();

    if (children[0]->entities.size() > maxLoad) children[0]->partitionChildren(maxLoad);
    if (children[1]->entities.size() > maxLoad) children[1]->partitionChildren(maxLoad);
    if (children[2]->entities.size() > maxLoad) children[2]->partitionChildren(maxLoad);
    if (children[3]->entities.size() > maxLoad) children[3]->partitionChildren(maxLoad);
}

template<typename TCoord, typename TPayload>
bool QuadTree<TCoord, TPayload>::Partition::isLeaf() const {
    return children[0] == nullptr;
}

template<typename TCoord, typename TPayload>
void QuadTree<TCoord, TPayload>::Partition::addSelf(std::vector<Partition*>& v) {
    if (isLeaf()) {
        if (!entities.empty()) v.push_back(this);
    }
    else {
        children[0]->addSelf(v);
        children[1]->addSelf(v);
        children[2]->addSelf(v);
        children[3]->addSelf(v);
    }
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::Area::Area(const TCoord& pos, const TCoord& s)
: position(pos)
, size(s) {}

template<typename TCoord, typename TPayload>
bool QuadTree<TCoord, TPayload>::Area::contains(const TCoord& pos) const {
    const auto dx = pos.x - position.x;
    const auto dy = pos.y - position.y;
    return dx <= size.x && dy <= size.y;
}

template<typename TCoord, typename TPayload>
bool QuadTree<TCoord, TPayload>::Area::intersects(const Area& area) const {
    const auto interLeft   = std::max(position.x, area.position.x);
    const auto interTop    = std::max(position.y, area.position.y);
    const auto interRight  = std::min(position.x + size.x, area.position.x + area.size.x);
    const auto interBottom = std::min(position.y + size.y, area.position.y + area.size.y);
    return interLeft < interRight && interTop < interBottom;
}

template<typename TCoord, typename TPayload>
typename QuadTree<TCoord, TPayload>::Area QuadTree<TCoord, TPayload>::Area::topLeft() const {
    return Area(position, {size.x / 2, size.y / 2});
}

template<typename TCoord, typename TPayload>
typename QuadTree<TCoord, TPayload>::Area QuadTree<TCoord, TPayload>::Area::topRight() const {
    return Area({position.x + size.x / 2, position.y}, {size.x / 2, size.y / 2});
}

template<typename TCoord, typename TPayload>
typename QuadTree<TCoord, TPayload>::Area QuadTree<TCoord, TPayload>::Area::bottomRight() const {
    return Area({position.x + size.x / 2, position.y + size.y / 2}, {size.x / 2, size.y / 2});
}

template<typename TCoord, typename TPayload>
typename QuadTree<TCoord, TPayload>::Area QuadTree<TCoord, TPayload>::Area::bottomLeft() const {
    return Area({position.x, position.y + size.y / 2}, {size.x / 2, size.y / 2});
}

template<typename TCoord, typename TPayload>
typename QuadTree<TCoord, TPayload>::ResultSet::Iterator
QuadTree<TCoord, TPayload>::ResultSet::begin() {
    return {leafs, 0, 0};
}

template<typename TCoord, typename TPayload>
typename QuadTree<TCoord, TPayload>::ResultSet::Iterator
QuadTree<TCoord, TPayload>::ResultSet::end() {
    return {leafs, leafs.size(), 0};
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::ResultSet::Iterator::Iterator(std::vector<Partition*>& leafs,
                                                          std::size_t li, std::size_t vi)
: leafs(leafs)
, leafIndex(li)
, valueIndex(vi) {}

template<typename TCoord, typename TPayload>
TPayload& QuadTree<TCoord, TPayload>::ResultSet::Iterator::operator*() {
    return leafs[leafIndex]->entities[valueIndex].second;
}

template<typename TCoord, typename TPayload>
const TCoord& QuadTree<TCoord, TPayload>::ResultSet::Iterator::position() {
    return leafs[leafIndex]->entities[valueIndex].first;
}

template<typename TCoord, typename TPayload>
TPayload* QuadTree<TCoord, TPayload>::ResultSet::Iterator::operator->() {
    return &leafs[leafIndex]->entities[valueIndex].second;
}

template<typename TCoord, typename TPayload>
void QuadTree<TCoord, TPayload>::ResultSet::Iterator::operator++() {
    valueIndex += 1;
    if (valueIndex >= leafs[leafIndex]->entities.size()) {
        valueIndex = 0;
        leafIndex += 1;
    }
}

template<typename TCoord, typename TPayload>
bool QuadTree<TCoord, TPayload>::ResultSet::Iterator::operator==(const Iterator& it) const {
    return &leafs == &it.leafs && leafIndex == it.leafIndex && valueIndex == it.valueIndex;
}

template<typename TCoord, typename TPayload>
bool QuadTree<TCoord, TPayload>::ResultSet::Iterator::operator!=(const Iterator& it) const {
    return !(*this == it);
}

} // namespace container
} // namespace bl

#endif
