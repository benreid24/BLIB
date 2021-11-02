#ifndef BLIB_GUI_ELEMENTS_COMBOBOX_HPP
#define BLIB_GUI_ELEMENTS_COMBOBOX_HPP

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>

#include <BLIB/Interfaces/GUI/Elements/Box.hpp>
#include <BLIB/Interfaces/GUI/Elements/Canvas.hpp>
#include <BLIB/Interfaces/GUI/Elements/Label.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief A dropdown option selector. Note that fillColor is the background and
 *        secondaryFillColor is the mouseover background when opened
 *
 * @ingroup GUI
 *
 */
class ComboBox : public Element {
public:
    typedef std::shared_ptr<ComboBox> Ptr;

    static constexpr float OptionPadding = 2;

    /**
     * @brief Create a new ComboBox
     *
     * @return Ptr The new ComboBox
     */
    static Ptr create();

    /**
     * @brief Set the color of the text labels
     *
     */
    void setLabelColor(const sf::Color& color);

    /**
     * @brief Adds the option to the dropdown
     *
     * @param text Text to display
     */
    void addOption(const std::string& text);

    /**
     * @brief Removes all options
     *
     */
    void clearOptions();

    /**
     * @brief Sets the maximum height the box can fill before limiting the size and scrolling. Set
     *        to 0 for unbounded heoght
     *
     * @param height The maximum height of the dropdown
     */
    void setMaxHeight(float height);

    /**
     * @brief Returns the selected option's index. -1 means no selection
     *
     */
    int getSelectedOption() const;

    /**
     * @brief Returns the selected option's text
     *
     */
    const std::string& getSelectedOptionText() const;

    /**
     * @brief Set the selected option. -1 to clear selection
     *
     * @param i The index of the option to select
     * @param fireEvent True to fire a changed event, false to not
     *
     */
    void setSelectedOption(int i, bool fireEvent = true);

    /**
     * @brief Set the selected option by text label. Has no effect if text is not an option
     *
     * @param text The text of the option to select
     * @param fireEvent True to fire a changed event, false to not
     *
     */
    void setSelectedOption(const std::string& text, bool fireEvent = true);

    /**
     * @brief Returns whether or not the dropdown is opened
     *
     */
    bool isOpened() const;

    /**
     * @brief Returns true if open
     *
     */
    virtual bool handleScroll(const Event& scroll) override;

    /**
     * @brief Updates the combo box and its children if opened
     *
     * @param dt Elapsed time in seconds
     */
    virtual void update(float dt) override;

protected:
    /**
     * @brief Create a new ComboBox
     *
     */
    ComboBox();

    /**
     * @brief Returns the size of the largest label plus the dropdown arrow
     *
     */
    virtual sf::Vector2f minimumRequisition() const override;

    /**
     * @brief Packs the labels
     *
     */
    void onAcquisition();

    /**
     * @brief Calls Container::propagateEvent() and returns false
     *
     * @param event The event that fired
     * @return False
     */
    virtual bool propagateEvent(const Event& event) override;

    /**
     * @brief Renders the box and options
     *
     * @param target The target to render to
     * @param states Render states to apply
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

private:
    Canvas::Ptr arrow;
    std::vector<std::string> options;
    std::vector<Label::Ptr> labels;
    sf::Vector2f labelSize;
    sf::FloatRect labelRegion;
    std::optional<sf::Color> labelColor;
    float maxHeight;
    float totalHeight;
    float scroll;
    int selected; // -1 means none
    bool opened;
    mutable bool arrowRendered;

    void onSettings(); // update label settings
    void optionClicked(const std::string& text);
    void clicked();
    void scrolled(const Event& scroll);

    void packOpened();
    void packClosed();
};

} // namespace gui
} // namespace bl

#endif
