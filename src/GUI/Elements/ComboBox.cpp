#include <BENG/GUI/Elements/ComboBox.hpp>

#include <BENG/GUI/Packers/Packer.hpp>

namespace bg
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
, arrow(Canvas::create(32, 32, group, id + "-dropdown"))
, selected(-1)
, opened(false)
, arrowRendered(false) {
    getSignal(Action::RenderSettingsChanged)
        .willAlwaysCall(std::bind(&ComboBox::onSettings, this));
    getSignal(Action::LeftClicked).willAlwaysCall(std::bind(&ComboBox::clicked, this));
}

void ComboBox::addChildren() { add(arrow); }

void ComboBox::setLabelColor(const sf::Color& c) {
    labelColor = c;
    onSettings();
}

void ComboBox::addOption(const std::string& text) {
    options.push_back(text);
    labels.push_back(Label::create(text, group(), id() + "-" + text));
    labels.back()->setExpandsWidth(true);
    labels.back()->setHorizontalAlignment(RenderSettings::Left);
    labels.back()
        ->getSignal(Action::LeftClicked)
        .willAlwaysCall(std::bind(&ComboBox::optionClicked, this, options.back()));
    add(labels.back());
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
        lreq.x                 = std::max(req.x + 4, lreq.x);
        lreq.y                 = std::max(req.y + 4, lreq.y);
    }
    return {lreq.x + lreq.y, lreq.y};
}

void ComboBox::onAcquisition() {
    labelSize   = {getAcquisition().width - getAcquisition().height + OptionPadding,
                 getAcquisition().height + OptionPadding};
    labelRegion = {0,
                   getAcquisition().height,
                   labelSize.x,
                   labelSize.y * static_cast<int>(options.size())};
    arrow->scaleToSize({static_cast<float>(getAcquisition().height),
                        static_cast<float>(getAcquisition().height)},
                       false);
    Packer::manuallyPackElement(
        arrow, {labelSize.x, 0, getAcquisition().height, getAcquisition().height});
    if (opened)
        packOpened();
    else
        packClosed();
}

bool ComboBox::handleRawEvent(const RawEvent& event) {
    Container::handleRawEvent(event);
    return labelRegion.contains(
               static_cast<sf::Vector2i>(transformEvent(event).localMousePos)) &&
           opened;
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
        renderer.renderComboBoxDropdown(arrow->getTexture());
    }

    const sf::View oldView = target.getView();
    sf::IntRect region     = getAcquisition();
    region.height += options.size() * labelSize.y;
    target.setView(computeView(target, states.transform, region, false));
    renderer.renderComboBox(
        target, states, *this, labelSize, opened ? options.size() : 0, moused);
    transformStates(states);
    renderChildrenRawFiltered(target, states, renderer, {});
    target.setView(oldView);
}

void ComboBox::onSettings() {
    for (Label::Ptr label : labels) {
        if (labelColor.has_value())
            label->setColor(labelColor.value(), sf::Color::Transparent);
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
    sf::Vector2i pos(OptionPadding, labelSize.y + OptionPadding);
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
            Packer::manuallyPackElement(
                labels[i], {OptionPadding, OptionPadding, labelSize.x, labelSize.y});
            labels[i]->setVisible(true, false);
        }
        else
            labels[i]->setVisible(false, false);
    }
}

} // namespace gui
} // namespace bg