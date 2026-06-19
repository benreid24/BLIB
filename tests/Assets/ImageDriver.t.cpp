#include "Common.hpp"
#include <BLIB/Assets/Builtin/ImagePayload.hpp>
#include <BLIB/Assets/Drivers/ImageDriver.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <SFML/Graphics/Image.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace as
{
namespace unittest
{
TEST_F(RepositoryTest, ImageDriver) {
    util::UUID uuid;
    sf::Image sourceImage;
    sourceImage.resize({10, 10}, sf::Color::Red);
    sourceImage.setPixel({5, 5}, sf::Color::Green);

    {
        Repository repo(Mode::Editor, AssetDirectory);
        asi::ImageDriver::CreateParams createParams;
        createParams.sourceImage = &sourceImage;
        auto asset               = repo.createAsset<asi::ImagePayload>("TestImage", createParams);
        ASSERT_TRUE(asset.isValid());
        ASSERT_TRUE(repo.exportRepository(BundleDirectory));
        uuid = asset.getUUID();
    }

    Repository repo(Mode::Game, BundleDirectory);
    ASSERT_TRUE(repo.loadRepository());

    auto asset = repo.getTypedAsset<asi::ImagePayload>(uuid);
    ASSERT_TRUE(asset.isValid());
    ASSERT_EQ(asset->get().getSize(), sourceImage.getSize());
    for (unsigned int x = 0; x < sourceImage.getSize().x; ++x) {
        for (unsigned int y = 0; y < sourceImage.getSize().y; ++y) {
            const sf::Color expectedColor = (x == 5 && y == 5) ? sf::Color::Green : sf::Color::Red;
            EXPECT_EQ(asset->get().getPixel({x, y}), expectedColor);
        }
    }
}

} // namespace unittest
} // namespace as
} // namespace bl
