#include <BLIB/Graphics/Text.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Systems/TextSyncSystem.hpp>

namespace bl
{
namespace gfx
{
Text::Text()
: textSystem(nullptr)
, font(nullptr)
, wordWrapWidth(-1.f) {
    sections.reserve(4);
}

void Text::create(engine::Engine& engine, const sf::VulkanFont& f, const sf::String& content,
                  unsigned int fontSize, const glm::vec4& color, std::uint32_t style) {
    textSystem  = &engine.systems().getSystem<sys::TextSyncSystem>();
    font        = &f;
    needsCommit = true;

    Drawable::create(engine);
    Textured::create(engine.ecs(), entity(), font->syncTexture(engine.renderer()));
    OverlayScalable::create(engine, entity());
    component().pipeline = rc::Config::PipelineIds::Text;

    const std::uint32_t vc = std::max(content.getSize(), static_cast<std::size_t>(20)) * 6;
    component().create(engine.renderer().vulkanState(), vc, vc);

    addSection(content, fontSize, color, style);
}

txt::BasicText& Text::addSection(const sf::String& content, unsigned int fontSize,
                                 const glm::vec4& color, std::uint32_t style) {
    txt::BasicText& t = sections.emplace_back();
    t.setString(content);
    t.setFillColor(color);
    t.setCharacterSize(fontSize);
    t.setStyle(style);
    needsCommit = true;
    return t;
}

void Text::setFont(const sf::VulkanFont& f) {
    font        = &f;
    needsCommit = true;
}

bool Text::refreshRequired() const {
    if (needsCommit) return true;

    for (const auto& section : sections) {
        if (section.refreshNeeded) return true;
    }

    return false;
}

void Text::onAdd(const rc::rcom::SceneObjectRef&) {
    textSystem->registerText(this);
    commit();
}

void Text::onRemove() {
    if (textSystem) { textSystem->removeText(this); }
}

void Text::commit() {
    if (refreshRequired()) {
        needsCommit = false;

        // word wrap
        computeWordWrap();

        // count required vertex amount
        std::uint32_t vertexCount = 0;
        glm::vec2 trash;
        for (auto& section : sections) {
            vertexCount += section.refreshVertices(*font, nullptr, trash);
        }

        // create larger buffer if required
        // TODO - sync destruction?
        if (component().gpuBuffer.vertices().size() < vertexCount) {
            component().create(engine().renderer().vulkanState(), vertexCount * 2, vertexCount * 2);
            for (unsigned int i = 0; i < component().gpuBuffer.indices().size(); ++i) {
                component().gpuBuffer.indices()[i] = i;
            }
        }

        // assign vertices
        std::uint32_t vi = 0;
        glm::vec2 cornerPos(0.f, 0.f);
        for (auto& section : sections) {
            vi += section.refreshVertices(*font, &component().gpuBuffer.vertices()[vi], cornerPos);
        }

        // upload vertices
        component().gpuBuffer.queueTransfer(rc::tfr::Transferable::SyncRequirement::Immediate);

        const auto bounds = getLocalBounds();
        OverlayScalable::setLocalSize({bounds.width + bounds.left, bounds.height + bounds.top});

        // update draw parameters
        component().drawParams            = component().gpuBuffer.getDrawParameters();
        component().drawParams.indexCount = vi;
        if (component().sceneRef.object) { component().syncDrawParamsToScene(); }
    }

    // always upload new font atlas if required
    font->syncTexture(engine().renderer());
}

void Text::wordWrap(float w) {
    wordWrapWidth = w;
    needsCommit   = true;
}

void Text::stopWordWrap() {
    wordWrapWidth = -1.f;
    needsCommit   = true;
}

void Text::computeWordWrap() {
    for (auto& section : sections) {
        section.wordWrappedContent = section.content;
        section.cachedLineHeight   = section.computeLineSpacing(*font);
    }
    if (wordWrapWidth <= 0.f) { return; }

    const float maxWidth = wordWrapWidth / getTransform().getScale().x;
    const Iter EndIter   = Iter::end(sections);

    glm::vec2 nextPos(0.f, 0.f);
    float maxLineHeight = 0.f;
    Iter prevSpace      = EndIter;
    glm::vec2 wordStartPos(0.f, 0.f);
    bool lineOnNextSpace   = false;
    std::uint32_t prevChar = 0;

    const auto resetLine = [this, &maxLineHeight, &prevSpace, &lineOnNextSpace, EndIter](Iter it) {
        prevSpace                     = EndIter;
        lineOnNextSpace               = false;
        it.getText().cachedLineHeight = it.getText().computeLineSpacing(*font);
        maxLineHeight                 = it.getText().cachedLineHeight;
    };

    for (Iter it = Iter::begin(sections); it != EndIter; ++it) {
        maxLineHeight = std::max(it.getText().computeLineSpacing(*font), maxLineHeight);
        it.getText().cachedLineHeight = maxLineHeight;
        glm::vec2 advance =
            it.getText().advanceCharacterPos(*font, nextPos, it.getChar(), prevChar);
        prevChar = it.getChar();

        if (it.getChar() == ' ') {
            if (lineOnNextSpace) {
                it.makeNewline();
                resetLine(it);
                wordStartPos = glm::vec2{0.f, advance.y + maxLineHeight};
            }
            else {
                prevSpace    = it;
                wordStartPos = advance;
            }
        }
        else if (it.getChar() == '\n') {
            resetLine(it);
            wordStartPos = advance;
        }
        else {
            if (nextPos.x > maxWidth) { // nextPos == prevPos here
                if (prevSpace != EndIter) {
                    prevSpace.makeNewline();
                    resetLine(it);
                    const float wordWidth = advance.x - wordStartPos.x;
                    advance               = glm::vec2{wordWidth, advance.y + maxLineHeight};
                    wordStartPos          = advance;
                }
                else { lineOnNextSpace = true; }
            }
        }

        nextPos = advance;
    }
}

sf::FloatRect Text::getLocalBounds() const {
    float minX = 0.f;
    float maxX = 0.f;
    float minY = 0.f;
    float maxY = 0.f;

    for (const auto& section : sections) {
        const sf::FloatRect& bounds = section.getBounds();
        minX                        = std::min(bounds.left, minX);
        maxX                        = std::max(bounds.left + bounds.width, maxX);
        minY                        = std::min(bounds.top, minY);
        maxY                        = std::max(bounds.top + bounds.height, maxY);
    }

    return sf::FloatRect(minX, minY, maxX - minX, maxY - minY);
}

Text::CharSearchResult Text::findCharacterAtPosition(const glm::vec2& targetPos) const {
    const sf::FloatRect& targetBounds = getTargetRegion();
    if (!targetBounds.contains({targetPos.x, targetPos.y})) { return {0, 0}; }

    const glm::vec4 overlayPos((targetPos.x - targetBounds.left) / targetBounds.width,
                               (targetPos.y - targetBounds.top) / targetBounds.height,
                               0.f,
                               1.f);
    const glm::vec2 localPosGlm = getTransform().getInverse() * overlayPos;
    const sf::Vector2f localPos(localPosGlm.x, localPosGlm.y);

    glm::vec2 nextPos(0.f, 0.f);
    std::uint32_t prevChar = 0;
    for (std::uint32_t si = 0; si < sections.size(); ++si) {
        for (std::uint32_t i = 0; i < sections[si].wordWrappedContent.getSize(); ++i) {
            const std::uint32_t curChar = sections[si].wordWrappedContent[i];
            const glm::vec2 advance =
                sections[si].advanceCharacterPos(*font, nextPos, curChar, prevChar);
            const sf::FloatRect bounds(nextPos.x,
                                       nextPos.y,
                                       advance.x - nextPos.x,
                                       sections[si].computeLineSpacing(*font));

            if (bounds.contains(localPos)) { return {si, i}; }

            nextPos  = advance;
            prevChar = curChar;
        }

        // transition to next section
        switch (prevChar) {
        case ' ':
        case '\t':
        case '\n':
            break;
        default:
            nextPos.x += sections[si].computeWhitespaceWidth(*font);
            break;
        }
    }

    BL_LOG_WARN << "Could not find character position";
    return {0, 0};
}

} // namespace gfx
} // namespace bl
