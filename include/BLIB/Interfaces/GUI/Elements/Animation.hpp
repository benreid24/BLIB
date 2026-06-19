#ifndef BLIB_INTERFACES_GUI_ELEMENTS_ANIMATION_HPP
#define BLIB_INTERFACES_GUI_ELEMENTS_ANIMATION_HPP

#include <BLIB/Assets/Builtin/Animation2DSetPayload.hpp>
#include <BLIB/Assets/TypedRef.hpp>
#include <BLIB/Interfaces/GUI/Elements/Element.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Simple element that displays a constantly playing animation. Similar to Image
 *
 * @ingroup GUI
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
    static Ptr create(as::TypedRef<asi::Animation2DSetPayload> anim);

    /**
     * @brief Sets the animation to render. Does not reset any applied size
     *
     * @param anim The animation to render
     */
    void setAnimation(as::TypedRef<asi::Animation2DSetPayload> anim);

    /**
     * @brief Scales the rendered animation to the given size
     *
     * @param size The size to render to, in overlay space
     */
    void scaleToSize(const sf::Vector2f& size);

    /**
     * @brief Returns the animation being rendered by this element
     */
    as::TypedRef<asi::Animation2DSetPayload> getAnimation();

    /**
     * @brief Returns the size being scaled to, if any
     */
    const std::optional<sf::Vector2f>& getSize() const;

protected:
    /**
     * @brief Create a new Animation element
     *
     * @param anim The animation to use
     */
    Animation(as::TypedRef<asi::Animation2DSetPayload> anim);

    /**
     * @brief Returns the size required to render the animation
     */
    virtual sf::Vector2f minimumRequisition() const override;

    /**
     * @brief Creates the visual component for this element
     *
     * @param renderer The renderer to use to create visual Components
     * @return The visual component for this element
     */
    virtual rdr::Component* doPrepareRender(rdr::Renderer& renderer) override;

private:
    as::TypedRef<asi::Animation2DSetPayload> source;
    std::optional<sf::Vector2f> size;
};

} // namespace gui
} // namespace bl

#endif
