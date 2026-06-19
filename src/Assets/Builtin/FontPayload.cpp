#include <BLIB/Assets/Builtin/FontPayload.hpp>

// TODO - consider splitting out runtime state
#include <BLIB/Render/Renderer.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H
#include FT_STROKER_H
#include <cmath>
#include <cstdlib>
#include <cstring>

namespace bl
{
namespace asi
{
namespace
{
// FreeType callbacks that operate on a sf::InputStream
unsigned long read(FT_Stream rec, unsigned long offset, unsigned char* buffer,
                   unsigned long count) {
    std::uint64_t convertedOffset = static_cast<std::uint64_t>(offset);
    sf::InputStream* stream       = static_cast<sf::InputStream*>(rec->descriptor.pointer);
    if (stream->seek(convertedOffset) == convertedOffset) {
        if (count > 0) {
            auto read =
                stream->read(reinterpret_cast<char*>(buffer), static_cast<std::uint64_t>(count));
            return read.value_or(0);
        }
        else { return 0; }
    }
    else
        return count > 0 ? 0 : 1; // error code is 0 if we're reading, or nonzero if we're seeking
}
void close(FT_Stream) {}

// Helper to interpret memory as a specific type
template<typename T, typename U>
inline T reinterpret(const U& input) {
    T output;
    std::memcpy(&output, &input, sizeof(U));
    return output;
}

// Combine outline thickness, boldness and font glyph index into a single 64-bit key
std::uint64_t combine(std::uint8_t size, float outlineThickness, bool bold, std::uint32_t index) {
    return (static_cast<std::uint64_t>(reinterpret<std::uint32_t>(outlineThickness)) << 32) |
           (static_cast<std::uint64_t>(bold) << 31) | (static_cast<std::uint64_t>(size) << 32) |
           index;
}
} // namespace

FontPayload::FontPayload(const as::Payload::ConstructContext& ctx)
: Payload(ctx)
, needsUpload(true)
, m_library(NULL)
, m_face(NULL)
, m_streamRec(NULL)
, m_stroker(NULL)
, m_isSmooth(true)
, m_info()
, nextRow(3) {
    texture.resize({128, 128}, sf::Color(255, 255, 255, 0));

    // Reserve a 2x2 white square for texturing underlines
    for (unsigned int x = 0; x < 2; ++x) {
        for (unsigned int y = 0; y < 2; ++y) {
            texture.setPixel({x, y}, sf::Color(255, 255, 255, 255));
        }
    }

#ifdef SFML_SYSTEM_ANDROID
    m_stream = NULL;
#endif
}

FontPayload::~FontPayload() { cleanup(); }

bool FontPayload::loadFromMemory() {
    needsUpload = true;

    // Cleanup the previous resources (excluding buffer that we are loading from)
    cleanup(false);

    // Initialize FreeType
    // Note: we initialize FreeType for every font instance in order to avoid having a single
    // global manager that would create a lot of issues regarding creation and destruction order.
    FT_Library library;
    if (FT_Init_FreeType(&library) != 0) {
        BL_LOG_ERROR << "Failed to load font from memory (failed to initialize FreeType)";
        return false;
    }
    m_library = library;

    // Load the new font face from the specified file
    FT_Face face;
    if (FT_New_Memory_Face(static_cast<FT_Library>(m_library),
                           reinterpret_cast<const FT_Byte*>(buffer.data()),
                           static_cast<FT_Long>(buffer.size()),
                           0,
                           &face) != 0) {
        BL_LOG_ERROR << "Failed to load font from memory (failed to create the font face)";
        return false;
    }

    // Load the stroker that will be used to outline the font
    FT_Stroker stroker;
    if (FT_Stroker_New(static_cast<FT_Library>(m_library), &stroker) != 0) {
        BL_LOG_ERROR << "Failed to load font from memory (failed to create the stroker)";
        FT_Done_Face(face);
        return false;
    }

    // Select the Unicode character map
    if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0) {
        BL_LOG_ERROR << "Failed to load font from memory (failed to set the Unicode character set)";
        FT_Stroker_Done(stroker);
        FT_Done_Face(face);
        return false;
    }

    // Store the loaded font in our ugly void* :)
    m_stroker = stroker;
    m_face    = face;

    // Store the font information
    m_info.family = face->family_name ? face->family_name : std::string();

    return true;
}

const FontPayload::Info& FontPayload::getInfo() const { return m_info; }

const sf::Glyph& FontPayload::getGlyph(std::uint32_t codePoint, unsigned int characterSize,
                                       bool bold, float outlineThickness) const {
    // Build the key by combining the glyph index (based on code point), bold flag, and outline
    // thickness
    std::uint64_t key = combine(characterSize,
                                outlineThickness,
                                bold,
                                FT_Get_Char_Index(static_cast<FT_Face>(m_face), codePoint));

    // Search the glyph into the cache
    GlyphTable::const_iterator it = glyphs.find(key);
    if (it != glyphs.end()) {
        // Found: just return it
        return it->second;
    }
    else {
        // Not found: we have to load it
        needsUpload      = true;
        sf ::Glyph glyph = loadGlyph(codePoint, characterSize, bold, outlineThickness);
        return glyphs.insert(std::make_pair(key, glyph)).first->second;
    }
}

bool FontPayload::hasGlyph(std::uint32_t codePoint) const {
    return FT_Get_Char_Index(static_cast<FT_Face>(m_face), codePoint) != 0;
}

float FontPayload::getKerning(std::uint32_t first, std::uint32_t second, unsigned int characterSize,
                              bool bold) const {
    // Special case where first or second is 0 (null character)
    if (first == 0 || second == 0) return 0.f;

    FT_Face face = static_cast<FT_Face>(m_face);

    if (face && setCurrentSize(characterSize)) {
        // Convert the characters to indices
        FT_UInt index1 = FT_Get_Char_Index(face, first);
        FT_UInt index2 = FT_Get_Char_Index(face, second);

        // Retrieve position compensation deltas generated by FT_LOAD_FORCE_AUTOHINT flag
        float firstRsbDelta  = static_cast<float>(getGlyph(first, characterSize, bold).rsbDelta);
        float secondLsbDelta = static_cast<float>(getGlyph(second, characterSize, bold).lsbDelta);

        // Get the kerning vector if present
        FT_Vector kerning;
        kerning.x = kerning.y = 0;
        if (FT_HAS_KERNING(face))
            FT_Get_Kerning(face, index1, index2, FT_KERNING_UNFITTED, &kerning);

        // X advance is already in pixels for bitmap fonts
        if (!FT_IS_SCALABLE(face)) return static_cast<float>(kerning.x);

        // Combine kerning with compensation deltas and return the X advance
        // Flooring is required as we use FT_KERNING_UNFITTED flag which is not quantized in 64
        // based grid
        return std::floor((secondLsbDelta - firstRsbDelta + static_cast<float>(kerning.x) + 32) /
                          static_cast<float>(1 << 6));
    }
    else {
        // Invalid font
        return 0.f;
    }
}

float FontPayload::getLineSpacing(unsigned int characterSize) const {
    FT_Face face = static_cast<FT_Face>(m_face);

    if (face && setCurrentSize(characterSize)) {
        return static_cast<float>(face->size->metrics.height) / static_cast<float>(1 << 6);
    }
    else { return 0.f; }
}

float FontPayload::getUnderlinePosition(unsigned int characterSize) const {
    FT_Face face = static_cast<FT_Face>(m_face);

    if (face && setCurrentSize(characterSize)) {
        // Return a fixed position if font is a bitmap font
        if (!FT_IS_SCALABLE(face)) return static_cast<float>(characterSize) / 10.f;

        return -static_cast<float>(
                   FT_MulFix(face->underline_position, face->size->metrics.y_scale)) /
               static_cast<float>(1 << 6);
    }
    else { return 0.f; }
}

float FontPayload::getUnderlineThickness(unsigned int characterSize) const {
    FT_Face face = static_cast<FT_Face>(m_face);

    if (face && setCurrentSize(characterSize)) {
        // Return a fixed thickness if font is a bitmap font
        if (!FT_IS_SCALABLE(face)) return static_cast<float>(characterSize) / 14.f;

        return static_cast<float>(
                   FT_MulFix(face->underline_thickness, face->size->metrics.y_scale)) /
               static_cast<float>(1 << 6);
    }
    else { return 0.f; }
}

const sf::Image& FontPayload::getGlyphAtlas() const { return texture; }

void FontPayload::cleanup(bool freeBuffer) {
    // Check if we must destroy the FreeType pointers
    if (m_library) {
        // Destroy the stroker
        if (m_stroker) FT_Stroker_Done(static_cast<FT_Stroker>(m_stroker));

        // Destroy the font face
        if (m_face) FT_Done_Face(static_cast<FT_Face>(m_face));

        // Destroy the stream rec instance, if any (must be done after FT_Done_Face!)
        if (m_streamRec) delete static_cast<FT_StreamRec*>(m_streamRec);

        // Close the library
        if (m_library) FT_Done_FreeType(static_cast<FT_Library>(m_library));
    }

    // Reset members
    m_library   = NULL;
    m_face      = NULL;
    m_stroker   = NULL;
    m_streamRec = NULL;
    glyphs.clear();
    rows.clear();
    nextRow     = 3;
    needsUpload = true;
    vulkanTexture.release();
    std::vector<std::uint8_t>().swap(m_pixelBuffer);
    if (freeBuffer) { buffer.clear(); }
}

sf::Glyph FontPayload::loadGlyph(std::uint32_t codePoint, unsigned int characterSize, bool bold,
                                 float outlineThickness) const {
    // The glyph to return
    sf::Glyph glyph;

    // First, transform our ugly void* to a FT_Face
    FT_Face face = static_cast<FT_Face>(m_face);
    if (!face) return glyph;

    // Set the character size
    if (!setCurrentSize(characterSize)) return glyph;

    // Load the glyph corresponding to the code point
    FT_Int32 flags = FT_LOAD_TARGET_NORMAL | FT_LOAD_FORCE_AUTOHINT;
    if (outlineThickness != 0) flags |= FT_LOAD_NO_BITMAP;
    if (FT_Load_Char(face, codePoint, flags) != 0) return glyph;

    // Retrieve the glyph
    FT_Glyph glyphDesc;
    if (FT_Get_Glyph(face->glyph, &glyphDesc) != 0) return glyph;

    // Apply bold and outline (there is no fallback for outline) if necessary -- first technique
    // using outline (highest quality)
    FT_Pos weight = 1 << 6;
    bool outline  = (glyphDesc->format == FT_GLYPH_FORMAT_OUTLINE);
    if (outline) {
        if (bold) {
            FT_OutlineGlyph outlineGlyph = reinterpret_cast<FT_OutlineGlyph>(glyphDesc);
            FT_Outline_Embolden(&outlineGlyph->outline, weight);
        }

        if (outlineThickness != 0) {
            FT_Stroker stroker = static_cast<FT_Stroker>(m_stroker);

            FT_Stroker_Set(stroker,
                           static_cast<FT_Fixed>(outlineThickness * static_cast<float>(1 << 6)),
                           FT_STROKER_LINECAP_ROUND,
                           FT_STROKER_LINEJOIN_ROUND,
                           0);
            FT_Glyph_Stroke(&glyphDesc, stroker, true);
        }
    }

    // Convert the glyph to a bitmap (i.e. rasterize it)
    // Warning! After this line, do not read any data from glyphDesc directly, use
    // bitmapGlyph.root to access the FT_Glyph data.
    FT_Glyph_To_Bitmap(&glyphDesc, FT_RENDER_MODE_NORMAL, 0, 1);
    FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyphDesc);
    FT_Bitmap& bitmap          = bitmapGlyph->bitmap;

    // Apply bold if necessary -- fallback technique using bitmap (lower quality)
    if (!outline) {
        if (bold) FT_Bitmap_Embolden(static_cast<FT_Library>(m_library), &bitmap, weight, weight);

        if (outlineThickness != 0)
            BL_LOG_ERROR << "Failed to outline glyph (no fallback available)";
    }

    // Compute the glyph's advance offset
    glyph.advance = static_cast<float>(bitmapGlyph->root.advance.x >> 16);
    if (bold) glyph.advance += static_cast<float>(weight) / static_cast<float>(1 << 6);

    glyph.lsbDelta = static_cast<int>(face->glyph->lsb_delta);
    glyph.rsbDelta = static_cast<int>(face->glyph->rsb_delta);

    unsigned int width  = bitmap.width;
    unsigned int height = bitmap.rows;

    if ((width > 0) && (height > 0)) {
        // Leave a small padding around characters, so that filtering doesn't
        // pollute them with pixels from neighbors
        const unsigned int padding = 2;

        width += 2 * padding;
        height += 2 * padding;

        // Find a good position for the new glyph into the texture
        glyph.textureRect = findGlyphRect(width, height);

        // Make sure the texture data is positioned in the center
        // of the allocated texture rectangle
        glyph.textureRect.position.x += static_cast<int>(padding);
        glyph.textureRect.position.y += static_cast<int>(padding);
        glyph.textureRect.size.x -= static_cast<int>(2 * padding);
        glyph.textureRect.size.y -= static_cast<int>(2 * padding);

        // Compute the glyph's bounding box
        glyph.bounds.position.x = static_cast<float>(bitmapGlyph->left);
        glyph.bounds.position.y = static_cast<float>(-bitmapGlyph->top);
        glyph.bounds.size.x     = static_cast<float>(bitmap.width);
        glyph.bounds.size.y     = static_cast<float>(bitmap.rows);

        // Resize the pixel buffer to the new size and fill it with transparent white pixels
        m_pixelBuffer.resize(width * height * 4);

        std::uint8_t* current = &m_pixelBuffer[0];
        std::uint8_t* end     = current + width * height * 4;

        while (current != end) {
            (*current++) = 255;
            (*current++) = 255;
            (*current++) = 255;
            (*current++) = 0;
        }

        // Extract the glyph's pixels from the bitmap
        const std::uint8_t* pixels = bitmap.buffer;
        if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
            // Pixels are 1 bit monochrome values
            for (unsigned int y = padding; y < height - padding; ++y) {
                for (unsigned int x = padding; x < width - padding; ++x) {
                    // The color channels remain white, just fill the alpha channel
                    std::size_t index = x + y * width;
                    m_pixelBuffer[index * 4 + 3] =
                        ((pixels[(x - padding) / 8]) & (1 << (7 - ((x - padding) % 8)))) ? 255 : 0;
                }
                pixels += bitmap.pitch;
            }
        }
        else {
            // Pixels are 8 bits gray levels
            for (unsigned int y = padding; y < height - padding; ++y) {
                for (unsigned int x = padding; x < width - padding; ++x) {
                    // The color channels remain white, just fill the alpha channel
                    std::size_t index            = x + y * width;
                    m_pixelBuffer[index * 4 + 3] = pixels[x - padding];
                }
                pixels += bitmap.pitch;
            }
        }

        // Write the pixels to the texture
        unsigned int x = static_cast<unsigned int>(glyph.textureRect.position.x) - padding;
        unsigned int y = static_cast<unsigned int>(glyph.textureRect.position.y) - padding;
        unsigned int w = static_cast<unsigned int>(glyph.textureRect.size.x) + 2 * padding;
        unsigned int h = static_cast<unsigned int>(glyph.textureRect.size.y) + 2 * padding;
        for (unsigned int xi = 0; xi < w; ++xi) {
            for (unsigned int yi = 0; yi < h; ++yi) {
                std::size_t index = xi + yi * width;
                texture.setPixel({x + xi, y + yi},
                                 sf::Color{255, 255, 255, m_pixelBuffer[index * 4 + 3]});
            }
        }
    }

    // Delete the FT glyph
    FT_Done_Glyph(glyphDesc);

    // Done :)
    return glyph;
}

sf::IntRect FontPayload::findGlyphRect(unsigned int width, unsigned int height) const {
    // Find the line that fits well the glyph
    Row* row        = NULL;
    float bestRatio = 0;
    for (std::vector<Row>::iterator it = rows.begin(); it != rows.end() && !row; ++it) {
        float ratio = static_cast<float>(height) / static_cast<float>(it->height);

        // Ignore rows that are either too small or too high
        if ((ratio < 0.7f) || (ratio > 1.f)) continue;

        // Check if there's enough horizontal space left in the row
        if (width > texture.getSize().x - it->width) continue;

        // Make sure that this new row is the best found so far
        if (ratio < bestRatio) continue;

        // The current row passed all the tests: we can select it
        row       = &*it;
        bestRatio = ratio;
    }

    // If we didn't find a matching row, create a new one (10% taller than the glyph)
    if (!row) {
        unsigned int rowHeight = height + height / 10;
        while ((nextRow + rowHeight >= texture.getSize().y) || (width >= texture.getSize().x)) {
            // Not enough space: resize the texture if possible
            unsigned int textureWidth  = texture.getSize().x;
            unsigned int textureHeight = texture.getSize().y;

            // Make the texture 2 times bigger
            sf::Image newTexture;
            newTexture.resize({textureWidth * 2, textureHeight * 2}, sf::Color(255, 255, 255, 0));
            newTexture.copy(texture, {0, 0});
            texture = std::move(newTexture);
        }

        // We can now create the new row
        rows.emplace_back(nextRow, rowHeight);
        nextRow += rowHeight;
        row = &rows.back();
    }

    // Find the glyph's rectangle on the selected row
    sf::IntRect rect(sf::Rect<unsigned int>({row->width, row->top}, {width, height}));

    // Update the row informations
    row->width += width;

    return rect;
}

bool FontPayload::setCurrentSize(unsigned int characterSize) const {
    // FT_Set_Pixel_Sizes is an expensive function, so we must call it
    // only when necessary to avoid killing performances

    FT_Face face          = static_cast<FT_Face>(m_face);
    FT_UShort currentSize = face->size->metrics.x_ppem;

    if (currentSize != characterSize) {
        FT_Error result = FT_Set_Pixel_Sizes(face, 0, characterSize);

        if (result == FT_Err_Invalid_Pixel_Size) {
            // In the case of bitmap fonts, resizing can
            // fail if the requested size is not available
            if (!FT_IS_SCALABLE(face)) {
                BL_LOG_ERROR << "Failed to set bitmap font size to " << characterSize;
                BL_LOG_ERROR << "Available sizes are: ";
                for (int i = 0; i < face->num_fixed_sizes; ++i) {
                    const long size = (face->available_sizes[i].y_ppem + 32) >> 6;
                    BL_LOG_ERROR << size << " ";
                }
                BL_LOG_ERROR;
            }
            else { BL_LOG_ERROR << "Failed to set font size to " << characterSize; }
        }

        return result == FT_Err_Ok;
    }

    return true;
}

bl::rc::res::TextureRef FontPayload::syncTexture(bl::rc::Renderer& renderer) const {
    if (!vulkanTexture) { vulkanTexture = renderer.texturePool().createTexture(texture); }
    else if (needsUpload) {
        needsUpload = false;
        vulkanTexture->ensureSize({texture.getSize().x, texture.getSize().y});
        vulkanTexture->update(texture);
    }
    return vulkanTexture;
}

} // namespace asi
} // namespace bl
