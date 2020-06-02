#ifndef BLIB_GUI_PACKERS_PACKER_HPP
#define BLIB_GUI_PACKERS_PACKER_HPP

#include <BLIB/GUI/Elements/Element.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <memory>

namespace bl
{
namespace gui
{
/**
 * @brief Base class for Element packers. A Packer assigns acquisitions to Elements in a
 *        Container. Custom packers may be implemented from this interface. Care must be
 *        taken to avoid packing elements that are marked not visible. They will not be
 *        rendered but it will result in gaps that may be unwanted
 *
 * @ingroup GUI
 *
 */
class Packer : public bl::NonCopyable {
public:
    typedef std::shared_ptr<Packer> Ptr;

    virtual ~Packer() = default;

    /**
     * @brief Reports the minimum requisition of the element list given
     *
     * @param elements The list of Elements to consider
     * @return sf::Vector2i The minimum requisition for the list given the packing strategy
     */
    virtual sf::Vector2i getRequisition(const std::vector<Element::Ptr>& elements) = 0;

    /**
     * @brief Pack the elements into the assigned acquisition
     *
     * @param acquisition Region to pack into. Always at least as big as getRequisition()
     * @param elements The elements to pack
     */
    virtual void pack(const sf::IntRect& acquisition,
                      const std::vector<Element::Ptr>& elements) = 0;

    /**
     * @brief Manually pack the element. Note that the acquisition is relative to the parent
     *        element and likely does not correspond to screen coordinates
     *
     * @param element The element to pack
     * @param area The atra to pack it into
     */
    static void manuallyPackElement(Element::Ptr element, const sf::IntRect& area);

protected:
    /**
     * @brief Packs an element into the given space. Obeys Element expand properties
     *
     * @param element The element to pack
     * @param space The space to use as computed by the packer
     */
    static void packElementIntoSpace(Element::Ptr element, const sf::IntRect& space);
};

} // namespace gui
} // namespace bl

#endif