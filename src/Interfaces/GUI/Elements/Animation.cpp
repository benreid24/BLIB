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
}

void Animation::scaleToSize(const sf::Vector2f& s) {
    size = s;
    animation.setScale({s.x / source->getMaxSize().x, s.y / source->getMaxSize().y});
    makeDirty();
}

void Animation::update(float dt) { animation.update(dt); }

sf::Vector2i Animation::minimumRequisition() const {
    return sf::Vector2i(size.value_or(source->getMaxSize()));
}

void Animation::doRender(sf::RenderTarget& target, sf::RenderStates states, const Renderer&) const {
    states.transform.translate(getAcquisition().left, getAcquisition().top);
    target.draw(animation, states);
}

} // namespace gui
} // namespace bl
