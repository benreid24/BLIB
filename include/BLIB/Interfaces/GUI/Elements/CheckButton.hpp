#ifndef BLIB_GUI_ELEMENTS_CHECKBUTTON_HPP
#define BLIB_GUI_ELEMENTS_CHECKBUTTON_HPP

#include <BLIB/Interfaces/GUI/Elements/ToggleButton.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Checkbox button
 *
 * @ingroup GUI
 *
 */
class CheckButton : public ToggleButton {
public:
    typedef std::shared_ptr<CheckButton> Ptr;

    /**
     * @brief Create a new CheckButton with a Label as it's child
     *
     * @param text The text to put to the right of the check box
     * @return Ptr The new button
     */
    static Ptr create(const std::string& text);

    /**
     * @brief Create a new CheckButton with any child to its right
     *
     * @param child The element to put to the right of the check box
     * @return Ptr The new button
     */
    static Ptr create(const Element::Ptr& child);

protected:
    /**
     * @brief Create a new CheckButton with any child to its right
     *
     * @param child The element to put to the right of the check box
     */
    CheckButton(const Element::Ptr& child);

    /**
     * @brief Renders the unchecked and checked box
     *
     * @param activeBut The canvas to render the checked box to
     * @param inactiveBut The canvas to render the unchecked box to
     * @param renderer The renderer to use to do it
     */
    virtual void renderToggles(Canvas& activeBut, Canvas& inactiveBut,
                               const Renderer& renderer) const override;
};

} // namespace gui
} // namespace bl

#endif
