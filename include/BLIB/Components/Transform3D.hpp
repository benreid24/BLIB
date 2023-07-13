#ifndef BLIB_COMPONENTS_TRANSFORM3D_HPP
#define BLIB_COMPONENTS_TRANSFORM3D_HPP

#include <BLIB/Components/Orientation3D.hpp>
#include <BLIB/Render/Components/DescriptorComponentBase.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Representation of a full 3d transform. Includes position, orientation, and scale
 *
 * @ingroup Components
 */
class Transform3D : public rc::rcom::DescriptorComponentBase<Transform3D, glm::mat4> {
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
     * @brief Computes the transform and populates the given transform matrix
     *
     * @param dest The matrix to populate
     */
    void refreshDescriptor(glm::mat4& dest);

private:
    glm::vec3 position;
    Orientation3D orientation;
    glm::vec3 scaleFactors;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const glm::vec3& Transform3D::getPosition() const { return position; }

inline constexpr const Orientation3D& Transform3D::getOrientation() const { return orientation; }

inline constexpr const glm::vec3& Transform3D::getScale() const { return scaleFactors; }

} // namespace com
} // namespace bl

#endif
