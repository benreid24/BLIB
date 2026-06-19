#include <BLIB/Assets/Builtin/ImagePayload.hpp>
#include <BLIB/Assets/Builtin/TexturePayload.hpp>
#include <BLIB/Assets/Drivers/ImageDriver.hpp>
#include <BLIB/Assets/Drivers/TextureDriver.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <SFML/Graphics/Image.hpp>
#include <gtest/gtest.h>

#include "Common.hpp"

namespace bl
{
namespace as
{
namespace unittest
{
TEST_F(RepositoryTest, TextureDriver) {
    util::UUID textureUuid;
    sf::Image sourceImage;
    sourceImage.resize({4, 4}, sf::Color::Blue);

    {
        Repository repo(Mode::Editor, AssetDirectory);

        asi::ImageDriver::CreateParams imageParams;
        imageParams.sourceImage = &sourceImage;
        auto imageAsset = repo.createAsset<asi::ImagePayload>("TestTextureImage", imageParams);
        ASSERT_TRUE(imageAsset.isValid());

        asi::TexturePayload::CreateData textureParams(imageAsset.getUUID());
        textureParams.type       = asi::TexturePayload::NormalMap;
        textureParams.colorSpace = asi::TexturePayload::Linear;
        auto textureAsset = repo.createAsset<asi::TexturePayload>("TestTexture", textureParams);
        ASSERT_TRUE(textureAsset.isValid());
        textureUuid = textureAsset.getUUID();

        ASSERT_TRUE(repo.exportRepository(BundleDirectory));
    }

    Repository repo(Mode::Game, BundleDirectory);
    ASSERT_TRUE(repo.loadRepository());

    auto textureAsset = repo.getTypedAsset<asi::TexturePayload>(textureUuid);
    ASSERT_TRUE(textureAsset.isValid());
    EXPECT_EQ(textureAsset->type, asi::TexturePayload::NormalMap);
    EXPECT_EQ(textureAsset->colorSpace, asi::TexturePayload::Linear);

    const sf::Image& loadedImage = textureAsset->image.get().get();
    ASSERT_EQ(loadedImage.getSize(), sourceImage.getSize());
    for (unsigned int x = 0; x < sourceImage.getSize().x; ++x) {
        for (unsigned int y = 0; y < sourceImage.getSize().y; ++y) {
            EXPECT_EQ(loadedImage.getPixel({x, y}), sf::Color::Blue);
        }
    }
}

} // namespace unittest
} // namespace as
} // namespace bl
