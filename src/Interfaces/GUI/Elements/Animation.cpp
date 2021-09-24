#include <BLIB/Interfaces/GUI/Elements/Animation.hpp>

namespace bl
{
namespace gui
{
Animation::Ptr Animation::create(resource::Resource<gfx::AnimationData>::Ref anim, bool c) {
    return Ptr(new Animation(anim, c));
}

Animation::Animation(resource::Resource<gfx::AnimationData>::Ref anim, bool c)
: Element()
, centered(c)
, source(anim)
, animation(*anim) {
    animation.setIsLoop(true);
    animation.setIsCentered(centered);
    animation.play();

    getSignal(Event::AcquisitionChanged).willAlwaysCall([this](const Event&, Element*) {
        const sf::Vector2f offset =
            centered ? sf::Vector2f{static_cast<float>(getAcquisition().width) * 0.5f,
                                    static_cast<float>(getAcquisition().height) * 0.5f} :
                       sf::Vector2f{0.f, 0.f};
        animation.setPosition(sf::Vector2f{static_cast<float>(getAcquisition().left),
                                           static_cast<float>(getAcquisition().top)} +
                              offset);
    });
}

void Animation::setAnimation(resource::Resource<gfx::AnimationData>::Ref src, bool c) {
    centered = c;
    source   = src;
    animation.setData(*source);
    animation.setIsLoop(true);
    animation.setIsCentered(centered);
    animation.play();
    makeDirty();
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
