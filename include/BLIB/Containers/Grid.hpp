#ifndef BLIB_CONTAINERS_GRID_HPP
#define BLIB_CONTAINERS_GRID_HPP

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace bl
{
namespace container
{
/**
 * @brief Basic spatial partioning grid class for breaking down areas into equal sized boxes
 *
 * @tparam T The type of payload to store. Should be trivially copyable
 * @ingroup Containers
 */
template<typename T>
class Grid {
    struct Cell;

public:
    /**
     * @brief Iterator class for iterating over ranges
     *
     */
    class Iterator {
    public:
        /**
         * @brief Access the value pointed to by this iterator
         *
         */
        T& operator*();

        /**
         * @brief Access the value pointed to by this iterator
         *
         */
        T* operator->();

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
        const unsigned int w;
        unsigned int cx, cy, i;

        Iterator(Grid* owner, unsigned int x, unsigned int y, unsigned int w, unsigned int i);

        friend class Grid;
    };

    /**
     * @brief Basic pair class representing an iterable range of elements
     *
     */
    class Range {
    public:
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
        const Iterator b, e;
        friend class Grid;
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
     */
    void add(float x, float y, const T& value);

    /**
     * @brief Move a payload from one position to another
     *
     * @param oldX The old x coordinate in spaital units
     * @param oldY The old y coordinate in spaital units
     * @param newX The new x coordinate in spaital units
     * @param newY The new y coordinate in spaital units
     * @param value The payload to move
     */
    void move(float oldX, float oldY, float newX, float newY, const T& value);

    /**
     * @brief Removes an element from the grid. Iterators containing the affected cell invalidated
     *
     * @param x The x coordinate in spaital units
     * @param y The y coordinate in spaital units
     * @param value The element to remove
     */
    void remove(float x, float y, const T& value);

    /**
     * @brief Removes an element using an iterator pointing to it. Iterators containing the affected
     *        cell invalidated
     *
     * @param it The element to remove
     */
    void remove(const Iterator& it);

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

private:
    struct Cell {
        std::vector<T> content;

        void remove(const T& value);
        void add(const T& value);
    };

    std::vector<Cell> cells;
    unsigned int width, height;
    float cellWidth, cellHeight;

    Cell& get(unsigned int x, unsigned int y);

    friend class Iterator;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
Grid<T>::Grid(float w, float h, float cw, float ch)
: width(std::floor(w / cw))
, height(std::floor(h / ch))
, cellWidth(cw)
, cellHeight(ch)
, cells(width * height) {}

template<typename T>
void Grid<T>::setSize(float w, float h, float cw, float ch) {
    clear();
    width      = std::floor(w / cw);
    height     = std::floor(h / ch);
    cellWidth  = cw;
    cellHeight = ch;
}

template<typename T>
void Grid<T>::add(float x, float y, const T& v) {
    const unsigned int xi = std::floor(x / cellWidth);
    const unsigned int yi = std::floor(y / cellHeight);
    get(xi, yi).add(v);
}

template<typename T>
void Grid<T>::move(float ox, float oy, float nx, float ny, const T& v) {
    const unsigned int oxi = std::floor(ox / cellWidth);
    const unsigned int oyi = std::floor(oy / cellHeight);
    const unsigned int nxi = std::floor(nx / cellWidth);
    const unsigned int nyi = std::floor(ny / cellHeight);
    get(oxi, oyi).remove(v);
    get(nxi, nyi).add(v);
}

template<typename T>
void Grid<T>::remove(float x, float y, const T& v) {
    const unsigned int xi = std::floor(x / cellWidth);
    const unsigned int yi = std::floor(y / cellHeight);
    get(xi, yi).remove(v);
}

template<typename T>
void Grid<T>::clear() {
    for (Cell& c : cells) { c.content.clear(); }
}

template<typename T>
Grid<T>::Range Grid<T>::getCell(float x, float y) {
    const unsigned int xi = std::floor(x / cellWidth);
    const unsigned int yi = std::floor(y / cellHeight);
    return {Iterator(this, xi, yi, 1, 0), Iterator(this, xi, yi, 1, get(xi, yi).content.size())};
}

template<typename T>
Grid<T>::Range Grid<T>::getCellAndNeighbors(float x, float y) {
    const unsigned int xi = std::floor(x / cellWidth);
    const unsigned int yi = std::floor(y / cellHeight);
    const unsigned int sx = xi > 0 ? xi - 1 : 0;
    const unsigned int sy = yi > 0 ? yi - 1 : 0;
    const unsigned int ex = std::min(sx + (xi > 0 ? 3 : 2) - 1, width - 1);
    const unsigned int ey = std::min(sx + (yi > 0 ? 3 : 2) - 1, height - 1);
    const unsigned int w  = ex - sx + 1;
    return {Iterator(this, sx, sy, w, 0), Iterator(this, ex, ey, w, get(ex, ey).content.size())};
}

template<typename T>
Grid<T>::Range Grid<T>::getArea(float x, float y, float w, float h) {
    const unsigned int sx = std::floor(std::max(x, 0) / cellWidth);
    const unsigned int sy = std::floor(std::max(y, 0) / cellHeight);
    const unsigned int ex = std::min(std::floor(std::max(x + w, 0) / cellWidth), width - 1);
    const unsigned int ey = std::min(std::floor(std::max(y + h, 0) / cellHeight), height - 1);
    const unsigned int w  = ex - sx + 1;
    return {Iterator(this, sx, sy, w, 0), Iterator(this, ex, ey, w, get(ex, ey).content.size())};
}

template<typename T>
Grid<T>::Cell& Grid<T>::get(unsigned int x, unsigned int y) {
    return cells[y * width + x];
}

template<typename T>
void Grid<T>::Cell::add(const T& v) {
    content.push_back(v);
}

template<typename T>
void Grid<T>::Cell::remove(const T& v) {
    auto it = std::find(content.begin(), content.end(), v);
    if (it != content.end()) { content.erase(it); }
}

template<typename T>
Grid<T>::Iterator::Iterator(Grid* o, unsigned int x, unsigned int y, unsigned int w, unsigned int i)
: owner(o)
, sx(x)
, cx(x)
, cy(y)
, w(w)
, i(i) {}

template<typename T>
T& Grid<T>::Iterator::operator*() {
    return owner->get(cx, cy).content[i];
}

template<typename T>
T* Grid<T>::Iterator::operator->() {
    return &owner->get(cx, cy).content[i];
}

template<typename T>
Grid<T>::Iterator Grid<T>::Iterator::operator++() {
    ++i;
    if (i >= owner->get(cx, cy).content.size()) {
        i = 0;
        ++cx;
        if (cx >= w) {
            cx = sx;
            ++cy;
        }
    }

    return *this;
}

template<typename T>
Grid<T>::Iterator Grid<T>::Iterator::operator++(int) {
    const auto it = *this;
    this->operator++();
    return it;
}

template<typename T>
bool Grid<T>::Iterator::operator==(const Iterator& right) const {
    return owner == right.owner && cx == right.cx && cy == right.ci && i == right.i;
}

template<typename T>
bool Grid<T>::Iterator::operator!=(const Iterator& right) const {
    return owner != right.owner || cx != right.cx || cy != right.ci || i != right.i;
}

template<typename T>
const Grid<T>::Iterator& Grid<T>::Range::begin() const {
    return b;
}

template<typename T>
const Grid<T>::Iterator& Grid<T>::Range::end() const {
    return e;
}

template<typename T>
bool Grid<T>::Range::empty() const {
    return b == e;
}

} // namespace container
} // namespace bl

#endif
