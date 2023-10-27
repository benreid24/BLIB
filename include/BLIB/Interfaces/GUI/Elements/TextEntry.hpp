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

    /// The mode a TextEntry is in
    enum struct Mode : std::uint8_t {
        Any      = 0x0,      /// Any text may be input
        Integer  = 0x1 << 0, /// Only integer values may be entered
        Float    = 0x1 << 1, /// Only numbers may be entered
        Unsigned = 0x1 << 2, /// Positive numbers only
        Signed   = 0x1 << 3, /// Positive and negative numbers allowed
    };

    /**
     * @brief Create a new TextEntry element
     *
     * @param lineCount The number of lines in the entry
     * @param allowMoreLines True to allow more lines than fits (scroll)
     * @return Ptr The new TextEntry
     */
    static Ptr create(unsigned int lineCount = 1, bool allowMoreLines = false);

    /**
     * @brief Set the mode of the input. Default is Any. Existing input is filtered based on the new
     *        mode
     *
     * @param mode The mode to be in
     */
    void setMode(Mode mode);

    /**
     * @brief Set the maximum amount of characters that can be input
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
     * @brief Returns the scrolling offset of the text within the entry
     *
     */
    const sf::Vector2f& getTextOffset() const;

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
     * @param allowMoreLines True to allow more lines than fits (scroll)
     * @return Ptr The new TextEntry
     */
    TextEntry(unsigned int lineCount, bool allowMoreLines);

    /**
     * @brief Computes the minimum area required based on the number of lines. Returns small
     *        width. Width should be set via setRequisition() or by setExpandsWidth(true)
     *
     */
    virtual sf::Vector2f minimumRequisition() const override;

    /**
     * @brief Creates the visual component for this element
     *
     * @param renderer The renderer to use to create visual Components
     * @return The visual component for this element
     */
    virtual rdr::Component* doPrepareRender(rdr::Renderer& renderer) override;

private:
    const unsigned lineCount;
    const bool allowMoreLines;
    Mode mode;
    std::optional<unsigned int> maxInputLen;
    std::string input;
    sf::Text renderText;
    sf::Vector2f textOffset;

    unsigned int cursorPos;
    bool cursorShowing;
    float cursorTime;

    std::vector<int> newlines;
    unsigned int currentLine;

    void filter();
    void recalcText();
    void recalcNewlines();
    void recalcOffset();
    void refresh();

    void cursorUp();
    void cursorDown();

    void onInput(const Event& action);
    void onKeypress(const Event& action);
    void onClicked(const Event& action);
    void fireChanged();
};

inline TextEntry::Mode operator|(TextEntry::Mode l, TextEntry::Mode r) {
    using T    = std::underlying_type_t<TextEntry::Mode>;
    const T lc = static_cast<T>(l);
    const T rc = static_cast<T>(r);
    return static_cast<TextEntry::Mode>(lc | rc);
}

inline TextEntry::Mode operator&(TextEntry::Mode l, TextEntry::Mode r) {
    using T    = std::underlying_type_t<TextEntry::Mode>;
    const T lc = static_cast<T>(l);
    const T rc = static_cast<T>(r);
    return static_cast<TextEntry::Mode>(lc & rc);
}

} // namespace gui
} // namespace bl

#endif
