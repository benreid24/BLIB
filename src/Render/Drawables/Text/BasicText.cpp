#include <BLIB/Render/Drawables/Text/BasicText.hpp>

namespace bl
{
namespace render
{
namespace draw
{
namespace txt
{
namespace
{
std::uint32_t addLine(prim::Vertex* vertices, std::uint32_t i, float lineLength, float lineTop,
                      const glm::vec4& color, float offset, float thickness,
                      float outlineThickness = 0) {
    const float top    = std::floor(lineTop + offset - (thickness / 2) + 0.5f);
    const float bottom = top + std::floor(thickness + 0.5f);

    if (vertices) {
        const glm::vec2 texCoords(1.f, 1.f);
        vertices[i] = prim::Vertex(
            glm::vec3(-outlineThickness, top - outlineThickness, 0.f), color, glm::vec2(1, 1));
        vertices[i + 1] =
            prim::Vertex(glm::vec3(lineLength + outlineThickness, top - outlineThickness, 0.f),
                         color,
                         texCoords);
        vertices[i + 2] = prim::Vertex(
            glm::vec3(-outlineThickness, bottom + outlineThickness, 0.f), color, texCoords);
        vertices[i + 3] = prim::Vertex(
            glm::vec3(-outlineThickness, bottom + outlineThickness, 0.f), color, texCoords);
        vertices[i + 4] =
            prim::Vertex(glm::vec3(lineLength + outlineThickness, top - outlineThickness, 0.f),
                         color,
                         texCoords);
        vertices[i + 5] =
            prim::Vertex(glm::vec3(lineLength + outlineThickness, bottom + outlineThickness, 0.f),
                         color,
                         texCoords);
    }
    return i + 6;
}

std::uint32_t addGlyphQuad(prim::Vertex* vertices, std::uint32_t i, glm::vec2 position,
                           const glm::vec4& color, const sf::Glyph& glyph, float italicShear) {
    constexpr float padding = 1.0;

    const float left   = glyph.bounds.left - padding;
    const float top    = glyph.bounds.top - padding;
    const float right  = glyph.bounds.left + glyph.bounds.width + padding;
    const float bottom = glyph.bounds.top + glyph.bounds.height + padding;

    const float u1 = static_cast<float>(glyph.textureRect.left) - padding;
    const float v1 = static_cast<float>(glyph.textureRect.top) - padding;
    const float u2 = static_cast<float>(glyph.textureRect.left + glyph.textureRect.width) + padding;
    const float v2 = static_cast<float>(glyph.textureRect.top + glyph.textureRect.height) + padding;

    if (vertices) {
        vertices[i] =
            prim::Vertex(glm::vec3(position.x + left - italicShear * top, position.y + top, 0.f),
                         color,
                         glm::vec2(u1, v1));
        vertices[i] =
            prim::Vertex(glm::vec3(position.x + right - italicShear * top, position.y + top, 0.f),
                         color,
                         glm::vec2(u2, v1));
        vertices[i] = prim::Vertex(
            glm::vec3(position.x + left - italicShear * bottom, position.y + bottom, 0.f),
            color,
            glm::vec2(u1, v2));
        vertices[i] = prim::Vertex(
            glm::vec3(position.x + left - italicShear * bottom, position.y + bottom, 0.f),
            color,
            glm::vec2(u1, v2));
        vertices[i] =
            prim::Vertex(glm::vec3(position.x + right - italicShear * top, position.y + top, 0.f),
                         color,
                         glm::vec2(u2, v1));
        vertices[i] = prim::Vertex(
            glm::vec3(position.x + right - italicShear * bottom, position.y + bottom, 0.f),
            color,
            glm::vec2(u2, v2));
    }

    return i + 6;
}

constexpr float letterSpacingFactor = 1.f;
constexpr float lineSpacingFactor   = 1.f;
} // namespace

BasicText::BasicText()
: style(sf::Text::Style::Regular)
, fillColor{0.f, 0.f, 0.f, 1.f}
, outlineColor(fillColor)
, fontSize(18)
, outlineThickness(0)
, refreshNeeded(true) {}

void BasicText::setString(const sf::String& s) {
    content       = s;
    refreshNeeded = true;
}

void BasicText::setStyle(std::uint32_t s) {
    style         = s;
    refreshNeeded = true;
}

void BasicText::setFillColor(const glm::vec4& c) {
    fillColor     = c;
    refreshNeeded = true;
}

void BasicText::setOutlineColor(const glm::vec4& c) {
    outlineColor  = c;
    refreshNeeded = true;
}

void BasicText::setCharacterSize(unsigned int s) {
    fontSize      = s;
    refreshNeeded = true;
}

void BasicText::setOutlineThickness(unsigned int t) {
    outlineThickness = t;
    refreshNeeded    = true;
}

std::uint32_t BasicText::refreshVertices(const sf::VulkanFont& font, prim::Vertex* vertices,
                                         const glm::vec2& cornerPos) {
    // Mark geometry as updated
    refreshNeeded = false;

    // Clear the previous geometry
    cachedBounds = sf::FloatRect();

    // No text: nothing to draw
    if (content.isEmpty()) return 0;

    // Compute values related to the text style
    const bool isBold          = style & sf::Text::Bold;
    const bool isUnderlined    = style & sf::Text::Underlined;
    const bool isStrikeThrough = style & sf::Text::StrikeThrough;
    const float italicShear    = (style & sf::Text::Italic) ? 0.209f : 0.f; // 12 degrees in radians
    const float underlineOffset    = font.getUnderlinePosition(fontSize);
    const float underlineThickness = font.getUnderlineThickness(fontSize);

    // Compute the location of the strike through dynamically
    // We use the center point of the lowercase 'x' glyph as the reference
    // We reuse the underline thickness as the thickness of the strike through as well
    const sf::FloatRect xBounds     = font.getGlyph(L'x', fontSize, isBold).bounds;
    const float strikeThroughOffset = xBounds.top + xBounds.height / 2.f;

    // Precompute the variables needed by the algorithm
    const float glyphWidth      = font.getGlyph(L' ', fontSize, isBold).advance;
    const float letterSpacing   = (glyphWidth / 3.f) * (letterSpacingFactor - 1.f);
    const float whitespaceWidth = glyphWidth + letterSpacing;
    const float lineSpacing     = font.getLineSpacing(fontSize) * lineSpacingFactor;
    float x                     = cornerPos.x;
    float y                     = cornerPos.y + static_cast<float>(fontSize);

    // Create one quad for each character
    float minX             = static_cast<float>(fontSize);
    float minY             = static_cast<float>(fontSize);
    float maxX             = 0.f;
    float maxY             = 0.f;
    std::uint32_t prevChar = 0;
    std::uint32_t vi       = 0;
    for (std::size_t i = 0; i < content.getSize(); ++i) {
        const std::uint32_t curChar = content[i];

        // Skip the \r char to avoid weird graphical issues
        if (curChar == L'\r') continue;

        // Apply the kerning offset
        x += font.getKerning(prevChar, curChar, fontSize, isBold);

        // If we're using the underlined style and there's a new line, draw a line
        if (isUnderlined && (curChar == L'\n' && prevChar != L'\n')) {
            vi = addLine(vertices, vi, x, y, fillColor, underlineOffset, underlineThickness);

            if (outlineThickness != 0) {
                vi = addLine(vertices,
                             vi,
                             x,
                             y,
                             outlineColor,
                             underlineOffset,
                             underlineThickness,
                             outlineThickness);
            }
        }

        // If we're using the strike through style and there's a new line, draw a line across all
        // characters
        if (isStrikeThrough && (curChar == L'\n' && prevChar != L'\n')) {
            vi = addLine(vertices, vi, x, y, fillColor, strikeThroughOffset, underlineThickness);

            if (outlineThickness != 0)
                vi = addLine(vertices,
                             vi,
                             x,
                             y,
                             outlineColor,
                             strikeThroughOffset,
                             underlineThickness,
                             outlineThickness);
        }

        prevChar = curChar;

        // Handle special characters
        if ((curChar == L' ') || (curChar == L'\n') || (curChar == L'\t')) {
            // Update the current bounds (min coordinates)
            minX = std::min(minX, x);
            minY = std::min(minY, y);

            switch (curChar) {
            case L' ':
                x += whitespaceWidth;
                break;
            case L'\t':
                x += whitespaceWidth * 4;
                break;
            case L'\n':
                y += lineSpacing;
                x = 0;
                break;
            }

            // Update the current bounds (max coordinates)
            maxX = std::max(maxX, x);
            maxY = std::max(maxY, y);

            // Next glyph, no need to create a quad for whitespace
            continue;
        }

        // Apply the outline
        if (outlineThickness != 0) {
            const sf::Glyph& glyph = font.getGlyph(curChar, fontSize, isBold, outlineThickness);

            // Add the outline glyph to the vertices
            vi = addGlyphQuad(vertices, vi, glm::vec2(x, y), outlineColor, glyph, italicShear);
        }

        // Extract the current glyph's description
        const sf::Glyph& glyph = font.getGlyph(curChar, fontSize, isBold);

        // Add the glyph to the vertices
        vi = addGlyphQuad(vertices, vi, glm::vec2(x, y), fillColor, glyph, italicShear);

        // Update the current bounds
        const float left   = glyph.bounds.left;
        const float top    = glyph.bounds.top;
        const float right  = glyph.bounds.left + glyph.bounds.width;
        const float bottom = glyph.bounds.top + glyph.bounds.height;

        minX = std::min(minX, x + left - italicShear * bottom);
        maxX = std::max(maxX, x + right - italicShear * top);
        minY = std::min(minY, y + top);
        maxY = std::max(maxY, y + bottom);

        // Advance to the next character
        x += glyph.advance + letterSpacing;
    }

    // If we're using outline, update the current bounds
    if (outlineThickness != 0) {
        float outline = std::abs(std::ceil(outlineThickness));
        minX -= outline;
        maxX += outline;
        minY -= outline;
        maxY += outline;
    }

    // If we're using the underlined style, add the last line
    if (isUnderlined && (x > 0)) {
        vi = addLine(vertices, vi, x, y, fillColor, underlineOffset, underlineThickness);

        if (outlineThickness != 0)
            vi = addLine(vertices,
                         vi,
                         x,
                         y,
                         outlineColor,
                         underlineOffset,
                         underlineThickness,
                         outlineThickness);
    }

    // If we're using the strike through style, add the last line across all characters
    if (isStrikeThrough && (x > 0)) {
        vi = addLine(vertices, vi, x, y, fillColor, strikeThroughOffset, underlineThickness);

        if (outlineThickness != 0) {
            vi = addLine(vertices,
                         vi,
                         x,
                         y,
                         outlineColor,
                         strikeThroughOffset,
                         underlineThickness,
                         outlineThickness);
        }
    }

    // Update the bounding rectangle
    cachedBounds.left   = minX;
    cachedBounds.top    = minY;
    cachedBounds.width  = maxX - minX;
    cachedBounds.height = maxY - minY;

    return vi;
}

glm::vec2 BasicText::findCharacterPos(const sf::VulkanFont& font, std::uint32_t index) const {
    if (index > content.getSize()) { index = content.getSize(); }

    // Precompute the variables needed by the algorithm
    const bool isBold           = style & sf::Text::Bold;
    const float glyphWidth      = font.getGlyph(L' ', fontSize, isBold).advance;
    const float letterSpacing   = (glyphWidth / 3.f) * (letterSpacingFactor - 1.f);
    const float whitespaceWidth = glyphWidth + letterSpacing;
    const float lineSpacing     = font.getLineSpacing(fontSize) * lineSpacingFactor;

    // Compute the position
    glm::vec2 position;
    std::uint32_t prevChar = 0;
    for (std::size_t i = 0; i < index; ++i) {
        const std::uint32_t curChar = content[i];

        // Apply the kerning offset
        position.x += font.getKerning(prevChar, curChar, fontSize, isBold);
        prevChar = curChar;

        // Handle special characters
        switch (curChar) {
        case ' ':
            position.x += whitespaceWidth;
            continue;
        case '\t':
            position.x += whitespaceWidth * 4;
            continue;
        case '\n':
            position.y += lineSpacing;
            position.x = 0;
            continue;
        }

        // For regular characters, add the advance offset of the glyph
        position.x += font.getGlyph(curChar, fontSize, isBold).advance + letterSpacing;
    }

    return position;
}

} // namespace txt
} // namespace draw
} // namespace render
} // namespace bl
