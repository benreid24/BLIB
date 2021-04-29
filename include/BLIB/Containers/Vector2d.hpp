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
     * @brief Creates the vector of the given width and height
     *
     * @param w The width of the vector
     * @param h The height of the vector
     * @param val The value to fill the vector with
     */
    Vector2D(unsigned int w, unsigned int h, const T& val = {}) { setSize(w, h, val); }

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
    void setSize(unsigned int x, unsigned int y, const T& val = {}) {
        W = x;
        H = y;
        data.resize(W * H, val);
    }

    /**
     * @brief Returns the width of the vector
     */
    int getWidth() const { return W; }

    /**
     * @brief Returns the height of the vector
     */
    int getHeight() const { return H; }

    /**
     * @brief Clears all memory used by the vector, erasing all elements
     */
    void clear() { data.clear(); }

private:
    std::vector<T> data;
    int W, H;
};

} // namespace container
} // namespace bl

#endif
