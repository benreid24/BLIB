#ifndef BLIB_GUI_ELEMENTS_WINDOW_HPP
#define BLIB_GUI_ELEMENTS_WINDOW_HPP

#include <BLIB/GUI/Elements/Button.hpp>
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
        Titlebar    = 1 << 0, /// Set if the window should have a title bar
        CloseButton = 1 << 1, /// Set if the window should have a close button in the corner
        Moveable    = 1 << 2, /// Set if the window can be moved by dragging
        Default     = Titlebar | CloseButton | Moveable /// Default style
    };

    /**
     * @brief Construct a new window that is sized adaptively. To set a fixed size use
     *        setSize()
     *
     * @param packer The packer to use on the window's elements
     * @param title Title of the window. Only has effect if style contains Titlebar
     * @param style The style to render with
     * @param position The position to create the window at
     * @param group Group the window belongs to
     * @param id The id of this window
     */
    static Ptr create(Packer::Ptr packer, const std::string& title = "", Style style = Default,
                      const sf::Vector2i& position = {30, 30}, const std::string& group = "",
                      const std::string& id = "");

    /**
     * @brief Destroy the Window object
     *
     */
    virtual ~Window() = default;

    /**
     * @brief Set the height of the titlebar. Default is 22
     *
     * @param height Height of the titlebar, in pixels
     */
    void setTitlebarHeight(unsigned int height);

    /**
     * @brief Set the position of the window
     *
     * @param pos The position to put the window at
     */
    void setPosition(const sf::Vector2i& pos);

    /**
     * @brief Returns false. Windows handle their own positioning and do not need to be packed
     *
     */
    virtual bool packable() const override;

    /**
     * @brief Returns a Ptr to the title label. May be null if there is no titlebar
     *
     */
    Label::Ptr getTitleLabel();

    /**
     * @brief Returns a Ptr to the titlebar container. May be null if there is no titlebar
     *
     */
    Container::Ptr getTitlebar();

    /**
     * @brief Returns a Ptr to the close button. May be null if there is no titlebar or close
     *        button
     *
     */
    Button::Ptr getCloseButton();

protected:
    /**
     * @brief Construct a new window that is sized adaptively. To set a fixed size use
     *        setSize()
     *
     * @param packer The packer to use on the window's elements
     * @param title Title of the window. Only has effect if style contains Titlebar
     * @param style The style to render with
     * @param position The position to create the window at
     * @param group Group the window belongs to
     * @param id The id of this window
     */
    Window(Packer::Ptr packer, const std::string& title, Style style,
           const sf::Vector2i& position, const std::string& group, const std::string& id);

    /**
     * @brief Returns size required by children elements plus the titlebar
     *
     */
    virtual sf::Vector2i minimumRequisition() const override;

    /**
     * @brief Resets the acquisition and repacks all elements
     *
     */
    virtual void makeClean() override;

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
    const bool moveable;
    unsigned int titlebarHeight;
    Container::Ptr titlebar;
    Container::Ptr leftTitleSide, rightTitleSide;
    Label::Ptr title;
    Button::Ptr closeButton;

    void handleDrag(const Action& drag);
    void closed();
    void onAcquisition();
    void titleActive();

    void addTitlebar();
};

} // namespace gui
} // namespace bl

#endif