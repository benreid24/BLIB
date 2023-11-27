#ifndef BLIB_GUI_ELEMENTS_CANVAS_HPP
#define BLIB_GUI_ELEMENTS_CANVAS_HPP

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>
#include <BLIB/Interfaces/GUI/Renderer/CanvasComponentBase.hpp>
#include <BLIB/Render/Resources/SceneRef.hpp>

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
     * @param scene The scene to render to the canvas
     * @return Ptr The new canvas
     */
    static Ptr create(unsigned int w, unsigned int h, rc::SceneRef scene = {});

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
     * @brief Sets the scene to render in the texture
     *
     * @param scene The scene to render
     */
    void setScene(rc::SceneRef scene);

    /**
     * @brief Returns the scene that is rendering to the canvas
     */
    rc::SceneRef getScene() const;

    /**
     * @brief Returns the size of the texture being rendered to
     */
    const sf::Vector2u& getTextureSize() const;

    /**
     * @brief Returns the offset of the texture from the position of the element
     */
    const sf::Vector2f& getOffset() const;

    /**
     * @brief Returns the scale that the texture should be rendered with
     */
    const sf::Vector2f& getScale() const;

    /**
     * @brief Replaces the camera to render the current scene with
     *
     * @tparam TCamera The type of camera to install
     * @tparam ...TArgs Argument types to the camera's constructor
     * @param ...args Arguments to the camera's constructor
     * @return A pointer to the new camera
     */
    template<typename TCamera, typename... TArgs>
    TCamera* setCamera(TArgs&&... args);

    /**
     * @brief Sets the color to clear the canvas with each frame. Default is black
     *
     * @param color The color to reset the canvas with
     */
    void setClearColor(const sf::Color& color);

    /**
     * @brief Returns the color that the canvas will be cleared with each frame
     */
    const sf::Color& getClearColor() const;

protected:
    /**
     * @brief Create a new Canvas with the desired size
     *
     * @param w Width of the renderable area in pixels
     * @param h Height of the renderable area in pixels
     * @param scene The scene to render to the canvas
     */
    Canvas(unsigned int w, unsigned int h, rc::SceneRef scene);

    /**
     * @brief Returns the size the canvas is set to render to. Default is the canvas size
     *
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
    sf::Vector2u textureSize;
    std::optional<sf::Vector2f> size;
    rc::SceneRef scene;
    bool fillAcq;
    bool maintainAR;
    sf::Vector2f scale;
    sf::Vector2f offset;
    std::unique_ptr<cam::Camera> camera;
    sf::Color clearColor;

    void setScale();
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TCamera, typename... TArgs>
TCamera* Canvas::setCamera(TArgs&&... args) {
    rdr::CanvasComponentBase* com = dynamic_cast<rdr::CanvasComponentBase*>(getComponent());
    if (com) { return com->getRenderTexture().setCamera<TCamera>(std::forward<TArgs>(args)...); }
    else {
        camera = std::make_unique<TCamera>(std::forward<TArgs>(args)...);
        return static_cast<TCamera*>(camera.get());
    }
}

} // namespace gui
} // namespace bl

#endif
