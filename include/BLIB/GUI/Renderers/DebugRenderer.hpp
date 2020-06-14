#ifndef BLIB_GUI_RENDERERS_DEBUGRENDERER_HPP
#define BLIB_GUI_RENDERERS_DEBUGRENDERER_HPP

#include <BLIB/GUI/Renderers/DefaultRenderer.hpp>

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

    /**
     * @brief Set whether or not to print group names by elements
     *
     * @param show True to print group names of elements
     */
    void showGroups(bool show = true);

    /**
     * @brief Set whether or not to print element ids
     *
     * @param show True to print element ids
     */
    void showIds(bool show = true);

    /// @see Renderer::renderCustom
    virtual void renderCustom(sf::RenderTarget& target, sf::RenderStates states,
                              const Element& element) const override;

    /// @see Renderer::renderBox
    virtual void renderBox(sf::RenderTarget& target, sf::RenderStates states,
                           const Container& container) const override;

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

    /// @see Renderer::renderNotebook
    virtual void renderNotebook(sf::RenderTarget& target, sf::RenderStates states,
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

    /// @see Renderer::renderToggleButton
    virtual void renderToggleButton(sf::RenderTarget& target, sf::RenderStates states,
                                    const ToggleButton& button) const override;

    /// @see Renderer::renderToggleCheckButton
    virtual void renderToggleCheckButton(sf::RenderTexture& texture,
                                         bool active) const override;

    /// @see Renderer::renderToggleRadioButton
    virtual void renderToggleRadioButton(sf::RenderTexture& texture,
                                         bool active) const override;

    /// @see Renderer::renderWindow
    virtual void renderWindow(sf::RenderTarget& target, sf::RenderStates states,
                              const Container* titlebar, const Window& window) const override;

private:
    Renderer::Ptr underlying;
    bool showBounds;
    bool printGroups;
    bool printIds;

    DebugRenderer(Renderer::Ptr renderer);
    void addInfo(sf::RenderTarget& target, sf::RenderStates states, const sf::IntRect& region,
                 const std::string& group, const std::string& id) const;
};

} // namespace gui
} // namespace bl

#endif