#include <BLIB/Interfaces/GUI/Elements/ComboBox.hpp>

#include <BLIB/Interfaces/GUI/Packers/LinePacker.hpp>
#include <BLIB/Interfaces/Utilities/ViewUtil.hpp>

namespace bl
{
namespace gui
{
ComboBox::Ptr ComboBox::create() { return Ptr(new ComboBox()); }

ComboBox::ComboBox()
: Element()
, arrow(Canvas::create(32, 32))
, maxHeight(0.f)
, scroll(0.f)
, selected(-1)
, opened(false)
, arrowRendered(false) {
    getSignal(Event::RenderSettingsChanged).willAlwaysCall(std::bind(&ComboBox::onSettings, this));
    getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&ComboBox::clicked, this));
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&ComboBox::onAcquisition, this));
    getSignal(Event::Moved).willAlwaysCall(std::bind(&ComboBox::onAcquisition, this));
}

void ComboBox::setLabelColor(const sf::Color& c) {
    labelColor = c;
    onSettings();
}

void ComboBox::addOption(const std::string& text) {
    options.push_back(text);
    labels.push_back(Label::create(text));
    labels.back()->setExpandsWidth(true);
    labels.back()->setHorizontalAlignment(RenderSettings::Left);
    labels.back()
        ->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(&ComboBox::optionClicked, this, text));
    onSettings();
}

void ComboBox::clearOptions() {
    for (Label::Ptr label : labels) { label->remove(); }
    options.clear();
    labels.clear();
    selected = -1;
    opened   = false;
}

int ComboBox::getSelectedOption() const { return selected; }

const std::string& ComboBox::getSelectedOptionText() const {
    static const std::string empty;
    return selected >= 0 ? options[selected] : empty;
}

void ComboBox::setSelectedOption(int i) {
    selected = i;
    if (selected >= static_cast<int>(options.size())) selected = options.size() - 1;
    opened = false;
    packClosed();
    fireSignal(Event(Event::ValueChanged, options[i]));
}

void ComboBox::setSelectedOption(const std::string& t) {
    for (unsigned int i = 0; i < options.size(); ++i) {
        if (options[i] == t) {
            setSelectedOption(i);
            fireSignal(Event(Event::ValueChanged, t));
            break;
        }
    }
}

bool ComboBox::isOpened() const { return opened; }

sf::Vector2f ComboBox::minimumRequisition() const {
    sf::Vector2f lreq(0.f, 0.f);
    for (const Label::Ptr& label : labels) {
        const sf::Vector2f req = label->getRequisition();
        lreq.x                 = std::max(req.x + 4, lreq.x);
        lreq.y                 = std::max(req.y + 4, lreq.y);
    }
    return {lreq.x + lreq.y, lreq.y};
}

void ComboBox::onAcquisition() {
    labelSize = {getAcquisition().width - getAcquisition().height + OptionPadding,
                 getAcquisition().height + OptionPadding};

    totalHeight = labelSize.y * static_cast<int>(options.size());
    labelRegion = {getAcquisition().left,
                   getAcquisition().top + getAcquisition().height,
                   labelSize.x,
                   totalHeight};

    labelRegion.height =
        maxHeight > 0 ? std::min(maxHeight, labelRegion.height) : labelRegion.height;

    arrow->scaleToSize({getAcquisition().height, getAcquisition().height}, false);
    Packer::manuallyPackElement(arrow,
                                {getAcquisition().left + labelSize.x,
                                 getAcquisition().top,
                                 getAcquisition().height,
                                 getAcquisition().height});
    if (opened)
        packOpened();
    else
        packClosed();
}

bool ComboBox::propagateEvent(const Event& event) {
    const bool contained = labelRegion.contains(event.mousePosition());
    if (contained && opened) {
        if (event.type() == Event::Scrolled) {
            scrolled(event);
            const Event fakeMove(
                Event::MouseMoved,
                sf::Vector2f(event.mousePosition().x, event.mousePosition().y + scroll));
            for (Label::Ptr& option : labels) { option->processEvent(fakeMove); }
            return true;
        }
        else {
            const Event translated(
                event, sf::Vector2f(event.mousePosition().x, event.mousePosition().y + scroll));
            for (Label::Ptr& option : labels) { option->processEvent(translated); }
        }
    }
    if (opened && !contained && event.type() == Event::LeftMousePressed) {
        opened = false;
        packClosed();
        return true; // eat it
    }
    return contained && opened;
}

bool ComboBox::handleScroll(const Event& event) {
    if (opened) {
        scrolled(event);
        return true;
    }
    return false;
}

void ComboBox::doRender(sf::RenderTarget& target, sf::RenderStates states,
                        const Renderer& renderer) const {
    unsigned int moused = options.size();
    for (unsigned int i = 0; i < labels.size(); ++i) {
        if (labels[i]->mouseOver()) {
            moused = i;
            break;
        }
    }

    if (!arrowRendered) {
        arrowRendered = true;
        renderer.renderComboBoxDropdownArrow(arrow->getTexture());
    }

    renderer.renderComboBox(target, states, *this);
    arrow->render(target, states, renderer);

    if (opened) {
        const sf::View oldView = target.getView();
        target.setView(
            interface::ViewUtil::computeSubView(labelRegion, renderer.getOriginalView()));
        states.transform.translate(0, -scroll);
        renderer.renderComboBoxDropdownBoxes(
            target, states, *this, labelSize, opened ? options.size() : 0, moused);
        for (const Label::Ptr& option : labels) { option->render(target, states, renderer); }
        target.setView(oldView);
    }
    else if (selected >= 0) {
        labels[selected]->render(target, states, renderer);
    }
}

void ComboBox::onSettings() {
    for (Label::Ptr label : labels) {
        if (labelColor.has_value()) label->setColor(labelColor.value(), sf::Color::Transparent);
        if (renderSettings().font.has_value()) label->setFont(renderSettings().font.value());
        if (renderSettings().style.has_value()) label->setStyle(renderSettings().style.value());
        if (renderSettings().characterSize.has_value())
            label->setCharacterSize(renderSettings().characterSize.value());
    }
}

void ComboBox::optionClicked(const std::string& text) {
    if (opened) {
        setSelectedOption(text);
        opened = false;
        packClosed();
    }
}

void ComboBox::clicked() {
    takeFocus();
    moveToTop();
    opened = !opened;
    if (opened)
        packOpened();
    else
        packClosed();
}

void ComboBox::packOpened() {
    sf::Vector2f pos(labelRegion.left, labelRegion.top);
    for (Label::Ptr label : labels) {
        label->setVisible(true, false);
        Packer::manuallyPackElement(label, {pos, labelSize});
        pos.y += labelSize.y;
    }
}

void ComboBox::packClosed() {
    const unsigned sel = selected >= 0 ? selected : options.size();
    for (unsigned int i = 0; i < labels.size(); ++i) {
        if (i == sel) {
            Packer::manuallyPackElement(labels[i],
                                        {getAcquisition().left + OptionPadding,
                                         getAcquisition().top + OptionPadding,
                                         labelSize.x,
                                         labelSize.y});
            labels[i]->setVisible(true, false);
        }
        else
            labels[i]->setVisible(false, false);
    }
}

void ComboBox::setMaxHeight(float m) { maxHeight = m; }

void ComboBox::scrolled(const Event& a) {
    if (maxHeight > 0 && totalHeight > maxHeight) {
        const float maxScroll = totalHeight - maxHeight;

        scroll -= a.scrollDelta() * 6.f;
        if (scroll < 0.f)
            scroll = 0.f;
        else if (scroll > maxScroll)
            scroll = maxScroll;
    }
}

void ComboBox::update(float dt) {
    Element::update(dt);
    if (opened) {
        for (Label::Ptr& option : labels) { option->update(dt); }
    }
}

} // namespace gui
} // namespace bl
