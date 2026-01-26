#include <BLIB/Interfaces/GUI/Elements/Box.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

namespace bl
{
namespace gui
{
namespace
{
constexpr float DefaultOutline = 1.f;
}

Box::Ptr Box::create(Packer::Ptr packer) {
    auto p = Ptr(new Box(packer));
    return p;
}

Box::Box(Packer::Ptr packer)
: Container()
, packer(packer)
, computeView(true) {}

void Box::setConstrainView(bool c) {
    computeView = c;
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

bool Box::isViewConstrained() const { return computeView; }

void Box::setPacker(Packer::Ptr p) {
    packer = p;
    makeDirty();
}

sf::Vector2f Box::minimumRequisition() const {
    const float outline = renderSettings().outlineThickness.value_or(DefaultOutline);
    return packer->getRequisition(getChildren()) + 2.f * sf::Vector2f(outline, outline);
}

void Box::onAcquisition() {
    const float outline = renderSettings().outlineThickness.value_or(DefaultOutline);
    sf::FloatRect area(
        {getAcquisition().position.x + outline, getAcquisition().position.y + outline},
        {getAcquisition().size.x - outline * 2.f, getAcquisition().size.y - outline * 2.f});
    packer->pack(area, getChildren());
}

void Box::pack(Element::Ptr e) { add(e); }

void Box::pack(Element::Ptr e, bool fx, bool fy) {
    e->setExpandsWidth(fx);
    e->setExpandsHeight(fy);
    add(e);
}

rdr::Component* Box::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Box>(*this);
}

} // namespace gui
} // namespace bl
