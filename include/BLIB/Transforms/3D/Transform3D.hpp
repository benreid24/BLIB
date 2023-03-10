#ifndef BLIB_TRANSFORMS_3D_TRANSFORM3D_HPP
#define BLIB_TRANSFORMS_3D_TRANSFORM3D_HPP

#include <BLIB/Transforms/3D/Orientation3D.hpp>
#include <glm/glm.hpp>

namespace bl
{
/// Collection of transformations and helpers for 3d space
namespace t3d
{
/**
 * @brief Representation of a full 3d transform. Includes position, orientation, and scale
 *
 * @ingroup Transforms
 */
class Transform3D {
public:
    /**
     * @brief Creates a new transform with no scaling and sane defaults
     */
    Transform3D();

    /**
     * @brief Sets the position of the transform
     *
     * @param pos The position in 3d space
     */
    void setPosition(const glm::vec3& pos);

    /**
     * @brief Translates the transform
     *
     * @param offset Amount to change the position by
     */
    void move(const glm::vec3& offset);

    /**
     * @brief Returns the position of the transform
     */
    constexpr const glm::vec3& getPosition() const;

    /**
     * @brief Returns the orientation of the transform and marks itself as dirty. Use this if you
     *        are going to modify the orientation
     */
    Orientation3D& getOrientationForChange();

    /**
     * @brief Returns the orientation of the transform
     */
    constexpr const Orientation3D& getOrientation() const;

    /**
     * @brief Sets the scale of the transform
     *
     * @param scale The scale factors to set
     */
    void setScale(const glm::vec3& scale);

    /**
     * @brief Multiplies the current scale of the transform by the given factors
     *
     * @param factors The factors to multiply the current scale with
     */
    void scale(const glm::vec3& factors);

    /**
     * @brief Returns the current scale factors of the transform
     */
    constexpr const glm::vec3& getScale() const;

    /**
     * @brief Returns whether or not the transform needs to be recomputed
     */
    constexpr bool isDirty() const;

    /**
     * @brief Set the matrix to populate with the computed transform. Used by the renderer to
     *        populate scene buffers
     *
     * @param dest Pointer to the matrix to populate. Nullptr to disable
     */
    void setDest(glm::mat4* dest);

    /**
     * @brief Recomputes the transformation matrix and stores in the dest matrix, if any
     */
    void computeTransform();

private:
    glm::vec3 position;
    Orientation3D orientation;
    glm::vec3 scaleFactors;
    bool dirty;
    glm::mat4* dest;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const glm::vec3& Transform3D::getPosition() const { return position; }

inline constexpr const Orientation3D& Transform3D::getOrientation() const { return orientation; }

inline constexpr const glm::vec3& Transform3D::getScale() const { return scaleFactors; }

inline constexpr bool Transform3D::isDirty() const { return dirty && dest != nullptr; }

inline void Transform3D::setDest(glm::mat4* d) { dest = d; }

} // namespace t3d
} // namespace bl

#endif
