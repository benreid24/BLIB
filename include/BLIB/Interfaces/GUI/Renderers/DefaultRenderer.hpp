#ifndef BLIB_GUI_RENDERERS_DEFAULTRENDERER_HPP
#define BLIB_GUI_RENDERERS_DEFAULTRENDERER_HPP

#include <BLIB/Interfaces/GUI/Renderers/Renderer.hpp>

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

    /// @see Renderer::renderBox
    virtual void renderBox(sf::RenderTarget& target, sf::RenderStates states,
                           const Container& container) const override;

    /// @see Renderer::renderComboBox
    virtual void renderComboBox(sf::RenderTarget& target, sf::RenderStates states,
                                const ComboBox& box) const override;

    /// @see Renderer::renderComboBoxDropdownBoxes
    virtual void renderComboBoxDropdownBoxes(sf::RenderTarget& target, sf::RenderStates states,
                                             const ComboBox& box, const sf::Vector2f& optionSize,
                                             unsigned int optionCount,
                                             unsigned int mousedOption) const override;

    /// @see Renderer::renderComboBoxDropdownArrow
    virtual void renderComboBoxDropdownArrow(sf::RenderTexture& texture) const override;

    /// @see Renderer::renderImage
    virtual void renderImage(sf::RenderTarget& target, sf::RenderStates states,
                             const Element* element, const sf::Sprite& image) const override;

    /// @see Renderer::renderLabel
    virtual void renderLabel(sf::RenderTarget& target, sf::RenderStates states,
                             const Label& label) const override;

    /// @see Renderer::renderButton
    virtual void renderButton(sf::RenderTarget& target, sf::RenderStates states,
                              const Button& button) const override;

    /// @see Renderer::renderMouseoverOverlay
    virtual void renderMouseoverOverlay(sf::RenderTarget& target, sf::RenderStates states,
                                        const Element* element) const override;

    /// @see Renderer::renderNotebookTabs
    virtual void renderNotebookTabs(sf::RenderTarget& target, sf::RenderStates states,
                                    const Notebook& notebook) const override;

    /// @see Renderer::renderProgressBar
    virtual void renderProgressBar(sf::RenderTarget& target, sf::RenderStates states,
                                   const ProgressBar& progressBar) const override;
    /// @see Renderer::renderSeparator
    virtual void renderSeparator(sf::RenderTarget& target, sf::RenderStates states,
                                 const Separator& separator) const override;

    /// @see Renderer::renderSlider
    virtual void renderSlider(sf::RenderTarget& target, sf::RenderStates states,
                              const Slider& slider) const override;

    /// @see Renderer::renderSliderButton
    virtual void renderSliderButton(sf::RenderTexture& texture, bool horizontal,
                                    bool increasing) const override;

    /// @see Renderer::renderTextEntry
    virtual void renderTextEntry(sf::RenderTarget& target, sf::RenderStates states,
                                 const TextEntry& entry) const override;

    /// @see Renderer::renderToggleCheckButton
    virtual void renderToggleCheckButton(sf::RenderTexture& texture, bool active) const override;

    /// @see Renderer::renderToggleRadioButton
    virtual void renderToggleRadioButton(sf::RenderTexture& texture, bool active) const override;

    /// @see Renderer::renderWindow
    virtual void renderWindow(sf::RenderTarget& target, sf::RenderStates states,
                              const Container* titlebar, const Window& window) const override;
};

} // namespace gui
} // namespace bl

#endif