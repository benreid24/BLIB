#ifndef BLIB_GUI_ELEMENTS_COMBOBOX_HPP
#define BLIB_GUI_ELEMENTS_COMBOBOX_HPP

#include <BLIB/GUI/Elements/Container.hpp>

#include <BLIB/GUI/Elements/Canvas.hpp>
#include <BLIB/GUI/Elements/Label.hpp>

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
class ComboBox : public Container {
public:
    typedef std::shared_ptr<ComboBox> Ptr;

    /**
     * @brief Create a new ComboBox
     *
     * @param group The group the box is in
     * @param id The id of this box
     * @return Ptr
     */
    static Ptr create(const std::string& group = "", const std::string& id = "");

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
     */
    void setSelectedOption(int i);

    /**
     * @brief Set the selected option by text label. Has no effect if text is not an option
     *
     */
    void setSelectedOption(const std::string& text);

    /**
     * @brief Returns whether or not the dropdown is opened
     *
     */
    bool isOpened() const;

protected:
    /**
     * @brief Create a new ComboBox
     *
     * @param group The group the box is in
     * @param id The id of this box
     */
    ComboBox(const std::string& group, const std::string& id);

    /**
     * @brief Returns the size of the largest label plus the dropdown arrow
     *
     */
    virtual sf::Vector2i minimumRequisition() const override;

    /**
     * @brief Packs the labels
     *
     */
    virtual void onAcquisition() override;

    /**
     * @brief Calls Container::handleRawEvent() and returns false
     *
     * @param event The event that fired
     * @return False
     */
    virtual bool handleRawEvent(const RawEvent& event) override;

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
    sf::Vector2i labelSize;
    std::optional<sf::Color> labelColor;
    int selected; // -1 means none
    bool opened;
    mutable bool arrowRendered;

    void onSettings(); // update label settings
    void optionClicked(std::string text);
    void clicked();

    void packOpened();
    void packClosed();

    void addChildren();
};
} // namespace gui
} // namespace bl

#endif