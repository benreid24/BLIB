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
 *        Container
 *
 * @ingroup GUI
 *
 */
class Packer : public bl::NonCopyable {
public:
    typedef std::shared_ptr<Packer> Ptr;

    /**
     * @brief Reports the minimum requisition of the element list given
     *
     * @param elements The list of Elements to consider
     * @return sf::Vector2f The minimum requisition for the list given the packing strategy
     */
    virtual sf::Vector2f getRequisition(const std::vector<Element::Ptr>& elements) = 0;

    /**
     * @brief Pack the elements into the assigned acquisition
     *
     * @param acquisition Region to pack into. Always at least as big as getRequisition()
     * @param elements The elements to pack
     */
    virtual void pack(const sf::IntRect& acquisition,
                      const std::vector<Element::Ptr>& elements) = 0;
};

} // namespace gui
} // namespace bl

#endif