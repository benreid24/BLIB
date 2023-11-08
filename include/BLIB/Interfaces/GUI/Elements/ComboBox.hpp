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

    static constexpr float OptionPadding = 2.f;

    /**
     * @brief Create a new ComboBox
     *
     * @return Ptr The new ComboBox
     */
    static Ptr create();

    /**
     * @brief Adds the option to the dropdown
     *
     * @param text The text to display as an option
     */
    void addOption(const std::string& text);

    /**
     * @brief Removes all options
     */
    void clearOptions();

    /**
     * @brief Sets the maximum height the box can fill before limiting the size and scrolling. Set
     *        to 0 for unbounded height
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
     * @brief Returns the number of options in the combo box
     */
    int optionCount() const;

    /**
     * @brief Returns whether or not the dropdown is opened
     */
    bool isOpened() const;

    /**
     * @brief Returns true if open
     */
    virtual bool handleScroll(const Event& scroll) override;

    /**
     * @brief Returns whether or not this element should receive events that occurred outside the
     *        acquisition of its parent
     *
     * @return True if it should take outside events, false for contained only
     */
    virtual bool receivesOutOfBoundsEvents() const override;

    /**
     * @brief Returns the index of the currently moused-over option, -1 if none
     */
    int getMousedOption() const { return moused; }

    /**
     * @brief Returns the size of the options in the dropdown
     */
    const sf::Vector2f& getOptionSize() const { return labelSize; }

    /**
     * @brief Returns the acquisition of the space where options are rendered, constrained by the
     *        max size
     */
    const sf::FloatRect& getOptionRegion() const { return labelRegion; }

    /**
     * @brief Returns the current amount of scrolling
     */
    float getScroll() const { return scroll; }

protected:
    /**
     * @brief Create a new ComboBox
     */
    ComboBox();

    /**
     * @brief Returns the size of the largest label plus the dropdown arrow
     */
    virtual sf::Vector2f minimumRequisition() const override;

    /**
     * @brief Packs the labels
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
     * @brief Creates the visual component for this element
     *
     * @param renderer The renderer to use to create visual Components
     * @return The visual component for this element
     */
    virtual rdr::Component* doPrepareRender(rdr::Renderer& renderer) override;

private:
    std::vector<std::string> options;
    sf::Vector2f labelSize;
    sf::FloatRect labelRegion;
    std::optional<sf::Color> labelColor;
    float maxHeight;
    float totalHeight;
    float scroll;
    int selected; // -1 means none
    int moused;
    bool opened;

    void optionClicked(const std::string& text);
    void clicked();
    void scrolled(const Event& scroll);
    void refreshLabelRegion();
};

} // namespace gui
} // namespace bl

#endif
