#ifndef BLIB_GUI_ELEMENTS_SLIDER_HPP
#define BLIB_GUI_ELEMENTS_SLIDER_HPP

#include <BLIB/GUI/Elements/Element.hpp>

#include <BLIB/GUI/Elements/Button.hpp>
#include <BLIB/GUI/Elements/Container.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief Slidable button element. Can be used as an input. Used by ScrollArea
 *
 * @ingroup GUI
 *
 */
class Slider : public Element {
public:
    typedef std::shared_ptr<Slider> Ptr;

    /**
     * @brief Which direction the slider should move
     *
     */
    enum Direction { Vertical, Horizontal };

    /**
     * @brief Create a new slider
     *
     * @param dir The direction to slide in
     * @param group The group the slider is in
     * @param id The id of the slider
     * @return Ptr The new slider
     */
    static Ptr create(Direction dir, const std::string& group = "",
                      const std::string& id = "");

    /**
     * @brief Returns the position of the slider, normalized to [0,1]
     *
     * @return float Normalized position. 0 is top or left position
     */
    float getValue() const;

    /**
     * @brief Set the position of the slider
     *
     * @param value The normlized position in the range [0,1]
     */
    void setValue(float value);

    /**
     * @brief Set the size of the slider button relative to the full size of the area it moves
     *        in. A value of 0 is undefined. A value of 1 allows for no movement
     *
     * @param size The relative size of the slider button in the range (0,1]
     */
    void setSliderSize(float size);

    /**
     * @brief Set the amount the slider should move, in the (0,1] range, when either the
     *        increase or decrease buttons are pressed
     *
     * @param inc
     */
    void setSliderIncrement(float inc);

    /**
     * @brief Returns the slider button to be styled and customized
     *
     */
    Button::Ptr getSlider();

    /**
     * @brief Returns the decrease button to be styled and customized
     *
     */
    Button::Ptr getIncreaseButton();

    /**
     * @brief Returns the decrease button to be styled and customized
     *
     */
    Button::Ptr getDecreaseButton();

protected:
    /**
     * @brief Create a new slider
     *
     * @param dir The direction to slide in
     * @param group The group the slider is in
     * @param id The id of the slider
     */
    Slider(Direction dir, const std::string& group, const std::string& id);

    /**
     * @brief Returns the space required by the slider button and the others if visible
     *
     */
    virtual sf::Vector2i minimumRequisition() const override;

    /**
     * @brief Passes the raw event down to the owned buttons
     *
     * @param event The raw event
     * @return True if the event is consumed and no more Elements should be notified
     */
    virtual bool handleRawEvent(const RawEvent& event) override;

    /**
     * @brief Renders the slider and buttons
     *
     * @param target The target to render to
     * @param states Render states to apply
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          Renderer::Ptr renderer) const override;

private:
    const Direction dir;
    float buttonSize;
    float value;
    float increment;
    Button::Ptr increaseBut; // TODO - switch to image buttton or smth
    Button::Ptr decreaseBut;
    Button::Ptr slider;

    unsigned int calculateFreeSize() const;
    void packElements();

    void sliderMoved(const Action& drag);
    void decreaseClicked();
    void increaseClicked();
};

} // namespace gui
} // namespace bl

#endif