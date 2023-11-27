#include <BLIB/Interfaces/GUI/Elements/RadioButton.hpp>

#include <BLIB/Interfaces/GUI/Elements/Label.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

namespace bl
{
namespace gui
{
RadioButton::Ptr RadioButton::create(const std::string& text, const std::string& name,
                                     Group* radioGroup) {
    return create(Label::create(text), name, radioGroup);
}

RadioButton::Ptr RadioButton::create(const Element::Ptr& child, const std::string& name,
                                     Group* radioGroup) {
    return Ptr(new RadioButton(child, name, radioGroup));
}

RadioButton::RadioButton(const Element::Ptr& child, const std::string& n, Group* radioGroup)
: ToggleButton(child)
, name(n)
, myGroup(this)
, rgroup(radioGroup ? radioGroup : &myGroup) {
    rgroup->buttons.push_back(this);
    getSignal(Event::ValueChanged).willAlwaysCall([this](const Event&, Element*) {
        if (getValue()) rgroup->setActiveButton(this);
    });
}

RadioButton::~RadioButton() { rgroup->removeButton(this); }

RadioButton::Group* RadioButton::getRadioGroup() { return rgroup; }

rdr::Component* RadioButton::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<RadioButton>(*this);
}

void RadioButton::onClick() {
    if (!getValue()) {
        setValue(true);
        rgroup->setActiveButton(this);
    }
}

RadioButton::Group::Group(RadioButton* o)
: owner(o)
, active(nullptr) {}

RadioButton* RadioButton::Group::getActiveButton() { return active; }

void RadioButton::Group::setActiveButton(RadioButton* but) {
    for (RadioButton* b : buttons) {
        if (b != but) b->setValue(false);
    }
    active = but;
}

void RadioButton::Group::removeButton(RadioButton* but) {
    if (but == active) active = nullptr;
    for (unsigned int i = 0; i < buttons.size(); ++i) {
        if (buttons[i] == but) {
            buttons.erase(buttons.begin() + i);
            --i;
        }
    }

    if (but == owner && buttons.size() > 1) {
        RadioButton* newOwner = owner;
        for (RadioButton* b : buttons) {
            if (newOwner != owner) break;
            newOwner = b;
        }
        if (newOwner != owner) {
            newOwner->myGroup.buttons = buttons;
            for (RadioButton* b : buttons) { b->rgroup = &newOwner->myGroup; }
        }
    }
}

const std::string& RadioButton::getName() const { return name; }

} // namespace gui
} // namespace bl
