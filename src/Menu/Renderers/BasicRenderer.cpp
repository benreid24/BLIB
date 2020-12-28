#include <BENG/Menu/Renderers/BasicRenderer.hpp>

namespace bg
{
namespace menu
{
BasicRenderer::BasicRenderer(Alignment halign, Alignment valign, float hpad, float vpad)
: horizontalAlignment(halign)
, verticalAlignment(valign)
, padding(hpad, vpad) {}

void BasicRenderer::setHorizontalAlignment(Alignment align) { horizontalAlignment = align; }

void BasicRenderer::setVerticalAlignment(Alignment align) { verticalAlignment = align; }

void BasicRenderer::setHorizontalPadding(float pad) { padding.x = pad; }

void BasicRenderer::setVerticalPadding(float pad) { padding.y = pad; }

sf::Vector2f BasicRenderer::renderItem(sf::RenderTarget& target, sf::RenderStates renderStates,
                                       const Item& item, const sf::Vector2f& position,
                                       float columnWidth, float rowHeight, int x,
                                       int y) const {
    sf::Vector2f pos = position + padding;
    if (horizontalAlignment == Center)
        pos.x += columnWidth / 2.f - item.getRenderItem().getSize().x / 2.f;
    else if (horizontalAlignment == Right)
        pos.x += columnWidth - item.getRenderItem().getSize().x - padding.x;
    if (verticalAlignment == Center)
        pos.y += rowHeight / 2.f - item.getRenderItem().getSize().y / 2.f;
    else if (verticalAlignment == Bottom)
        pos.y += rowHeight - item.getRenderItem().getSize().y - padding.y;

    item.getRenderItem().render(target, renderStates, pos);
    return pos + padding + item.getRenderItem().getSize() - position;
}

sf::Vector2f BasicRenderer::estimateItemSize(const Item& item) const {
    return item.getRenderItem().getSize() + 2.f * padding;
}

} // namespace menu
} // namespace bg
