#ifndef BLIB_CONTAINERS_2DVECTOR_HPP
#define BLIB_CONTAINERS_2DVECTOR_HPP

#include <vector>

namespace bl
{
namespace container
{
/**
 * @brief Utility class that wraps std::vector, making it indexable in two dimensions
 *
 * @ingroup Containers
 */
template<typename T>
class Vector2D {
public:
    /**
     * @brief Creates an empty 2d vector
     *
     */
    Vector2D()
    : W(0)
    , H(0) {}

    /**
     * @brief Copy construct a new Vector2D
     *
     * @param copy The vector to copy from
     */
    Vector2D(const Vector2D& copy)
    : data(copy.data)
    , W(copy.W)
    , H(copy.H) {}

    /**
     * @brief Move construct a new Vector2D
     *
     * @param move The vector to move from
     */
    Vector2D(Vector2D&& move)
    : data(std::move<std::vector<T>>(move.data))
    , W(move.W)
    , H(move.H) {}

    /**
     * @brief Copies into this vector from the given vector
     *
     * @param copy The vector to copy from
     * @return Vector2D& A reference to this vector
     */
    Vector2D& operator=(const Vector2D& copy) {
        data = copy.data;
        W    = copy.W;
        H    = copy.H;
        return *this;
    }

    /**
     * @brief Moves from the given vector into this vector
     *
     * @param move The vector to move from
     * @return Vector2D& A reference to this vector
     */
    Vector2D& operator=(Vector2D&& move) {
        data = std::move(move.data);
        W    = move.W;
        H    = move.H;
        return *this;
    }

    /**
     * @brief Creates the vector of the given width and height
     *
     * @param w The width of the vector
     * @param h The height of the vector
     * @param val The value to fill the vector with
     */
    Vector2D(unsigned int w, unsigned int h, const T& val = T{}) { setSize(w, h, val); }

    /**
     * @brief Returns a reference to the element at the given position
     *
     * @param x The x coordinate of the element to access
     * @param y The y coordinate of the element to access
     * @return A reference to the element at the given indices
     */
    T& operator()(unsigned int x, unsigned int y) { return data[y * W + x]; }

    /**
     * @brief Returns a const reference to the element at the given position
     *
     * @param x The x coordinate of the element to access
     * @param y The y coordinate of the element to access
     * @return A reference to the element at the given indices
     */
    const T& operator()(unsigned int x, unsigned int y) const { return data.at(y * W + x); }

    /**
     * @brief Resizes the vector to the given size
     *
     * @param x The new width
     * @param y The new height
     */
    void setSize(unsigned int x, unsigned int y, const T& val = T{}) {
        W = x;
        H = y;
        data.resize(W * H, val);
    }

    /**
     * @brief Returns the width of the vector
     */
    unsigned int getWidth() const { return W; }

    /**
     * @brief Returns the height of the vector
     */
    unsigned int getHeight() const { return H; }

    /**
     * @brief Clears all memory used by the vector, erasing all elements
     */
    void clear() { data.clear(); }

    /**
     * @brief Provides a raw iterator to the underlying data. Useful for iterating all elements
     *        efficiently without needing pointer arithmetic or nested loops
     *
     * @return std::vector<T>::iterator Iterator to the front of the stored data
     */
    typename std::vector<T>::iterator begin() { return data.begin(); }

    /**
     * @brief Provides a raw iterator to the underlying data. Useful for iterating all elements
     *        efficiently without needing pointer arithmetic or nested loops
     *
     * @return std::vector<T>::iterator Iterator to the end of the stored data
     */
    typename std::vector<T>::iterator end() { return data.end(); }

private:
    std::vector<T> data;
    unsigned int W, H;
};

} // namespace container
} // namespace bl

#endif
