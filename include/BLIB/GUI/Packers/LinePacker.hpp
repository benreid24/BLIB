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
        Fill,    /// Each element gets an equal acquisition such that the total space is filled
        Uniform  /// Each element gets acquisition of the largest element
    };

    /**
     * @brief Which direction to pack elements. SpaceMode only applies to the dimension that is
     *        being packed in. In the other dimension elements are all set to the largest
     *        elements requisition
     *
     */
    enum Direction { Vertical, Horizontal };

    /**
     * @brief Controls if the packing starts on the top/left, or if it is aligned with the
     *        right/bottom. Leftto-right/top-to-bottom element is preserved
     *
     */
    enum PackStart {
        TopLeft,    /// Start packing at the left/top
        BottomRight /// Start packing at the right/bottom
    };

    /**
     * @brief Allocates a new LinePacker. Defaults to Compact packing
     *
     * @param dir The pack direction
     *
     */
    Packer::Ptr create(Direction dir = Horizontal, SpaceMode mode = Compact,
                       PackStart start = TopLeft);

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
    const PackStart start;

    LinePacker(Direction dir, SpaceMode mode, PackStart start);
};

} // namespace gui
} // namespace bl

#endif