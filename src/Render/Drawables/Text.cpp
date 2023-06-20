#include <BLIB/Render/Drawables/Text.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Render/Scenes/StagePipelines.hpp>

namespace bl
{
namespace render
{
namespace draw
{
namespace
{
const scene::StagePipelines Pipelines =
    scene::StagePipelineBuilder()
        .withPipeline(Config::SceneObjectStage::OpaquePass, Config::PipelineIds::Text)
        .build();
}

Text::Text()
: font(nullptr)
, wordWrapWidth(-1.f) {
    sections.reserve(4);
}

Text::Text(const sf::VulkanFont& f)
: Text() {
    font = &f;
}

void Text::create(engine::Engine& engine, const sf::VulkanFont& f, const sf::String& content,
                  unsigned int fontSize, const glm::vec4& color, std::uint32_t style) {
    font        = &f;
    needsCommit = true;

    Drawable::create(engine);
    Textured::create(engine.ecs(), entity(), font->syncTexture(engine.renderer()));
    OverlayScalable::create(engine, entity());

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

void Text::onAdd(const com::SceneObjectRef&) {
    commit(); // TODO - need to sync on change too
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
        if (component().vertices.size() < vertexCount) {
            component().create(engine().renderer().vulkanState(), vertexCount * 2, vertexCount * 2);
        }

        // assign indices
        for (unsigned int i = 0; i < component().indices.size(); ++i) {
            component().indices[i] = i;
        }

        // assign vertices
        std::uint32_t vi = 0;
        glm::vec2 cornerPos(0.f, 0.f);
        for (auto& section : sections) {
            vi += section.refreshVertices(*font, &component().vertices[vi], cornerPos);
        }

        // upload vertices
        component().gpuBuffer.sendToGPU();

        // TODO - get full bounds
        const auto& bounds = getSection().getBounds();
        OverlayScalable::setLocalSize({bounds.width + bounds.left, bounds.height + bounds.top});

        // update draw parameters
        component().drawParams            = component().gpuBuffer.getDrawParameters();
        component().drawParams.indexCount = vi;
        if (component().sceneRef.object) { component().syncToScene(); }
    }

    // always upload new font atlas if required
    font->syncTexture(engine().renderer());
}

void Text::addTextToOverlay(Overlay* overlay, UpdateSpeed descriptorUpdateFreq,
                            ecs::Entity parent) {
    Drawable::addToOverlayWithCustomPipelines(overlay, descriptorUpdateFreq, Pipelines, parent);
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

    // TODO - word wrap again on transform change
    const float maxWidth = 350.f; // wordWrapWidth / getTransform().getScale().x;
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

        // TODO - how to handle tabs?
        if (it.getChar() == ' ') {
            if (lineOnNextSpace) {
                it.set('\n');
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
                    prevSpace.set('\n');
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

} // namespace draw
} // namespace render
} // namespace bl
