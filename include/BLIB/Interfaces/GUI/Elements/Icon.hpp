#ifndef BLIB_GUI_ELEMENTS_ICON_HPP
#define BLIB_GUI_ELEMENTS_ICON_HPP

#include <BLIB/Graphics/Icon.hpp>
#include <BLIB/Interfaces/GUI/Elements/Element.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Basic element for a graphical icon. See gfx::Icon
 *
 * @ingroup GUI
 */
class Icon : public Element {
public:
    using Ptr  = std::shared_ptr<Icon>;
    using Type = gfx::Icon::Type;

    /**
     * @brief Creates a new Icon element
     *
     * @param type The type of icon to create
     * @param size The size of the icon
     * @param rotation Optional rotation in degrees
     * @return The new icon element
     */
    static Ptr create(Type type, const sf::Vector2f& size = {16.f, 16.f}, float rotation = 0.f);

    /**
     * @brief Destroys the icon element
     */
    virtual ~Icon() = default;

    /**
     * @brief Sets the size of the icon
     *
     * @param size The size of the icon
     */
    void setIconSize(const sf::Vector2f& size);

    /**
     * @brief Returns the size of the icon
     */
    const sf::Vector2f& getIconSize() const;

    /**
     * @brief Returns the type of icon this is
     */
    Type getType() const;

    /**
     * @brief Sets the rotation of the icon around its center
     *
     * @param rotation The rotation in degrees
     */
    void setRotation(float rotation);

    /**
     * @brief Returns the rotation of the icon, in degrees, around its center
     */
    float getRotation() const;

    /**
     * @brief Set whether or not the icon should fill the assigned acquisition. This will
     *        scale the icon to fill the full acquisition if it is larger than the icon
     *        size or the size set in create(). If the rendered area is smaller than the
     *        acquisition then the position will depend on the alignment set in
     *        setHorizontalAlignment() and setVerticalAlignment(). Default is centered
     *
     * @param fill True to render to the full acquisition, false to render to the fixed size
     * @param maintainAR Whether the icon should keep its aspect ratio or be stretched
     */
    void setFillAcquisition(bool fill, bool maintainAR = true);

protected:
    /**
     * @brief Creates a new Icon element
     *
     * @param type The type of icon to create
     * @param size The size of the icon
     * @param rotation Optional rotation in degrees
     */
    Icon(Type type, const sf::Vector2f& size, float rotation);

    /**
     * @brief Returns the minimum space required to render the icon
     *
     * @return sf::Vector2i Space required
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
    const Type type;
    sf::Vector2f ogSize;
    sf::Vector2f size;
    float rotation;
    bool fillAcq;
    bool maintainAR;

    void onAcquisition();
};

} // namespace gui
} // namespace bl

#endif
