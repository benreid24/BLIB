#include <BLIB/Render/Systems/TextSyncSystem.hpp>

#include <BLIB/Render/Drawables/Text.hpp>

namespace bl
{
namespace render
{
namespace sys
{
TextSyncSystem::TextSyncSystem() { texts.reserve(32); }

void TextSyncSystem::init(engine::Engine&) { bl::event::Dispatcher::subscribe(this); }

void TextSyncSystem::update(std::mutex&, float) {
    for (draw::Text* text : texts) {
        if (text->refreshRequired()) { text->commit(); }
    }
}

void TextSyncSystem::registerText(draw::Text* text) { texts.emplace_back(text); }

void TextSyncSystem::removeText(draw::Text* text) {
    for (auto it = texts.begin(); it != texts.end(); ++it) {
        if (*it == text) {
            texts.erase(it);
            return;
        }
    }
}

void TextSyncSystem::observe(const event::OverlayEntityScaled& event) {
    for (draw::Text* text : texts) {
        if (text->entity() == event.entity && text->wordWrapWidth > 0.f) {
            text->needsCommit = true;
            break;
        }
    }
}

} // namespace sys
} // namespace render
} // namespace bl
