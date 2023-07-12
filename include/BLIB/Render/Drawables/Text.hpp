#ifndef BLIB_RENDER_DRAWABLES_TEXT_HPP
#define BLIB_RENDER_DRAWABLES_TEXT_HPP

#include <BLIB/Components/Mesh.hpp>
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

namespace rc
{
namespace sys
{
class TextSyncSystem;
}

namespace draw
{
/**
 * @brief Drawable component for format-able text. Similar to sf::Text except that multiple
 * formatted sections are able to be handled by a single instance
 *
 * @ingroup Renderer
 */
class Text
: public Drawable<com::Mesh>
, private base::Textured
, public base::OverlayScalable {
public:
    /**
     * @brief Basic struct representing a lookup from position to character
     */
    struct CharSearchResult {
        CharSearchResult(std::uint32_t sectionIndex, std::uint32_t characterIndex)
        : sectionIndex(sectionIndex)
        , characterIndex(characterIndex) {}

        std::uint32_t sectionIndex;
        std::uint32_t characterIndex;
    };

    /**
     * @brief Creates an empty Text
     */
    Text();

    /**
     * @brief Creates the text entity and components in the ECS and creates a section using the
     *        given settings. This must be called before using any other method on the Text object
     *
     * @param engine The game engine instance
     * @param font The font to use
     * @param content The string to render
     * @param fontSize The font size of the text
     * @param color The color of the text
     * @param style The style of the text
     */
    void create(engine::Engine& engine, const sf::VulkanFont& font, const sf::String& content = {},
                unsigned int fontSize = 18, const glm::vec4& color = {0.f, 0.f, 0.f, 1.f},
                std::uint32_t style = sf::Text::Regular);

    /**
     * @brief Changes the font used to render the text
     *
     * @param font The new font to use
     */
    void setFont(const sf::VulkanFont& font);

    /**
     * @brief Returns the font used by the text
     */
    constexpr const sf::VulkanFont& getFont() const;

    /**
     * @brief Returns a section of formatted text within this text
     *
     * @param i The section index
     * @return The section of formatted text
     */
    txt::BasicText& getSection(unsigned int i = 0);

    /**
     * @brief Returns a section of formatted text within this text
     *
     * @param i The section index
     * @return The section of formatted text
     */
    const txt::BasicText& getSection(unsigned int i = 0) const;

    /**
     * @brief Creates a section using the given settings
     *
     * @param engine The game engine instance
     * @param font The font to use
     * @param content The string to render
     * @param fontSize The font size of the text
     * @param color The color of the text
     * @param style The style of the text
     * @return A reference to the new text section
     */
    txt::BasicText& addSection(const sf::String& content = {}, unsigned int fontSize = 18,
                               const glm::vec4& color = {0.f, 0.f, 0.f, 1.f},
                               std::uint32_t style    = sf::Text::Regular);

    /**
     * @brief Returns the bounding rectangle of the text in pre-transform space
     */
    sf::FloatRect getLocalBounds() const;

    /**
     * @brief Searches for the character that contains the given position
     *
     * @param targetPos Position to search for. In window space
     * @return The found character index and section index
     */
    CharSearchResult findCharacterAtPosition(const glm::vec2& targetPos) const;

    /**
     * @brief Adds newlines as required to limit the text width. Adjusts as scale and content change
     *
     * @param width The post-transform max width to take up
     */
    void wordWrap(float width);

    /**
     * @brief Stops word wrapping the text. Reverts to the original content
     */
    void stopWordWrap();

    /**
     * @brief Updates the text vertices. Called automatically as required
     */
    void commit();

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

    sys::TextSyncSystem* textSystem;
    const sf::VulkanFont* font;
    std::vector<txt::BasicText> sections;
    float wordWrapWidth;
    bool needsCommit;

    bool refreshRequired() const;
    void computeWordWrap();
    virtual void onAdd(const rcom::SceneObjectRef& sceneRef) override;
    virtual void onRemove() override;

    friend class sys::TextSyncSystem;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const sf::VulkanFont& Text::getFont() const { return *font; }

inline txt::BasicText& Text::getSection(unsigned int i) { return sections[i]; }

inline const txt::BasicText& Text::getSection(unsigned int i) const { return sections[i]; }

} // namespace draw
} // namespace rc
} // namespace bl

#endif
