#include <BLIB/Interfaces/GUI/Elements/Animation.hpp>

namespace bl
{
namespace gui
{
Animation::Ptr Animation::create(resource::Ref<gfx::AnimationData> anim) {
    return Ptr(new Animation(anim));
}

Animation::Animation(resource::Ref<gfx::AnimationData> anim)
: Element()
, centered(false)
, source(anim)
, animation(*anim) {
    animation.setIsLoop(true);
    animation.play();

    const auto updatePos = [this](const Event&, Element*) {
        const sf::Vector2f offset =
            centered ? sf::Vector2f{getAcquisition().width * 0.5f, getAcquisition().height * 0.5f} :
                       sf::Vector2f{0.f, 0.f};
        animation.setPosition(getPosition() + offset);
    };
    getSignal(Event::AcquisitionChanged).willAlwaysCall(updatePos);
    getSignal(Event::Moved).willAlwaysCall(updatePos);
}

void Animation::setAnimation(resource::Ref<gfx::AnimationData> src) {
    source = src;
    animation.setData(*source);
    animation.setIsLoop(true);
    animation.play();
    makeDirty();
}

void Animation::scaleToSize(const sf::Vector2f& s) {
    size = s;
    animation.setScale({s.x / source->getMaxSize().x, s.y / source->getMaxSize().y});
    makeDirty();
}

void Animation::update(float dt) {
    Element::update(dt);
    animation.update(dt);
}

sf::Vector2f Animation::minimumRequisition() const { return size.value_or(source->getMaxSize()); }

void Animation::doRender(sf::RenderTarget& target, sf::RenderStates states, const Renderer&) const {
    target.draw(animation, states);
}

} // namespace gui
} // namespace bl
