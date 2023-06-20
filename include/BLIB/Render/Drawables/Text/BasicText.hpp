#ifndef BLIB_RENDER_DRAWABLES_TEXT_BASICTEXT_HPP
#define BLIB_RENDER_DRAWABLES_TEXT_BASICTEXT_HPP

#include <BLIB/Render/Components/Mesh.hpp>
#include <BLIB/Render/Drawables/Drawable.hpp>
#include <BLIB/Render/Drawables/Text/VulkanFont.hpp>
#include <BLIB/Render/Primitives/Vertex.hpp>
#include <BLIB/Resources.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/String.hpp>
#include <glm/glm.hpp>
#include <string>

namespace bl
{
namespace render
{
namespace draw
{
class Text;

namespace txt
{
/**
 * @brief Drawable for a single section of text. Similar to sf::Text. Do not use directly, use Text
 *        class to create and manage BasicText segments
 *
 * @ingroup Renderer
 */
class BasicText {
public:
    /**
     * @brief Creates empty text with sane defaults
     */
    BasicText();

    /**
     * @brief Sets the content of this text
     *
     * @param content The content to render
     */
    void setString(const sf::String& content);

    /**
     * @brief Returns the string that will be rendered
     */
    constexpr const sf::String& getString() const;

    /**
     * @brief Sets the style of the text. See sf::Text::Style
     *
     * @param style A combination of sf::Text::Style flags
     */
    void setStyle(std::uint32_t style);

    /**
     * @brief Returns the style of the rendered text
     */
    constexpr std::uint32_t getStyle() const;

    /**
     * @brief Sets the color of the text
     *
     * @param color The color of the rendered text
     */
    void setFillColor(const glm::vec4& color);

    /**
     * @brief Returns the color of the text
     */
    constexpr const glm::vec4& getFillColor() const;

    /**
     * @brief Sets the color of the outline around the text
     *
     * @param color The color of the outline if the thickness is greater than 0
     */
    void setOutlineColor(const glm::vec4& color);

    /**
     * @brief Returns the color of the text outline
     */
    constexpr const glm::vec4& getOutlineColor() const;

    /**
     * @brief Sets the character size of the text
     *
     * @param size Point size of the rendered text
     */
    void setCharacterSize(unsigned int size);

    /**
     * @brief Returns the character size of the text
     */
    constexpr unsigned int getCharacterSize() const;

    /**
     * @brief Set the outline thickness in pixels
     *
     * @param thickness The outline thickness in pixels
     */
    void setOutlineThickness(unsigned int thickness);

    /**
     * @brief Returns the outline thickness of the text in pixels
     */
    constexpr unsigned int getOutlineThickness() const;

    /**
     * @brief Returns the width of glyphs when using the given font
     *
     * @param font The font to get the glyph width for
     * @return The glyph width in local units
     */
    float computeGlyphWidth(const sf::VulkanFont& font) const;

    /**
     * @brief Returns the letter spacing when using the given font
     *
     * @param font The font to get the letter spacing for
     * @return The letter spacing in local units
     */
    float computeLetterSpacing(const sf::VulkanFont& font) const;

    /**
     * @brief Returns the width of whitespace when using the given font
     *
     * @param font The font to get the whitespace width for
     * @return The whitespace width in local units
     */
    float computeWhitespaceWidth(const sf::VulkanFont& font) const;

    /**
     * @brief Returns the line spacing when using the given font
     *
     * @param font The font to get the line spacing for
     * @return The line spacing in local units
     */
    float computeLineSpacing(const sf::VulkanFont& font) const;

private:
    sf::String content;
    sf::String wordWrappedContent;
    std::uint32_t style;
    glm::vec4 fillColor;
    glm::vec4 outlineColor;
    unsigned int fontSize;
    unsigned int outlineThickness;
    float letterSpacingFactor;
    float lineSpacingFactor;
    float cachedLineHeight;

    bool refreshNeeded;
    sf::FloatRect cachedBounds;

    std::uint32_t refreshVertices(const sf::VulkanFont& font, prim::Vertex* vertices,
                                  glm::vec2& cornerPos);
    glm::vec2 advanceCharacterPos(const sf::VulkanFont& font, glm::vec2 pos, std::uint32_t curChar,
                                  std::uint32_t prevChar) const;
    const sf::FloatRect& getBounds() const;

    friend class Text;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const sf::String& BasicText::getString() const { return content; }

inline constexpr std::uint32_t BasicText::getStyle() const { return style; }

inline constexpr const glm::vec4& BasicText::getFillColor() const { return fillColor; }

inline constexpr const glm::vec4& BasicText::getOutlineColor() const { return outlineColor; }

inline constexpr unsigned int BasicText::getCharacterSize() const { return fontSize; }

inline constexpr unsigned int BasicText::getOutlineThickness() const { return outlineThickness; }

inline const sf::FloatRect& BasicText::getBounds() const {
#ifdef BLIB_DEBUG
    if (refreshNeeded) { BL_LOG_ERROR << "Querying bounds of stale text"; }
#endif
    return cachedBounds;
}

inline float BasicText::computeGlyphWidth(const sf::VulkanFont& font) const {
    return font.getGlyph(L' ', fontSize, (style & sf::Text::Bold) != 0).advance;
}

inline float BasicText::computeLetterSpacing(const sf::VulkanFont& font) const {
    return (computeGlyphWidth(font) / 3.f) * (letterSpacingFactor - 1.f);
}

inline float BasicText::computeWhitespaceWidth(const sf::VulkanFont& font) const {
    return computeGlyphWidth(font) + computeLetterSpacing(font);
}

inline float BasicText::computeLineSpacing(const sf::VulkanFont& font) const {
    return font.getLineSpacing(fontSize) * lineSpacingFactor;
}

} // namespace txt
} // namespace draw
} // namespace render
} // namespace bl

#endif
