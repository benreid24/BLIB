#ifndef BLIB_CONTAINERS_QUADTREE_HPP
#define BLIB_CONTAINERS_QUADTREE_HPP

#include <cstdint>
#include <vector>

namespace bl
{
namespace container
{
template<typename TCoord, typename TPayload>
class QuadTree {
    struct Partition;

public:
    struct Area;

    class ResultSet {
    public:
        class Iterator {
        public:
            TPayload& operator*();

            TPayload* operator->();

            void operator++();

            bool operator==(const Iterator& it) const;

            bool operator!=(const Iterator& it) const;

        private:
            std::vector<Partition*>& leafs;
            std::size_t leafIndex;
            std::size_t valueIndex;

            Iterator(std::vector<Partition*>& leafs, std::size_t leafIndex, std::size_t valueIndex);

            friend class ResultSet;
        };

        Iterator begin();

        Iterator end();

    private:
        std::vector<Partition*> leafs;

        ResultSet() = default;

        friend class QuadTree;
    };

    QuadTree();

    QuadTree(const QuadTree& copy);

    QuadTree(QuadTree&& move);

    ~QuadTree();

    void setMaxLoad(std::size_t maxLoad);

    void setIndexedArea(const Area& area);

    void add(const TCoord& position, const TPayload& value);

    void add(const TCoord& position, TPayload&& value);

    bool updatePosition(const TCoord& oldPosition, const TCoord& newPosition);

    bool remove(const TCoord& position);

    ResultSet all();

    ResultSet getInArea(const Area& area);

    ResultSet getQuad(const TCoord& position);

    void clear();

    struct Area {
        TCoord position;
        TCoord size;

        Area() = default;

        Area(const TCoord& position, const TCoord& size);

        bool contains(const TCoord& point) const;

        bool intersects(const Area& area) const;

        Area topLeft() const;

        Area topRight() const;

        Area bottomLeft() const;

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
        ~Partition();

        void partitionChildren();
        bool isLeaf() const;
        void addSelf(std::vector<Partition*>& results) const;
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
    if (current->entities.size() > maxLoad) current->partitionChildren();
}

template<typename TCoord, typename TPayload>
void QuadTree<TCoord, TPayload>::add(const TCoord& pos, TPayload&& val) {
    Partition* current = findLeaf(pos);
    current->entities.emplace_back(pos, std::forward<TPayload>(val));
    if (current->entities.size() > maxLoad) current->partitionChildren();
}

template<typename TCoord, typename TPayload>
bool QuadTree<TCoord, TPayload>::updatePosition(const TCoord& oldPos, const TCoord& newPos) {
    Partition* p = findLeaf(oldPos);
    for (unsigned int i = 0; i < p->entities.size(); ++i) {
        if (p->entities[i].first == oldPos) {
            const TPayload temp(std::move<TPayload>(p->entities[i].second));
            p->entities.erase(p->entities.begin() + i);
            add(newPos, std::move<TPayload>(temp));
            return true;
        }
    }
    return false;
}

template<typename TCoord, typename TPayload>
bool QuadTree<TCoord, TPayload>::remove(const TCoord& pos) {
    Partition* p = findLeaf(pos);
    for (unsigned int i = 0; i < p->entities.size(); ++i) {
        if (p->entities[i].first == oldPos) {
            p->entities.erase(p->entities.begin() + i);
            return true;
        }
    }
    return false;
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::ResultSet QuadTree<TCoord, TPayload>::all() {
    ResultSet result;
    top->addSelf(result.leafs);
    return result;
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::ResultSet QuadTree<TCoord, TPayload>::getQuad(const TCoord& pos) {
    ResultSet result;
    result.leafs.push_back(findLeaf(pos));
    return result;
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::ResultSet QuadTree<TCoord, TPayload>::getInArea(const Area& area) {
    ResultSet result;
    result.leafs.reserve(10);

    std::vector<Partition*> toVisit;
    toVisit.reserve(10);
    toVisit.push_back(top);

    while (!toVisit.empty()) {
        Partition* current = toVisit.back();
        toVisit.pop_back();

        if (current->isLeaf())
            result.leafs.push_back(current);
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
    top               = new Partition(nullptr);
    top->area         = oldTop->area;
    delete oldTop;
}

template<typename TCoord, typename TPayload>
std::size_t QuadTree<TCoord, TPayload>::index(const Area& area, const TCoord& pos) {
    if (area.position.x + area.size.x / 2.f > pos.x) {
        if (area.position.y + area.size.y / 2.f > pos.y)
            return TopLeft;
        else
            return BottomLeft;
    }
    else {
        if (area.position.y + area.size.y / 2.f > pos.y)
            return TopRight;
        else
            return BottomRight;
    }
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::Partition* QuadTree<TCoord, TPayload>::findLeaf(
    const TCoord& pos) const {
    Partition* current = top;
    while (!current->isLeaf()) { current = current->children(index(current->area, pos)); }
    return current;
}

template<typename TCoord, typename TPayload>
void QuadTree<TCoord, TPayload>::repartition() {
    ResultSet a       = all();
    Partition* oldTop = top;
    top               = new Partition(nullptr);
    top->area         = oldTop->area;
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
QuadTree<TCoord, TPayload>::Partition::Partition(Partition* parent, const Partition& copy)
: Partition(parent)
, area(copy.area)
, entities(copy.entities) {
    if (!copy.isLeaf()) {
        children[0] = new Partition(this, *copy.children[0]);
        children[1] = new Partition(this, *copy.children[1]);
        children[2] = new Partition(this, *copy.children[2]);
        children[3] = new Partition(this, *copy.children[3]);
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
void QuadTree<TCoord, TPayload>::Partition::partitionChildren() {
    children[TopLeft]     = new Partition(area.topLeft());
    children[BottomLeft]  = new Partition(area.bottomLeft());
    children[TopRight]    = new Partition(area.topRight());
    children[BottomRight] = new Partition(area.bottomRight());

    for (auto& pair : entities) {
        children[QuadTree<TCoord, TPayload>::index(area, pair.first)]->entities.emplace_back(
            std::move<std::pair<TCoord, TPayload>>(pair));
    }
    entities.clear();
}

template<typename TCoord, typename TPayload>
bool QuadTree<TCoord, TPayload>::Partition::isLeaf() const {
    return children[0] == nullptr;
}

template<typename TCoord, typename TPayload>
void QuadTree<TCoord, TPayload>::Partition::addSelf(std::vector<Partition*>& v) const {
    if (isLeaf()) { v.push_back(this); }
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
QuadTree<TCoord, TPayload>::Area QuadTree<TCoord, TPayload>::Area::topLeft() const {
    return Area(position, {size.x / 2.f, size.y / 2.f});
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::Area QuadTree<TCoord, TPayload>::Area::topRight() const {
    return Area({position.x + size.x / 2.f, position.y}, {size.x / 2.f, size.y / 2.f});
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::Area QuadTree<TCoord, TPayload>::Area::bottomRight() const {
    return Area({position.x + size.x / 2.f, position.y + size.y / 2.f},
                {size.x / 2.f, size.y / 2.f});
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::Area QuadTree<TCoord, TPayload>::Area::bottomLeft() const {
    return Area({position.x, position.y + size.y / 2.f}, {size.x / 2.f, size.y / 2.f});
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::ResultSet::Iterator QuadTree<TCoord, TPayload>::ResultSet::begin() {
    return {leafs, 0, 0};
}

template<typename TCoord, typename TPayload>
QuadTree<TCoord, TPayload>::ResultSet::Iterator QuadTree<TCoord, TPayload>::ResultSet::end() {
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
    return leafs[leafIndex]->entities[valueIndex];
}

template<typename TCoord, typename TPayload>
TPayload* QuadTree<TCoord, TPayload>::ResultSet::Iterator::operator->() {
    return &leafs[leafIndex]->entities[valueIndex];
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
