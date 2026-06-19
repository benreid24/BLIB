#ifndef BLIB_ASSETS_BUILTIN_FONTPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_FONTPAYLOAD_HPP

#include <BLIB/Assets/Payload.hpp>

#include <BLIB/Render/Resources/TextureRef.hpp>
#include <SFML/Graphics/Export.hpp>
#include <SFML/Graphics/Glyph.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace bl
{
namespace rc
{
class Renderer;
}

namespace asi
{
class FontDriver;

/**
 * @brief Payload for fonts. Derived from sf::Font
 *
 * @ingroup Assets
 */
class FontPayload : public as::Payload {
public:
    /**
     * @brief Contains information about the font
     */
    struct Info {
        std::string family;
    };

    /**
     * @brief Creates the font payload
     *
     * @param ctx The context to construct with
     */
    FontPayload(const as::Payload::ConstructContext& ctx);

    /**
     * @brief Destroys the payload
     */
    virtual ~FontPayload();

    /**
     * @brief Returns the font information
     */
    const Info& getInfo() const;

    /**
     * @brief Returns the glyph information for the requested character and settings
     *
     * @param codePoint The character to get the glyph for
     * @param characterSize The font size to get the glyph for
     * @param bold True for a bold glyph, false for a regular glyph
     * @param outlineThickness Thickness of the outline, in pixels
     * @return The requested glyph
     */
    const sf::Glyph& getGlyph(std::uint32_t codePoint, unsigned int characterSize, bool bold,
                              float outlineThickness = 0.f) const;

    /**
     * @brief Returns whether or not the font contains the requested character
     *
     * @param codePoint The character to check for
     * @return True if the font can render the given character, false if not
     */
    bool hasGlyph(std::uint32_t codePoint) const;

    /**
     * @brief Returns the kerning between the given characters
     *
     * @param first The left character
     * @param second The right character
     * @param characterSize The font size
     * @param bold Whether or not the text is bold
     * @return The distance between the characters
     */
    float getKerning(std::uint32_t first, std::uint32_t second, unsigned int characterSize,
                     bool bold = false) const;

    /**
     * @brief Returns the amount of space between letters for the given font size
     *
     * @param characterSize The font size
     * @return The spacing in pixels
     */
    float getLineSpacing(unsigned int characterSize) const;

    /**
     * @brief Returns the position of the underline for the given font size
     *
     * @param characterSize The font size
     * @return The y position of the underline
     */
    float getUnderlinePosition(unsigned int characterSize) const;

    /**
     * @brief Returns the thickness of the underline for the given font size
     *
     * @param characterSize The font size
     * @return The thickness of the underline
     */
    float getUnderlineThickness(unsigned int characterSize) const;

    /**
     * @brief Returns the rendered glyph atlas
     * @return
     */
    const sf::Image& getGlyphAtlas() const;

    /**
     * @brief Creates or updates the Vulkan texture for the rendered glyphs if necessary
     *
     * @param renderer The renderer instance
     * @return A handle to the Vulkan texture to use when rendering
     */
    bl::rc::res::TextureRef syncTexture(bl::rc::Renderer& renderer) const;

private:
    struct Row {
        Row(unsigned int rowTop, unsigned int rowHeight)
        : width(0)
        , top(rowTop)
        , height(rowHeight) {}

        unsigned int width;  //!< Current width of the row
        unsigned int top;    //!< Y position of the row into the texture
        unsigned int height; //!< Height of the row
    };

    typedef std::map<std::uint64_t, sf::Glyph> GlyphTable;

    bool loadFromMemory();
    void cleanup(bool freeBuffer = true);

    sf::Glyph loadGlyph(std::uint32_t codePoint, unsigned int characterSize, bool bold,
                        float outlineThickness) const;
    sf::IntRect findGlyphRect(unsigned int width, unsigned int height) const;
    bool setCurrentSize(unsigned int characterSize) const;

    ////////////////////////////////////////////////////////////
    // Runtime data
    ////////////////////////////////////////////////////////////
    mutable bool needsUpload;
    mutable bl::rc::res::TextureRef vulkanTexture;

    void* m_library;
    void* m_face;
    void* m_streamRec;
    void* m_stroker;
    bool m_isSmooth;
    Info m_info;
    mutable GlyphTable glyphs;
    mutable sf::Image texture;
    mutable unsigned int nextRow;
    mutable std::vector<Row> rows;
    mutable std::vector<std::uint8_t> m_pixelBuffer;

    ////////////////////////////////////////////////////////////
    // Underlying data. is serialized
    ////////////////////////////////////////////////////////////
    std::vector<char> buffer;

    friend class FontDriver;
};

} // namespace asi
} // namespace bl

#endif
