#ifndef BLIB_GUI_ELEMENTS_LABEL_HPP
#define BLIB_GUI_ELEMENTS_LABEL_HPP

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>
#include <BLIB/Resources/Loader.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Display only text element. Used to display text in a GUI. Note that any render
 *        settings changed here will take precedence over any overrides the Renderer has set,
 *        even id level overrides
 *
 * @ingroup GUI
 */
class Label : public Element {
public:
    typedef std::shared_ptr<Label> Ptr;

    static constexpr int DefaultFontSize = 20;

    /// Enum representing the different wrapping modes for label text
    enum TextWrapBehavior { WrapDisabled, WrapToAcquisition, WrapFixedWidth };

    /**
     * @brief Construct a new Label
     *
     * @param text The text to display
     */
    static Ptr create(const std::string& text);

    /**
     * @brief Destroy the Label object
     *
     */
    virtual ~Label() = default;

    /**
     * @brief Set the text displayed in the label
     *
     */
    void setText(const std::string& text);

    /**
     * @brief Returns the currently displayed text
     *
     */
    const std::string& getText() const;

    /**
     * @brief Set the text wrapping behavior for the label
     *
     * @param behavior The text wrapping behavior
     * @param width The width to wrap to. Only used for WrapFixedWidth
     */
    void setTextWrapping(TextWrapBehavior behavior, float width = -1.f);

    /**
     * @brief Returns the text wrapping behavior of the label
     */
    TextWrapBehavior getTextWrapBehavior() const;

    /**
     * @brief Returns the wrapping width
     */
    float getTextWrapWidth() const;

    /**
     * @brief Performs update logic on the Label
     *
     * @param dt Elapsed time in seconds
     */
    virtual void update(float dt) override;

protected:
    /**
     * @brief Construct a new Label
     *
     * @param text The text to display
     */
    Label(const std::string& text);

    /**
     * @brief Returns the minimum space required to render the label
     *
     * @return sf::Vector2i Space required
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
    std::string text;
    TextWrapBehavior wrapBehavior;
    float wrapWidth;

    void settingsChanged();
};

} // namespace gui
} // namespace bl

#endif
