#ifndef BLIB_INTERFACES_GUI_ELEMENTS_ANIMATION_HPP
#define BLIB_INTERFACES_GUI_ELEMENTS_ANIMATION_HPP

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>

#include <BLIB/Media/Graphics/Animation.hpp>
#include <BLIB/Resources.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Simple element that displays a constantly playing animation. Similar to Image
 *
 * @ingroup GUI
 *
 */
class Animation : public Element {
public:
    using Ptr = std::shared_ptr<Animation>;

    virtual ~Animation() = default;

    /**
     * @brief Create a new Animation element
     *
     * @param anim The animation to use
     * @return Ptr The new Animation
     */
    static Ptr create(resource::Ref<gfx::AnimationData> anim);

    /**
     * @brief Sets the animation to render. Does not reset any applied size
     *
     * @param anim The animation to render
     */
    void setAnimation(resource::Ref<gfx::AnimationData> anim);

    /**
     * @brief Scales the rendered animation to the given size
     *
     * @param size The size to render to, in pixels
     */
    void scaleToSize(const sf::Vector2f& size);

    /**
     * @brief Updates the animation
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(float dt) override;

protected:
    /**
     * @brief Create a new Animation element
     *
     * @param anim The animation to use
     */
    Animation(resource::Ref<gfx::AnimationData> anim);

    /**
     * @brief Returns the size required to render the animation
     *
     */
    virtual sf::Vector2f minimumRequisition() const override;

    /**
     * @brief Renders the animation
     *
     * @param target The target to render to
     * @param states Render states to apply
     * @param renderer Unused
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

private:
    bool centered;
    resource::Ref<gfx::AnimationData> source;
    gfx::Animation animation;
    std::optional<sf::Vector2f> size;
};

} // namespace gui
} // namespace bl

#endif
