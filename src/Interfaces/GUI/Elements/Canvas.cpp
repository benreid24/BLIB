#include <BLIB/Interfaces/GUI/Elements/Canvas.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

namespace bl
{
namespace gui
{
Canvas::Ptr Canvas::create(unsigned int w, unsigned int h, rc::SceneRef scene) {
    return Ptr(new Canvas(w, h, scene));
}

Canvas::Canvas(unsigned int w, unsigned int h, rc::SceneRef scene)
: Element()
, textureSize(w, h)
, scene(scene)
, fillAcq(false)
, maintainAR(true)
, scale(1.f, 1.f)
, offset(0.f, 0.f) {
    getSignal(Event::AcquisitionChanged).willAlwaysCall(std::bind(&Canvas::setScale, this));
}

void Canvas::resize(unsigned int w, unsigned int h, bool resetScale) {
    textureSize.x = w;
    textureSize.y = h;
    if (resetScale) size.reset();
    setScale();
    makeDirty();
}

void Canvas::scaleToSize(const sf::Vector2f& s, bool md) {
    size = s;
    setScale();
    if (md) makeDirty();
}

void Canvas::setFillAcquisition(bool fill, bool mar) {
    fillAcq    = fill;
    maintainAR = mar;
    setScale();
    makeDirty();
}

sf::Vector2f Canvas::minimumRequisition() const { return size.value_or(sf::Vector2f(textureSize)); }

rdr::Component* Canvas::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Canvas>(
        *this, getParentComponent(), getWindowOrGuiParentComponent());
}

void Canvas::setScale() {
    const sf::Vector2f origSize(textureSize);
    sf::Vector2f area = size.value_or(origSize);
    if (fillAcq) {
        area.x = getAcquisition().width;
        area.y = getAcquisition().height;
    }
    scale.x = area.x / origSize.x;
    scale.y = area.y / origSize.y;
    if (fillAcq && maintainAR) {
        scale.x = std::min(scale.x, scale.y);
        scale.y = scale.x;
    }
    offset = RenderSettings::calculatePosition(
        getRenderSettings().horizontalAlignment.value_or(RenderSettings::Center),
        getRenderSettings().horizontalAlignment.value_or(RenderSettings::Center),
        getAcquisition(),
        {origSize.x * scale.x, origSize.y * scale.y});

    if (getComponent()) { getComponent()->onElementUpdated(); }
}

void Canvas::setScene(rc::SceneRef s) {
    if (s.get() != scene.get()) {
        scene = s;
        if (getComponent()) { getComponent()->onElementUpdated(); }
    }
}

const sf::Vector2u& Canvas::getTextureSize() const { return textureSize; }

const sf::Vector2f& Canvas::getOffset() const { return offset; }

const sf::Vector2f& Canvas::getScale() const { return scale; }

} // namespace gui
} // namespace bl
