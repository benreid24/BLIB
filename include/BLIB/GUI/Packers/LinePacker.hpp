#ifndef BLIB_GUI_PACKERS_LINEPACKER_HPP
#define BLIB_GUI_PACKERS_LINEPACKER_HPP

#include <BLIB/GUI/Packers/Packer.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Packer that positions Elements all in a single row or column
 *
 * @ingroup GUI
 */
class LinePacker : public Packer {
public:
    /**
     * @brief How to assign acquisitions to packed elements
     *
     */
    enum SpaceMode {
        Compact, /// Each element gets it's requisition and no more
        Fill,    /// Each element has its acquisition expanded to fill all space
        Uniform  /// Each element gets the same acquisition size, if space permits
    };

    /**
     * @brief Which direction to pack elements. SpaceMode only applies to the dimension that is
     *        being packed in. In the other dimension elements are all set to the largest
     *        elements requisition
     *
     */
    enum Direction { Vertical, Horizontal };

    /**
     * @brief Allocates a new LinePacker. Defaults to Compact packing
     *
     * @param dir The pack direction
     *
     */
    Packer::Ptr create(Direction dir, SpaceMode mode = Compact);

    /**
     * @brief Reports the minimum requisition of the element list given
     *
     * @param elements The list of Elements to consider
     * @return sf::Vector2i The minimum requisition for the list given the packing strategy
     */
    virtual sf::Vector2i getRequisition(const std::vector<Element::Ptr>& elements) override;

    /**
     * @brief Pack the elements into the assigned acquisition
     *
     * @param acquisition Region to pack into. Always at least as big as getRequisition()
     * @param elements The elements to pack
     */
    virtual void pack(const sf::IntRect& acquisition,
                      const std::vector<Element::Ptr>& elements) override;

private:
    const Direction dir;
    const SpaceMode mode;

    LinePacker(Direction dir, SpaceMode mode);
};

} // namespace gui
} // namespace bl

#endif