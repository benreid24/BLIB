#ifndef BLIB_INPUT_ACTOR_HPP
#define BLIB_INPUT_ACTOR_HPP

#include <BLIB/Input/Control.hpp>
#include <BLIB/Input/Listener.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <stack>
#include <vector>

namespace bl
{
namespace input
{
class InputSystem;

/**
 * @brief Represents a single actor (player) that can give input. Listeners subscribe to a single
 *        actor for their input. Each actor has it's own control set and scheme
 *
 * @ingroup Input
 *
 */
class Actor : private util::NonCopyable {
public:
    /**
     * @brief Destroy the Actor object
     *
     */
    ~Actor();

    /**
     * @brief Sets which joystick this actor will receive input from. If this is not called then the
     *        Actor will listen to all joysticks
     *
     * @param joystickId The joystick index from SFML
     */
    void assignJoystick(unsigned int joystickId);

    /**
     * @brief Returns a reference to the given trigger control
     *
     * @param controlIndex The trigger control to get. In range [0, controlCount)
     */
    Trigger& getTriggerControl(unsigned int controlIndex);

    /**
     * @brief Returns a reference to the given movement control
     *
     * @param controlIndex The movement control to get. In range [0, controlCount)
     */
    MovementControl& getMovementControl(unsigned int controlIndex);

    /**
     * @brief Returns a reference to the given directional control
     *
     * @param controlIndex The directional control to get. In range [0, controlCount)
     */
    DirectionalControl& getDirectionalControl(unsigned int controlIndex);

    /**
     * @brief Returns a reference to the given trigger control
     *
     * @param controlIndex The trigger control to get. In range [0, controlCount)
     */
    const Trigger& getTriggerControl(unsigned int controlIndex) const;

    /**
     * @brief Returns a reference to the given movement control
     *
     * @param controlIndex The movement control to get. In range [0, controlCount)
     */
    const MovementControl& getMovementControl(unsigned int controlIndex) const;

    /**
     * @brief Returns a reference to the given directional control
     *
     * @param controlIndex The directional control to get. In range [0, controlCount)
     */
    const DirectionalControl& getDirectionalControl(unsigned int controlIndex) const;

    /**
     * @brief Returns whether or not the given control is active. Works only for trigger controls
     *
     * @param controlIndex The control to get. In range [0, controlCount)
     * @return True if the control is active, false if inactive
     */
    bool controlActive(unsigned int controlIndex) const;

    /**
     * @brief Reads the given directional or movement control
     *
     * @param controlIndex The control to get. In range [0, controlCount)
     * @return sf::Vector2f The control value. Directional controls will be normalized
     */
    sf::Vector2f readControl(unsigned int controlIndex) const;

    /**
     * @brief Adds a new input listener
     *
     * @param listener The listener to make active
     */
    void addListener(input::Listener& listener);

    /**
     * @brief Removes the given listener. Does not have to be the active listener
     *
     * @param listener The input listener to remove
     */
    void removeListener(input::Listener& listener);

    /**
     * @brief Replaces the old listener with the new, keeping it at the same position in the queue
     *
     * @param oldListener The listener to replace
     * @param newListener The listener to take its place
     */
    void replaceListener(input::Listener& oldListener, input::Listener& newListener);

private:
    InputSystem& owner;
    unsigned int joystick;
    std::vector<Control> controls;
    std::vector<Listener*> listeners;

    void dispatch(unsigned int ctrl, DispatchType dtype, bool onEvent);

    // meant to be called by InputSystem
    Actor(InputSystem& owner, const std::vector<Control::Type>& schema);
    void process(const sf::Event& event);
    void update();

    friend class InputSystem;
};

} // namespace input
} // namespace bl

#endif
