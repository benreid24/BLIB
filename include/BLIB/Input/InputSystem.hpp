#ifndef BLIB_INPUT_INPUTSYSTEM_HPP
#define BLIB_INPUT_INPUTSYSTEM_HPP

#include <BLIB/Events/Listener.hpp>
#include <BLIB/Input/Actor.hpp>

namespace bl
{
namespace engine
{
class Engine;
}
namespace input
{
/**
 * @brief System for managing complex input workflows and maintaining a stack of subscribers to
 *        input based events
 *
 * @ingroup Input
 *
 */
class InputSystem : public event::Listener<sf::Event> {
public:
    /**
     * @brief Construct a new Input System
     *
     * @param engine The game engine the input system belongs to
     */
    InputSystem(engine::Engine& engine);

    /**
     * @brief Set how many controls there will be. This is intended to be called once before adding
     *        any actors
     *
     * @param controlCount The number of controls to create within each actor
     */
    void setControlCount(unsigned int controlCount);

    /**
     * @brief Configures the given control to a trigger control with the given policy
     *
     * @param controlIndex The control to make a trigger control
     */
    void configureTriggerControl(unsigned int controlIndex);

    /**
     * @brief Configures the given control to a directional control
     *
     * @param controlIndex The control to make a directional control
     */
    void configureDirectionalControl(unsigned int controlIndex);

    /**
     * @brief Configures the given control to a movement control
     *
     * @param controlIndex The control to make a movement control
     */
    void configureMovementControl(unsigned int controlIndex);

    /**
     * @brief Adds a new Actor to the system. setControlCount MUST be called prior to this
     *
     * @return Actor& The newly added actor
     */
    Actor& addActor();

    /**
     * @brief Get the given Actor
     *
     * @param i The index of the actor to get
     * @return Actor& The actor at the given index
     */
    Actor& getActor(unsigned int i = 0);

    /**
     * @brief Get the given Actor
     *
     * @param i The index of the actor to get
     * @return const Actor& The actor at the given index
     */
    const Actor& getActor(unsigned int i = 0) const;

    /**
     * @brief Removes the actor at the given index
     *
     * @param i The index of the actor to remove
     */
    void removeActor(unsigned int i);

    /**
     * @brief Returns the number of actors in the system
     *
     */
    unsigned int actorCount() const;

    /**
     * @brief Returns a normalized vector representing the direction of the mouse from the window
     *        center. Does not account for the window viewport or letterboxing
     *
     */
    const sf::Vector2f& mouseUnitVector() const;

    /**
     * @brief Called once per frame by the game engine
     *
     */
    void update();

    /**
     * @brief Saves the control config to the engine configuration store
     *
     */
    void saveToConfig() const;

    /**
     * @brief Loads the control config from the engine configuration store
     *
     */
    void loadFromConfig();

private:
    engine::Engine& engine;
    std::vector<Control::Type> controlTemplate;
    std::vector<std::unique_ptr<Actor>> actors;
    sf::Vector2f mouseVector;

    virtual void observe(const sf::Event& event) override;
};

} // namespace input
} // namespace bl

#endif
