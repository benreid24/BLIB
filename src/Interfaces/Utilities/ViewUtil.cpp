#include <BLIB/Interfaces/Utilities/ViewUtil.hpp>

namespace bl
{
namespace interface
{
namespace
{
sf::View constructView(const sf::Vector2f& size, float x, float y, float w, float h) {
    sf::View view(size * 0.5f, size);
    view.setViewport({x, y, w, h});
    return view;
}
} // namespace

sf::View ViewUtil::computeView(const sf::Vector2f& size, const sf::View& oldView,
                               const sf::FloatRect& coverArea) {
    const float x = oldView.getViewport().left + oldView.getViewport().width * coverArea.left;
    const float y = oldView.getViewport().top + oldView.getViewport().height * coverArea.top;
    const float w = oldView.getViewport().width * coverArea.width;
    const float h = oldView.getViewport().height * coverArea.height;

    return constructView(size, x, y, w, h);
}

sf::View ViewUtil::computeViewPreserveAR(const sf::Vector2f& size, const sf::View& oldView,
                                         const sf::Vector2f& position, float width) {
    const float x      = oldView.getViewport().left + oldView.getViewport().width * position.x;
    const float y      = oldView.getViewport().top + oldView.getViewport().height * position.y;
    const float w      = oldView.getViewport().width * width;
    const float height = size.y / size.x * w;
    const float h      = oldView.getViewport().height * height;

    return constructView(size, x, y, w, h);
}

sf::View ViewUtil::computeViewAnchored(const sf::Vector2f& size, const sf::View& oldView,
                                       const sf::Vector2f& viewportSize, Anchor side) {
    const float w = oldView.getViewport().width * viewportSize.x;
    const float h = oldView.getViewport().height * viewportSize.y;
    float x       = oldView.getViewport().left + oldView.getViewport().width / 2.f - w / 2.f;
    float y       = oldView.getViewport().top + oldView.getViewport().height / 2.f - h / 2.f;

    switch (side) {
    case Top:
        y = oldView.getViewport().top;
        break;
    case Right:
        x = oldView.getViewport().left + oldView.getViewport().width - w;
        break;
    case Bottom:
        y = oldView.getViewport().top + oldView.getViewport().height - h;
        break;
    default: // Don't worry about bad values
        x = oldView.getViewport().left;
        break;
    }

    return constructView(size, x, y, w, h);
}

sf::View ViewUtil::computeViewAnchoredPreserveAR(const sf::Vector2f& size, const sf::View& oldView,
                                                 float width, Anchor side) {
    const float w = oldView.getViewport().width * width;
    const float h = oldView.getViewport().height * size.y / size.x * width;
    float x       = oldView.getViewport().left + oldView.getViewport().width / 2.f - w / 2.f;
    float y       = oldView.getViewport().top + oldView.getViewport().height / 2.f - h / 2.f;

    switch (side) {
    case Top:
        y = oldView.getViewport().top;
        break;
    case Right:
        x = oldView.getViewport().left + oldView.getViewport().width - w;
        break;
    case Bottom:
        y = oldView.getViewport().top + oldView.getViewport().height - h;
        break;
    default: // Don't worry about bad values
        x = oldView.getViewport().left;
        break;
    }

    return constructView(size, x, y, w, h);
}

} // namespace interface
} // namespace bl
