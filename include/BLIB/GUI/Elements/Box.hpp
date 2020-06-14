#ifndef BLIB_GUI_ELEMENTS_BOX_HPP
#define BLIB_GUI_ELEMENTS_BOX_HPP

#include <BLIB/GUI/Elements/Container.hpp>
#include <BLIB/GUI/Packers/Packer.hpp>
#include <list>
#include <unordered_set>

namespace bl
{
namespace gui
{
/**
 * @brief Simple Container for packing elements into a visible box on screen
 *
 * @ingroup GUI
 *
 */
class Box : public Container {
public:
    typedef std::shared_ptr<Box> Ptr;

    /**
     * @brief Construct a new Box with the given id, group, and Packer
     *
     * @param packer The Packer to pack child elements with
     * @param group The group of the Element
     * @param id The id of this Element
     */
    static Ptr create(Packer::Ptr packer, const std::string& group = "",
                      const std::string& id = "");

    /**
     * @brief Destroy the Box object
     *
     */
    virtual ~Box() = default;

    /**
     * @brief Update the Packer used. Marks the Element as dirty
     *
     */
    void setPacker(Packer::Ptr packer);

    /**
     * @brief Adds the Element as a new child. Causes a refresh of acquisitions
     *
     * @param child The child to add
     */
    void pack(Element::Ptr child);

    /**
     * @brief Adds the Element as a new child. Causes a refresh of acquisitions
     *        Sets the expand properties of the child element
     *
     * @param child The child to add and modify
     * @param expandX Whether or not the element should expand horizontally when packing
     * @param expandY Whether or not the element should expand vertically when packing
     */
    void pack(Element::Ptr child, bool fillX, bool fillY);

protected:
    /**
     * @brief Construct a new Container with the given id, group, and Packer
     *
     * @param packer The Packer to pack child elements with
     * @param group The group of the Element
     * @param id The id of this Element
     */
    Box(Packer::Ptr packer, const std::string& group, const std::string& id);

    /**
     * @brief Returns the minimum requisition for the Container. This depends on the child
     *        elements and the Packer
     *
     * @return sf::Vector2i Minimum size required by the Container
     */
    virtual sf::Vector2i minimumRequisition() const override;

    /**
     * @brief Renders the container and all of its children in bottom up Z order
     *
     * @param target The target to render to
     * @param states Render states to apply
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

    /**
     * @brief Packs packable children into the assigned acquisition
     *
     */
    virtual void onAcquisition() override;

private:
    Packer::Ptr packer;
};

} // namespace gui
} // namespace bl

#endif