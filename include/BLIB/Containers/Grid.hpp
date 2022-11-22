#ifndef BLIB_CONTAINERS_GRID_HPP
#define BLIB_CONTAINERS_GRID_HPP

#include <BLIB/Containers/FastEraseVector.hpp>
#include <BLIB/Containers/Vector2d.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <algorithm>
#include <cmath>
#include <utility>

namespace bl
{
namespace container
{
/**
 * @brief Basic spatial partioning grid class for breaking down areas into equal sized boxes. Grid
 *        is meant only to point to objects stored elsewhere, however all trivial types are allowed.
 *        Contained values should be unique, otherwise move() and remove() may behave unexpectedly
 *
 * @tparam T The type of payload to point to
 * @ingroup Containers
 */
template<typename T>
class Grid {
public:
    static_assert(std::is_trivial_v<T>, "Grid should only be used with trivial types");

    /// @brief Single cell in the grid pointing to objects within that region
    using Cell = FastEraseVector<T>;

    /**
     * @brief Creates the grid
     *
     * @param space The spatial region to cover
     * @param cellWidth The width of each cell in spatial units
     * @param cellHeight The height of each cell in spatial units
     */
    Grid(const sf::FloatRect& space, float cellWidth, float cellHeight);

    /**
     * @brief Resizes the grid. Stored elements are cleared
     *
     * @param space The spatial region to cover
     * @param cellWidth The width of each cell in spatial units
     * @param cellHeight The height of each cell in spatial units
     */
    void setSize(const sf::FloatRect& space, float cellWidth, float cellHeight);

    /**
     * @brief Adds an entity to the grid at the given position
     *
     * @param pos The position of the entity to add, in spatial units
     * @param value The payload to add
     */
    void add(const sf::Vector2f& pos, T value);

    /**
     * @brief Removes an entity to the grid at the given position
     *
     * @param pos The position of the entity to remove, in spatial units
     * @param value The payload to remove
     */
    void remove(const sf::Vector2f& pos, T value);

    /**
     * @brief Moves the given value from it's old position in the grid to the new position
     *
     * @param oldPos The current position in spatial units
     * @param newPos The new position in spacial units
     * @param value The value to move
     */
    void move(const sf::Vector2f& oldPos, const sf::Vector2f& newPos, T value);

    /**
     * @brief Removes all elements. All iterators invalidated. Grid size is unchanged
     *
     */
    void clear();

    /**
     * @brief Returns the cell at the given position in spacial units
     *
     * @param pos The coordinate in spaital units
     * @return Range A range containing elements in the given cell
     */
    const Cell& getCell(const sf::Vector2f& pos);

    /**
     * @brief Returns the 0-based cell index containing the given spacial position. Returned value
     *        is bounded to the grid size, even if the requested position is outside of the Grid
     *        space.
     *
     * @param pos The coordinate in spacial units
     * @return sf::Vector2u The indices of the cell
     */
    sf::Vector2u getIndexAtPosition(const sf::Vector2f& pos);

    /**
     * @brief Returns the cell at the given x and y indices
     *
     * @param indices The x and y indices to access. No bounds check performed
     * @return const Cell& The cell at the given position
     */
    const Cell& getCellByIndex(const sf::Vector2u& indices);

    /**
     * @brief Iterates over all contained values within the given region. Callback can optionally
     *        return a boolean to end iteration early. Return true to end early
     *
     * @tparam TCallback Callback signature of the vistor to apply
     * @param region The region to iterate over contained entities within
     * @param cb The visitor to invoke for each contained element
     */
    template<typename TCallback>
    void forAllInRegion(const sf::FloatRect& region, const TCallback& cb);

    /**
     * @brief Iterates over all contained values within the cell and it's neighbors. Callback can
     *        optionally return a boolean to end iteration early. Return true to end early
     *
     * @tparam TCallback Callback signature of the vistor to apply
     * @param region The point to get the cell and neighbors for
     * @param cb The visitor to invoke for each contained element
     */
    template<typename TCallback>
    void forAllInCellAndNeighbors(const sf::Vector2f& cellPoint, const TCallback& cb);

private:
    sf::Vector2f origin;
    float cellWidth, cellHeight;
    Vector2D<Cell> cells;

    void doAdd(Cell& cell, T val);
    void doRemove(Cell& cell, T val);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
Grid<T>::Grid(const sf::FloatRect& space, float cw, float ch)
: origin(space.left, space.top)
, cellWidth(cw)
, cellHeight(ch)
, cells(std::ceil(space.width / cw), std::ceil(space.height / ch)) {
    for (auto& cell : cells) { cell.reserve(64); }
}

template<typename T>
void Grid<T>::setSize(const sf::FloatRect& space, float cw, float ch) {
    clear();
    origin.x   = space.left;
    origin.y   = space.top;
    cellWidth  = cw;
    cellHeight = ch;
    cells.setSize(std::ceil(space.width / cw), std::ceil(space.height / ch));
    for (auto& cell : cells) { cell.reserve(64); }
}

template<typename T>
void Grid<T>::add(const sf::Vector2f& pos, T val) {
    const sf::Vector2u ci = getIndexAtPosition(pos);
    doAdd(cells(ci.x, ci.y), val);
}

template<typename T>
void Grid<T>::doAdd(Cell& cell, T val) {
    cell.emplace_back(val);
}

template<typename T>
void Grid<T>::remove(const sf::Vector2f& pos, T val) {
    const sf::Vector2u ci = getIndexAtPosition(pos);
    doRemove(cells(ci.x, ci.y), val);
}

template<typename T>
void Grid<T>::doRemove(Cell& cell, T val) {
    for (auto it = cell.begin(); it != cell.end(); ++it) {
        if (*it == val) {
            cell.erase(it);
            break;
        }
    }
}

template<typename T>
void Grid<T>::move(const sf::Vector2f& oldPos, const sf::Vector2f& newPos, T val) {
    const sf::Vector2u oi = getIndexAtPosition(oldPos);
    const sf::Vector2u ni = getIndexAtPosition(newPos);
    if (oi == ni) return;

    doRemove(cells(oi.x, oi.y), val);
    doAdd(cells(ni.x, ni.y), val);
}

template<typename T>
void Grid<T>::clear() {
    for (auto& cell : cells) { cell.clear(); }
}

template<typename T>
const typename Grid<T>::Cell& Grid<T>::getCell(const sf::Vector2f& pos) {
    const sf::Vector2u i = getIndexAtPosition(pos);
    return cells(i.x, i.y);
}

template<typename T>
sf::Vector2u Grid<T>::getIndexAtPosition(const sf::Vector2f& pos) {
    sf::Vector2f v2f(pos - origin);
    v2f.x = std::max(v2f.x, 0.f);
    v2f.y = std::max(v2f.y, 0.f);
    v2f.x /= cellWidth;
    v2f.y /= cellHeight;
    sf::Vector2u v2u(v2f);
    v2u.x = std::min(v2u.x, cells.getWidth() - 1);
    v2u.y = std::min(v2u.y, cells.getHeight() - 1);
    return v2u;
}

template<typename T>
const typename Grid<T>::Cell& Grid<T>::getCellByIndex(const sf::Vector2u& i) {
    return cells(i.x, i.y);
}

template<typename T>
template<typename TCb>
void Grid<T>::forAllInRegion(const sf::FloatRect& region, const TCb& cb) {
    static_assert(std::is_invocable<TCb, T>::value,
                  "Visitor signature is void(T val) or bool(T val");

    const sf::Vector2u sp = getIndexAtPosition({region.left, region.top});
    const sf::Vector2u ep =
        getIndexAtPosition({region.left + region.width, region.top + region.height});
    for (unsigned int x = sp.x; x <= ep.x; ++x) {
        for (unsigned int y = sp.y; y <= ep.y; ++y) {
            Cell& cell = cells(x, y);
            for (T val : cell) {
                if constexpr (std::is_same_v<std::invoke_result_t<TCb, T>, bool>) {
                    if (cb(val)) return;
                }
                else {
                    cb(val);
                }
            }
        }
    }
}

template<typename T>
template<typename TCb>
void Grid<T>::forAllInCellAndNeighbors(const sf::Vector2f& pos, const TCb& cb) {
    static_assert(std::is_invocable<TCb, T>::value,
                  "Visitor signature is void(T val) or bool(T val");

    const sf::Vector2u i  = getIndexAtPosition(pos);
    const unsigned int sx = i.x > 0 ? i.x - 1 : 0;
    const unsigned int ex = i.x < cells.getWidth() - 2 ? i.x + 1 : cells.getWidth() - 1;
    const unsigned int sy = i.y > 0 ? i.y - 1 : 0;
    const unsigned int ey = i.y < cells.getHeight() - 2 ? i.y + 1 : cells.getHeight() - 1;

    for (unsigned int x = sx; x <= ex; ++x) {
        for (unsigned int y = sy; y <= ey; ++y) {
            Cell& cell = cells(x, y);
            for (T val : cell) {
                if constexpr (std::is_same_v<std::invoke_result_t<TCb, T>, bool>) {
                    if (cb(val)) return;
                }
                else {
                    cb(val);
                }
            }
        }
    }
}

} // namespace container
} // namespace bl

#endif
