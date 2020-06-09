#ifndef BLIB_GUI_RENDERERS_RENDERER_HPP
#define BLIB_GUI_RENDERERS_RENDERER_HPP

#include <BLIB/GUI/Renderers/RenderSettings.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>

namespace bl
{
namespace gui
{
class Element;
class Container;
class Label;
class Button;
class Separator;
class Slider;
class Window;

/**
 * @brief Interface for renderers for GUIs. Derived classes may elect to inherit from
 *        DefaultRenderer and only override what they need. Provides some common functionality
 *
 * @ingroup GUI
 *
 */
class Renderer : public bl::NonCopyable {
public:
    typedef std::shared_ptr<Renderer> Ptr;

    /**
     * @brief Add or set RenderSettings for the given group. Overriden for Elements with id
     *        level overrides
     *
     * @param group The group to apply the settings to
     * @param settings The render settings
     */
    void setGroupSettings(const std::string& group, const RenderSettings& settings);

    /**
     * @brief Add or set RenderSettings for the given id. Settings applied in the element
     *        itself, if any, override these
     *
     * @param id Id of the Element to set for
     * @param settings The render settings
     */
    void setIdSettings(const std::string& id, const RenderSettings& settings);

    /**
     * @brief Destroy the Renderer object
     *
     */
    virtual ~Renderer() = default;

    /**
     * @brief Render a user defined GUI Element. User code may use group() and id() to
     *        determine how to render
     *
     */
    virtual void renderCustom(sf::RenderTarget& target, sf::RenderStates states,
                              const Element& element) const = 0;

    /**
     * @brief Renders a Box element
     *
     * @param target Target to render to
     * @param states RenderStates to apply
     * @param container Container to render
     */
    virtual void renderBox(sf::RenderTarget& target, sf::RenderStates states,
                           const Container& container) const = 0;

    /**
     * @brief Renders an image. This is used by both Canvas and Image
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param element The element to render. Gives access to acquisition and render settings
     * @param image The image to render. Only position should be changed, scale is set
     */
    virtual void renderImage(sf::RenderTarget& target, sf::RenderStates states,
                             const Element* element, const sf::Sprite& image) const = 0;

    /**
     * @brief Renders a Label element
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param label Label to render
     */
    virtual void renderLabel(sf::RenderTarget& target, sf::RenderStates states,
                             const Label& label) const = 0;

    /**
     * @brief Renders a Button element
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param button Button to render
     */
    virtual void renderButton(sf::RenderTarget& target, sf::RenderStates states,
                              const Button& button) const = 0;

    /**
     * @brief Render a highlight or dark overlay over an element based on it's current state
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param element The element who's state and acquisition should be used
     */
    virtual void renderMouseoverOverlay(sf::RenderTarget& target, sf::RenderStates states,
                                        const Element* element) const = 0;

    /**
     * @brief Renders a Separator element. Note that the outline thickness should be used for
     *        line thickness
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param separator The separator to render
     */
    virtual void renderSeparator(sf::RenderTarget& target, sf::RenderStates states,
                                 const Separator& separator) const = 0;

    /**
     * @brief Render a Slider element. The track only, none of the buttons should be rendered
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param slider Slider to render
     */
    virtual void renderSlider(sf::RenderTarget& target, sf::RenderStates states,
                              const Slider& slider) const = 0;

    /**
     * @brief Render to the rendertexture of a sliders increase or decrease button. This is
     *        typically called only once per button and the result is reused
     *
     * @param texture The rendertexture to render to
     * @param horizontal True if the slider is horizontal, false if vertical
     * @param increasing True if the button represents an increasing value, false otherwise
     */
    virtual void renderSliderButton(sf::RenderTexture& texture, bool horizontal,
                                    bool increasing) const = 0;

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
                              const Container* titlebar, const Window& window) const = 0;

    /**
     * @brief Utility method to render text with the given settings. Custom Elements may use
     *        this to implement rendering
     *
     * @param target Target to render to
     * @param text The text to render
     * @param acquisition The area to render inside of
     * @param settings The settings to apply
     * @param defaults Default settings to apply if settings is missing a parameter
     */
    void renderText(sf::RenderTarget& target, sf::RenderStates states, const std::string& text,
                    const sf::IntRect& acquisition, const RenderSettings& settings,
                    const RenderSettings& defaults = {}) const;

    /**
     * @brief Utility method to render a simple rectangle with the given render settings.
     *        Custom elements may use this to implement rendering
     *
     * @param target The target to render to
     * @param states The render states to use
     * @param area The rectangle to render
     * @param settings The settings to use
     * @param defaults Default settings to apply if settings is missing a parameter
     */
    void renderRectangle(sf::RenderTarget& target, sf::RenderStates states,
                         const sf::IntRect& area, const RenderSettings& settings,
                         const RenderSettings& defaults = {}) const;

    /**
     * @brief Returns an aggregated RenderSettings object for the given Element. Settings
     *        with no values are left empty.
     *
     */
    RenderSettings getSettings(const Element* element) const;

    /**
     * @brief Calculates the position to render an element at given its alignments and size
     *
     * @param horizontalAlignment Horizontal alignment of the element being rendered
     * @param verticalAlignment Vertical alignment of the element being rendered
     * @param region Area the element is to be rendered in
     * @param size The size of the element being rendered
     * @return sf::Vector2f The position to render the element at to respect alignments
     */
    static sf::Vector2f calculatePosition(RenderSettings::Alignment horizontalAlignment,
                                          RenderSettings::Alignment verticalAlignment,
                                          const sf::IntRect& region, const sf::Vector2f& size);

protected:
    /**
     * @brief Construct a new default renderer
     *
     */
    Renderer() = default;

private:
    std::unordered_map<std::string, RenderSettings> groupSettings;
    std::unordered_map<std::string, RenderSettings> idSettings;
};

} // namespace gui
} // namespace bl

#endif