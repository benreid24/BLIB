#ifndef BLIB_GUI_ELEMENTS_SLIDER_HPP
#define BLIB_GUI_ELEMENTS_SLIDER_HPP

#include <BLIB/Interfaces/GUI/Elements/Button.hpp>
#include <BLIB/Interfaces/GUI/Elements/Canvas.hpp>
#include <BLIB/Interfaces/GUI/Elements/CompositeElement.hpp>

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
class Slider : public CompositeElement<3> {
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
     * @return Ptr The new slider
     */
    static Ptr create(Direction dir);

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
     * @param fireEvent True to fire the changed event, false to not
     */
    void setValue(float value, bool fireEvent = true);

    /**
     * @brief Increase or decrease the value by the given number of increments
     *
     */
    void incrementValue(float increments);

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
     */
    Slider(Direction dir);

    /**
     * @brief Returns the space required by the slider button and the others if visible
     *
     */
    virtual sf::Vector2f minimumRequisition() const override;

    /**
     * @brief Renders the slider and buttons
     *
     * @param target The target to render to
     * @param states Render states to apply
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

    /**
     * @brief Handles the scroll and returns true
     *
     * @param scroll The scroll event
     * @return True
     */
    virtual bool handleScroll(const Event& scroll) override;

    /**
     * @brief Propagates the given event to the child elements
     *
     * @param event The event to propagate
     * @return True if the event was handled, false if unhandled
     */
    virtual bool propagateEvent(const Event& event) override;

    /**
     * @brief Packs the child elements when the acquisition changes
     *
     */
    virtual void onAcquisition() override;

private:
    const Direction dir;
    float sliderSizeRatio;
    float value;
    float increment;
    float sliderSize;
    float freeSpace;
    Canvas::Ptr increaseImg;
    Button::Ptr increaseBut;
    Canvas::Ptr decreaseImg;
    Button::Ptr decreaseBut;
    Button::Ptr slider;
    mutable bool renderedButs;

    int calculateFreeSize() const;

    void valueChanged();
    void fireChanged();
    void updateSliderPos();
    void constrainValue();

    void sliderMoved(const Event& drag);
    void clicked(const Event& click);
};

} // namespace gui
} // namespace bl

#endif
