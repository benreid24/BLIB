#include <BLIB/Interfaces/GUI/Elements/ComboBox.hpp>

#include <BLIB/Interfaces/GUI/Packers/LinePacker.hpp>
#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

namespace bl
{
namespace gui
{
ComboBox::Ptr ComboBox::create() { return Ptr(new ComboBox()); }

ComboBox::ComboBox()
: Element()
, maxHeight(0.f)
, scroll(0.f)
, selected(-1)
, moused(-1)
, opened(false) {
    getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&ComboBox::clicked, this));
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&ComboBox::onAcquisition, this));
    getSignal(Event::Moved).willAlwaysCall(std::bind(&ComboBox::onAcquisition, this));
}

void ComboBox::addOption(const std::string& text) {
    options.push_back(text);
    if (getComponent()) { getComponent()->onElementUpdated(); }
    refreshLabelRegion();
}

void ComboBox::clearOptions() {
    options.clear();
    selected = -1;
    opened   = false;
    refreshLabelRegion();
}

int ComboBox::optionCount() const { return options.size(); }

int ComboBox::getSelectedOption() const { return selected; }

const std::string& ComboBox::getSelectedOptionText() const {
    static const std::string empty;
    return selected >= 0 ? options[selected] : empty;
}

void ComboBox::setSelectedOption(int i, bool f) {
    selected = i;
    if (selected >= static_cast<int>(options.size())) selected = options.size() - 1;
    opened = false;
    if (f) fireSignal(Event(Event::ValueChanged, static_cast<float>(i)));
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

void ComboBox::setSelectedOption(const std::string& t, bool f) {
    for (unsigned int i = 0; i < options.size(); ++i) {
        if (options[i] == t) {
            setSelectedOption(i, f);
            break;
        }
    }
}

bool ComboBox::isOpened() const { return opened; }

sf::Vector2f ComboBox::minimumRequisition() const {
    sf::Vector2f lreq(80.f, 20.f);
    if (getComponent()) { lreq = getComponent()->getRequisition(); }
    return {lreq.x + lreq.y + OptionPadding * 4.f, lreq.y};
}

void ComboBox::onAcquisition() { refreshLabelRegion(); }

void ComboBox::refreshLabelRegion() {
    labelSize = {getAcquisition().width - getAcquisition().height + OptionPadding,
                 getAcquisition().height + OptionPadding};

    totalHeight = labelSize.y * static_cast<float>(options.size());
    labelRegion = {getAcquisition().left,
                   getAcquisition().top + getAcquisition().height,
                   labelSize.x,
                   totalHeight};

    labelRegion.height =
        maxHeight > 0.f ? std::min(maxHeight, labelRegion.height) : labelRegion.height;
}

bool ComboBox::propagateEvent(const Event& event) {
    const bool contained = labelRegion.contains(event.mousePosition());
    if (contained && opened) {
        bool updated = false;

        // determine which option the event was on
        const auto findEventOption = [this, &event]() -> int {
            const sf::Vector2f translated(event.mousePosition().x,
                                          event.mousePosition().y + scroll);
            sf::FloatRect test(labelRegion.left, labelRegion.top, labelSize.x, labelSize.y);

            for (unsigned int i = 0; i < options.size(); ++i) {
                if (test.contains(translated)) { return i; }
                test.top += labelSize.y;
            }
            return -1;
        };

        // handle event
        switch (event.type()) {
        case Event::Scrolled:
            scrolled(event);
            moused  = findEventOption();
            updated = true;
            break;

        case Event::LeftMousePressed:
            setSelectedOption(findEventOption());
            updated = false;
            break;

        case Event::MouseMoved: {
            const int priorMoused = moused;
            moused                = findEventOption();
            updated               = moused != priorMoused;
        } break;

        default:
            break;
        }

        if (updated && getComponent()) { getComponent()->onElementUpdated(); }
        return true;
    }
    if (opened && !contained && event.type() == Event::LeftMousePressed) {
        opened = false;
        if (getComponent()) { getComponent()->onElementUpdated(); }
        return true; // eat it
    }

    return false;
}

bool ComboBox::handleScroll(const Event& event) {
    if (opened && labelRegion.contains(event.mousePosition())) {
        scrolled(event);
        if (getComponent()) { getComponent()->onElementUpdated(); }
        return true;
    }
    return false;
}

rdr::Component* ComboBox::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<ComboBox>(*this);
}

void ComboBox::optionClicked(const std::string& text) {
    if (opened) {
        opened = false;
        setSelectedOption(text);
    }
}

void ComboBox::clicked() {
    takeFocus();
    moveToTop();
    opened = !opened;
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

void ComboBox::setMaxHeight(float m) { maxHeight = m; }

void ComboBox::scrolled(const Event& a) {
    if (maxHeight > 0.f && totalHeight > maxHeight) {
        const float maxScroll = totalHeight - maxHeight;
        const float f         = maxScroll / 140.f * 6.f;

        scroll -= a.scrollDelta() * f;
        if (scroll < 0.f)
            scroll = 0.f;
        else if (scroll > maxScroll)
            scroll = maxScroll;
    }
}

bool ComboBox::receivesOutOfBoundsEvents() const { return opened; }

} // namespace gui
} // namespace bl
