#ifndef BLIB_CONTAINERS_2DVECTOR_HPP
#define BLIB_CONTAINERS_2DVECTOR_HPP

#include <vector>

namespace bl
{
/**
 * @brief Utility class that wraps std::vector, making it indexable in two dimensions
 *
 * @ingroup Containers
 */
template<typename T>
class Vector2D {
public:
    Vector2D() = default;
    Vector2D(int w, int h) { setSize(w, h); }

    /**
     * @brief Returns a reference to the element at the given position
     *
     * @param x The x coordinate of the element to access
     * @param y The y coordinate of the element to access
     * @return A reference to the element at the given indices
     */
    T& operator()(int x, int y) { return data[y * W + x]; }

    /**
     * @brief Resizes the vector to the given size
     *
     * @param x The new width
     * @param y The new height
     */
    void setSize(int x, int y, const T& val = {}) {
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

} // namespace bl

#endif
