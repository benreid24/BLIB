#ifndef BLIB_INPUT_INPUTSYSTEM_HPP
#define BLIB_INPUT_INPUTSYSTEM_HPP

#include <BLIB/Input/Actor.hpp>
#include <BLIB/Input/InputConfiguration.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <memory>

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
class InputSystem : public sig::Listener<sf::Event> {
public:
    /**
     * @brief Construct a new Input System
     *
     * @param engine The game engine the input system belongs to
     */
    InputSystem(engine::Engine& engine);

    /**
     * @brief Configures the entire input system using the convenience type
     *
     * @param config The full input configuration to use for the system
     */
    void configure(const InputConfiguration& config);

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
     * @brief Returns the number of actors in the system
     */
    unsigned int actorCount() const;

    /**
     * @brief Returns a normalized vector representing the direction of the mouse from the window
     *        center. Does not account for the window viewport or letterboxing
     */
    const glm::vec2& mouseUnitVector() const;

    /**
     * @brief Captures the mouse to the center of the window. Directional mouse controls based on
     *        delta movement each frame
     */
    void captureMouse(bool hide = true);

    /**
     * @brief Releases the mouse. Directional mouse controls based on mouse position
     */
    void releaseMouse();

    /**
     * @brief Called once per frame by the game engine
     */
    void update();

    /**
     * @brief Saves the control config to the engine configuration store
     */
    void saveToConfig() const;

    /**
     * @brief Loads the control config from the engine configuration store
     */
    void loadFromConfig();

private:
    engine::Engine& engine;
    InputConfiguration controlTemplate;
    std::vector<std::unique_ptr<Actor>> actors;
    glm::vec2 mouseVector;
    bool mouseCaptured;

    virtual void process(const sf::Event& event) override;

    Actor& addActor();
    void removeActor(unsigned int i);
    void resetControls();

    friend class engine::Engine;
};

} // namespace input
} // namespace bl

#endif
