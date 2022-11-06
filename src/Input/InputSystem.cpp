#include <BLIB/Input/InputSystem.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Math/Vector.hpp>

namespace bl
{
namespace input
{
namespace
{
std::string actorPrefix(unsigned int i) { return "blib.input.player." + std::to_string(i); }
} // namespace

InputSystem::InputSystem(engine::Engine& engine)
: engine(engine) {}

void InputSystem::setControlCount(unsigned int c) {
    controlTemplate.resize(c, Control::Type::SingleTrigger);
}

void InputSystem::configureDirectionalControl(unsigned int c) {
    controlTemplate[c] = Control::Type::Directional;
}

void InputSystem::configureMovementControl(unsigned int c) {
    controlTemplate[c] = Control::Type::Movement;
}

void InputSystem::configureTriggerControl(unsigned int c) {
    controlTemplate[c] = Control::Type::SingleTrigger;
}

Actor& InputSystem::addActor() {
    actors.emplace_back(new Actor(*this, controlTemplate));
    return *actors.back();
}

Actor& InputSystem::getActor(unsigned int i) { return *actors[i]; }

const Actor& InputSystem::getActor(unsigned int i) const { return *actors[i]; }

void InputSystem::removeActor(unsigned int i) { actors.erase(actors.begin() + i); }

unsigned int InputSystem::actorCount() const { return actors.size(); }

const sf::Vector2f& InputSystem::mouseUnitVector() const { return mouseVector; }

void InputSystem::update() {
    for (auto& ap : actors) { ap->update(); }
}

void InputSystem::observe(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        mouseVector = math::normalized(sf::Vector2f(event.mouseMove.x, event.mouseMove.y) -
                                       sf::Vector2f(engine.window().getSize()) * 0.5f);
    }

    for (auto& ap : actors) { ap->process(event); }
}

void InputSystem::saveToConfig() const {
    for (unsigned int i = 0; i < actors.size(); ++i) { actors[i]->saveToConfig(actorPrefix(i)); }
}

void InputSystem::loadFromConfig() {
    for (unsigned int i = 0; i < actors.size(); ++i) { actors[i]->loadFromConfig(actorPrefix(i)); }
}

} // namespace input
} // namespace bl
