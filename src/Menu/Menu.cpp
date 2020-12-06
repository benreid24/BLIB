#include <BLIB/Logging.hpp>
#include <BLIB/Menu/Menu.hpp>

namespace bl
{
namespace menu
{
Menu::Menu(Item::Ptr root)
: rootItem(root)
, selectedItem(root) {}

void Menu::refresh() {
    const auto itemVisitor = [this](const Item& item, int x, int y) {
        auto it = columnWidths.find(x);
        if (it == columnWidths.end())
            columnWidths[x] = item.getRenderItem().getSize().x;
        else if (it->second < item.getRenderItem().getSize().x)
            it->second = item.getRenderItem().getSize().x;
        it = rowHeights.find(y);
        if (it == rowHeights.end())
            rowHeights[y] = item.getRenderItem().getSize().y;
        else if (it->second < item.getRenderItem().getSize().y)
            rowHeights[y] = item.getRenderItem().getSize().y;
    };
    rootItem->visit(itemVisitor);
}

void Menu::render(const Renderer& renderer, sf::RenderTarget& target,
                  const sf::Vector2f& position, sf::RenderStates renderStates) {
    std::list<std::pair<int, int>> rendered;
    itemAreas.clear();
    renderItem(renderer, target, rootItem, position, renderStates, 0, 0, rendered);
}

void Menu::renderItem(const Renderer& renderer, sf::RenderTarget& target, Item::Ptr item,
                      const sf::Vector2f& position, sf::RenderStates renderStates, int x,
                      int y, std::list<std::pair<int, int>>& rendered) {
    if (std::find(rendered.begin(), rendered.end(), std::make_pair(x, y)) != rendered.end())
        return;
    rendered.push_back(std::make_pair(x, y));

    float columnWidth = item->getRenderItem().getSize().x;
    float rowHeight   = item->getRenderItem().getSize().y;
    auto it           = columnWidths.find(x);
    if (it != columnWidths.end()) columnWidth = columnWidths[x];
    it = rowHeights.find(y);
    if (it != rowHeights.end()) rowHeight = rowHeights[y];

    const sf::Vector2f size =
        renderer.renderItem(*item, position, columnWidth, rowHeight, x, y);
    itemAreas.push_back(std::make_pair(sf::FloatRect(position, size), item));
    if (item.get() == selectedItem.get()) {
        // TODO - render current item
    }

    if (item->attachments[Item::Top]) {
        renderItem(renderer,
                   target,
                   item->attachments[Item::Top],
                   {position.x, position.y - size.y},
                   renderStates,
                   x,
                   y - 1,
                   rendered);
    }
    if (item->attachments[Item::Right]) {
        renderItem(renderer,
                   target,
                   item->attachments[Item::Right],
                   {position.x + size.x, position.y},
                   renderStates,
                   x + 1,
                   y,
                   rendered);
    }
    if (item->attachments[Item::Bottom]) {
        renderItem(renderer,
                   target,
                   item->attachments[Item::Bottom],
                   {position.x, position.y + size.y},
                   renderStates,
                   x,
                   y + 1,
                   rendered);
    }
    if (item->attachments[Item::Left]) {
        renderItem(renderer,
                   target,
                   item->attachments[Item::Left],
                   {position.x - size.x, position.y},
                   renderStates,
                   x - 1,
                   y,
                   rendered);
    }
}

void Menu::processEvent(const Event& event) {
    switch (event.type) {
    case Event::SelectorMove: {
        if (selectedItem->attachments[event.moveEvent.direction]) {
            Item::Ptr item = selectedItem->attachments[event.moveEvent.direction];
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
        for (unsigned int i = 0; i < itemAreas.size(); ++i) {
            if (itemAreas[i].first.contains(event.locationEvent.position)) {
                if (itemAreas[i].second->isSelectable()) {
                    selectedItem->getSignal(Item::Deselected)();
                    selectedItem = itemAreas[i].second;
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

} // namespace menu
} // namespace bl
