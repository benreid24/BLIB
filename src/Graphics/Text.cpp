#include <BLIB/Graphics/Text.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>

namespace bl
{
namespace gfx
{
Text::Text()
: font(nullptr)
, wrapType(WrapType::None)
, wordWrapWidth(-1.f)
, boundsComputedWhileDirty(false) {
    sections.reserve(4);
}

Text::~Text() {
    if (commitTask.isQueued()) { commitTask.cancel(); }
}

void Text::create(engine::World& world, const sf::VulkanFont& f, const sf::String& content,
                  unsigned int fontSize, const rc::Color& color, std::uint32_t style) {
    systems = &world.engine().systems();
    font    = &f;
    queueCommit();

    Drawable::create(world);
    Textured::create(
        world.engine().renderer(), &material(), font->syncTexture(world.engine().renderer()));
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::getOverlayScaler().setScaleCallback([this]() {
        if (wordWrapWidth > 0.f) { queueCommit(); }
    });

    const std::uint32_t vc = std::max(content.getSize(), static_cast<std::size_t>(20)) * 6;
    component().vertices.create(world.engine().renderer().vulkanState(), vc);

    sections.clear();
    addSection(content, fontSize, color, style);
}

txt::BasicText& Text::addSection(const sf::String& content, unsigned int fontSize,
                                 const rc::Color& color, std::uint32_t style) {
    txt::BasicText& t = sections.emplace_back(*this);
    t.setString(content);
    t.setFillColor(color);
    t.setCharacterSize(fontSize);
    t.setStyle(style);
    queueCommit();
    return t;
}

void Text::setFont(const sf::VulkanFont& f) {
    font = &f;
    queueCommit();
}

void Text::ensureLocalSizeUpdated() {
    const auto bounds = getLocalBounds();
    OverlayScalable::setLocalSize({bounds.width + bounds.left, bounds.height + bounds.top});
}

void Text::computeBoundsIfNeeded() const {
    if (commitTask.isQueued() && !boundsComputedWhileDirty) {
        boundsComputedWhileDirty = true;

        const_cast<Text&>(*this).computeWordWrap();

        // call commit in each section to update bounds
        glm::vec2 cornerPos(0.f, 0.f);
        for (const auto& section : sections) {
            txt::BasicText& sec = const_cast<txt::BasicText&>(section);
            sec.refreshVertices(*font, nullptr, cornerPos);
        }
    }
}

void Text::commit() {
    if (commitTask.isQueued()) { commitTask.cancel(); }
    commitTask = {};

    // word wrap
    computeWordWrap();

    // count required vertex amount
    std::uint32_t vertexCount = 0;
    glm::vec2 trash;
    for (auto& section : sections) {
        vertexCount += section.refreshVertices(*font, nullptr, trash);
    }

    // create larger buffer if required
    if (component().vertices.vertexCount() < vertexCount) {
        component().vertices.create(engine().renderer().vulkanState(), vertexCount * 2);
    }

    // assign vertices
    std::uint32_t vi = 0;
    glm::vec2 cornerPos(0.f, 0.f);
    for (auto& section : sections) {
        vi += section.refreshVertices(*font, &component().vertices.vertices()[vi], cornerPos);
    }

    // upload vertices
    component().vertices.queueTransfer(rc::tfr::Transferable::SyncRequirement::Immediate);

    const auto bounds = getLocalBounds();
    OverlayScalable::setLocalSize({bounds.width + bounds.left, bounds.height + bounds.top});

    // update draw parameters
    component().updateDrawParams(vertexCount);

    // always upload new font atlas if required
    font->syncTexture(engine().renderer());

    boundsComputedWhileDirty = false;
}

glm::vec2 Text::findCharacterPosition(unsigned int section, unsigned int index) const {
    computeBoundsIfNeeded();
    return sections[section].findCharacterPos(*font, index);
}

void Text::wordWrap(float w) {
    wrapType      = WrapType::Absolute;
    wordWrapWidth = w;
    queueCommit();
}

void Text::wordWrapToParent(float w) {
    wrapType      = WrapType::Relative;
    wordWrapWidth = w;
    queueCommit();
}

void Text::stopWordWrap() {
    wrapType      = WrapType::None;
    wordWrapWidth = -1.f;
    queueCommit();
}

void Text::computeWordWrap() {
    for (auto& section : sections) {
        section.wordWrappedContent = section.content;
        section.cachedLineHeight   = section.computeLineSpacing(*font);
    }
    if (wrapType == WrapType::None || wordWrapWidth <= 0.f) { return; }

    float pw = 1.f;
    if (wrapType == WrapType::Relative && getTransform().hasParent()) {
        const ecs::Entity parent = engine().ecs().getEntityParent(entity());
        auto cset =
            engine().ecs().getComponentSet<ecs::Require<com::OverlayScaler, com::Transform2D>>(
                parent);
        if (cset.isValid()) {
            pw = cset.get<com::OverlayScaler>()->getEntityBounds().width *
                 cset.get<com::Transform2D>()->getScale().x;
        }
    }
    const float maxWidth = wordWrapWidth * pw / getTransform().getScale().x;
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
    float minX = 100.f;
    float maxX = 0.f;
    float minY = 100.f;
    float maxY = 0.f;

    computeBoundsIfNeeded();

    for (const auto& section : sections) {
        const sf::FloatRect& bounds = section.getBounds();
        minX                        = std::min(bounds.left, minX);
        maxX                        = std::max(bounds.left + bounds.width, maxX);
        minY                        = std::min(bounds.top, minY);
        maxY                        = std::max(bounds.top + bounds.height, maxY);
    }

    return sf::FloatRect(minX, minY, maxX - minX, maxY - minY);
}

glm::vec2 Text::getLocalSize() const {
    const sf::FloatRect bounds = getLocalBounds();
    return {bounds.left + bounds.width, bounds.top + bounds.height};
}

Text::CharSearchResult Text::findCharacterAtWindowPosition(const glm::vec2& targetPos) const {
    const sf::FloatRect& targetBounds = getTargetRegion();
    if (!targetBounds.contains({targetPos.x, targetPos.y})) { return {}; }

    return findCharacterAtLocalPosition(
        {(targetPos.x - targetBounds.left) / targetBounds.width * OverlayScalable::getLocalSize().x,
         (targetPos.y - targetBounds.top) / targetBounds.height *
             OverlayScalable::getLocalSize().y});
}

Text::CharSearchResult Text::findCharacterAtPosition(const glm::vec2& position) const {
    const glm::mat4 inv = glm::inverse(getTransform().computeGlobalTransform());
    const glm::vec4 rev = inv * glm::vec4(position, 0.f, 1.f);
    return findCharacterAtLocalPosition({rev.x, rev.y});
}

Text::CharSearchResult Text::findCharacterAtLocalPosition(const glm::vec2& position) const {
    const sf::Vector2f localPos(position.x, position.y);

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

    BL_LOG_DEBUG << "Could not find character position";
    return {};
}

void Text::queueCommit() {
    boundsComputedWhileDirty = false;
    if (!commitTask.isQueued()) {
        commitTask = systems->addFrameTask(engine::FrameStage::RenderEarlyRefresh,
                                           std::bind(&Text::commit, this));
    }
}

void Text::scaleToSize(const glm::vec2& size) {
    getTransform().setScale(size / OverlayScalable::getLocalSize());
}

} // namespace gfx
} // namespace bl
