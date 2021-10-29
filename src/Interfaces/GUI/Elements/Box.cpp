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

sf::Vector2f Box::minimumRequisition() const { return packer->getRequisition(getChildren()); }

void Box::onAcquisition() { packer->pack(getAcquisition(), getChildren()); }

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
