#include <BLIB/Interfaces/GUI/Elements/Animation.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

namespace bl
{
namespace gui
{

Animation::Ptr Animation::create(resource::Ref<gfx::a2d::AnimationData> anim) {
    return Ptr(new Animation(anim));
}

Animation::Animation(resource::Ref<gfx::a2d::AnimationData> anim)
: Element()
, centered(false)
, source(anim) {
    const auto updatePos = [this](const Event&, Element*) {
        const sf::Vector2f offset =
            centered ? sf::Vector2f{getAcquisition().width * 0.5f, getAcquisition().height * 0.5f} :
                       sf::Vector2f{0.f, 0.f};
        if (getComponent()) {
            getComponent()->onMove(getPosition() + offset,
                                   getPosition() - getWindowOrGuiParent().getPosition());
        }
    };
    getSignal(Event::AcquisitionChanged).willAlwaysCall(updatePos);
    getSignal(Event::Moved).willAlwaysCall(updatePos);
}

void Animation::setAnimation(resource::Ref<gfx::a2d::AnimationData> src) {
    source = src;
    makeDirty();
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

void Animation::scaleToSize(const sf::Vector2f& s) {
    size = s;
    if (getComponent()) { getComponent()->onElementUpdated(); }
    makeDirty();
}

sf::Vector2f Animation::minimumRequisition() const { return size.value_or(source->getMaxSize()); }

rdr::Component* Animation::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Animation>(*this, getWindowOrGuiParentComponent());
}

} // namespace gui
} // namespace bl
