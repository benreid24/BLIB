////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2022 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this
// software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#ifndef BLIB_GRAPHICS_TEXT_VULKANFONT_HPP
#define BLIB_GRAPHICS_TEXT_VULKANFONT_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <SFML/Graphics/Export.hpp>
#include <SFML/Graphics/Glyph.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <map>
#include <string>
#include <vector>

namespace bl
{
namespace rc
{
class Renderer;

namespace res
{
class TextureRef;
}
} // namespace rc
} // namespace bl

namespace sf
{
class InputStream;

/**
 * @brief Adaptation of sf::Font for use with Vulkan
 *
 * @ingroup Graphics
 */
class VulkanFont {
public:
    struct Info {
        std::string family;
    };

public:
    /**
     * @brief Creates an empty font
     */
    VulkanFont();

    /**
     * @brief Copies from another font
     *
     * @param copy The font to copy from
     */
    VulkanFont(const VulkanFont& copy);

    /**
     * @brief Destroys the font
     */
    ~VulkanFont();

    /**
     * @brief Loads the font from the given file
     *
     * @param filename The full file path to load from
     * @return True on success, false on error
     */
    bool loadFromFile(const std::string& filename);

    /**
     * @brief Loads the font from the given memory
     *
     * @param data The memory to load from
     * @param sizeInBytes The size of the memory, in bytes
     * @return True on success, false on error
     */
    bool loadFromMemory(const void* data, std::size_t sizeInBytes);

    /**
     * @brief Loads the font from the given stream
     *
     * @param stream The stream to load from
     * @return True on success, false on error
     */
    bool loadFromStream(InputStream& stream);

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
    const Glyph& getGlyph(Uint32 codePoint, unsigned int characterSize, bool bold,
                          float outlineThickness = 0.f) const;

    /**
     * @brief Returns whether or not the font contains the requested character
     *
     * @param codePoint The character to check for
     * @return True if the font can render the given character, false if not
     */
    bool hasGlyph(Uint32 codePoint) const;

    /**
     * @brief Returns the kerning between the given characters
     *
     * @param first The left character
     * @param second The right character
     * @param characterSize The font size
     * @param bold Whether or not the text is bold
     * @return The distance between the characters
     */
    float getKerning(Uint32 first, Uint32 second, unsigned int characterSize,
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
    const Image& getGlyphAtlas() const;

    /**
     * @brief Copies the value from another font
     *
     * @param right The font to copy
     * @return A reference to this font
     */
    VulkanFont& operator=(const VulkanFont& right);

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

    typedef std::map<Uint64, Glyph> GlyphTable; //!< Table mapping a codepoint to its glyph

    void cleanup();
    Glyph loadGlyph(Uint32 codePoint, unsigned int characterSize, bool bold,
                    float outlineThickness) const;
    IntRect findGlyphRect(unsigned int width, unsigned int height) const;
    bool setCurrentSize(unsigned int characterSize) const;

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    mutable bool needsUpload;
    mutable bl::rc::res::TextureRef vulkanTexture;

    void* m_library;   //!< Pointer to the internal library interface (it is typeless to avoid
                       //!< exposing implementation details)
    void* m_face;      //!< Pointer to the internal font face (it is typeless to avoid exposing
                       //!< implementation details)
    void* m_streamRec; //!< Pointer to the stream rec instance (it is typeless to avoid exposing
                       //!< implementation details)
    void* m_stroker;   //!< Pointer to the stroker (it is typeless to avoid exposing implementation
                       //!< details)
    int* m_refCount;   //!< Reference counter used by implicit sharing
    bool m_isSmooth;   //!< Status of the smooth filter
    Info m_info;       //!< Information about the font
    mutable GlyphTable glyphs;     //!< Table mapping code points to their corresponding glyph
    mutable Image texture;         //!< Image containing the pixels of the glyphs
    mutable unsigned int nextRow;  //!< Y position of the next new row in the texture
    mutable std::vector<Row> rows; //!< List containing the position of all the existing rows
    mutable std::vector<Uint8> m_pixelBuffer; //!< Pixel buffer holding a glyph's pixels before
                                              //!< being written to the texture
    std::vector<char> buffer;
#ifdef SFML_SYSTEM_ANDROID
    void* m_stream; //!< Asset file streamer (if loaded from file)
#endif
};

} // namespace sf

#endif
