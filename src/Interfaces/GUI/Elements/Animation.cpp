#include <BLIB/Interfaces/GUI/Elements/Animation.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

namespace bl
{
namespace gui
{

Animation::Ptr Animation::create(as::TypedRef<asi::Animation2DSetPayload> anim) {
    return Ptr(new Animation(anim));
}

Animation::Animation(as::TypedRef<asi::Animation2DSetPayload> anim)
: Element()
, source(anim) {}

void Animation::setAnimation(as::TypedRef<asi::Animation2DSetPayload> src) {
    source = src;
    makeDirty();
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

as::TypedRef<asi::Animation2DSetPayload> Animation::getAnimation() { return source; }

const std::optional<sf::Vector2f>& Animation::getSize() const { return size; }

void Animation::scaleToSize(const sf::Vector2f& s) {
    size = s;
    if (getComponent()) { getComponent()->onElementUpdated(); }
    makeDirty();
}

sf::Vector2f Animation::minimumRequisition() const {
    const auto ms = source->getMaxSize();
    return size.value_or({ms.x, ms.y});
}

rdr::Component* Animation::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<Animation>(*this);
}

} // namespace gui
} // namespace bl
