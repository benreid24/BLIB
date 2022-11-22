#include <BLIB/Interfaces/Menu/Drivers/KeyboardDriver.hpp>

namespace bl
{
namespace menu
{
KeyboardDriver::KeyboardDriver(Menu& menu, sf::Keyboard::Key upCtrl, sf::Keyboard::Key rightCtrl,
                               sf::Keyboard::Key downCtrl, sf::Keyboard::Key leftCtrl,
                               sf::Keyboard::Key activateCtrl)
: menu(menu)
, moveControls{upCtrl, rightCtrl, downCtrl, leftCtrl}
, activateControl(activateCtrl) {}

void KeyboardDriver::setMoveControl(Item::AttachPoint dir, sf::Keyboard::Key ctrl) {
    moveControls[dir] = ctrl;
}

void KeyboardDriver::setActivateControl(sf::Keyboard::Key ctrl) { activateControl = ctrl; }

void KeyboardDriver::observe(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == moveControls[Item::Top]) {
            menu.processEvent(Event(Event::MoveEvent(Item::Top)));
        }
        else if (event.key.code == moveControls[Item::Right]) {
            menu.processEvent(Event(Event::MoveEvent(Item::Right)));
        }
        else if (event.key.code == moveControls[Item::Bottom]) {
            menu.processEvent(Event(Event::MoveEvent(Item::Bottom)));
        }
        else if (event.key.code == moveControls[Item::Left]) {
            menu.processEvent(Event(Event::MoveEvent(Item::Left)));
        }
        else if (event.key.code == activateControl) {
            menu.processEvent(Event(Event::ActivateEvent()));
        }
    }
}

} // namespace menu
} // namespace bl
