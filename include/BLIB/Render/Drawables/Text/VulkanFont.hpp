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

#ifndef BLIB_RENDER_DRAWABLES_TEXT_VULKANFONT_HPP
#define BLIB_RENDER_DRAWABLES_TEXT_VULKANFONT_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/Export.hpp>
#include <SFML/Graphics/Glyph.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <map>
#include <string>
#include <vector>

namespace sf
{
class InputStream;

/**
 * @brief Adaptation of sf::Font for use with Vulkan
 *
 * @ingroup Renderer
 */
class VulkanFont {
public:
    struct Info {
        std::string family;
    };

public:
    VulkanFont();

    VulkanFont(const VulkanFont& copy);

    ~VulkanFont();

    bool loadFromFile(const std::string& filename);

    bool loadFromMemory(const void* data, std::size_t sizeInBytes);

    bool loadFromStream(InputStream& stream);

    const Info& getInfo() const;

    const Glyph& getGlyph(Uint32 codePoint, unsigned int characterSize, bool bold,
                          float outlineThickness = 0) const;

    bool hasGlyph(Uint32 codePoint) const;

    float getKerning(Uint32 first, Uint32 second, unsigned int characterSize,
                     bool bold = false) const;

    float getLineSpacing(unsigned int characterSize) const;

    float getUnderlinePosition(unsigned int characterSize) const;

    float getUnderlineThickness(unsigned int characterSize) const;

    const Image& getTexture(unsigned int characterSize) const;

    void setSmooth(bool smooth);

    bool isSmooth() const;

    VulkanFont& operator=(const Font& right);

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

    struct Page {
        explicit Page(bool smooth);

        GlyphTable glyphs;     //!< Table mapping code points to their corresponding glyph
        Image texture;         //!< Image containing the pixels of the glyphs
        unsigned int nextRow;  //!< Y position of the next new row in the texture
        std::vector<Row> rows; //!< List containing the position of all the existing rows
    };

    void cleanup();

    Page& loadPage(unsigned int characterSize) const;

    Glyph loadGlyph(Uint32 codePoint, unsigned int characterSize, bool bold,
                    float outlineThickness) const;

    IntRect findGlyphRect(Page& page, unsigned int width, unsigned int height) const;

    bool setCurrentSize(unsigned int characterSize) const;

    typedef std::map<unsigned int, Page> PageTable;

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
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
    mutable PageTable m_pages; //!< Table containing the glyphs pages by character size
    mutable std::vector<Uint8> m_pixelBuffer; //!< Pixel buffer holding a glyph's pixels before
                                              //!< being written to the texture
#ifdef SFML_SYSTEM_ANDROID
    void* m_stream; //!< Asset file streamer (if loaded from file)
#endif
};

} // namespace sf

#endif
