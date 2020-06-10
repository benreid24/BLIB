#include <BLIB/GUI/Elements/ProgressBar.hpp>

namespace bl
{
namespace gui
{
ProgressBar::Ptr ProgressBar::create(FillDirection dir, const std::string& g,
                                     const std::string& i) {
    return Ptr(new ProgressBar(dir, g, i));
}

ProgressBar::ProgressBar(FillDirection dir, const std::string& g, const std::string& i)
: Element(g, i)
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
                           Renderer::Ptr renderer) const {
    renderer->renderProgressBar(target, states, *this);
}

} // namespace gui
} // namespace bl