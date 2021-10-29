#ifndef BLIB_GUI_ELEMENTS_CANVAS_HPP
#define BLIB_GUI_ELEMENTS_CANVAS_HPP

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Specialized Element that allows user code to render to its area.
 *
 * @see Image
 * @ingroup GUI
 *
 */
class Canvas : public Element {
public:
    typedef std::shared_ptr<Canvas> Ptr;

    virtual ~Canvas() = default;

    /**
     * @brief Create a new Canvas with the desired size
     *
     * @param w Width of the renderable area in pixels
     * @param h Height of the renderable area in pixels
     * @return Ptr The new canvas
     */
    static Ptr create(unsigned int w, unsigned int h);

    /**
     * @brief Resize the underlying texture of the Canvas
     *
     * @param w The width of the renderable area
     * @param h The height of the renderable area
     * @param resetScale True to reset any override set in scaleToSize(), false to keep
     */
    void resize(unsigned int w, unsigned int h, bool resetScale = true);

    /**
     * @brief Override the target size the canvas renders to. The default is the size of the
     *        canvas. This is the size that the Canvas will request for its acquisition
     *
     * @param size The size to render to
     * @param markDirty True to mark the Canvas dirty after scaling
     */
    void scaleToSize(const sf::Vector2f& size, bool markDirty = true);

    /**
     * @brief Set whether or not the rendering should fill the assigned acquisition. This will
     *        scale the canvas to fill the full acquisition if it is larger than the canvas
     *        size or the size set in scaleToSize(). If the rendered area is smaller than the
     *        acquisition then the position will depend on the alignment set in
     *        setHorizontalAlignment() and setVerticalAlignment(). Default is centered
     *
     * @param fill True to render to the full acquisition, false to render to the fixed size
     * @param maintainAR Whether the canvas should keep its aspect ratio or be stretched
     */
    void setFillAcquisition(bool fill, bool maintainAR = true);

    /**
     * @brief Returns a mutable reference to the underlying texture. This may be rendered to
     *        Note that display() must be called on this object after rendering is complete
     *
     * @return sf::RenderTexture& Reference to the underlying texture
     */
    sf::RenderTexture& getTexture();

protected:
    /**
     * @brief Create a new Canvas with the desired size
     *
     * @param w Width of the renderable area in pixels
     * @param h Height of the renderable area in pixels
     */
    Canvas(unsigned int w, unsigned int h);

    /**
     * @brief Returns the size the canvas is set to render to. Default is the canvas size
     *
     */
    virtual sf::Vector2f minimumRequisition() const override;

    /**
     * @brief Renders the texture to the acquisition area
     *
     * @param target The target to render to
     * @param states Render states to apply
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

private:
    sf::RenderTexture texture;
    sf::Sprite sprite;
    std::optional<sf::Vector2f> size;
    bool fillAcq;
    bool maintainAR;

    void setScale();
    void moveCb();
};

} // namespace gui
} // namespace bl

#endif
