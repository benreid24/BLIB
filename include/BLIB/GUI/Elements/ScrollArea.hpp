#ifndef BLIB_GUI_ELEMENTS_SCROLLAREA_HPP
#define BLIB_GUI_ELEMENTS_SCROLLAREA_HPP

#include <BLIB/GUI/Elements/Container.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Similar to Box, except scrolling is possible both horizontally and vertically if the
 *        size of the elements packed exceeds the maximum allowed space
 *
 * @ingroup GUI
 * @see Box
 *
 */
class ScrollArea : public Container {
public:
    typedef std::shared_ptr<ScrollArea> Ptr;

    virtual ~ScrollArea() = default;
};
} // namespace gui
} // namespace bl

#endif