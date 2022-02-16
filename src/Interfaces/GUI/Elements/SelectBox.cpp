#include <BLIB/Interfaces/GUI/Elements/SelectBox.hpp>

#include <BLIB/Interfaces/GUI/Packers/LinePacker.hpp>

namespace bl
{
namespace gui
{
namespace
{
constexpr unsigned int NoSelection = std::numeric_limits<unsigned int>::max();
}

SelectBox::Ptr SelectBox::create() { return Ptr(new SelectBox()); }

SelectBox::SelectBox()
: content(ScrollArea::create(LinePacker::create(LinePacker::Vertical)))
, selected(NoSelection) {
    content->includeScrollbarsInRequisition(true);
    content->setColor(sf::Color::White, sf::Color::Black);
    content->setOutlineThickness(1.5f);
    Element* c[] = {content.get()};
    registerChildren(c);
}

void SelectBox::addOption(const std::string& o) {
    values.emplace_back(Box::create(LinePacker::create()), Label::create(o));
    values.back().first->pack(values.back().second);
    values.back()
        .first->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(
            &SelectBox::onLabelClick, this, std::placeholders::_1, std::placeholders::_2));
    values.back()
        .second->getSignal(Event::LeftClicked)
        .willAlwaysCall(std::bind(
            &SelectBox::onLabelClick, this, std::placeholders::_1, std::placeholders::_2));
    content->pack(values.back().first, true, false);
}

void SelectBox::editOptionText(unsigned int i, const std::string& t) {
    values[i].second->setText(t);
}

void SelectBox::removeOption(const std::string& o) {
    for (unsigned int i = 0; i < values.size(); ++i) {
        if (values[i].second->getText() == o) {
            removeOption(i);
            break;
        }
    }
}

void SelectBox::removeOption(unsigned int i) {
    if (i < values.size()) {
        values[i].first->remove();
        values.erase(values.begin() + i);
        if (selected > i) { selected -= 1; }
        else if (selected == i) {
            removeSelection();
        }
    }
}

unsigned int SelectBox::optionCount() const { return values.size(); }

const std::string& SelectBox::getOption(unsigned int i) const {
    static const std::string empty;
    return i < values.size() ? values[i].second->getText() : empty;
}

void SelectBox::clearOptions() {
    for (auto& o : values) { o.first->remove(); }
    values.clear();
    selected = NoSelection;
}

std::optional<unsigned int> SelectBox::getSelectedOption() const {
    return selected < values.size() ? selected : std::optional<unsigned int>();
}

void SelectBox::setSelectedOption(const std::string& o) {
    for (unsigned int i = 0; i < values.size(); ++i) {
        if (values[i].second->getText() == o) {
            onLabelClick({Event::LeftClicked}, values[i].first.get());
            break;
        }
    }
}

void SelectBox::setSelectedOption(unsigned int i) {
    onLabelClick({Event::LeftClicked}, values[i].first.get());
}

void SelectBox::removeSelection() {
    selected = NoSelection;
    for (auto& l : values) { l.first->setColor(sf::Color::Transparent, sf::Color::Transparent); }
}

void SelectBox::setMaxSize(const sf::Vector2f& s) { content->setMaxSize(s); }

void SelectBox::onAcquisition() { Packer::manuallyPackElement(content, getAcquisition(), true); }

sf::Vector2f SelectBox::minimumRequisition() const { return content->getRequisition(); }

void SelectBox::doRender(sf::RenderTarget& target, sf::RenderStates states,
                         const Renderer& r) const {
    content->render(target, states, r);
}

void SelectBox::onLabelClick(const Event&, Element* l) {
    removeSelection();
    for (unsigned int i = 0; i < values.size(); ++i) {
        if (values[i].first.get() == l || values[i].second.get() == l) {
            selected = i;
            values[i].first->setColor(sf::Color(30, 50, 245), sf::Color::Blue);
            break;
        }
    }
}

bool SelectBox::propagateEvent(const Event& e) { return sendEventToChildren(e); }

void SelectBox::getAllOptions(std::vector<std::string>& output) const {
    output.clear();
    output.reserve(values.size());
    for (const auto& v : values) { output.emplace_back(v.second->getText()); }
}

} // namespace gui
} // namespace bl
