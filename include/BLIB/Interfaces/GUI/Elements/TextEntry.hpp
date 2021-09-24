#ifndef BLIB_GUI_ELEMENTS_TEXTENTRY_HPP
#define BLIB_GUI_ELEMENTS_TEXTENTRY_HPP

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>

#include <SFML/System/String.hpp>
#include <list>

namespace bl
{
namespace gui
{
/**
 * @brief Single or multiline text entry Element. Note that the text color is the secondary
 *        color in render settings. The box color is the primary
 *
 */
class TextEntry : public Element {
public:
    typedef std::shared_ptr<TextEntry> Ptr;

    static constexpr float CursorFlashPeriod           = 0.75;
    static constexpr unsigned int DefaultCharacterSize = 14;

    /**
     * @brief Create a new TextEntry element
     *
     * @param lineCount The number of lines in the entry
     * @return Ptr The new TextEntry
     */
    static Ptr create(unsigned int lineCount = 1);

    /**
     * @brief Set the maximum amount of characters that can be input. Has no affect if
     *
     * @param maxLength The number of characters that can be input
     */
    void setMaxInputLength(unsigned int maxLength);

    /**
     * @brief Returns the current input
     *
     */
    const std::string& getInput() const;

    /**
     * @brief Set the text in the entry
     *
     * @param input The text to set
     */
    void setInput(const std::string& input);

    /**
     * @brief Returns whether or not the cursor is visible
     *
     * @return true
     * @return false
     */
    bool cursorVisible() const;

    /**
     * @brief Returns the position of the cursor, in characters
     *
     */
    unsigned int getCursorPosition() const;

    /**
     * @brief Manages cursor visibility based on elapsed time and active status
     *
     * @param dt Time elapsed since last update, in seconds
     */
    virtual void update(float dt) override;

protected:
    /**
     * @brief Create a new TextEntry element
     *
     * @param lineCount The number of lines in the entry
     * @return Ptr The new TextEntry
     */
    TextEntry(unsigned int lineCount);

    /**
     * @brief Computes the minimum area required based on the number of lines. Returns small
     *        width. Width should be set via setRequisition() or by setExpandsWidth(true)
     *
     */
    virtual sf::Vector2i minimumRequisition() const override;

    /**
     * @brief Renders the text entry
     *
     * @param target Target to render to
     * @param states RenderStates to apply
     * @param renderer Renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

private:
    const unsigned lineCount;
    std::optional<unsigned int> maxInputLen;
    std::string input;
    sf::Text renderText;

    unsigned int cursorPos;
    bool cursorShowing;
    float cursorTime;

    std::vector<int> newlines;
    unsigned int currentLine;

    void recalcText();
    void recalcNewlines();

    void cursorUp();
    void cursorDown();

    void onInput(const Event& action);
    void onKeypress(const Event& action);
    void onClicked(const Event& action);
};

} // namespace gui
} // namespace bl

#endif