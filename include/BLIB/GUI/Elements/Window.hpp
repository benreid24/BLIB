#ifndef BLIB_GUI_ELEMENTS_WINDOW_HPP
#define BLIB_GUI_ELEMENTS_WINDOW_HPP

#include <BLIB/GUI/Elements/Container.hpp>
#include <BLIB/GUI/Elements/Label.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Element that represents a window in a GUI. Window elements are the top level element
 *        of any GUI. Windows can be moveable or fixed and are highly configurable.
 *
 * @ingroup GUI
 */
class Window : public Container {
public:
    typedef std::shared_ptr<Window> Ptr;

    /**
     * @brief Set of possible style options for a window. Also controls the behavior of the
     *        window
     *
     */
    enum Style {
        Titlebar    = 1 << 1, /// Set if the window should have a title bar
        CloseButton = 1 << 2, /// Set if the window should have a close button in the corner
        Borderless  = 1 << 3, /// Set if the window should not render a border
        Moveable    = 1 << 4, /// Set if the window can be moved by dragging
        Default     = Titlebar | CloseButton | Moveable /// Default style
    };

    /**
     * @brief Construct a new window that is sized adaptively. To set a fixed size use
     *        setSize()
     *
     * @param packer The packer to use on the window's elements
     * @param title Title of the window. Only has effect if style contains Titlebar
     * @param style The style to render with
     * @param group Group the window belongs to
     * @param id The id of this window
     */
    Window(Packer::Ptr packer, const std::string& title = "", Style style = Default,
           const std::string& group = "", const std::string& id = "");

    /**
     * @brief Destroy the Window object
     *
     */
    virtual ~Window() = default;

    /**
     * @brief Calls Container::update, handles reassigning acquisitions if moved, and handles
     *        the close button being clicked
     *
     */
    virtual void update(float dt) override;

    /**
     * @brief Returns if the window has a border or not
     *
     */
    bool borderless() const;

    /**
     * @brief Returns false. Windows handle their own positioning and do not need to be packed
     *
     */
    virtual bool packable() const override;

    // TODO - expose title, titlebar, close button for styling

protected:
    /**
     * @brief Renders the window and children elements to the given target
     *
     * @param target The target to render to
     * @param states Render states to use
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          Renderer::Ptr renderer) const override;

private:
    const bool hasBorder;
    Container::Ptr titlebar;
    Container::Ptr leftTitleSide, rightTitleSide;
    Label::Ptr title;
    // TODO - close button

    const bool moveable;
    std::optional<sf::Vector2i> dragAmount;

    void handleDrag(const Action& drag, Element*);
};

} // namespace gui
} // namespace bl

#endif