#ifndef BLIB_RENDER_DRAWABLES_TEXT_BASICTEXT_HPP
#define BLIB_RENDER_DRAWABLES_TEXT_BASICTEXT_HPP

#include <BLIB/Render/Components/Mesh.hpp>
#include <BLIB/Render/Drawables/Drawable.hpp>
#include <BLIB/Render/Drawables/Text/Style.hpp>
#include <BLIB/Render/Primitives/Vertex.hpp>
#include <BLIB/Resources.hpp>
#include <SFML/Graphics/Font.hpp>
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

    glm::vec2 findCharacterPos(std::uint32_t index) const;

    const sf::FloatRect& getBounds();

private:
    std::string content;
    Style style;

    bool refreshNeeded;
    sf::FloatRect cachedBounds;
    prim::Vertex* vertices;
    std::uint32_t vertexCount;

    std::uint32_t requiredVertexCount() const;
    void refreshVertices();

    friend class Text;
};

} // namespace txt
} // namespace draw
} // namespace render
} // namespace bl

#endif
