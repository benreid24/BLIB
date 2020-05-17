#ifndef BLIB_GUI_RENDERERS_DEBUGRENDERER_HPP
#define BLIB_GUI_RENDERERS_DEBUGRENDERER_HPP

#include <BLIB/GUI/Renderers/Renderer.hpp>

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
    static Ptr create(Renderer::Ptr renderer = Renderer::create());

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
    virtual void renderCustom(sf::RenderTarget& target, const Element& element) const override;

    /// @see Renderer::renderContainer
    virtual void renderContainer(sf::RenderTarget& target,
                                 const Container& container) const override;

    /// @see Renderer::renderLabel
    virtual void renderLabel(sf::RenderTarget& target, const Label& label) const override;

    /// @see Renderer::renderButton
    virtual void renderButton(sf::RenderTarget& target, const Button& button) const override;

    /// @see Renderer::renderWindow
    virtual void renderWindow(sf::RenderTarget& target,
                              const Container& window) const override;

private:
    Renderer::Ptr underlying;
    bool showBounds;
    bool printGroups;
    bool printIds;

    DebugRenderer(Renderer::Ptr renderer);
    void addInfo(sf::RenderTarget& target, const sf::IntRect& region, const std::string& group,
                 const std::string& id) const;
};

} // namespace gui
} // namespace bl

#endif