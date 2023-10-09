#include <BLIB/Systems/TextSyncSystem.hpp>

#include <BLIB/Graphics/Text.hpp>

namespace bl
{
namespace sys
{
TextSyncSystem::TextSyncSystem() { texts.reserve(32); }

void TextSyncSystem::init(engine::Engine&) { bl::event::Dispatcher::subscribe(this); }

void TextSyncSystem::update(std::mutex&, float, float, float, float) {
    for (gfx::Text* text : texts) {
        if (text->refreshRequired()) { text->commit(); }
    }
}

void TextSyncSystem::registerText(gfx::Text* text) { texts.emplace_back(text); }

void TextSyncSystem::removeText(gfx::Text* text) {
    for (auto it = texts.begin(); it != texts.end(); ++it) {
        if (*it == text) {
            texts.erase(it);
            return;
        }
    }
}

void TextSyncSystem::observe(const rc::event::OverlayEntityScaled& event) {
    for (gfx::Text* text : texts) {
        if (text->entity() == event.entity && text->wordWrapWidth > 0.f) {
            text->needsCommit = true;
            break;
        }
    }
}

} // namespace sys
} // namespace bl
