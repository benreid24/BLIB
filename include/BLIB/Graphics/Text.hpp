#ifndef BLIB_GRAPHICS_TEXT_HPP
#define BLIB_GRAPHICS_TEXT_HPP

#include <BLIB/Components/Text.hpp>
#include <BLIB/Engine/Systems.hpp>
#include <BLIB/Graphics/Components/OverlayScalable.hpp>
#include <BLIB/Graphics/Components/Textured.hpp>
#include <BLIB/Graphics/Components/Transform2D.hpp>
#include <BLIB/Graphics/Drawable.hpp>
#include <BLIB/Graphics/Text/BasicText.hpp>
#include <BLIB/Graphics/Text/VulkanFont.hpp>
#include <BLIB/Render/Primitives/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include <initializer_list>
#include <vector>

namespace bl
{
namespace engine
{
class Engine;
}

namespace gfx
{
/**
 * @brief Drawable component for format-able text. Similar to sf::Text except that multiple
 * formatted sections are able to be handled by a single instance
 *
 * @ingroup Graphics
 */
class Text
: public Drawable<com::Text>
, private bcom::Textured
, public bcom::OverlayScalable {
public:
    /**
     * @brief Basic struct representing a lookup from position to character
     */
    struct CharSearchResult {
        CharSearchResult()
        : found(false)
        , sectionIndex(0)
        , characterIndex(0) {}

        CharSearchResult(std::uint32_t sectionIndex, std::uint32_t characterIndex)
        : found(true)
        , sectionIndex(sectionIndex)
        , characterIndex(characterIndex) {}

        bool found;
        std::uint32_t sectionIndex;
        std::uint32_t characterIndex;
    };

    /**
     * @brief Creates an empty Text
     */
    Text();

    /**
     * @brief Cleans up
     */
    virtual ~Text();

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
    void create(engine::Engine& engine, const sf::VulkanFont& font, const sf::String& content,
                unsigned int fontSize, const sf::Color& color,
                std::uint32_t style = sf::Text::Regular) {
        create(engine, font, content, fontSize, sfcol(color), style);
    }

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
    void create(engine::Engine& engine, const sf::VulkanFont& font, const sf::String& content,
                unsigned int fontSize, std::initializer_list<float> color,
                std::uint32_t style = sf::Text::Regular) {
        create(engine, font, content, fontSize, sfcol(color), style);
    }

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
    txt::BasicText& addSection(const sf::String& content, unsigned int fontSize,
                               const sf::Color& color, std::uint32_t style = sf::Text::Regular) {
        return addSection(content, fontSize, sfcol(color), style);
    }

    /**
     * @brief Returns the bounding rectangle of the text in pre-transform space
     */
    sf::FloatRect getLocalBounds() const;

    /**
     * @brief Returns localBounds size plus position to give total size
     */
    glm::vec2 getLocalSize() const;

    /**
     * @brief Searches for the character that contains the given window position
     *
     * @param targetPos Position to search for. In window space
     * @return The found character index and section index
     */
    CharSearchResult findCharacterAtWindowPosition(const glm::vec2& targetPos) const;

    /**
     * @brief Searches for the character that contains the given position
     *
     * @param targetPos Position to search for. In world space
     * @return The found character index and section index
     */
    CharSearchResult findCharacterAtPosition(const glm::vec2& position) const;

    /**
     * @brief Searches for the character that contains the given position
     *
     * @param targetPos Position to search for. In text local space
     * @return The found character index and section index
     */
    CharSearchResult findCharacterAtLocalPosition(const glm::vec2& position) const;

    /**
     * @brief Returns the text-local position of the character at the given section and index
     *
     * @param section The index of the section to search in
     * @param index The index of the character to get the position of
     * @return The text-local position of the given character
     */
    glm::vec2 findCharacterPosition(unsigned int section, unsigned int index) const;

    /**
     * @brief Adds newlines as required to limit the text width. Adjusts as scale and contents
     *        change, as well as on parent changes
     *
     * @param width The post-transform max width to take up
     */
    void wordWrap(float width);

    /**
     * @brief Word wraps to a normalized width relative to the parent component
     *
     * @param width Normalized width with respect to the parent entity
     */
    void wordWrapToParent(float width);

    /**
     * @brief Stops word wrapping the text. Reverts to the original content
     */
    void stopWordWrap();

    /**
     * @brief Updates the text vertices. Called automatically as required
     */
    void commit();

    /**
     * @brief Helper method to set the scale so that the entity is a certain size
     *
     * @param size The size to scale to
     */
    virtual void scaleToSize(const glm::vec2& size) override;

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

    enum struct WrapType { None, Absolute, Relative };

    engine::Systems* systems;
    const sf::VulkanFont* font;
    std::vector<txt::BasicText> sections;
    WrapType wrapType;
    float wordWrapWidth;
    engine::Systems::TaskHandle commitTask;
    mutable bool boundsComputedWhileDirty;

    void queueCommit();
    void computeWordWrap();
    virtual void onAdd(const rc::rcom::SceneObjectRef& sceneRef) override;
    virtual void onRemove() override;
    virtual void ensureLocalSizeUpdated() override;
    void computeBoundsIfNeeded() const;

    friend class txt::BasicText;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const sf::VulkanFont& Text::getFont() const { return *font; }

inline txt::BasicText& Text::getSection(unsigned int i) { return sections[i]; }

inline const txt::BasicText& Text::getSection(unsigned int i) const { return sections[i]; }

} // namespace gfx
} // namespace bl

#endif
