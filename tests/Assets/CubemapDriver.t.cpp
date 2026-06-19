#include <BLIB/Assets/Builtin/CubemapPayload.hpp>
#include <BLIB/Assets/Builtin/ImagePayload.hpp>
#include <BLIB/Assets/Builtin/TexturePayload.hpp>
#include <BLIB/Assets/Drivers/CubemapDriver.hpp>
#include <BLIB/Assets/Drivers/ImageDriver.hpp>
#include <BLIB/Assets/Drivers/TextureDriver.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <gtest/gtest.h>

#include "Common.hpp"

#include <array>

namespace bl
{
namespace as
{
namespace unittest
{
TEST_F(RepositoryTest, CubemapDriver) {
    // One distinct colour per face for unambiguous per-face verification
    constexpr std::array<sf::Color, 6> faceColors = {
        sf::Color::Red,     // top
        sf::Color::Green,   // bottom
        sf::Color::Blue,    // left
        sf::Color::Yellow,  // right
        sf::Color::Cyan,    // front
        sf::Color::Magenta  // back
    };
    constexpr std::array<const char*, 6> faceNames = {
        "CubemapTop", "CubemapBottom", "CubemapLeft", "CubemapRight", "CubemapFront", "CubemapBack"
    };

    util::UUID cubemapUuid;
    {
        Repository repo(Mode::Editor, AssetDirectory);

        std::array<util::UUID, 6> texUuids;
        for (int i = 0; i < 6; ++i) {
            sf::Image img;
            img.resize({4, 4}, faceColors[i]);

            asi::ImageDriver::CreateParams imageParams;
            imageParams.sourceImage = &img;
            auto imageAsset = repo.createAsset<asi::ImagePayload>(
                std::string(faceNames[i]) + "_img", imageParams);
            ASSERT_TRUE(imageAsset.isValid());

            asi::TexturePayload::CreateData texData(imageAsset.getUUID());
            auto texAsset = repo.createAsset<asi::TexturePayload>(
                std::string(faceNames[i]) + "_tex", texData);
            ASSERT_TRUE(texAsset.isValid());
            texUuids[i] = texAsset.getUUID();
        }

        asi::CubemapPayload::CreateParams params(
            texUuids[0], texUuids[1], texUuids[2], texUuids[3], texUuids[4], texUuids[5]);
        auto cubemapAsset = repo.createAsset<asi::CubemapPayload>("TestCubemap", params);
        ASSERT_TRUE(cubemapAsset.isValid());
        ASSERT_TRUE(repo.exportRepository(BundleDirectory));
        cubemapUuid = cubemapAsset.getUUID();
    }

    Repository repo(Mode::Game, BundleDirectory);
    ASSERT_TRUE(repo.loadRepository());

    auto cubemapAsset = repo.getTypedAsset<asi::CubemapPayload>(cubemapUuid);
    ASSERT_TRUE(cubemapAsset.isValid());

    const asi::CubemapPayload& cubemap = *cubemapAsset;

    const std::array<const as::Dependency<asi::TexturePayload>*, 6> faces = {
        &cubemap.top, &cubemap.bottom, &cubemap.left,
        &cubemap.right, &cubemap.front, &cubemap.back
    };

    for (int i = 0; i < 6; ++i) {
        ASSERT_TRUE(faces[i]->isValid()) << "Face " << faceNames[i] << " is not valid";
        const sf::Image& img = faces[i]->get().image.get().get();
        ASSERT_EQ(img.getSize(), (sf::Vector2u{4, 4})) << "Face " << faceNames[i] << " wrong size";
        EXPECT_EQ(img.getPixel({0, 0}), faceColors[i]) << "Face " << faceNames[i] << " wrong colour";
    }
}

} // namespace unittest
} // namespace as
} // namespace bl
