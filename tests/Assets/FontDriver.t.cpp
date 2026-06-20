#include <BLIB/Assets/Builtin/FontPayload.hpp>
#include <BLIB/Assets/Drivers/FontDriver.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <BLIB/Interfaces/GUI/Font.hpp>
#include <gtest/gtest.h>

#include "Common.hpp"

namespace bl
{
namespace as
{
namespace unittest
{
TEST_F(RepositoryTest, FontDriver) {
    std::span<const char> fontData = gui::Font::getData();
    ASSERT_GT(fontData.size(), 0u);

    util::UUID uuid;
    {
        Repository repo(Mode::Editor, AssetDirectory);

        asi::FontDriver::CreateParams params(fontData);
        auto asset = repo.createAsset<asi::FontPayload>("TestFont", params);
        ASSERT_TRUE(asset.isValid());
        ASSERT_TRUE(repo.exportRepository(BundleDirectory));
        uuid = asset.getUUID();
    }

    Repository repo(Mode::Game, BundleDirectory);
    ASSERT_TRUE(repo.loadRepository());

    auto asset = repo.getTypedAsset<asi::FontPayload>(uuid);
    ASSERT_TRUE(asset.isValid());

    const asi::FontPayload& font = *asset;

    // Verify the font can serve glyphs for standard printable ASCII characters
    constexpr unsigned int charSize = 16;
    for (std::uint32_t c = 32; c < 127; ++c) {
        EXPECT_TRUE(font.hasGlyph(c)) << "Missing glyph for character " << c;
        const sf::Glyph& glyph = font.getGlyph(c, charSize, false);
        // A valid glyph's texture rect should have non-negative dimensions
        EXPECT_GE(glyph.textureRect.size.x, 0) << "Negative glyph width for char " << c;
        EXPECT_GE(glyph.textureRect.size.y, 0) << "Negative glyph height for char " << c;
    }

    EXPECT_GT(font.getLineSpacing(charSize), 0.f);
}

} // namespace unittest
} // namespace as
} // namespace bl
