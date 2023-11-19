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
, source(anim) {}

void Animation::setAnimation(resource::Ref<gfx::a2d::AnimationData> src) {
    source = src;
    makeDirty();
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

resource::Ref<gfx::a2d::AnimationData>& Animation::getAnimation() { return source; }

const std::optional<sf::Vector2f>& Animation::getSize() const { return size; }

void Animation::scaleToSize(const sf::Vector2f& s) {
    size = s;
    if (getComponent()) { getComponent()->onElementUpdated(); }
    makeDirty();
}

sf::Vector2f Animation::minimumRequisition() const { return size.value_or(source->getMaxSize()); }

rdr::Component* Animation::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Animation>(
        *this, getParentComponent(), getWindowOrGuiParentComponent());
}

} // namespace gui
} // namespace bl
