#include <BLIB/Interfaces/GUI/Elements/ProgressBar.hpp>

#include <BLIB/Interfaces/GUI/Renderer/Renderer.hpp>

namespace bl
{
namespace gui
{
ProgressBar::Ptr ProgressBar::create(FillDirection dir) { return Ptr(new ProgressBar(dir)); }

ProgressBar::ProgressBar(FillDirection dir)
: Element()
, direction(dir)
, progress(0.f) {}

void ProgressBar::setProgress(float p) {
    progress = p;
    if (progress < 0.f) progress = 0.f;
    if (progress > 1.f) progress = 1.f;
    if (getComponent()) { getComponent()->onElementUpdated(); }
}

float ProgressBar::getProgress() const { return progress; }

ProgressBar::FillDirection ProgressBar::getFillDirection() const { return direction; }

sf::Vector2f ProgressBar::minimumRequisition() const { return {5.f, 5.f}; }

rdr::Component* ProgressBar::doPrepareRender(rdr::Renderer& renderer) {
    return renderer.createComponent<ProgressBar>(
        *this, getParentComponent(), getWindowOrGuiParentComponent());
}

} // namespace gui
} // namespace bl
