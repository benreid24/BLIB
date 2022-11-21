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

sf::View ViewUtil::computeSubView(const sf::FloatRect& region, const sf::View& defaultView) {
    const sf::Vector2f corner = defaultView.getCenter() - defaultView.getSize() * 0.5f;
    const float x =
        (region.left - corner.x) / defaultView.getSize().x * defaultView.getViewport().width +
        defaultView.getViewport().left;
    const float y =
        (region.top - corner.y) / defaultView.getSize().y * defaultView.getViewport().height +
        defaultView.getViewport().top;
    const float w = region.width / defaultView.getSize().x * defaultView.getViewport().width;
    const float h = region.height / defaultView.getSize().y * defaultView.getViewport().height;

    sf::View result = constructView({region.width, region.height}, x, y, w, h);
    result.move(region.left, region.top);

    if (x < 0.f) {
        const float shift = defaultView.getSize().x * (-x);
        result.setSize(result.getSize().x - shift, result.getSize().y);
        result.move(shift * 0.5f, 0.f);
        sf::FloatRect t = result.getViewport();
        t.left          = 0.f;
        t.width += x;
        result.setViewport(t);
    }
    if (y < 0.f) {
        const float shift = defaultView.getSize().y * (-y);
        result.setSize(result.getSize().x, result.getSize().y - shift);
        result.move(0.f, shift * 0.5f);
        sf::FloatRect t = result.getViewport();
        t.top           = 0.f;
        t.height += y;
        result.setViewport(t);
    }

    return result;
}

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

void ViewUtil::constrainView(sf::View& view, const sf::View& oldView) {
    const sf::FloatRect& newPort = view.getViewport();
    const sf::FloatRect& oldPort = oldView.getViewport();
    const sf::Vector2f m(view.getSize().x / newPort.width, view.getSize().y / newPort.height);
    const float oldBottom = oldPort.top + oldPort.height;
    const float oldRight  = oldPort.left + oldPort.width;

    if (newPort.left < oldPort.left) {
        const float d = oldPort.left - newPort.left;
        const float s = d * m.x;
        view.setSize(view.getSize() - sf::Vector2f(s, 0.f));
        view.move(s * 0.5f, 0.f);
        view.setViewport({oldPort.left, newPort.top, newPort.width - d, newPort.height});
    }

    const float newRight = newPort.left + newPort.width;
    if (newRight > oldRight) {
        const float d = newRight - oldRight;
        const float s = d * m.x;
        view.setSize(view.getSize() - sf::Vector2f(s, 0.f));
        view.move(-s * 0.5f, 0.f);
        view.setViewport({newPort.left, newPort.top, newPort.width - d, newPort.height});
    }

    if (newPort.top < oldPort.top) {
        const float d = oldPort.top - newPort.top;
        const float s = d * m.y;
        view.setSize(view.getSize() - sf::Vector2f(0.f, s));
        view.move(0.f, s * 0.5f);
        view.setViewport({newPort.left, oldPort.top, newPort.width, newPort.height - d});
    }

    const float newBottom = newPort.top + newPort.height;
    if (newBottom > oldBottom) {
        const float d = newBottom - oldBottom;
        const float s = d * m.y;
        view.setSize(view.getSize() - sf::Vector2f(0.f, s));
        view.move(0.f, -s * 0.5f);
        view.setViewport({newPort.left, newPort.top, newPort.width, newPort.height - d});
    }
}

} // namespace interface
} // namespace bl
