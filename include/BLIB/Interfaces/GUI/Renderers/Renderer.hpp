#ifndef BLIB_GUI_RENDERERS_RENDERER_HPP
#define BLIB_GUI_RENDERERS_RENDERER_HPP

#include <BLIB/Interfaces/GUI/Renderers/RenderSettings.hpp>
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

class Button;
class ComboBox;
class Label;
class Notebook;
class ProgressBar;
class Separator;
class Slider;
class TextEntry;
class ToggleButton;
class Window;

/**
 * @brief Interface for renderers for GUIs. Derived classes may elect to inherit from
 *        DefaultRenderer and only override what they need. Provides some common functionality
 *
 * @ingroup GUI
 *
 */
class Renderer : public bl::util::NonCopyable {
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
     * @brief Render a ComboBox base box
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param box The box to render
     */
    virtual void renderComboBox(sf::RenderTarget& target, sf::RenderStates states,
                                const ComboBox& box) const = 0;

    /**
     * @brief Render a ComboBox dropdown boxes
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param box The box to render
     * @param optionSize The size of the option panels to render (background only)
     * @param optionCount The number of option panels to render
     * @param mousedOption Which option should get the alternate background color for mouse
     *                     over. None are moused over if mousedOption >= optioncount
     */
    virtual void renderComboBoxDropdownBoxes(sf::RenderTarget& target, sf::RenderStates states,
                                             const ComboBox& box, const sf::Vector2i& optionSize,
                                             unsigned int optionCount,
                                             unsigned int mousedOption) const = 0;

    /**
     * @brief Render the dropdown arrow to be used by ComboBox's
     *
     * @param texture The texture to render to
     */
    virtual void renderComboBoxDropdownArrow(sf::RenderTexture& texture) const = 0;

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
     * @brief Renders a Notebook element. Must also render the tabs and active content using
     * getPages()
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param notebook Notebook to render
     */
    virtual void renderNotebook(sf::RenderTarget& target, sf::RenderStates states,
                                const Notebook& notebook) const = 0;

    /**
     * @brief Renders a ProgressBar element
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param progressBar ProgressBar to render
     */
    virtual void renderProgressBar(sf::RenderTarget& target, sf::RenderStates states,
                                   const ProgressBar& progressBar) const = 0;

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
     * @brief Renders a TextEntry element
     *
     * @param target The target to render to
     * @param states RenderStates to apply
     * @param entry TextEntry to render
     */
    virtual void renderTextEntry(sf::RenderTarget& target, sf::RenderStates states,
                                 const TextEntry& entry) const = 0;

    /**
     * @brief Render a check button to an internal texture
     *
     * @param texture The canvas texture to render to
     * @param active True if the button should be rendered as active, false for inactive
     */
    virtual void renderToggleCheckButton(sf::RenderTexture& texture, bool active) const = 0;

    /**
     * @brief Render a radio button to an internal texture
     *
     * @param texture The canvas texture to render to
     * @param active True if the button should be rendered as active, false for inactive
     */
    virtual void renderToggleRadioButton(sf::RenderTexture& texture, bool active) const = 0;

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

protected:
    /**
     * @brief Construct a new default renderer
     *
     */
    Renderer() = default;

    /**
     * @brief Returns whether or not the given view is valid for rendering
     *
     */
    bool viewValid(const sf::View& view) const;

    /**
     * @brief Helper function to get render settings for the given element
     *
     * @param e The element to get settings for
     * @return const RenderSettings& The render settings to use
     */
    const RenderSettings& getSettings(const Element* e) const;
};

} // namespace gui
} // namespace bl

#endif