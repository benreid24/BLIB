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
 *        settings changed here will take precedance over any overrides the Renderer has set,
 *        even id level overrides
 *
 * @ingroup GUI
 *
 */
class Label : public Element {
public:
    typedef std::shared_ptr<Label> Ptr;

    static constexpr int DefaultFontSize = 20;

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
     * @brief Render the Label to the given target
     *
     * @param target The target to render to
     * @param states Render states to use
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

private:
    std::string text;
    sf::Text renderText;

    void settingsChanged();
};

} // namespace gui
} // namespace bl

#endif
