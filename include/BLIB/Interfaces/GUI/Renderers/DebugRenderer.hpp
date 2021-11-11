#ifndef BLIB_GUI_RENDERERS_DEBUGRENDERER_HPP
#define BLIB_GUI_RENDERERS_DEBUGRENDERER_HPP

#include <BLIB/Interfaces/GUI/Renderers/DefaultRenderer.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Utility renderer that can show the groups, ids, and acquisitions of elements
 *
 * @ingroup GUI
 */
class DebugRenderer : public Renderer {
public:
    typedef std::shared_ptr<DebugRenderer> Ptr;

    /**
     * @brief Creates a new DebugRenderer with the given underlying renderer
     *
     * @param renderer The renderer to use to render elements
     */
    static Ptr create(Renderer::Ptr renderer = DefaultRenderer::create());

    /**
     * @brief Destroy the DebugRenderer
     *
     */
    virtual ~DebugRenderer() = default;

    /**
     * @brief Set whether or not to draw outlines showing element acquisitions
     *
     * @param show True to show acquisitions
     */
    void showAcquisitions(bool show = true);

    /// @see Renderer::renderBox
    virtual void renderBox(sf::RenderTarget& target, sf::RenderStates states,
                           const Element& container) const override;

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
                                    const Notebook& notebook, float scroll) const override;

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

    /// @see Renderer::renderTooltip
    virtual void renderTooltip(sf::RenderTarget& target, sf::RenderStates states,
                               const Element* tooltip, const sf::Vector2f& mousePos) const override;

private:
    Renderer::Ptr underlying;
    bool showBounds;

    DebugRenderer(Renderer::Ptr renderer);
    void addInfo(sf::RenderTarget& target, sf::RenderStates states,
                 const sf::FloatRect& region) const;
};

} // namespace gui
} // namespace bl

#endif
