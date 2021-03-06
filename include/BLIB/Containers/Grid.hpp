#ifndef BLIB_CONTAINERS_GRID_HPP
#define BLIB_CONTAINERS_GRID_HPP

#include <BLIB/Containers/Vector2d.hpp>
#include <BLIB/Util/NonCopyable.hpp>

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>
#include <vector>

namespace bl
{
namespace container
{
/**
 * @brief Basic spatial partioning grid class for breaking down areas into equal sized boxes
 *
 * @tparam T The type of payload to store
 * @ingroup Containers
 */
template<typename T>
class Grid {
    struct Cell;

public:
    /**
     * @brief Class containing all elements in the grid as a payload
     *
     */
    class Payload
    : private util::NonCopyable
    , public std::enable_shared_from_this<Payload> {
    public:
        using Ptr = std::shared_ptr<Payload>;

        /**
         * @brief Cleans up
         *
         */
        ~Payload();

        /**
         * @brief Returns a reference to the stored value
         *
         */
        T& get();

        /**
         * @brief Returns a reference to the stored value
         *
         */
        const T& get() const;

        /**
         * @brief Moves this item to a new location in its Grid
         *
         * @param newX The new x coordinate in spatial coordinates
         * @param newY The new y coordinate in spatial coordinates
         */
        void move(float newX, float newY);

        /**
         * @brief Removes this item from its Grid
         *
         */
        void remove();

        /**
         * @brief Returns whether or not this payload is contained in a grid
         *
         */
        bool inGrid() const;

    private:
        Grid& owner;
        T value;
        unsigned int x, y;
        Ptr prev, next;

        template<typename... TArgs>
        Payload(Grid& owner, TArgs... args);
        Payload(Grid& owner, const T& val);
        Payload(Grid& owner, T&& val);

        friend class Grid;
    };

    /**
     * @brief Iterator class for iterating over ranges
     *
     */
    class Iterator {
    public:
        /**
         * @brief Creates an invalid iterator
         *
         */
        Iterator();

        /**
         * @brief Copies from the given iterator
         *
         * @param copy The iterator to copy from
         */
        Iterator(const Iterator& copy);

        /**
         * @brief Copies from the given iterator
         *
         * @param copy The iterator to copy from
         * @return Iterator& A reference to this iterator
         */
        Iterator& operator=(const Iterator& copy);

        /**
         * @brief Access the value pointed to by this iterator
         *
         */
        Payload& operator*();

        /**
         * @brief Access the value pointed to by this iterator
         *
         */
        Payload* operator->();

        /**
         * @brief Access the value pointed to by this iterator
         *
         */
        const Payload& operator*() const;

        /**
         * @brief Access the value pointed to by this iterator
         *
         */
        const Payload* operator->() const;

        /**
         * @brief Increments the iterator
         *
         */
        Iterator operator++();

        /**
         * @brief Increments the iterator
         *
         */
        Iterator operator++(int);

        /**
         * @brief Tests whether this iterator is the same as another
         *
         */
        bool operator==(const Iterator& right) const;

        /**
         * @brief Tests whether this iterator is the same as another
         *
         */
        bool operator!=(const Iterator& right) const;

    private:
        Grid* const owner;
        unsigned int sx;
        const unsigned int ex, ey;
        unsigned int cx, cy;
        typename Payload::Ptr current;

        Iterator(Grid* owner, unsigned int x, unsigned int y, unsigned int ex, unsigned int ey,
                 const typename Payload::Ptr& current, bool s = true);
        void skipEmpty();
        bool atEnd() const;

        friend class Grid;
    };

    /**
     * @brief Basic pair class representing an iterable range of elements
     *
     */
    class Range {
    public:
        /**
         * @brief Construct an empty Range
         *
         */
        Range() = default;

        /**
         * @brief The first element in the range
         *
         */
        const Iterator& begin() const;

        /**
         * @brief The last element in the range
         *
         */
        const Iterator& end() const;

        /**
         * @brief Returns whether or not the range is empty
         *
         */
        bool empty() const;

    private:
        Iterator b, e;
        friend class Grid;

        Range(const Iterator& b, const Iterator& e);
    };

    /**
     * @brief Creates the grid
     *
     * @param width The width of the grid in spatial units
     * @param height The height of the grid in spatial units
     * @param cellWidth The width of each cell in spatial units
     * @param cellHeight The height of each cell in spatial units
     */
    Grid(float width, float height, float cellWidth, float cellHeight);

    /**
     * @brief Resizes the grid. Stored elements are cleared
     *
     * @param width The width of the grid in spatial units
     * @param height The height of the grid in spatial units
     * @param cellWidth The width of each cell in spatial units
     * @param cellHeight The height of each cell in spatial units
     */
    void setSize(float width, float height, float cellWidth, float cellHeight);

    /**
     * @brief Add a payload to the grid at the given position
     *
     * @param x The x coordinate in spaital units
     * @param y The y coordinate in spatial units
     * @param value The payload to add
     * @return A Ptr to the newly created payload
     */
    typename Payload::Ptr add(float x, float y, const T& value);

    /**
     * @brief Add a payload to the grid at the given position
     *
     * @param x The x coordinate in spaital units
     * @param y The y coordinate in spatial units
     * @param value The payload to add
     * @return A Ptr to the newly created payload
     */
    typename Payload::Ptr add(float x, float y, T&& value);

    /**
     * @brief Construct an element in place
     *
     * @tparam TArgs Constructor parameter types
     * @param x The x coordinate in spatial units
     * @param y The y coordinate in spatial units
     * @param args The arguments to construct the value with
     * @return A Ptr to the newly created payload
     */
    template<typename... TArgs>
    typename Payload::Ptr emplace(float x, float y, TArgs... args);

    /**
     * @brief Removes all elements. All iterators invalidated
     *
     */
    void clear();

    /**
     * @brief Returns a range pointing to all elements in the cell containing the given position
     *
     * @param x The x coordinate in spaital units
     * @param y The y coordinate in spaital units
     * @return Range A range containing elements in the given cell
     */
    Range getCell(float x, float y);

    /**
     * @brief Returns a range to elements in neighboring cells. Includes the containing cell
     *
     * @param x The x coordinate in spaital units
     * @param y The y coordinate in spaital units
     * @return Range A range containing neighbors
     */
    Range getCellAndNeighbors(float x, float y);

    /**
     * @brief Returns a range containing at least all of the elements within the given spatial area
     *
     * @param x The x coordinate in spaital units
     * @param y The y coordinate in spaital units
     * @param w The width in spaital units
     * @param h The height in spaital units
     * @return Range A range with all contained elements
     */
    Range getArea(float x, float y, float w, float h);

    /**
     * @brief Returns a range containing all of the stored elements
     *
     * @return Range Iterable Range of all contained elements
     */
    Range getAll();

private:
    unsigned int width, height;
    float cellWidth, cellHeight;
    Vector2D<typename Payload::Ptr> cells;

    void doAdd(float x, float y, typename Payload::Ptr& payload);

    friend class Iterator;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
Grid<T>::Grid(float w, float h, float cw, float ch)
: width(std::max(std::floor(w / cw), 1.f))
, height(std::max(std::floor(h / ch), 1.f))
, cellWidth(cw)
, cellHeight(ch)
, cells(width, height) {}

template<typename T>
void Grid<T>::setSize(float w, float h, float cw, float ch) {
    clear();
    width      = std::max(std::floor(w / cw), 1.f);
    height     = std::max(std::floor(h / ch), 1.f);
    cellWidth  = cw;
    cellHeight = ch;
    cells.setSize(width, height);
}

template<typename T>
typename Grid<T>::Payload::Ptr Grid<T>::add(float x, float y, const T& v) {
    typename Payload::Ptr payload(new Payload(*this, v));
    doAdd(x, y, payload);
    return payload;
}

template<typename T>
typename Grid<T>::Payload::Ptr Grid<T>::add(float x, float y, T&& v) {
    typename Payload::Ptr payload(new Payload(*this, std::forward<T>(v)));
    doAdd(x, y, payload);
    return payload;
}

template<typename T>
template<typename... TArgs>
typename Grid<T>::Payload::Ptr Grid<T>::emplace(float x, float y, TArgs... args) {
    typename Payload::Ptr payload(new Payload(*this, args...));
    doAdd(x, y, payload);
    return payload;
}

template<typename T>
void Grid<T>::doAdd(float x, float y, typename Payload::Ptr& payload) {
    payload->x =
        std::max(0u, std::min(static_cast<unsigned int>(std::floor(x / cellWidth)), width - 1u));
    payload->y =
        std::max(0u, std::min(static_cast<unsigned int>(std::floor(y / cellHeight)), height - 1u));
    typename Payload::Ptr& cell = cells(payload->x, payload->y);
    if (!cell) { cell = payload; }
    else {
        payload->next = cell;
        cell->prev    = payload;
        cell          = payload;
    }
}

template<typename T>
void Grid<T>::clear() {
    for (unsigned int x = 0; x < width; ++x) {
        for (unsigned int y = 0; y < height; ++y) {
            while (cells(x, y)) {
                auto keepAlive = cells(x, y); // to prevent delete in middle of remove()
                cells(x, y)->remove();
            }
        }
    }
}

template<typename T>
typename Grid<T>::Range Grid<T>::getCell(float x, float y) {
    const unsigned int xi = std::floor(x / cellWidth);
    const unsigned int yi = std::floor(y / cellHeight);
    return {Iterator(this, xi, yi, xi, yi, cells(xi, yi)),
            Iterator(this, xi, yi, xi, yi, nullptr, false)};
}

template<typename T>
typename Grid<T>::Range Grid<T>::getCellAndNeighbors(float x, float y) {
    const unsigned int xi = std::floor(x / cellWidth);
    const unsigned int yi = std::floor(y / cellHeight);
    const unsigned int sx = xi > 0 ? xi - 1 : 0;
    const unsigned int sy = yi > 0 ? yi - 1 : 0;
    const unsigned int ex = std::min(sx + (xi > 0 ? 3 : 2) - 1, width - 1);
    const unsigned int ey = std::min(sy + (yi > 0 ? 3 : 2) - 1, height - 1);
    return {Iterator(this, sx, sy, ex, ey, cells(sx, sy)),
            Iterator(this, ex, ey, ex, ey, nullptr, false)};
}

template<typename T>
typename Grid<T>::Range Grid<T>::getArea(float x, float y, float w, float h) {
    const unsigned int sx = std::floor(std::max(x, 0.f) / cellWidth);
    const unsigned int sy = std::floor(std::max(y, 0.f) / cellHeight);
    const unsigned int ex = std::min(
        static_cast<unsigned int>(std::floor(std::max(x + w, 0.f) / cellWidth)), width - 1);
    const unsigned int ey = std::min(
        static_cast<unsigned int>(std::floor(std::max(y + h, 0.f) / cellHeight)), height - 1);
    return {Iterator(this, sx, sy, ex, ey, cells(sx, sy)),
            Iterator(this, ex, ey, ex, ey, nullptr, false)};
}

template<typename T>
typename Grid<T>::Range Grid<T>::getAll() {
    return {Iterator(this, 0, 0, width - 1, height - 1, cells(0, 0)),
            Iterator(this, width - 1, height - 1, width - 1, height - 1, nullptr, false)};
}

template<typename T>
Grid<T>::Payload::Payload(Grid& owner, const T& v)
: owner(owner)
, value(v) {}

template<typename T>
Grid<T>::Payload::Payload(Grid& owner, T&& v)
: owner(owner)
, value(std::forward<T>(v)) {}

template<typename T>
template<typename... TArgs>
Grid<T>::Payload::Payload(Grid& owner, TArgs... args)
: owner(owner)
, value(args...) {}

template<typename T>
Grid<T>::Payload::~Payload() {
    remove();
}

template<typename T>
T& Grid<T>::Payload::get() {
    return value;
}

template<typename T>
const T& Grid<T>::Payload::get() const {
    return value;
}

template<typename T>
void Grid<T>::Payload::move(float nx, float ny) {
    const unsigned int xi = std::floor(nx / owner.cellWidth);
    const unsigned int yi = std::floor(ny / owner.cellHeight);
    if (xi != x || yi != y) {
        remove();
        auto ptr = this->shared_from_this();
        owner.doAdd(nx, ny, ptr);
    }
}

template<typename T>
void Grid<T>::Payload::remove() {
    if (prev) { prev->next = next; }
    else if (owner.cells(x, y).get() == this) {
        owner.cells(x, y) = next;
    }
    if (next) { next->prev = prev; }

    next.reset();
    prev.reset();
}

template<typename T>
bool Grid<T>::Payload::inGrid() const {
    return prev || next;
}

template<typename T>
Grid<T>::Iterator::Iterator()
: Iterator(nullptr, 0, 0, 0, 0, nullptr, false) {}

template<typename T>
Grid<T>::Iterator::Iterator(Grid* o, unsigned int x, unsigned int y, unsigned int ex,
                            unsigned int ey, const typename Payload::Ptr& current, bool s)
: owner(o)
, sx(x)
, ex(ex)
, ey(ey)
, cx(x)
, cy(y)
, current(current) {
    if (s) skipEmpty();
}

template<typename T>
Grid<T>::Iterator::Iterator(const Iterator& copy)
: owner(copy.owner)
, sx(copy.sx)
, ex(copy.ex)
, ey(copy.ey)
, cx(copy.cx)
, cy(copy.cy)
, current(copy.current) {}

template<typename T>
typename Grid<T>::Iterator& Grid<T>::Iterator::operator=(const Iterator& copy) {
    const_cast<Grid<T>*&>(owner)  = copy.owner;
    sx                            = copy.sx;
    cx                            = copy.cx;
    cy                            = copy.cy;
    const_cast<unsigned int&>(ex) = copy.ex;
    const_cast<unsigned int&>(ey) = copy.ey;
    current                       = copy.current;
    return *this;
}

template<typename T>
typename Grid<T>::Payload& Grid<T>::Iterator::operator*() {
    return *current;
}

template<typename T>
typename Grid<T>::Payload* Grid<T>::Iterator::operator->() {
    return current.get();
}

template<typename T>
const typename Grid<T>::Payload& Grid<T>::Iterator::operator*() const {
    return *current;
}

template<typename T>
const typename Grid<T>::Payload* Grid<T>::Iterator::operator->() const {
    return current.get();
}

template<typename T>
bool Grid<T>::Iterator::atEnd() const {
    return cy == ey && cx == ex;
}

template<typename T>
typename Grid<T>::Iterator Grid<T>::Iterator::operator++() {
    if (!current) return *this;

    if (current->next || atEnd()) { current = current->next; }
    else {
        ++cx;
        if (cx > ex) {
            cx = sx;
            ++cy;
        }
        current = owner->cells(cx, cy);
        skipEmpty();
    }

    return *this;
}

template<typename T>
typename Grid<T>::Iterator Grid<T>::Iterator::operator++(int) {
    const auto it = *this;
    this->operator++();
    return it;
}

template<typename T>
void Grid<T>::Iterator::skipEmpty() {
    while (!current && !atEnd()) {
        ++cx;
        if (cx > ex) {
            cx = sx;
            ++cy;
        }
        current = owner->cells(cx, cy);
    }
    current = owner->cells(cx, cy); // if at end need to update to end pointer
}

template<typename T>
bool Grid<T>::Iterator::operator==(const Iterator& right) const {
    return owner == right.owner && cx == right.cx && cy == right.cy &&
           current.get() == right.current.get();
}

template<typename T>
bool Grid<T>::Iterator::operator!=(const Iterator& right) const {
    return owner != right.owner || cx != right.cx || cy != right.cy ||
           current.get() != right.current.get();
    ;
}

template<typename T>
Grid<T>::Range::Range(const Iterator& b, const Iterator& e)
: b(b)
, e(e) {}

template<typename T>
const typename Grid<T>::Iterator& Grid<T>::Range::begin() const {
    return b;
}

template<typename T>
const typename Grid<T>::Iterator& Grid<T>::Range::end() const {
    return e;
}

template<typename T>
bool Grid<T>::Range::empty() const {
    return b == e;
}

} // namespace container
} // namespace bl

#endif
