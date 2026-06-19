#include <BLIB/Assets/Builtin/MaterialPayload.hpp>
#include <BLIB/Assets/Builtin/ModelPayload.hpp>
#include <BLIB/Assets/Drivers/MaterialDriver.hpp>
#include <BLIB/Assets/Drivers/ModelDriver.hpp>
#include <BLIB/Assets/Repository.hpp>
#include <BLIB/Models/BoneSet.hpp>
#include <BLIB/Models/Mesh.hpp>
#include <BLIB/Models/MeshSet.hpp>
#include <BLIB/Models/NodeSet.hpp>
#include <BLIB/Models/Vertex.hpp>
#include <gtest/gtest.h>

#include "Common.hpp"

#include <glm/glm.hpp>

namespace bl
{
namespace as
{
namespace unittest
{
TEST_F(RepositoryTest, ModelDriver) {
    // Build a minimal triangle mesh
    mdl::Vertex v0, v1, v2;
    v0.pos = {0.0f, 0.0f, 0.0f};
    v1.pos = {1.0f, 0.0f, 0.0f};
    v2.pos = {0.5f, 1.0f, 0.0f};
    v0.texCoord = {0.0f, 0.0f};
    v1.texCoord = {1.0f, 0.0f};
    v2.texCoord = {0.5f, 1.0f};

    mdl::MeshSet meshes;
    mdl::Mesh& mesh = meshes.addMesh();
    mesh.init({v0, v1, v2}, {0, 1, 2}, 0, false);

    // Build a root node referencing mesh index 0
    mdl::NodeSet nodes;
    mdl::Node& root = nodes.addNode(0);
    root.init("Root", glm::mat4(1.0f), {0}, std::nullopt);

    util::UUID modelUuid;
    {
        Repository repo(Mode::Editor, AssetDirectory);

        // Create an empty material asset
        mdl::Material emptyMaterial;
        asi::MaterialPayload::CreateData matData(emptyMaterial);
        auto matAsset = repo.createAsset<asi::MaterialPayload>("TestModelMat", matData);
        ASSERT_TRUE(matAsset.isValid());

        asi::ModelDriver::CreateParams params;
        params.nodes     = std::move(nodes);
        params.meshes    = std::move(meshes);
        params.materials = {matAsset};

        auto modelAsset = repo.createAsset<asi::ModelPayload>("TestModel", params);
        ASSERT_TRUE(modelAsset.isValid());
        ASSERT_TRUE(repo.exportRepository(BundleDirectory));
        modelUuid = modelAsset.getUUID();
    }

    Repository repo(Mode::Game, BundleDirectory);
    ASSERT_TRUE(repo.loadRepository());

    auto modelAsset = repo.getTypedAsset<asi::ModelPayload>(modelUuid);
    ASSERT_TRUE(modelAsset.isValid());

    const asi::ModelPayload& model = *modelAsset;

    // Verify node hierarchy
    ASSERT_GE(model.getNodes().size(), 1u);
    const mdl::Node& loadedRoot = model.getRoot();
    EXPECT_EQ(loadedRoot.getName(), "Root");
    ASSERT_EQ(loadedRoot.getMeshes().size(), 1u);
    EXPECT_EQ(loadedRoot.getMeshes()[0], 0u);

    // Verify mesh data
    ASSERT_EQ(model.getMeshes().getMeshCount(), 1u);
    const mdl::Mesh& loadedMesh = model.getMeshes().getMesh(0);
    ASSERT_EQ(loadedMesh.getVertices().size(), 3u);
    ASSERT_EQ(loadedMesh.getIndices().size(), 3u);
    EXPECT_EQ(loadedMesh.getMaterialIndex(), 0u);
    EXPECT_FALSE(loadedMesh.getIsSkinned());

    EXPECT_EQ(loadedMesh.getVertices()[0].pos, v0.pos);
    EXPECT_EQ(loadedMesh.getVertices()[1].pos, v1.pos);
    EXPECT_EQ(loadedMesh.getVertices()[2].pos, v2.pos);
    EXPECT_EQ(loadedMesh.getVertices()[0].texCoord, v0.texCoord);

    EXPECT_EQ(loadedMesh.getIndices()[0], 0u);
    EXPECT_EQ(loadedMesh.getIndices()[1], 1u);
    EXPECT_EQ(loadedMesh.getIndices()[2], 2u);

    // Verify material dependency round-tripped
    EXPECT_EQ(model.getMaterialCount(), 1u);
    EXPECT_EQ(model.getAnimationCount(), 0u);
    EXPECT_EQ(model.getBones().numBones(), 0u);
}

} // namespace unittest
} // namespace as
} // namespace bl
