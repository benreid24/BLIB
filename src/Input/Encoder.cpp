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

void init() {
    // Letters
    for (int i = 0; i < 26; ++i) {
        const sf::Keyboard::Key k = static_cast<sf::Keyboard::Key>(sf::Keyboard::A + i);
        keymap[k]                 = std::string(1, 'A' + i);
    }

    // Numbers
    for (int i = 0; i < 10; ++i) {
        const sf::Keyboard::Key row = static_cast<sf::Keyboard::Key>(sf::Keyboard::Num0 + i);
        const sf::Keyboard::Key pad = static_cast<sf::Keyboard::Key>(sf::Keyboard::Numpad0 + i);
        keymap[row]                 = std::to_string(i);
        keymap[pad]                 = std::string("Numpad ") + std::to_string(i);
    }

    // Function keys
    for (int i = 0; i < 15; ++i) {
        const sf::Keyboard::Key k = static_cast<sf::Keyboard::Key>(sf::Keyboard::F1 + i);
        keymap[k]                 = "F" + std::to_string(i + 1);
    }

    // Misc keys
    keymap[sf::Keyboard::Space]     = "Space";
    keymap[sf::Keyboard::Tab]       = "Tab";
    keymap[sf::Keyboard::LControl]  = "Left Ctrl";
    keymap[sf::Keyboard::LShift]    = "Left Shift";
    keymap[sf::Keyboard::LAlt]      = "Left Alt";
    keymap[sf::Keyboard::Tilde]     = "~";
    keymap[sf::Keyboard::Enter]     = "Enter";
    keymap[sf::Keyboard::RAlt]      = "Right Alt";
    keymap[sf::Keyboard::RShift]    = "Right Shift";
    keymap[sf::Keyboard::RControl]  = "Right Ctrl";
    keymap[sf::Keyboard::Semicolon] = ";";
    keymap[sf::Keyboard::Subtract]  = "Numpad -";
    keymap[sf::Keyboard::Add]       = "Numpad +";
    keymap[sf::Keyboard::Backspace] = "Backspace";
    keymap[sf::Keyboard::Backslash] = "\\";
    keymap[sf::Keyboard::Comma]     = ",";
    keymap[sf::Keyboard::Quote]     = "'";
    keymap[sf::Keyboard::Escape]    = "Esc";
    keymap[sf::Keyboard::Equal]     = "=";
    keymap[sf::Keyboard::End]       = "End";
    keymap[sf::Keyboard::Home]      = "Home";
    keymap[sf::Keyboard::Period]    = ".";
    keymap[sf::Keyboard::PageDown]  = "Page Down";
    keymap[sf::Keyboard::PageUp]    = "Page Up";
    keymap[sf::Keyboard::Pause]     = "Pause";
    keymap[sf::Keyboard::Menu]      = "Menu";
    keymap[sf::Keyboard::Slash]     = "/";
    keymap[sf::Keyboard::Hyphen]    = "-";
    keymap[sf::Keyboard::Delete]    = "Delete";
    keymap[sf::Keyboard::Insert]    = "Insert";
    keymap[sf::Keyboard::Multiply]  = "*";
    keymap[sf::Keyboard::Divide]    = "Numpad /";
    keymap[sf::Keyboard::Up]        = "Up";
    keymap[sf::Keyboard::Right]     = "Right";
    keymap[sf::Keyboard::Down]      = "Down";
    keymap[sf::Keyboard::Left]      = "Left";

    // Reverse key lookup
    for (const auto& pair : keymap) { strKeymap[pair.second] = pair.first; }

    // Mouse buttons
    mouseButtonMap[sf::Mouse::Left]     = "Left Mouse";
    mouseButtonMap[sf::Mouse::Right]    = "Right Mouse";
    mouseButtonMap[sf::Mouse::Middle]   = "Middle Mouse";
    mouseButtonMap[sf::Mouse::XButton1] = "Mouse 4";
    mouseButtonMap[sf::Mouse::XButton2] = "Mouse 5";

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

    if (s == VerticalWheelUp) { return ControlInfo{sf::Mouse::VerticalWheel, true}; }
    if (s == VerticalWheelDown) { return ControlInfo{sf::Mouse::VerticalWheel, false}; }
    if (s == HorizontalWheelLeft) { return ControlInfo{sf::Mouse::HorizontalWheel, false}; }
    if (s == HorizontalWheelRight) { return ControlInfo{sf::Mouse::HorizontalWheel, true}; }

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
        return ctrl.mouseWheel == sf::Mouse::VerticalWheel ? VerticalWheelUp : HorizontalWheelRight;
    case ControlInfo::MouseWheelDown:
        return ctrl.mouseWheel == sf::Mouse::VerticalWheel ? VerticalWheelDown :
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
