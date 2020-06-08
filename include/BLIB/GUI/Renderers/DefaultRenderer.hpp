#ifndef BLIB_GUI_RENDERERS_DEFAULTRENDERER_HPP
#define BLIB_GUI_RENDERERS_DEFAULTRENDERER_HPP

#include <BLIB/GUI/Renderers/Renderer.hpp>

namespace bl
{
namespace gui
{
class Element;
class Container;
class Label;
class Button;
class Slider;
class Window;

/**
 * @brief Default renderer. Good enough for development or for tools, but a custom renderer may
 *        be preferred for better visual quality
 *
 * @see Renderer
 * @ingroup GUI
 *
 */
class DefaultRenderer : public Renderer {
public:
    typedef std::shared_ptr<DefaultRenderer> Ptr;

    /**
     * @brief Create the default renderer
     *
     */
    static Ptr create();

    /**
     * @brief Destroy the DefaultRenderer object
     *
     */
    virtual ~DefaultRenderer() = default;

    /**
     * @brief Render a user defined GUI Element. User code may use group() and id() to
     *        determine how to render
     *
     */
    virtual void renderCustom(sf::RenderTarget& target, sf::RenderStates states,
                              const Element& element) const override;

    /**
     * @brief Renders a Container element
     *
     * @param target Target to render to
     * @param states RenderStates to apply
     * @param container Container to render
     */
    virtual void renderBox(sf::RenderTarget& target, sf::RenderStates states,
                           const Container& container) const override;

    /**
     * @brief Renders an image. This is used by both Canvas and Image
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param element The element to render. Gives access to acquisition and render settings
     * @param image The image to render. Only position should be changed, scale is set
     */
    virtual void renderImage(sf::RenderTarget& target, sf::RenderStates states,
                             const Element* element, const sf::Sprite& image) const override;

    /**
     * @brief Renders a Label element
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param label Label to render
     */
    virtual void renderLabel(sf::RenderTarget& target, sf::RenderStates states,
                             const Label& label) const override;

    /**
     * @brief Renders a Button element
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param button Button to render
     */
    virtual void renderButton(sf::RenderTarget& target, sf::RenderStates states,
                              const Button& button) const override;

    /**
     * @brief Render a highlight or dark overlay over an element based on it's current state
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param element The element who's state and acquisition should be used
     */
    virtual void renderMouseoverOverlay(sf::RenderTarget& target, sf::RenderStates states,
                                        const Element* element) const override;

    /**
     * @brief Render a Slider element. The track only, none of the buttons should be rendered
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param slider Slider to render
     */
    virtual void renderSlider(sf::RenderTarget& target, sf::RenderStates states,
                              const Slider& slider) const override;

    /**
     * @brief Render a Window element. The titlebar will have no styling and should use
     *        the styling options from the window element
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param titlebar Pointer to the titlebar, nullptr if none
     * @param window The Window to render. Includes child Elements but not the titlebar
     */
    virtual void renderWindow(sf::RenderTarget& target, sf::RenderStates states,
                              const Container* titlebar, const Window& window) const override;
};

} // namespace gui
} // namespace bl

#endif