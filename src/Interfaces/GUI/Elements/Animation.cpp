#include <BLIB/Interfaces/GUI/Elements/Animation.hpp>

namespace bl
{
namespace gui
{
Animation::Ptr Animation::create(resource::Resource<gfx::AnimationData>::Ref anim,
                                 const std::string& group, const std::string& id) {
    return Ptr(new Animation(anim, group, id));
}

Animation::Animation(resource::Resource<gfx::AnimationData>::Ref anim, const std::string& group,
                     const std::string& id)
: Element(group, id)
, source(anim)
, animation(*anim) {
    animation.setIsLoop(true);
    animation.setIsCentered(false);
    animation.play();

    getSignal(Action::AcquisitionChanged).willAlwaysCall([this](const Action&, Element*) {
        animation.setPosition(
            {static_cast<float>(getAcquisition().left), static_cast<float>(getAcquisition().top)});
    });
}

void Animation::setAnimation(resource::Resource<gfx::AnimationData>::Ref src) {
    source = src;
    animation.setData(*source);
    animation.setIsLoop(true);
    animation.setIsCentered(false);
    animation.play();
}

void Animation::scaleToSize(const sf::Vector2f& s) {
    size = s;
    makeDirty();
}

void Animation::update(float dt) { animation.update(dt); }

sf::Vector2i Animation::minimumRequisition() const {
    return sf::Vector2i(size.value_or(source->getMaxSize()));
}

void Animation::doRender(sf::RenderTarget& target, sf::RenderStates states, const Renderer&) const {
    if (size.has_value()) {
        const sf::Vector2f& s = size.value();
        states.transform.scale({s.x / source->getMaxSize().x, s.y / source->getMaxSize().y});
    }
    target.draw(animation, states);
}

} // namespace gui
} // namespace bl
