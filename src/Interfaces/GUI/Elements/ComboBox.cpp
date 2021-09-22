#include <BLIB/Interfaces/GUI/Elements/ComboBox.hpp>

#include <BLIB/Interfaces/GUI/Packers/Packer.hpp>

namespace bl
{
namespace gui
{
ComboBox::Ptr ComboBox::create() {
    Ptr box(new ComboBox());
    box->addChildren();
    return box;
}

ComboBox::ComboBox()
: Container()
, arrow(Canvas::create(32, 32))
, maxHeight(0.f)
, scroll(0.f)
, selected(-1)
, opened(false)
, arrowRendered(false) {
    getSignal(Action::RenderSettingsChanged).willAlwaysCall(std::bind(&ComboBox::onSettings, this));
    getSignal(Action::LeftClicked).willAlwaysCall(std::bind(&ComboBox::clicked, this));
}

void ComboBox::addChildren() { add(arrow); }

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
    if (selected >= static_cast<int>(options.size())) selected = options.size() - 1;
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
    labelRegion = {
        0, getAcquisition().height, labelSize.x, labelSize.y * static_cast<int>(options.size())};
    labelRegion.height =
        maxHeight > 0 ? std::min(maxHeight, labelRegion.height) : labelRegion.height;
    arrow->scaleToSize(
        {static_cast<float>(getAcquisition().height), static_cast<float>(getAcquisition().height)},
        false);
    Packer::manuallyPackElement(arrow,
                                {labelSize.x, 0, getAcquisition().height, getAcquisition().height});
    if (opened)
        packOpened();
    else
        packClosed();
}

bool ComboBox::handleRawEvent(const RawEvent& event) {
    const RawEvent transformed = transformEvent(event);
    const bool contained =
        labelRegion.contains(static_cast<sf::Vector2i>(transformed.localMousePos));
    if (contained) {
        if (transformed.event.type == sf::Event::MouseWheelScrolled) {
            scrolled(Action::fromRaw(transformed));
            return true;
        }
    }
    Container::handleRawEvent(event.transformToLocal({0.f, -scroll}));
    return contained && opened;
}

bool ComboBox::handleScroll(const RawEvent& event) {
    if (opened) {
        scrolled(Action::fromRaw(event));
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

    const sf::View oldView = target.getView();

    target.setView(computeView(oldView, getAcquisition(), false));
    renderer.renderComboBox(target, states, *this);
    arrow->render(target, states, renderer);

    sf::IntRect region = labelRegion;
    region.top += getAcquisition().top;
    region.left += getAcquisition().left;
    region.height += labelSize.y;

    sf::View trickView = oldView;
    trickView.move(0, labelSize.y);
    target.setView(computeView(trickView, region, false));
    if (opened) states.transform.translate(0, -scroll);

    renderer.renderComboBoxDropdownBoxes(
        target, states, *this, labelSize, opened ? options.size() : 0, moused);
    renderChildrenRawFiltered(target, states, renderer, {arrow.get()});
    target.setView(oldView);
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
            Packer::manuallyPackElement(labels[i],
                                        {OptionPadding, OptionPadding, labelSize.x, labelSize.y});
            labels[i]->setVisible(true, false);
        }
        else
            labels[i]->setVisible(false, false);
    }
}

void ComboBox::setMaxHeight(int m) { maxHeight = m; }

void ComboBox::scrolled(const Action& a) {
    if (maxHeight > 0) {
        const float height    = labelSize.y * static_cast<int>(options.size() + 1);
        const float maxScroll = height - maxHeight;

        scroll -= a.data.scroll * 6.f;
        if (scroll < 0.f)
            scroll = 0.f;
        else if (scroll > maxScroll)
            scroll = maxScroll;
    }
}

} // namespace gui
} // namespace bl
