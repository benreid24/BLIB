#include <Input/Encoder.hpp>

#include <unordered_map>

namespace bl
{
namespace input
{
namespace
{
std::unordered_map<sf::Keyboard::Key, std::string> keymap;
std::unordered_map<std::string, sf::Keyboard::Key> strKeymap;
std::unordered_map<sf::Mouse::Button, std::string> mouseButtonMap;
std::unordered_map<std::string, sf::Mouse::Button> strMouseButtonMap;
std::unordered_map<sf::Joystick::Axis, std::string> axisMap;
std::unordered_map<std::string, sf::Joystick::Axis> strAxisMap;
bool inited = false;

const std::string VerticalWheelUp      = "Mouse Wheel Up";
const std::string VerticalWheelDown    = "Mouse Wheel Down";
const std::string HorizontalWheelLeft  = "Mouse Wheel Left";
const std::string HorizontalWheelRight = "Mouse Wheel Right";
const std::string JoystickButtonPrefix = "Joystick Button ";

sf::Keyboard::Key operator+(sf::Keyboard::Key k, int offset) {
    return static_cast<sf::Keyboard::Key>(static_cast<int>(k) + offset);
}

void init() {
    // Letters
    for (int i = 0; i < 26; ++i) {
        const sf::Keyboard::Key k = static_cast<sf::Keyboard::Key>(sf::Keyboard::Key::A + i);
        keymap[k]                 = std::string(1, 'A' + i);
    }

    // Numbers
    for (int i = 0; i < 10; ++i) {
        const sf::Keyboard::Key row = static_cast<sf::Keyboard::Key>(sf::Keyboard::Key::Num0 + i);
        const sf::Keyboard::Key pad =
            static_cast<sf::Keyboard::Key>(sf::Keyboard::Key::Numpad0 + i);
        keymap[row] = std::to_string(i);
        keymap[pad] = std::string("Numpad ") + std::to_string(i);
    }

    // Function keys
    for (int i = 0; i < 15; ++i) {
        const sf::Keyboard::Key k = static_cast<sf::Keyboard::Key>(sf::Keyboard::Key::F1 + i);
        keymap[k]                 = "F" + std::to_string(i + 1);
    }

    // Misc keys
    keymap[sf::Keyboard::Key::Space]      = "Space";
    keymap[sf::Keyboard::Key::Tab]        = "Tab";
    keymap[sf::Keyboard::Key::LControl]   = "Left Ctrl";
    keymap[sf::Keyboard::Key::LShift]     = "Left Shift";
    keymap[sf::Keyboard::Key::LAlt]       = "Left Alt";
    keymap[sf::Keyboard::Key::Grave]      = "~";
    keymap[sf::Keyboard::Key::Enter]      = "Enter";
    keymap[sf::Keyboard::Key::RAlt]       = "Right Alt";
    keymap[sf::Keyboard::Key::RShift]     = "Right Shift";
    keymap[sf::Keyboard::Key::RControl]   = "Right Ctrl";
    keymap[sf::Keyboard::Key::Semicolon]  = ";";
    keymap[sf::Keyboard::Key::Subtract]   = "Numpad -";
    keymap[sf::Keyboard::Key::Add]        = "Numpad +";
    keymap[sf::Keyboard::Key::Backspace]  = "Backspace";
    keymap[sf::Keyboard::Key::Backslash]  = "\\";
    keymap[sf::Keyboard::Key::Comma]      = ",";
    keymap[sf::Keyboard::Key::Apostrophe] = "'";
    keymap[sf::Keyboard::Key::Escape]     = "Esc";
    keymap[sf::Keyboard::Key::Equal]      = "=";
    keymap[sf::Keyboard::Key::End]        = "End";
    keymap[sf::Keyboard::Key::Home]       = "Home";
    keymap[sf::Keyboard::Key::Period]     = ".";
    keymap[sf::Keyboard::Key::PageDown]   = "Page Down";
    keymap[sf::Keyboard::Key::PageUp]     = "Page Up";
    keymap[sf::Keyboard::Key::Pause]      = "Pause";
    keymap[sf::Keyboard::Key::Menu]       = "Menu";
    keymap[sf::Keyboard::Key::Slash]      = "/";
    keymap[sf::Keyboard::Key::Hyphen]     = "-";
    keymap[sf::Keyboard::Key::Delete]     = "Delete";
    keymap[sf::Keyboard::Key::Insert]     = "Insert";
    keymap[sf::Keyboard::Key::Multiply]   = "*";
    keymap[sf::Keyboard::Key::Divide]     = "Numpad /";
    keymap[sf::Keyboard::Key::Up]         = "Up";
    keymap[sf::Keyboard::Key::Right]      = "Right";
    keymap[sf::Keyboard::Key::Down]       = "Down";
    keymap[sf::Keyboard::Key::Left]       = "Left";

    // Reverse key lookup
    for (const auto& pair : keymap) { strKeymap[pair.second] = pair.first; }

    // Mouse buttons
    mouseButtonMap[sf::Mouse::Button::Left]   = "Left Mouse";
    mouseButtonMap[sf::Mouse::Button::Right]  = "Right Mouse";
    mouseButtonMap[sf::Mouse::Button::Middle] = "Middle Mouse";
    mouseButtonMap[sf::Mouse::Button::Extra1] = "Mouse 4";
    mouseButtonMap[sf::Mouse::Button::Extra2] = "Mouse 5";

    // Reverse button lookup
    for (const auto& pair : mouseButtonMap) { strMouseButtonMap[pair.second] = pair.first; }

    // Joystick axes
    axisMap[sf::Joystick::Axis::X]    = "X Axis";
    axisMap[sf::Joystick::Axis::Y]    = "Y Axis";
    axisMap[sf::Joystick::Axis::Z]    = "Z Axis";
    axisMap[sf::Joystick::Axis::R]    = "R Axis";
    axisMap[sf::Joystick::Axis::U]    = "U Axis";
    axisMap[sf::Joystick::Axis::V]    = "V Axis";
    axisMap[sf::Joystick::Axis::PovX] = "PovX Axis";
    axisMap[sf::Joystick::Axis::PovY] = "PovY Axis";

    // Reverse axis lookup
    for (const auto& pair : axisMap) { strAxisMap[pair.second] = pair.first; }

    inited = true;
}
} // namespace

Encoder::ControlInfo Encoder::fromString(const std::string& s) {
    if (!inited) init();

    const auto keyIt = strKeymap.find(s);
    if (keyIt != strKeymap.end()) { return ControlInfo{keyIt->second}; }

    const auto mbutIt = strMouseButtonMap.find(s);
    if (mbutIt != strMouseButtonMap.end()) { return ControlInfo{mbutIt->second}; }

    auto axIt = strAxisMap.find(s);
    if (axIt != strAxisMap.end()) { return ControlInfo{axIt->second}; }

    axIt = strAxisMap.find(s.substr(1));
    if (axIt != strAxisMap.end()) { return ControlInfo(axIt->second, s[0] == '+'); }

    if (s == VerticalWheelUp) { return ControlInfo{sf::Mouse::Wheel::Vertical, true}; }
    if (s == VerticalWheelDown) { return ControlInfo{sf::Mouse::Wheel::Vertical, false}; }
    if (s == HorizontalWheelLeft) { return ControlInfo{sf::Mouse::Wheel::Horizontal, false}; }
    if (s == HorizontalWheelRight) { return ControlInfo{sf::Mouse::Wheel::Horizontal, true}; }

    std::string_view view(s);
    if (view.substr(0, JoystickButtonPrefix.size()) == JoystickButtonPrefix) {
        std::string_view num   = view.substr(JoystickButtonPrefix.size());
        const unsigned int but = std::atoi(std::string(num).c_str());
        if (but >= static_cast<unsigned int>(sf::Joystick::ButtonCount)) { return {}; }
        return ControlInfo{but};
    }
    return {};
}

std::string Encoder::toString(const ControlInfo& ctrl) {
    if (!inited) init();

    switch (ctrl.type) {
    case ControlInfo::Key:
        return keymap[ctrl.key];
    case ControlInfo::MouseButton:
        return mouseButtonMap[ctrl.mouseButton];
    case ControlInfo::MouseWheelUp:
        return ctrl.mouseWheel == sf::Mouse::Wheel::Vertical ? VerticalWheelUp :
                                                               HorizontalWheelRight;
    case ControlInfo::MouseWheelDown:
        return ctrl.mouseWheel == sf::Mouse::Wheel::Vertical ? VerticalWheelDown :
                                                               HorizontalWheelLeft;
    case ControlInfo::JoystickAxis:
        return axisMap[ctrl.joystickAxis];
    case ControlInfo::JoystickAxisPositive:
        return "+" + axisMap[ctrl.joystickAxis];
    case ControlInfo::JoystickAxisNegative:
        return "-" + axisMap[ctrl.joystickAxis];
    case ControlInfo::JoystickButton:
        return JoystickButtonPrefix + std::to_string(ctrl.joystickButton);
    case ControlInfo::Invalid:
    default:
        return "";
    }
}

Encoder::ControlInfo::ControlInfo()
: type(Invalid) {}

Encoder::ControlInfo::ControlInfo(sf::Keyboard::Key key)
: type(Key)
, key(key) {}

Encoder::ControlInfo::ControlInfo(sf::Mouse::Button mbut)
: type(MouseButton)
, mouseButton(mbut) {}

Encoder::ControlInfo::ControlInfo(sf::Mouse::Wheel wheel, bool upOrRight)
: type(upOrRight ? MouseWheelUp : MouseWheelDown)
, mouseWheel(wheel) {}

Encoder::ControlInfo::ControlInfo(unsigned int jbut)
: type(JoystickButton)
, joystickButton(jbut) {}

Encoder::ControlInfo::ControlInfo(sf::Joystick::Axis axis)
: type(JoystickAxis)
, joystickAxis(axis) {}

Encoder::ControlInfo::ControlInfo(sf::Joystick::Axis axis, bool pos)
: type(pos ? JoystickAxisPositive : JoystickAxisNegative)
, joystickAxis(axis) {}

} // namespace input
} // namespace bl
