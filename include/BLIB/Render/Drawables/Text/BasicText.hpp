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

private:
    sf::String content;
    std::uint32_t style;
    glm::vec4 fillColor;
    glm::vec4 outlineColor;
    unsigned int fontSize;
    unsigned int outlineThickness;

    bool refreshNeeded;
    sf::FloatRect cachedBounds;

    std::uint32_t refreshVertices(const sf::VulkanFont& font, prim::Vertex* vertices);
    glm::vec2 findCharacterPos(const sf::VulkanFont& font, std::uint32_t index) const;
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

} // namespace txt
} // namespace draw
} // namespace render
} // namespace bl

#endif
