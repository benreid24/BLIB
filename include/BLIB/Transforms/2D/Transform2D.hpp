#ifndef BLIB_TRANSFORMS_2D_TRANSFORM2D_HPP
#define BLIB_TRANSFORMS_2D_TRANSFORM2D_HPP

#include <BLIB/Render/Components/DescriptorComponentBase.hpp>
#include <glm/glm.hpp>

namespace bl
{
/// Collection of classes for 2d transforms
namespace t2d
{
/**
 * @brief Representation of a 2d transform with a depth component to allow the depth buffer to be
 *        used when rendering 2d entities
 *
 * @ingroup Transforms
 */
class Transform2D : public render::com::DescriptorComponentBase<Transform2D, glm::mat4> {
public:
    /**
     * @brief Creates a new transform with sane defaults
     */
    Transform2D();

    /**
     * @brief Convenience method to create a transform at the given position
     *
     * @param position The position to be at
     */
    Transform2D(const glm::vec2& position);

    /**
     * @brief Sets the origin to perform transforms around
     *
     * @param origin The origin to rotate around
     */
    void setOrigin(const glm::vec2& origin);

    /**
     * @brief Returns the origin of the transform
     */
    constexpr const glm::vec2& getOrigin() const;

    /**
     * @brief Sets the position of this transform
     *
     * @param position The new position to transform to
     */
    void setPosition(const glm::vec2& position);

    /**
     * @brief Applies an offset to the current position
     *
     * @param delta The vector to add to the current position
     */
    void move(const glm::vec2& delta);

    /**
     * @brief Returns the current position
     */
    constexpr const glm::vec2& getPosition() const;

    /**
     * @brief Sets the depth of the transform on the z-axis. Affects render order only. Default is 0
     *
     * @param depth The depth to set to
     */
    void setDepth(float depth);

    /**
     * @brief Adds the given value to the current depth
     *
     * @param delta The amount to add to the current depth
     */
    void addDepth(float delta);

    /**
     * @brief Returns the current depth of the transform
     */
    float getDepth() const;

    /**
     * @brief Sets the scale factors of the transform
     *
     * @param factors The scale factors to set
     */
    void setScale(const glm::vec2& factors);

    /**
     * @brief Convenience method to set the x and y scale factors
     *
     * @param factor The scale factor to set to
     */
    void setScale(float factor);

    /**
     * @brief Scales the current scale factors by the given amount
     *
     * @param factors Scale factors to multiply by
     */
    void scale(const glm::vec2& factors);

    /**
     * @brief Scales both scale factors by the given scale
     *
     * @param factor The factor to multiply both scale factors by
     */
    void scale(float factor);

    /**
     * @brief Returns the scale factors of the transform
     */
    constexpr const glm::vec2& getScale() const;

    /**
     * @brief Sets the rotation of the transform
     *
     * @param angle The angle in degrees
     */
    void setRotation(float angle);

    /**
     * @brief Applies the given rotation to the current rotation
     *
     * @param delta Amount of degrees to add to the current rotation
     */
    void rotate(float delta);

    /**
     * @brief Returns the current rotation in degrees
     */
    constexpr float getRotation() const;

    /**
     * @brief Populates the given 4x4 matrix to apply this transform
     *
     * @param dest The matrix to populate
     */
    void refreshDescriptor(glm::mat4& dest);

private:
    glm::vec2 origin;
    glm::vec2 position;
    glm::vec2 scaleFactors;
    float rotation;
    float depth;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr float Transform2D::getRotation() const { return rotation; }

inline constexpr const glm::vec2& Transform2D::getScale() const { return scaleFactors; }

inline constexpr const glm::vec2& Transform2D::getPosition() const { return position; }

inline constexpr const glm::vec2& Transform2D::getOrigin() const { return origin; }

} // namespace t2d
} // namespace bl

#endif
