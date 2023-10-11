#ifndef BLIB_COMPONENTS_TRANSFORM2D_HPP
#define BLIB_COMPONENTS_TRANSFORM2D_HPP

#include <BLIB/ECS/Traits/ParentAwareVersioned.hpp>
#include <BLIB/Render/Components/DescriptorComponentBase.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Representation of a 2d transform with a depth component to allow the depth buffer to be
 *        used when rendering 2d entities
 *
 * @ingroup Components
 */
class Transform2D
: public rc::rcom::DescriptorComponentBase<Transform2D, glm::mat4>
, public ecs::trait::ParentAwareVersioned<Transform2D> {
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
    const glm::vec2& getOrigin() const;

    /**
     * @brief Sets the position of this transform. Relative to the parent
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
     * @brief Returns the current position relative to the parent
     */
    const glm::vec2& getLocalPosition() const;

    /**
     * @brief Returns the position in global space
     */
    glm::vec2 getGlobalPosition();

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
     * @brief Returns the total depth of the transform. Total depth is the sum of all parent depths
     *        plus this depth
     */
    float getGlobalDepth() const;

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
    const glm::vec2& getScale() const;

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
    float getRotation() const;

    /**
     * @brief Populates the given 4x4 matrix to apply this transform
     *
     * @param dest The matrix to populate
     */
    void refreshDescriptor(glm::mat4& dest);

    /**
     * @brief Creates a transform matrix from the components of a 2d transform
     *
     * @param origin The local origin of the transform
     * @param position The position of the transform. z is the depth
     * @param scale The scale factors of the transform
     * @param rotation The rotation, in degrees, of the transform
     * @return A transformation matrix for the given parameters
     */
    static glm::mat4 createTransformMatrix(const glm::vec2& origin, const glm::vec3& position,
                                           const glm::vec2& scale, float rotation);

    /**
     * @brief Returns the local transform matrix of this transform
     */
    glm::mat4 getLocalTransform() const;

    /**
     * @brief Returns the global transform matrix for this transform
     */
    const glm::mat4& getGlobalTransform();

    /**
     * @brief Returns whether or not the transform requires a refresh. Prefer this to inherited
     *        methods from ecs traits or descriptor component base
     */
    bool requiresRefresh() const {
        return ParentAwareVersioned::refreshRequired() || DescriptorComponentBase::isDirty();
    }

private:
    glm::vec2 origin;
    glm::vec2 position;
    glm::vec2 scaleFactors;
    float rotation;
    float depth;
    glm::mat4 cachedGlobalTransform;

    void makeDirty();
    void ensureUpdated();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline float Transform2D::getRotation() const { return rotation; }

inline const glm::vec2& Transform2D::getScale() const { return scaleFactors; }

inline const glm::vec2& Transform2D::getLocalPosition() const { return position; }

inline const glm::vec2& Transform2D::getOrigin() const { return origin; }

} // namespace com
} // namespace bl

#endif
