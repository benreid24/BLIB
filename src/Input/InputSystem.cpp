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
: engine(engine)
, mouseCaptured(false) {
    addActor();
}

void InputSystem::configure(const InputConfiguration& config) {
    controlTemplate = config;
    resetControls();
}

void InputSystem::resetControls() {
    for (auto& actor : actors) { actor->resetControls(controlTemplate); }
}

Actor& InputSystem::addActor() {
    actors.emplace_back(new Actor(*this, controlTemplate));
    return *actors.back();
}

Actor& InputSystem::getActor(unsigned int i) { return *actors[i]; }

const Actor& InputSystem::getActor(unsigned int i) const { return *actors[i]; }

void InputSystem::removeActor(unsigned int i) { actors.erase(actors.begin() + i); }

unsigned int InputSystem::actorCount() const { return actors.size(); }

const glm::vec2& InputSystem::mouseUnitVector() const { return mouseVector; }

void InputSystem::captureMouse(bool hide) {
    if (mouseCaptured) { return; }
    engine.renderer().getWindow().getSfWindow().setMouseCursorGrabbed(true);
    if (hide) { engine.renderer().getWindow().getSfWindow().setMouseCursorVisible(false); }
}

void InputSystem::releaseMouse() {
    mouseCaptured = false;
    engine.renderer().getWindow().getSfWindow().setMouseCursorGrabbed(false);
    engine.renderer().getWindow().getSfWindow().setMouseCursorVisible(true);
}

void InputSystem::update() {
    for (auto& ap : actors) { ap->update(); }
}

void InputSystem::process(const sf::Event& event) {
    const sf::Event::MouseMoved* mm = event.getIf<sf::Event::MouseMoved>();
    if (mm) {
        const sf::Vector2i si(engine.renderer().getWindow().getSfWindow().getSize());
        const sf::Vector2i halfI = si / 2;
        const glm::vec2 hw       = glm::vec2(halfI.x, halfI.y);

        // prevent infinite loop
        if (mouseCaptured && mm->position == halfI) { return; }

        mouseVector = glm::normalize(glm::vec2(mm->position.x, mm->position.y) - hw);

        if (mouseCaptured) {
            sf::Mouse::setPosition(halfI, engine.renderer().getWindow().getSfWindow());
        }
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
