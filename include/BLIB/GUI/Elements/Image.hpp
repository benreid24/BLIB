#ifndef BLIB_GUI_ELEMENTS_IMAGE_HPP
#define BLIB_GUI_ELEMENTS_IMAGE_HPP

#include <BLIB/GUI/Elements/Element.hpp>

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
     * @brief Create a new Image with the desired size
     *
     * @param texture The texture of the image to render
     * @param group The group the image belongs to
     * @param id The id of this image
     * @return Ptr The new image
     */
    static Ptr create(resource::Resource<sf::Texture>::Ref texture, const std::string& group = "",
                      const std::string& id = "");

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

protected:
    /**
     * @brief Create a new Image with the desired size
     *
     * @param texture The texture of the image to render
     * @param group The group the image belongs to
     * @param id The id of this image
     * @return Ptr The new image
     */
    Image(resource::Resource<sf::Texture>::Ref texture, const std::string& group,
          const std::string& id);

    /**
     * @brief Returns the size the image is set to render to. Default is the image size
     *
     */
    virtual sf::Vector2i minimumRequisition() const override;

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
    resource::Resource<sf::Texture>::Ref texture;
    sf::Sprite sprite;
    std::optional<sf::Vector2f> size;
    bool fillAcq;
    bool maintainAR;

    void setScale();
};

} // namespace gui
} // namespace bl

#endif