#include <BLIB/Interfaces/GUI/Elements/RadioButton.hpp>

#include <BLIB/Interfaces/GUI/Elements/Label.hpp>

namespace bl
{
namespace gui
{
RadioButton::Ptr RadioButton::create(const std::string& text, Group* radioGroup) {
    return create(Label::create(text), radioGroup);
}

RadioButton::Ptr RadioButton::create(Element::Ptr child, Group* radioGroup) {
    Ptr but(new RadioButton(child, radioGroup));
    but->finishCreate();
    return but;
}

RadioButton::RadioButton(Element::Ptr child, Group* radioGroup)
: ToggleButton(child)
, myGroup(this)
, rgroup(radioGroup ? radioGroup : &myGroup) {
    rgroup->buttons.push_back(this);
    getSignal(Action::ValueChanged).willAlwaysCall([this](const Action&, Element*) {
        if (getValue()) rgroup->setActiveButton(this);
    });
}

RadioButton::~RadioButton() { rgroup->removeButton(this); }

RadioButton::Group* RadioButton::getRadioGroup() { return rgroup; }

void RadioButton::renderToggles(Canvas& activeBut, Canvas& inactiveBut,
                                const Renderer& renderer) const {
    renderer.renderToggleRadioButton(activeBut.getTexture(), true);
    renderer.renderToggleRadioButton(inactiveBut.getTexture(), false);
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
        for (RadioButton* but : buttons) {
            if (newOwner != owner) break;
            newOwner = but;
        }
        if (newOwner != owner) {
            newOwner->myGroup.buttons = buttons;
            for (RadioButton* but : buttons) { but->rgroup = &newOwner->myGroup; }
        }
    }
}

} // namespace gui
} // namespace bl