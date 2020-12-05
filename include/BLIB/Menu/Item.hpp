#ifndef BLIB_MENU_ITEM_HPP
#define BLIB_MENU_ITEM_HPP

#include <BLIB/Menu/RenderItem.hpp>
#include <memory>

namespace bl
{
namespace menu
{
/**
 * @brief Item for a mouseless menu. Can be 'attached' to other items in a Menu,
 *        configured to be selectable, and can call a callback if selected.
 * 
 * @ingroup Menu
 * 
 */
class Item {
public:
    typedef std::shared_ptr<Item> Ptr;

    /**
     * @brief Points where other items can be attached
     *
     */
    enum AttachPoints { Top, Right, Bottom, Left, _NUM };

private:
    Ptr attachments[_NUM];
};

} // namespace menu
} // namespace bl

#endif