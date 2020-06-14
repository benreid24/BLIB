#include <BLIB/GUI/Elements/ComboBox.hpp>

#include <BLIB/GUI/Packers/Packer.hpp>

namespace bl
{
namespace gui
{
ComboBox::Ptr ComboBox::create(const std::string& group, const std::string& id) {
    Ptr box(new ComboBox(group, id));
    box->addChildren();
    return box;
}

ComboBox::ComboBox(const std::string& group, const std::string& id)
: Container(group, id)
, arrow(Canvas::create(16, 16, group, id + "-dropdown"))
, selected(-1)
, opened(false) {
    getSignal(Action::RenderSettingsChanged)
        .willAlwaysCall(std::bind(&ComboBox::onSettings, this));
    getSignal(Action::LeftClicked).willAlwaysCall(std::bind(&ComboBox::clicked, this));
}

void ComboBox::addChildren() { add(arrow); }

void ComboBox::addOption(const std::string& text) {
    options.push_back(text);
    labels.push_back(Label::create(text, group(), id() + "-" + text));
    labels.back()->setExpandsWidth(true);
    labels.back()->setHorizontalAlignment(RenderSettings::Left);
    labels.back()
        ->getSignal(Action::LeftClicked)
        .willAlwaysCall(std::bind(&ComboBox::optionClicked, this, options.back()));
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
    if (selected >= options.size()) selected = options.size() - 1;
    opened = false;
    packClosed();
    fireSignal(Action(Action::ValueChanged));
}

void ComboBox::setSelectedOption(const std::string& t) {
    for (unsigned int i = 0; i < options.size(); ++i) {
        if (options[i] == t) {
            setSelectedOption(i);
            break;
        }
    }
}

bool ComboBox::isOpened() const { return opened; }

sf::Vector2i ComboBox::minimumRequisition() const {
    sf::Vector2i lreq(0, 0);
    for (Label::Ptr label : labels) {
        const sf::Vector2i req = label->getRequisition();
        lreq.x                 = std::max(req.x, lreq.x);
        lreq.y                 = std::max(req.y, lreq.y);
    }
    return {lreq.x + lreq.y, lreq.y};
}

void ComboBox::onAcquisition() {
    labelSize = {getAcquisition().width - getAcquisition().height, getAcquisition().height};
    Packer::manuallyPackElement(
        arrow, {labelSize.x, 0, getAcquisition().height, getAcquisition().height});
    if (opened)
        packOpened();
    else
        packClosed();
}

bool ComboBox::handleRawEvent(const RawEvent& event) {
    Container::handleRawEvent(event);
    return false;
}

void ComboBox::doRender(sf::RenderTarget& target, sf::RenderStates states,
                        const Renderer& renderer) const {
    // TODO
}

void ComboBox::onSettings() {
    for (Label::Ptr label : labels) {
        if (renderSettings().fillColor.has_value())
            label->setColor(renderSettings().fillColor.value(), sf::Color::Transparent);
        if (renderSettings().secondaryFillColor.has_value())
            label->setSecondaryColor(renderSettings().secondaryFillColor.value(),
                                     sf::Color::Transparent);
        if (renderSettings().font.has_value()) label->setFont(renderSettings().font.value());
        if (renderSettings().style.has_value())
            label->setStyle(renderSettings().style.value());
        if (renderSettings().characterSize.has_value())
            label->setCharacterSize(renderSettings().characterSize.value());
    }
}

void ComboBox::optionClicked(std::string text) {
    if (opened) setSelectedOption(text);
}

void ComboBox::clicked() {
    opened = !opened;
    if (opened)
        packOpened();
    else
        packClosed();
}

void ComboBox::packOpened() {
    sf::Vector2i pos(0, labelSize.y);
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
            Packer::manuallyPackElement(labels[i], {0, 0, labelSize.x, labelSize.y});
            labels[i]->setVisible(true, false);
        }
        else
            labels[i]->setVisible(false, false);
    }
}

} // namespace gui
} // namespace bl