#include <BLIB/Interfaces/GUI/Elements/Box.hpp>

namespace bl
{
namespace gui
{
Box::Ptr Box::create(Packer::Ptr packer) {
    auto p = Ptr(new Box(packer));
    return p;
}

Box::Box(Packer::Ptr packer)
: Container()
, packer(packer)
, computeView(true) {}

void Box::setConstrainView(bool c) { computeView = c; }

void Box::setPacker(Packer::Ptr p) {
    packer = p;
    makeDirty();
}

sf::Vector2f Box::minimumRequisition() const {
    const float outline = renderSettings().outlineThickness.value_or(1.f);
    return packer->getRequisition(getChildren()) + 2.f * sf::Vector2f(outline, outline);
}

void Box::onAcquisition() {
    const float outline = renderSettings().outlineThickness.value_or(2.f);
    sf::FloatRect area(getAcquisition().left + outline,
                       getAcquisition().top + outline,
                       getAcquisition().width - outline * 2.f,
                       getAcquisition().height - 2.f * outline);
    packer->pack(area, getChildren());
}

void Box::pack(Element::Ptr e) { add(e); }

void Box::pack(Element::Ptr e, bool fx, bool fy) {
    e->setExpandsWidth(fx);
    e->setExpandsHeight(fy);
    add(e);
}

void Box::doRender(sf::RenderTarget& target, sf::RenderStates states,
                   const Renderer& renderer) const {
    renderer.renderBox(target, states, *this);
    renderChildren(target, states, renderer, computeView);
}

} // namespace gui
} // namespace bl
