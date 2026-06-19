#include <BLIB/Assets/Builtin/ImagePayload.hpp>
#include <BLIB/Assets/Builtin/MaterialPayload.hpp>
#include <BLIB/Assets/Drivers/MaterialDriver.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <BLIB/Models/Material.hpp>
#include <BLIB/Models/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <gtest/gtest.h>

#include "Common.hpp"

namespace bl
{
namespace as
{
namespace unittest
{
TEST_F(RepositoryTest, MaterialDriver) {
    // Build a material with an embedded diffuse texture and known scalar values
    sf::Image diffuseImage;
    diffuseImage.resize({8, 8}, sf::Color::Cyan);
    diffuseImage.setPixel({0, 0}, sf::Color::Magenta);

    mdl::Material sourceMaterial;
    sourceMaterial.diffuse.makeFromImage(diffuseImage);
    sourceMaterial.shininess   = 0.75f;
    sourceMaterial.heightScale = 0.1f;

    util::UUID matUuid;
    {
        Repository repo(Mode::Editor, AssetDirectory);

        asi::MaterialPayload::CreateData createData(sourceMaterial);
        auto matAsset = repo.createAsset<asi::MaterialPayload>("TestMaterial", createData);
        ASSERT_TRUE(matAsset.isValid());
        ASSERT_TRUE(repo.exportRepository(BundleDirectory));
        matUuid = matAsset.getUUID();
    }

    Repository repo(Mode::Game, BundleDirectory);
    ASSERT_TRUE(repo.loadRepository());

    auto matAsset = repo.getTypedAsset<asi::MaterialPayload>(matUuid);
    ASSERT_TRUE(matAsset.isValid());

    const asi::MaterialPayload& mat = *matAsset;
    EXPECT_NEAR(mat.shininess, sourceMaterial.shininess, 0.001f);
    EXPECT_NEAR(mat.heightScale, sourceMaterial.heightScale, 0.001f);

    // Diffuse texture dependency should be loaded and contain the right image data
    as::TypedRef<asi::TexturePayload> diffuseRef = mat.diffuse.getRef();
    ASSERT_TRUE(diffuseRef.isValid());
    const asi::TexturePayload& diffuseTex = *diffuseRef;

    as::TypedRef<asi::ImagePayload> imageRef = diffuseTex.image.getRef();
    ASSERT_TRUE(imageRef.isValid());
    const sf::Image& loadedImage = imageRef->get();

    ASSERT_EQ(loadedImage.getSize(), diffuseImage.getSize());
    EXPECT_EQ(loadedImage.getPixel({0, 0}), sf::Color::Magenta);
    EXPECT_EQ(loadedImage.getPixel({1, 1}), sf::Color::Cyan);
}

} // namespace unittest
} // namespace as
} // namespace bl
