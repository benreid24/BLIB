#ifndef BLIB_RENDER_DRAWABLES_TEXT_BASICTEXT_HPP
#define BLIB_RENDER_DRAWABLES_TEXT_BASICTEXT_HPP

#include <BLIB/Render/Components/Mesh.hpp>
#include <BLIB/Render/Drawables/Drawable.hpp>
#include <BLIB/Render/Drawables/Text/VulkanFont.hpp>
#include <BLIB/Render/Primitives/Vertex.hpp>
#include <BLIB/Resources.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
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
class BasicText {
public:
    BasicText();

private:
    std::string content;
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
