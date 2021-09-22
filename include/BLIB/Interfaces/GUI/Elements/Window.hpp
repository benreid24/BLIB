#ifndef BLIB_GUI_ELEMENTS_WINDOW_HPP
#define BLIB_GUI_ELEMENTS_WINDOW_HPP

#include <BLIB/Interfaces/GUI/Elements/Box.hpp>
#include <BLIB/Interfaces/GUI/Elements/Button.hpp>
#include <BLIB/Interfaces/GUI/Elements/Container.hpp>
#include <BLIB/Interfaces/GUI/Elements/Label.hpp>

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
     *        setRequisition(). Note that the requisition includes the titlebar
     *
     * @param packer The packer to use on the window's elements
     * @param title Title of the window. Only has effect if style contains Titlebar
     * @param style The style to render with
     * @param position The position to create the window at
     */
    static Ptr create(Packer::Ptr packer, const std::string& title = "", Style style = Default,
                      const sf::Vector2i& position = {250, 150});

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
     * @brief Pack the given element into the windows main area
     *
     * @param e The element to pack
     */
    void pack(Element::Ptr e);

    /**
     * @brief Pack the given element into the windows main area
     *
     * @param e The element to pack
     * @param fillX True for the element to expand into all available width
     * @param fillY True for the element to expand into all available height
     */
    void pack(Element::Ptr e, bool fillX, bool fillY);

    /**
     * @brief Returns a Ptr to the title label. May be null if there is no titlebar
     *
     */
    Label::Ptr getTitleLabel();

    /**
     * @brief Returns a Ptr to the titlebar box. May be null if there is no titlebar
     *
     */
    Box::Ptr getTitlebar();

    /**
     * @brief Return a Ptr to the box that has all of the child elements
     *
     * @return Box::Ptr
     */
    Box::Ptr getElementArea();

    /**
     * @brief Returns a Ptr to the close button. May be null if there is no titlebar or close
     *        button
     *
     */
    Button::Ptr getCloseButton();

    /**
     * @brief Assigns its own acquisition and calls Container::update
     *
     * @param dt Time elapsed since last update, in seconds
     */
    virtual void update(float dt) override;

protected:
    /**
     * @brief Construct a new window that is sized adaptively. To set a fixed size use
     *        setSize()
     *
     * @param packer The packer to use on the window's elements
     * @param title Title of the window. Only has effect if style contains Titlebar
     * @param style The style to render with
     * @param position The position to create the window at
     */
    Window(Packer::Ptr packer, const std::string& title, Style style, const sf::Vector2i& position);

    /**
     * @brief Returns size required by children elements plus the titlebar
     *
     */
    virtual sf::Vector2i minimumRequisition() const override;

    /**
     * @brief Returns false. Windows handle their own positioning and do not need to be packed
     *
     */
    virtual bool shouldPack() const override;

    /**
     * @brief Resets the acquisition and repacks all elements
     *
     */
    virtual void onAcquisition() override;

    /**
     * @brief Renders the window and children elements to the given target
     *
     * @param target The target to render to
     * @param states Render states to use
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

    /**
     * @brief Returns true to prevent pass through
     *
     */
    virtual bool handleScroll(const RawEvent& scroll) override;

private:
    const bool moveable;
    unsigned int titlebarHeight;
    Box::Ptr titlebar;
    Box::Ptr leftTitleSide, rightTitleSide;
    Box::Ptr elementArea;
    Label::Ptr title;
    Button::Ptr closeButton;

    void handleDrag(const Action& drag);
    void closed();
    void titleActive();

    int computeTitleHeight() const;
    int computeTitleWidth() const;
    void addChildren();
};

} // namespace gui
} // namespace bl

#endif