#ifndef BLIB_INTERFACES_GUI_ELEMENTS_SELECTBOX_HPP
#define BLIB_INTERFACES_GUI_ELEMENTS_SELECTBOX_HPP

#include <BLIB/Interfaces/GUI/Elements/CompositeElement.hpp>
#include <BLIB/Interfaces/GUI/Elements/Element.hpp>
#include <BLIB/Interfaces/GUI/Elements/Label.hpp>
#include <BLIB/Interfaces/GUI/Elements/ScrollArea.hpp>
#include <optional>
#include <vector>

namespace bl
{
namespace gui
{
/**
 * @brief A scrollable element that packs labels and allows the selection of one
 *
 * @ingroup GUI
 *
 */
class SelectBox : public CompositeElement<1> {
public:
    using Ptr = std::shared_ptr<SelectBox>;

    /**
     * @brief Creates a new SelectBox
     *
     * @return Ptr The new SelectBox
     */
    static Ptr create();

    /**
     * @brief Adds an option to the select box
     *
     * @param option Text of the option to add
     */
    void addOption(const std::string& option);

    /**
     * @brief Removes the given option by its text value
     *
     * @param option The text of the option to remove
     */
    void removeOption(const std::string& option);

    /**
     * @brief Removes the option with the given index
     *
     * @param i The index of the option to remove
     */
    void removeOption(unsigned int i);

    /**
     * @brief Removes all options
     *
     */
    void clearOptions();

    /**
     * @brief Returns the index of the selected option, or no value if no selection is present
     *
     * @return std::optional<unsigned int> The index of the selected option, if any
     */
    std::optional<unsigned int> getSelectedOption() const;

    /**
     * @brief Set the selected option by its text value
     *
     * @param option The text of the option to select
     */
    void setSelectedOption(const std::string& option);

    /**
     * @brief Set the selected option by its index
     *
     * @param option The index of the option to select
     */
    void setSelectedOption(unsigned int i);

    /**
     * @brief Clears any active selection
     *
     */
    void removeSelection();

    /**
     * @brief Set the maximum size to grow to before scrolling
     *
     * @param size The maximum size in pixels
     */
    void setMaxSize(const sf::Vector2f& size);

private:
    ScrollArea::Ptr content;
    std::vector<std::pair<Box::Ptr, Label::Ptr>> values;
    unsigned int selected;

    SelectBox();

    virtual bool propagateEvent(const Event& e) override;
    virtual void onAcquisition() override;
    virtual sf::Vector2f minimumRequisition() const override;
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;
    void onLabelClick(const Event&, Element* label);
};

} // namespace gui
} // namespace bl

#endif
