#ifndef BLIB_GUI_ELEMENTS_PROGRESSBAR_HPP
#define BLIB_GUI_ELEMENTS_PROGRESSBAR_HPP

#include <BLIB/GUI/Elements/Element.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Simple progress bar
 *
 * @ingroup GUI
 *
 */
class ProgressBar : public Element {
public:
    typedef std::shared_ptr<ProgressBar> Ptr;

    virtual ~ProgressBar() = default;

    /// Direction the bar should fill in as progress is made
    enum FillDirection { LeftToRight, RightToLeft, TopToBottom, BottomToTop };

    /**
     * @brief Create a new ProgressBar
     *
     * @param fillDir The direction the bar should fill in
     * @param group The group the bar belongs to
     * @param id The id of this bar
     * @return Ptr The new ProgressBar
     */
    static Ptr create(FillDirection fillDir = LeftToRight, const std::string& group = "",
                      const std::string& id = "");

    /**
     * @brief Set the progress of the bar. In the range [0,1]
     *
     * @param progress The portion of the bar to fill
     */
    void setProgress(float progress);

    /**
     * @brief Get the progress of the bar
     *
     */
    float getProgress() const;

    /**
     * @brief Get the direction the bar is filling in
     *
     */
    FillDirection getFillDirection() const;

protected:
    /**
     * @brief Create a new ProgressBar
     *
     * @param fillDir The direction the bar should fill in
     * @param group The group the bar belongs to
     * @param id The id of this bar
     */
    ProgressBar(FillDirection fillDir, const std::string& group, const std::string& id);

    /**
     * @brief Returns (5,5)
     *
     */
    virtual sf::Vector2i minimumRequisition() const override;

    /**
     * @brief Renders the progress bar
     *
     * @param target The target to render to
     * @param states Render states to apply
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

private:
    const FillDirection direction;
    float progress;
};
} // namespace gui
} // namespace bl

#endif