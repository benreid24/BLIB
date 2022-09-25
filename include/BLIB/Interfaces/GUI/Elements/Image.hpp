#ifndef BLIB_GUI_ELEMENTS_IMAGE_HPP
#define BLIB_GUI_ELEMENTS_IMAGE_HPP

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>
#include <BLIB/Resources.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Element that renders an image to its acquisition
 *
 * @see Canvas
 * @ingroup GUI
 *
 */
class Image : public Element {
public:
    typedef std::shared_ptr<Image> Ptr;

    virtual ~Image() = default;

    /**
     * @brief Create a new Image from the given managed texture
     *
     * @param texture The texture of the image to render
     * @return Ptr The new image
     */
    static Ptr create(resource::Resource<sf::Texture>::Ref texture);

    /**
     * @brief Create a new Image from a non managed texture
     *
     * @param texture The texture of the image to render. Must remain in scope
     * @param group The group the image belongs to
     * @param id The id of this image
     * @return Ptr The new image
     */
    static Ptr create(const sf::Texture& texture);

    /**
     * @brief Override the target size the image renders to. The default is the size of the
     *        image. This is the size that the Image will request for its acquisition
     *
     * @param size The size to render to
     */
    void scaleToSize(const sf::Vector2f& size);

    /**
     * @brief Set whether or not the rendering should fill the assigned acquisition. This will
     *        scale the image to fill the full acquisition if it is larger than the image size
     *        or the size set in scaleToSize(). If the rendered area is smaller than the
     *        acquisition then the position will depend on the alignment set in
     *        setHorizontalAlignment() and setVerticalAlignment(). Default is centered
     *
     * @param fill True to render to the full acquisition, false to render to the fixed size
     * @param maintainAR Whether the image should keep its aspect ratio or be stretched to fill
     */
    void setFillAcquisition(bool fill, bool maintainAR = true);

    /**
     * @brief Updates the image to display with the new texture
     *
     * @param texture The new texture to display
     * @param resetScale True to reset the display size, false to keep it
     */
    void setImage(const sf::Texture& texture, bool resetScale);

    /**
     * @brief Updates the image to display with the new texture
     *
     * @param texture The new texture to display
     * @param resetScale True to reset the display size, false to keep it
     */
    void setImage(const resource::Resource<sf::Texture>::Ref& texture, bool resetScale);

protected:
    /**
     * @brief Create a new Image from the given managed texture
     *
     * @param texture The texture of the image to render
     * @return Ptr The new image
     */
    Image(resource::Resource<sf::Texture>::Ref texture);

    /**
     * @brief Create a new Image from a non managed texture
     *
     * @param texture The texture of the image to render. Must remain in scope
     * @return Ptr The new image
     */
    Image(const sf::Texture& texture);

    /**
     * @brief Returns the size the image is set to render to. Default is the image size
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
    resource::Resource<sf::Texture>::Ref textureHandle;
    const sf::Texture* texture;
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
