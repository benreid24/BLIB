#include <BLIB/Interfaces/Menu/Menu.hpp>

#include <BLIB/Logging.hpp>
#include <queue>
#include <unordered_set>

namespace bl
{
namespace menu
{
Menu::Menu(const Selector::Ptr& selector)
: selector(selector)
, selectedItem(nullptr)
, padding(10.f, 10.f)
, minSize(0.f, 0.f) {}

void Menu::setSelectedItem(Item* s) {
    selectedItem = s;
    selectedItem->getSignal(Item::Selected)();
}

void Menu::setPosition(const sf::Vector2f& pos) { position = pos; }

void Menu::setPadding(const sf::Vector2f& p) { padding = p; }

void Menu::setMinHeight(float h) { minSize.x = h; }

void Menu::setMinWidth(float w) { minSize.x = w; }

void Menu::setRootItem(const Item::Ptr& root) {
    items.clear();
    items.emplace_back(root);
    selectedItem = root.get();
    refreshPositions();
}

void Menu::addItem(const Item::Ptr& item, Item* parent, Item::AttachPoint ap, bool r) {
    items.emplace_back(item);
    parent->attachments[ap] = item.get();
    if (r) { item->attachments[Item::oppositeSide(ap)] = parent; }
    refreshPositions();
}

void Menu::attachExisting(Item* item, Item* parent, Item::AttachPoint ap, bool r) {
    parent->attachments[ap] = item;
    if (r) { item->attachments[Item::oppositeSide(ap)] = parent; }
    refreshPositions();
}

void Menu::removeItem(Item* item, bool c) {
    unsigned int i;
    for (unsigned int j = 0; j < items.size(); ++j) {
        if (items[j].get() == item) { i = j; }
        else {
            for (unsigned int k = 0; k < Item::AttachPoint::_NUM_ATTACHPOINTS; ++k) {
                if (items[j]->attachments[k] == item) {
                    if (c) { items[j]->attachments[k] = item->attachments[k]; }
                    else {
                        items[j]->attachments[k] = nullptr;
                    }
                }
            }
        }
    }
    items.erase(items.begin() + i);
    if (selectedItem == item) {
        selectedItem = nullptr;
        if (!items.empty()) {
            for (unsigned int k = 0; k < Item::AttachPoint::_NUM_ATTACHPOINTS; ++k) {
                if (item->attachments[k]) selectedItem = item->attachments[k];
            }
        }
        if (!selectedItem) { selectedItem = items.front().get(); }
    }
    refreshPositions();
}

void Menu::render(sf::RenderTarget& target, sf::RenderStates states) {
    states.transform.translate(position);
    for (const auto& item : items) {
        item->render(target, states, item->position);
        if (item.get() == selectedItem) {
            selector->render(target, states, {item->position, item->getSize()});
        }
    }
}

void Menu::processEvent(const Event& event) {
    switch (event.type) {
    case Event::SelectorMove: {
        if (selectedItem->attachments[event.moveEvent.direction]) {
            Item* item = selectedItem->attachments[event.moveEvent.direction];
            while (item) {
                if (item->isSelectable()) {
                    selectedItem->getSignal(Item::Deselected)();
                    selectedItem = item;
                    selectedItem->getSignal(Item::Selected)();
                    break;
                }
                if (!item->allowsSelectionCrossing()) break;
                item = item->attachments[event.moveEvent.direction];
            }
        }
    } break;
    case Event::Activate:
        selectedItem->getSignal(Item::Activated)();
        break;
    case Event::SelectorLocation: {
        for (const auto& item : items) {
            if (sf::FloatRect(item->position, item->getSize())
                    .contains(event.locationEvent.position)) {
                if (item->isSelectable()) {
                    selectedItem->getSignal(Item::Deselected)();
                    selectedItem = item.get();
                    selectedItem->getSignal(Item::Selected)();
                }
            }
        }
    } break;
    default:
        BL_LOG_ERROR << "Menu::processEvent received invalid Event::Type: " << event.type;
        break;
    }
}

void Menu::refreshPositions() {
    if (items.empty()) return;

    std::queue<Item*> toVisit;
    std::unordered_set<Item*> visited;
    toVisit.emplace(items.front().get());
    visited.insert(items.front().get());
    items.front()->position = {0.f, 0.f};

    while (!toVisit.empty()) {
        Item* item = toVisit.front();
        toVisit.pop();

        for (unsigned int i = 0; i < Item::AttachPoint::_NUM_ATTACHPOINTS; ++i) {
            Item* v = item->attachments[i];
            if (v && visited.find(v) == visited.end()) {
                if (!v->positionOverridden) {
                    v->position = move(item->position,
                                       item->getSize(),
                                       v->getSize(),
                                       static_cast<Item::AttachPoint>(i));
                }
                toVisit.emplace(v);
                visited.insert(v);
            }
        }
    }
}

sf::Vector2f Menu::move(const sf::Vector2f& pos, const sf::Vector2f& psize,
                        const sf::Vector2f& esize, Item::AttachPoint ap) {
    switch (ap) {
    case Item::AttachPoint::Top:
        return {pos.x, pos.y - std::max(minSize.y, esize.y) - padding.y};
    case Item::AttachPoint::Right:
        return {pos.x + std::max(minSize.x, psize.x) + padding.x, pos.y};
    case Item::AttachPoint::Bottom:
        return {pos.x, pos.y + std::max(minSize.y, psize.y) + padding.y};
    case Item::AttachPoint::Left:
        return {pos.x - std::max(minSize.x, esize.x) - padding.x, pos.y};
    default:
        return pos;
    }
}

} // namespace menu
} // namespace bl
