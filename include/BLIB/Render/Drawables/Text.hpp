#ifndef BLIB_RENDER_DRAWABLES_TEXT_HPP
#define BLIB_RENDER_DRAWABLES_TEXT_HPP

#include <BLIB/Render/Components/Mesh.hpp>
#include <BLIB/Render/Drawables/Components/Textured.hpp>
#include <BLIB/Render/Drawables/Components/Transform2D.hpp>
#include <BLIB/Render/Drawables/Components/Viewport.hpp>
#include <BLIB/Render/Drawables/Drawable.hpp>
#include <BLIB/Render/Drawables/Text/BasicText.hpp>
#include <BLIB/Render/Drawables/Text/VulkanFont.hpp>
#include <SFML/Graphics/Text.hpp>
#include <vector>

namespace bl
{
namespace engine
{
class Engine;
}

namespace render
{
namespace draw
{
class Text
: public Drawable<com::Mesh>
, public base::Textured
, public base::Transform2D
, public base::Viewport {
public:
    Text();

    Text(const sf::VulkanFont& font);

    void create(engine::Engine& engine, const sf::VulkanFont& font, const sf::String& content = {},
                unsigned int fontSize = 18, const glm::vec4& color = {0.f, 0.f, 0.f, 1.f},
                std::uint32_t style = sf::Text::Regular);

    void setFont(const sf::VulkanFont& font);

    constexpr const sf::VulkanFont& getFont() const;

    constexpr txt::BasicText& getSection(unsigned int i = 0);

    constexpr const txt::BasicText& getSection(unsigned int i = 0) const;

    txt::BasicText& addSection(const sf::String& content = {}, unsigned int fontSize = 18,
                               const glm::vec4& color = {0.f, 0.f, 0.f, 1.f},
                               std::uint32_t style    = sf::Text::Regular);

    void commit();

    // TODO - methods for bounds, character pos

    // TODO - word wrap here or in BasicText

private:
    const sf::VulkanFont* font;
    std::vector<txt::BasicText> sections;
    bool needsCommit;

    bool refreshRequired() const;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const sf::VulkanFont& Text::getFont() const { return *font; }

inline constexpr txt::BasicText& Text::getSection(unsigned int i) { return sections[i]; }

inline constexpr const txt::BasicText& Text::getSection(unsigned int i) const {
    return sections[i];
}

} // namespace draw
} // namespace render
} // namespace bl

#endif
