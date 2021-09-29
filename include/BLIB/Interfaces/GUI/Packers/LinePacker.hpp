#ifndef BLIB_GUI_PACKERS_LINEPACKER_HPP
#define BLIB_GUI_PACKERS_LINEPACKER_HPP

#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>

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
        Uniform, /// Each element gets an equal acquisition such that the total space is filled
    };

    /**
     * @brief Which direction to pack elements
     *
     */
    enum Direction { Vertical, Horizontal };

    /**
     * @brief Controls if the packing starts on the top/left, or if it is aligned with the
     *        right/bottom. Left-to-right/top-to-bottom element order is preserved
     *
     */
    enum PackStart {
        LeftAlign,               /// Start packing at the left
        TopAlign = LeftAlign,    /// Start packing at the top
        RightAlign,              /// Start packing at the right
        BottomAlign = RightAlign /// Start packing at the bottom
    };

    /**
     * @brief Allocates a new LinePacker. Defaults to Compact packing
     *
     * @param dir The pack direction
     * @param spacing The amount of pixels to put between each element
     * @param mode The mode to pack with
     * @param start Which side to pack from
     *
     */
    static Packer::Ptr create(Direction dir = Horizontal, float spacing = 2,
                              SpaceMode mode = Compact, PackStart start = LeftAlign);

    /**
     * @brief Reports the minimum requisition of the element list given
     *
     * @param elements The list of Elements to consider
     * @return sf::Vector2i The minimum requisition for the list given the packing strategy
     */
    virtual sf::Vector2f getRequisition(const std::vector<Element::Ptr>& elements) override;

    /**
     * @brief Pack the elements into the assigned acquisition
     *
     * @param acquisition Region to pack into. Always at least as big as getRequisition()
     * @param elements The elements to pack
     */
    virtual void pack(const sf::FloatRect& acquisition,
                      const std::vector<Element::Ptr>& elements) override;

    /**
     * @brief Destroy the Line Packer
     *
     */
    virtual ~LinePacker() = default;

private:
    const Direction dir;
    const SpaceMode mode;
    const PackStart start;
    const float spacing;

    LinePacker(Direction dir, int spacing, SpaceMode mode, PackStart start);
};

} // namespace gui
} // namespace bl

#endif
