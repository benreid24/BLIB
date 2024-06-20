#include <BLIB/Interfaces/Menu/Menu.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Primitives/Color.hpp>
#include <queue>
#include <unordered_set>

namespace bl
{
namespace menu
{
audio::AudioSystem::Handle Menu::defaultMoveSound   = audio::AudioSystem::InvalidHandle;
audio::AudioSystem::Handle Menu::defaultFailSound   = audio::AudioSystem::InvalidHandle;
audio::AudioSystem::Handle Menu::defaultSelectSound = audio::AudioSystem::InvalidHandle;

Menu::Menu()
: engine(nullptr)
, observer(nullptr)
, scene(nullptr)
, maxSize(-1.f, -1.f)
, offset{}
, selector()
, selectedItem(nullptr)
, position{}
, padding(10.f, 10.f)
, minSize(0.f, 0.f)
, bgndPadding(padding.x * 2.f, padding.y * 2.f, padding.x * 2.f, padding.y * 2.f)
, moveSound(defaultMoveSound)
, failSound(defaultFailSound)
, selectSound(defaultSelectSound)
, depth(cam::OverlayCamera::MinDepth + 100.f) {}

void Menu::create(engine::Engine& e, rc::Observer& o, const Selector::Ptr& sel) {
    engine   = &e;
    observer = &o;
    selector = sel;
    background.create(*engine, {100.f, 100.f});
    background.setFillColor({1.f, 1.f, 1.f, 0.f});
    background.setOutlineColor({1.f, 1.f, 1.f, 0.f});
    background.getOverlayScaler().setScissorMode(com::OverlayScaler::ScissorSelf);
    background.getTransform().setDepth(depth);
    e.ecs().setEntityParentDestructionBehavior(background.entity(),
                                               ecs::ParentDestructionBehavior::OrphanedByParent);
    selector->doCreate(*engine, background.entity());
    event::Dispatcher::subscribe(this);
}

void Menu::addToOverlay(ecs::Entity parent) {
    if (parent != ecs::InvalidEntity) { background.setParent(parent); }
    addToScene(observer->getOrCreateSceneOverlay());
}

void Menu::addToScene(rc::Scene* s) {
    scene = s;
    background.addToScene(scene, rc::UpdateSpeed::Static);
    selector->doSceneAdd(scene);
    for (auto& item : items) { item->doSceneAdd(scene); }
}

void Menu::setHidden(bool h) { background.setHidden(h); }

void Menu::removeFromScene() {
    scene = nullptr;
    background.removeFromScene();
    selector->doSceneRemove();
    for (auto& item : items) { item->doSceneRemove(); }
}

void Menu::setSelectedItem(Item* s) {
    if (selectedItem) { selectedItem->getSignal(Item::Deselected)(); }
    selectedItem = s;
    selectedItem->getSignal(Item::Selected)();
    com::Transform2D* tform =
        engine->ecs().getComponent<com::Transform2D>(selectedItem->getEntity());
    const glm::vec2& pos = tform->getLocalPosition();
    selector->notifySelection(selectedItem->getEntity(),
                              {pos.x, pos.y, selectedItem->getSize().x, selectedItem->getSize().y});
    refreshScroll();
}

void Menu::setPosition(const glm::vec2& pos) {
    position = pos;
    const glm::vec2 thick(background.getOutlineThickness(), background.getOutlineThickness());
    background.getTransform().setPosition(position + thick);
}

void Menu::setPadding(const glm::vec2& p) {
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
    background.setFillColor(sfcol(fill));
    background.setOutlineColor(sfcol(outline));
    background.setOutlineThickness(t);
    if (p.left >= 0.f) {
        bgndPadding = p;
        refreshPositions();
    }
    refreshBackground();
}

void Menu::setScissorToSelf() {
    background.getOverlayScaler().setScissorMode(com::OverlayScaler::ScissorSelfConstrained);
}

void Menu::setScissor(const sf::IntRect& scissor) {
    background.getOverlayScaler().setFixedScissor(scissor);
}

void Menu::removeScissor() {
    background.getOverlayScaler().setScissorMode(com::OverlayScaler::ScissorInherit);
}

sf::FloatRect Menu::getBounds() const {
    const auto& pos = background.getTransform().getLocalPosition();
    return {pos.x, pos.y, totalSize.x, totalSize.y};
}

glm::vec2 Menu::visibleSize() const {
    const glm::vec2 s(maxSize.x > 0.f ? std::min(maxSize.x, totalSize.x) : totalSize.x,
                      maxSize.y > 0.f ? std::min(maxSize.y, totalSize.y) : totalSize.y);
    const float b = background.getOutlineThickness() * 2.f;
    return s + glm::vec2(bgndPadding.left + bgndPadding.width + b,
                         bgndPadding.top + bgndPadding.height + b);
}

const glm::vec2& Menu::maximumSize() const { return maxSize; }

const glm::vec2& Menu::currentOffset() const { return offset; }

void Menu::setMaximumSize(const glm::vec2& m) {
    maxSize = m;
    refreshScroll();
    refreshBackground();
}

void Menu::setDepth(float d) {
    depth = d;
    if (background.entity() != ecs::InvalidEntity) { background.getTransform().setDepth(depth); }
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

    for (auto& item : items) { item->notifyOffset(offset); }
}

void Menu::setRootItem(const Item::Ptr& root) {
    selectedItem = nullptr;
    for (auto& item : items) { item->doSceneRemove(); }
    items.clear();
    items.emplace_back(root);
    root->create(*engine, background.entity());
    if (scene) { root->doSceneAdd(scene); }
    setSelectedItem(root.get());
    refreshPositions();
}

void Menu::addItem(const Item::Ptr& item, Item* parent, Item::AttachPoint ap, bool r) {
    items.emplace_back(item);
    parent->attachments[ap] = item.get();
    if (r) { item->attachments[Item::oppositeSide(ap)] = parent; }
    item->parent = ap;
    item->create(*engine, background.entity());
    if (scene) { item->doSceneAdd(scene); }
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
    items[i]->doSceneRemove();
    items.erase(items.begin() + i);
    if (selectedItem == item) {
        selectedItem = nullptr;
        if (!items.empty()) {
            for (unsigned int k = 0; k < Item::AttachPoint::_NUM_ATTACHPOINTS; ++k) {
                if (item->attachments[k]) {
                    setSelectedItem(item->attachments[k]);
                    break;
                }
            }
        }
        if (!selectedItem) { setSelectedItem(items.front().get()); }
    }
    refreshPositions();
}

void Menu::processEvent(const Event& event) {
    switch (event.type) {
    case Event::SelectorMove: {
        if (selectedItem->attachments[event.moveEvent.direction]) {
            const Item* ogSel = selectedItem;
            Item* item        = selectedItem->attachments[event.moveEvent.direction];
            while (item) {
                if (item->isSelectable()) {
                    setSelectedItem(item);
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
        const glm::vec2 os = observer->transformToOverlaySpace(
            {event.locationEvent.position.x, event.locationEvent.position.y});
        const glm::vec2 pos = os - position - offset;
        const sf::Vector2f sfpos(pos.x, pos.y);
        for (const auto& item : items) {
            const sf::FloatRect rect(
                item->position.x, item->position.y, item->getSize().x, item->getSize().y);
            if (rect.contains(sfpos)) {
                if (item->isSelectable() && selectedItem != item.get()) {
                    setSelectedItem(item.get());
                    playSound(moveSound);
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

        const glm::vec2 size = item->getSize();
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
                else { v->position = v->posOverride; }
                toVisit.emplace(v);
                visited.insert(v);
            }
        }
    }

    totalSize = {bounds.width - bounds.left, bounds.height - bounds.top};
    const glm::vec2 finalOffset(bounds.left - bgndPadding.left, bounds.top - bgndPadding.top);
    for (auto& item : items) { item->notifyPosition(item->position - finalOffset); }

    if (selectedItem != nullptr) {
        com::Transform2D* tform =
            engine->ecs().getComponent<com::Transform2D>(selectedItem->getEntity());
        const glm::vec2& pos = tform->getLocalPosition();
        selector->notifySelection(
            selectedItem->getEntity(),
            {pos.x, pos.y, selectedItem->getSize().x, selectedItem->getSize().y});
    }

    refreshScroll();
    refreshBackground();
}

void Menu::refreshBackground() {
    const glm::vec2 thick(background.getOutlineThickness(), background.getOutlineThickness());
    background.setSize(visibleSize() - thick * 2.f);
    background.getTransform().setPosition(position + thick);
}

glm::vec2 Menu::move(const glm::vec2& pos, const glm::vec2& psize, const glm::vec2& esize,
                     Item::AttachPoint ap) {
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

void Menu::observe(const rc::event::SceneDestroyed& event) {
    if (event.scene == static_cast<rc::Scene*>(scene)) { removeFromScene(); }
}

void Menu::draw(rc::scene::CodeScene::RenderContext& ctx) {
    if (!background.component().isHidden()) {
        background.draw(ctx);
        for (auto& item : items) { item->draw(ctx); }
        selector->draw(ctx);
    }
}

} // namespace menu
} // namespace bl
