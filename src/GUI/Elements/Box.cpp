#include <BLIB/GUI/Elements/Box.hpp>

namespace bl
{
namespace gui
{
Box::Ptr Box::create(Packer::Ptr packer, const std::string& group, const std::string& id) {
    return Ptr(new Box(packer, group, id));
}

Box::Box(Packer::Ptr packer, const std::string& group, const std::string& id)
: Container(group, id)
, packer(packer) {}

void Box::setPacker(Packer::Ptr p) {
    packer = p;
    makeDirty();
}

sf::Vector2i Box::minimumRequisition() const {
    return packer->getRequisition(getPackableChildren());
}

void Box::onAcquisition() {
    packer->pack({0, 0, getAcquisition().width, getAcquisition().height},
                 getPackableChildren());
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
    renderChildren(target, states, renderer);
}

} // namespace gui
} // namespace bl