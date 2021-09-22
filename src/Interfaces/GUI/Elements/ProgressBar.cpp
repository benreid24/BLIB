#include <BLIB/Interfaces/GUI/Elements/ProgressBar.hpp>

namespace bl
{
namespace gui
{
ProgressBar::Ptr ProgressBar::create(FillDirection dir) { return Ptr(new ProgressBar(dir)); }

ProgressBar::ProgressBar(FillDirection dir)
: Element()
, direction(dir)
, progress(0) {}

void ProgressBar::setProgress(float p) {
    progress = p;
    if (progress < 0) progress = 0;
    if (progress > 1) progress = 1;
}

float ProgressBar::getProgress() const { return progress; }

ProgressBar::FillDirection ProgressBar::getFillDirection() const { return direction; }

sf::Vector2i ProgressBar::minimumRequisition() const { return {5, 5}; }

void ProgressBar::doRender(sf::RenderTarget& target, sf::RenderStates states,
                           const Renderer& renderer) const {
    renderer.renderProgressBar(target, states, *this);
}

} // namespace gui
} // namespace bl