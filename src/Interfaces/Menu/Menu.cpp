#include <BLIB/Interfaces/Menu/Menu.hpp>

#include <BLIB/Interfaces/Utilities/ViewUtil.hpp>
#include <BLIB/Logging.hpp>
#include <queue>
#include <unordered_set>

namespace bl
{
namespace menu
{
audio::AudioSystem::Handle Menu::defaultMoveSound   = audio::AudioSystem::InvalidHandle;
audio::AudioSystem::Handle Menu::defaultFailSound   = audio::AudioSystem::InvalidHandle;
audio::AudioSystem::Handle Menu::defaultSelectSound = audio::AudioSystem::InvalidHandle;

Menu::Menu(const Selector::Ptr& selector)
: maxSize(-1.f, -1.f)
, selector(selector)
, selectedItem(nullptr)
, padding(10.f, 10.f)
, minSize(0.f, 0.f)
, bgndPadding(padding * 2.f, padding * 2.f)
, moveSound(defaultMoveSound)
, failSound(defaultFailSound)
, selectSound(defaultSelectSound) {
    background.setFillColor(sf::Color::Transparent);
    background.setOutlineColor(sf::Color::Transparent);
}

void Menu::setSelectedItem(Item* s) {
    if (selectedItem) selectedItem->getSignal(Item::Deselected)();
    selectedItem = s;
    selectedItem->getSignal(Item::Selected)();
    refreshScroll();
}

void Menu::setPosition(const sf::Vector2f& pos) { position = pos; }

void Menu::setPadding(const sf::Vector2f& p) {
    padding = p;
    refreshPositions();
}

void Menu::setMinHeight(float h) {
    minSize.y = h;
    refreshPositions();
}

void Menu::setMinWidth(float w) {
    minSize.x = w;
    refreshPositions();
}

void Menu::configureBackground(sf::Color fill, sf::Color outline, float t, const sf::FloatRect& p) {
    background.setFillColor(fill);
    background.setOutlineColor(outline);
    background.setOutlineThickness(t);
    bgndPadding = p.left >= 0.f ? p : sf::FloatRect(padding * 2.f, padding * 2.f);
}

sf::FloatRect Menu::getBounds() const { return {position, totalSize}; }

sf::Vector2f Menu::visibleSize() const {
    const sf::Vector2f s(maxSize.x > 0.f ? std::min(maxSize.x, totalSize.x) : totalSize.x,
                         maxSize.y > 0.f ? std::min(maxSize.y, totalSize.y) : totalSize.y);
    const float b = background.getOutlineThickness() * 2.f;
    return s + sf::Vector2f(bgndPadding.left + bgndPadding.width + b,
                            bgndPadding.top + bgndPadding.height + b);
}

const sf::Vector2f& Menu::maximumSize() const { return maxSize; }

const sf::Vector2f& Menu::currentOffset() const { return offset; }

void Menu::setMaximumSize(const sf::Vector2f& m) {
    maxSize = m;
    refreshScroll();
}

void Menu::refreshScroll() {
    if (!selectedItem) return;

    if (maxSize.x > 0.f) {
        // left side
        if (selectedItem->position.x < offset.x) {
            offset.x = selectedItem->position.x - bgndPadding.left;
        }
        // right side
        if (selectedItem->position.x + selectedItem->getSize().x > offset.x + maxSize.x) {
            offset.x = selectedItem->getSize().x + selectedItem->position.y + bgndPadding.width -
                       maxSize.x;
        }
    }
    if (maxSize.y > 0.f) {
        // top
        if (selectedItem->position.y < offset.y) {
            offset.y = selectedItem->position.y - bgndPadding.top;
        }
        // bottom
        if (selectedItem->position.y + selectedItem->getSize().y > offset.y + maxSize.y) {
            offset.y = selectedItem->getSize().y + selectedItem->position.y + bgndPadding.height -
                       maxSize.y;
        }
    }
}

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
    item->parent = ap;
    refreshPositions();
}

void Menu::attachExisting(Item* item, Item* parent, Item::AttachPoint ap, bool r) {
    parent->attachments[ap] = item;
    if (r) { item->attachments[Item::oppositeSide(ap)] = parent; }
}

void Menu::removeItem(Item* item, bool c) {
    unsigned int i = items.size();
    for (unsigned int j = 0; j < items.size(); ++j) {
        if (items[j].get() == item) { i = j; }
        else {
            for (unsigned int k = 0; k < Item::AttachPoint::_NUM_ATTACHPOINTS; ++k) {
                if (items[j]->attachments[k] == item) {
                    if (c) { items[j]->attachments[k] = item->attachments[k]; }
                    else { items[j]->attachments[k] = nullptr; }
                }
            }
        }
    }
    if (i == items.size()) return;
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
    selectedItem->getSignal(Item::Selected)();
    refreshPositions();
}

void Menu::render(sf::RenderTarget& target, sf::RenderStates states) const {
    const sf::View oldView = target.getView();
    if (maxSize.x > 0.f || maxSize.y > 0.f) {
        target.setView(interface::ViewUtil::computeSubView({position, visibleSize()}, oldView));
    }
    states.transform.translate(position);
    target.draw(background, states);
    states.transform.translate(
        sf::Vector2f(bgndPadding.left, bgndPadding.top) - offset +
        sf::Vector2f(background.getOutlineThickness(), background.getOutlineThickness()));
    for (const auto& item : items) {
        item->render(target, states, item->position);
        if (item.get() == selectedItem) {
            selector->render(target, states, {item->position, item->getSize()});
        }
    }
    target.setView(oldView);
}

void Menu::processEvent(const Event& event) {
    switch (event.type) {
    case Event::SelectorMove: {
        if (selectedItem->attachments[event.moveEvent.direction]) {
            const Item* ogSel = selectedItem;
            Item* item        = selectedItem->attachments[event.moveEvent.direction];
            while (item) {
                if (item->isSelectable()) {
                    selectedItem->getSignal(Item::Deselected)();
                    selectedItem = item;
                    selectedItem->getSignal(Item::Selected)();
                    refreshScroll();
                    break;
                }
                if (!item->allowsSelectionCrossing()) break;
                item = item->attachments[event.moveEvent.direction];
            }
            playSound(ogSel != selectedItem ? moveSound : failSound);
        }
        else { playSound(failSound); }
    } break;
    case Event::Activate:
        selectedItem->getSignal(Item::Activated)();
        playSound(selectSound);
        break;
    case Event::SelectorLocation: {
        const sf::Vector2f pos = event.locationEvent.position - position - offset;
        for (const auto& item : items) {
            if (sf::FloatRect(item->position, item->getSize()).contains(pos)) {
                if (item->isSelectable()) {
                    selectedItem->getSignal(Item::Deselected)();
                    if (selectedItem != item.get()) { playSound(selectSound); }
                    selectedItem = item.get();
                    selectedItem->getSignal(Item::Selected)();
                    refreshScroll();
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
    sf::FloatRect bounds    = {0.f, 0.f, 0.f, 0.f};

    while (!toVisit.empty()) {
        Item* item = toVisit.front();
        toVisit.pop();

        const sf::Vector2f size = item->getSize();
        if (item->position.x < bounds.left) bounds.left = item->position.x;
        if (item->position.x + size.x > bounds.width) bounds.width = item->position.x + size.x;
        if (item->position.y < bounds.top) bounds.top = item->position.y;
        if (item->position.y + size.y > bounds.height) bounds.height = item->position.y + size.y;

        for (unsigned int i = 0; i < Item::AttachPoint::_NUM_ATTACHPOINTS; ++i) {
            const Item::AttachPoint ap = static_cast<Item::AttachPoint>(i);

            Item* v = item->attachments[i];
            if (v && v->parent == ap && visited.find(v) == visited.end()) {
                if (!v->positionOverridden) {
                    v->position = move(item->position, size, v->getSize(), ap);
                }
                toVisit.emplace(v);
                visited.insert(v);
            }
        }
    }

    totalSize = {bounds.width - bounds.left, bounds.height - bounds.top};
    for (auto& item : items) {
        item->position.x -= bounds.left;
        item->position.y -= bounds.top;
    }

    const sf::Vector2f thick(background.getOutlineThickness(), background.getOutlineThickness());
    background.setSize(visibleSize() - thick * 2.f);
    background.setPosition(thick);

    refreshScroll();
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

const Item* Menu::getSelectedItem() const { return selectedItem; }

void Menu::playSound(audio::AudioSystem::Handle s) const {
    if (s != audio::AudioSystem::InvalidHandle) { audio::AudioSystem::playOrRestartSound(s); }
}

void Menu::setDefaultMoveSound(audio::AudioSystem::Handle s) { defaultMoveSound = s; }

void Menu::setDefaultMoveFailSound(audio::AudioSystem::Handle s) { defaultFailSound = s; }

void Menu::setDefaultSelectSound(audio::AudioSystem::Handle s) { defaultSelectSound = s; }

void Menu::setMoveSound(audio::AudioSystem::Handle s) { moveSound = s; }

void Menu::setMoveFailSound(audio::AudioSystem::Handle s) { failSound = s; }

void Menu::setSelectSound(audio::AudioSystem::Handle s) { selectSound = s; }

} // namespace menu
} // namespace bl
