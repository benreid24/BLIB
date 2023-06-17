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
: font(nullptr) {
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

void Text::onAdd(const com::SceneObjectRef& si) {
    commit(); // TODO - need to sync on change too
    OverlayScalable::notifySceneAdd(si);
}

void Text::onRemove() { OverlayScalable::notifySceneRemove(); }

void Text::commit() {
    if (refreshRequired()) {
        needsCommit = false;

        // count required vertex amount
        std::uint32_t vertexCount = 0;
        for (auto& section : sections) {
            vertexCount += section.refreshVertices(*font, nullptr, {});
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
        txt::BasicText* prevSection = nullptr;
        for (auto& section : sections) {
            const std::uint32_t prevChar =
                prevSection ? (prevSection->content.isEmpty() ?
                                   0 :
                                   prevSection->content[prevSection->content.getSize() - 1]) :
                              0;
            const unsigned int fontSize =
                std::max(section.fontSize, prevSection ? prevSection->fontSize : 0);
            const bool isBold = (prevSection && (prevSection->style & sf::Text ::Bold) != 0) ||
                                (section.style & sf::Text::Bold) != 0;
            cornerPos.x += font->getKerning(
                prevChar, section.content.isEmpty() ? 0 : section.content[0], fontSize, isBold);

            vi += section.refreshVertices(*font, &component().vertices[vi], cornerPos);
            cornerPos.x += section.getBounds().width;
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

} // namespace draw
} // namespace render
} // namespace bl