#ifndef BLIB_RENDER_DRAWABLES_TEXT_HPP
#define BLIB_RENDER_DRAWABLES_TEXT_HPP

#include <BLIB/Render/Components/Mesh.hpp>
#include <BLIB/Render/Drawables/Components/OverlayScalable.hpp>
#include <BLIB/Render/Drawables/Components/Textured.hpp>
#include <BLIB/Render/Drawables/Components/Transform2D.hpp>
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
, private base::Textured
, public base::OverlayScalable {
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

    // TODO - helper to create sections from formatted string

    /**
     * @brief Adds this entity to the given overlay
     *
     * @param overlay The overlay to add to
     * @param descriptorUpdateFreq Whether the entity is expected to be dynamic or static
     * @param parent The parent entity or InvalidEntity to make a root
     */
    void addTextToOverlay(Overlay* overlay, UpdateSpeed descriptorUpdateFreq,
                          ecs::Entity parent = ecs::InvalidEntity);

    void commit();

    // TODO - methods for bounds, character pos

    void wordWrap(float width);

    void stopWordWrap();

private:
    class Iter {
    public:
        static Iter begin(std::vector<txt::BasicText>& sections) {
            return Iter{&sections.front(), 0};
        }

        static Iter end(std::vector<txt::BasicText>& sections) {
            return Iter{&sections.back() + 1, 0};
        }

        Iter& operator++() {
            ++i;
            if (i > section->content.getSize()) {
                ++section;
                i = 0;
            }
            return *this;
        }

        constexpr bool operator==(const Iter& right) const {
            return section == right.section && i == right.i;
        }

        constexpr txt::BasicText& getText() { return *section; }
        std::uint32_t getChar() const {
            return i < section->content.getSize() ? section->content[i] : ' ';
        }
        constexpr std::uint32_t index() const { return i; }
        void makeNewline() {
            if (i < section->wordWrappedContent.getSize()) {
                section->wordWrappedContent[i] = '\n';
            }
            else { section->wordWrappedContent += "\n"; }
        }

    private:
        txt::BasicText* section;
        std::size_t i;

        Iter(txt::BasicText* section, std::size_t i)
        : section(section)
        , i(i) {}
    };

    const sf::VulkanFont* font;
    std::vector<txt::BasicText> sections;
    float wordWrapWidth;
    bool needsCommit;

    bool refreshRequired() const;
    void computeWordWrap();
    virtual void onAdd(const com::SceneObjectRef& sceneRef) override;
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
