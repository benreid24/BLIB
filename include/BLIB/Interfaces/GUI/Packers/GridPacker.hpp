#ifndef BLIB_GUI_PACKERS_GRIDPACKER_HPP
#define BLIB_GUI_PACKERS_GRIDPACKER_HPP

#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Packer that can pack elements into grids
 *
 * @ingroup GUI
 *
 */
class GridPacker : public Packer {
public:
    typedef std::shared_ptr<GridPacker> Ptr;

    /**
     * @brief Indicates whether to pack in rows or columns
     *
     */
    enum Direction {
        /// Elements are packed horizontally in rows and wrapped into multiple rows
        Rows,

        /// Elements are packed vertically in columns and wrapped into extra columns
        Columns
    };

    /**
     * @brief Creates a new GridPacker that has a fixed number of elements on each row or column
     *
     * @param dir Whether to pack into rows or columns
     * @param wrapCount Size to use to determine when to wrap elements
     * @param padding Padding between packed elements
     * @return Ptr The new GridPacker
     */
    static Ptr createFixedGrid(Direction dir, unsigned int wrapCount, float padding = 2);

    /**
     * @brief Creates a new GridPacker that has a dynamic number of elements on each row or column
     *
     * @param dir Whether to pack into rows or columns
     * @param size Size to use to determine when to wrap elements
     * @param padding Padding between packed elements
     * @return Ptr The new GridPacker
     */
    static Ptr createDynamicGrid(Direction dir, unsigned int size, float padding = 2);

    /**
     * @brief Returns the space required to pack the given list of elements
     *
     * @param elements The list of elements to pack
     * @return sf::Vector2i The space required to pack
     */
    virtual sf::Vector2f getRequisition(const std::vector<Element::Ptr>& elements) override;

    /**
     * @brief Packs the given list of elements into the given region
     *
     * @param acquisition The region to pack into
     * @param elements The elements to pack
     */
    virtual void pack(const sf::FloatRect& acquisition,
                      const std::vector<Element::Ptr>& elements) override;

    /**
     * @brief Destroy the Grid Packer
     *
     */
    virtual ~GridPacker() = default;

private:
    enum WrapRule { FixedAmount, Dynamic };

    const WrapRule rule;
    const Direction dir;
    const float padding;
    const unsigned int size;

    GridPacker(WrapRule rule, Direction dir, float padding, unsigned int s);

    void doPack(const sf::FloatRect& area, const std::vector<Element::Ptr>& elements,
                const std::function<void(Element::Ptr, const sf::FloatRect&)>& packCb);
};

} // namespace gui
} // namespace bl

#endif
