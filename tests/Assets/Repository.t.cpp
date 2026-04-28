#include <BLIB/Assets/Repository.hpp>
#include <gtest/gtest.h>

#include <BLIB/Assets/Builtin/ImagePayload.hpp>
#include <SFML/System.hpp>
#include <filesystem>

namespace bl
{
namespace as
{
namespace unittest
{
namespace
{
class RepositoryTest : public ::testing::Test {
public:
    void SetUp() override { util::FileUtil::deleteDirectory("test_assets"); }
};

struct TestCreateContext : public CreateContext::CreateData {
    TestCreateContext(const std::string& data, util::UUID dep = {})
    : data(data)
    , dep(dep) {}

    std::string data;
    util::UUID dep;
    util::UUID depLazy;
    std::vector<util::UUID> depList;
};

struct TestPayload : public Payload {
    TestPayload(const Payload::ConstructContext& ctx)
    : Payload(ctx) {}

    void init(const TestCreateContext& createData) { data = createData.data; }

    void init(const std::string& str) { data = str; }

    std::string data;
};

struct TestDriver : public Driver<TestPayload> {
    virtual ~TestDriver() = default;

    virtual bool doCreate(const CreateContext& ctx, TestPayload& payload) override {
        if (const TestCreateContext* createData = ctx.getCustomDataAsMaybe<TestCreateContext>()) {
            payload.init(*createData);
            return true;
        }
        else {
            payload.init(ctx.getCustomData().getPath());
            return true;
        }
    }

    virtual bool doRead(const ReadContext& ctx, TestPayload& payload) override {
        stream::InputStream input;
        if (!ctx.setupReadStream("payload.txt", input)) { return false; }
        std::vector<char> buf;
        buf.resize(100);
        const std::size_t read = input.read(buf.data(), 100);
        payload.data           = std::string(buf.begin(), buf.begin() + read);
        return true;
    }

    virtual bool doWrite(const WriteContext& ctx, const TestPayload& payload) override {
        stream::OutputStream output;
        if (!ctx.setupWriteStream("payload.txt", output)) { return false; }
        if (!output.write(payload.data.data(), payload.data.size())) { return false; }
        return true;
    }
};

struct TestPayloadWithDependency : public Payload {
    TestPayloadWithDependency(const Payload::ConstructContext& ctx)
    : Payload(ctx)
    , dependency(ctx.repo, *this, "depTag")
    , dependencyLazy(ctx.repo, *this, "depLazyTag")
    , dependencyList(ctx.repo, *this, "depListTag") {}

    bool init(const CreateContext& ctx) {
        if (const TestCreateContext* createData = ctx.getCustomDataAsMaybe<TestCreateContext>()) {
            localData = createData->data;
            for (const util::UUID& dep : createData->depList) {
                if (!dependencyList.addDependency(dep)) { return false; }
            }
            if (!dependencyLazy.init(createData->depLazy)) { return false; }
            return dependency.init(createData->dep);
        }
        return false;
    }

    std::string localData;
    Dependency<TestPayload> dependency;
    Dependency<TestPayload, LoadPolicy::Lazy, DependencyPolicy::Optional> dependencyLazy;
    DependencyList<TestPayload> dependencyList;
};

struct TestDriverWithDep : public Driver<TestPayloadWithDependency> {
    virtual ~TestDriverWithDep() = default;

    virtual bool doCreate(const CreateContext& ctx, TestPayloadWithDependency& payload) override {
        return payload.init(ctx);
    }

    virtual bool doRead(const ReadContext& ctx, TestPayloadWithDependency& payload) override {
        stream::InputStream input;
        if (!ctx.setupReadStream("payloadAdvanced.txt", input)) { return false; }
        std::vector<char> buf;
        buf.resize(100);
        const std::size_t read = input.read(buf.data(), 100);
        payload.localData      = std::string(buf.begin(), buf.begin() + read);
        return true;
    }

    virtual bool doWrite(const WriteContext& ctx,
                         const TestPayloadWithDependency& payload) override {
        stream::OutputStream output;
        if (!ctx.setupWriteStream("payloadAdvanced.txt", output)) { return false; }
        output.write(payload.localData.data(), payload.localData.size());
        return true;
    }
};

constexpr std::string_view TestTypeTag    = "test_type";
const std::string_view TestTypeWithDepTag = "test_type_with_dep";
} // namespace

TEST_F(RepositoryTest, CreateAsset) {
    Repository repo(Mode::Editor, "test_assets");
    repo.registerDriver<TestDriver>(TestTypeTag);

    auto asset = repo.createAsset<TestPayload>("TestName", TestCreateContext("test_data"));

    ASSERT_TRUE(asset.isValid());
    EXPECT_EQ(asset.getCount(), 1);
    EXPECT_EQ(asset->data, "test_data");
    EXPECT_EQ(asset.getAsset().getType(), TestTypeTag);
    EXPECT_EQ(asset.getAsset().getState(), State::Loaded);
    EXPECT_EQ(asset.getAsset().getMetadata().getDisplayName(), "TestName");
}

TEST_F(RepositoryTest, Dependencies) {
    Repository repo(Mode::Editor, "test_assets");
    repo.registerDriver<TestDriver>(TestTypeTag);
    repo.registerDriver<TestDriverWithDep>(TestTypeWithDepTag);

    auto child = repo.createAsset<TestPayload>("ChildName", TestCreateContext("child_data"));
    auto lazyChild =
        repo.createAsset<TestPayload>("LazyChildName", TestCreateContext("lazy_child_data"));
    auto asset = repo.createAsset<TestPayloadWithDependency>(
        "TestName", TestCreateContext("test_data", child.getAsset().getUUID()));
    auto badAsset = repo.createAsset<TestPayloadWithDependency>(
        "BadTestName", TestCreateContext("test_data", util::UUID::generate()));

    ASSERT_TRUE(asset.isValid());
    EXPECT_EQ(asset.getState(), State::Loaded);
    EXPECT_EQ(asset->localData, "test_data");
    EXPECT_EQ(asset->dependency.getState(), State::Loaded);
    EXPECT_EQ(asset->dependency.get().data, "child_data");
    EXPECT_FALSE(badAsset.isValid());
}

TEST_F(RepositoryTest, LazyDependencies) {
    util::UUID assetUUID;

    {
        Repository repo(Mode::Editor, "test_assets");
        repo.registerDriver<TestDriver>(TestTypeTag);
        repo.registerDriver<TestDriverWithDep>(TestTypeWithDepTag);

        auto child = repo.createAsset<TestPayload>("ChildName", TestCreateContext("child_data"));
        auto lazyChild =
            repo.createAsset<TestPayload>("LazyChildName", TestCreateContext("lazy_child_data"));
        TestCreateContext createData("test_data", child.getAsset().getUUID());
        createData.depLazy = lazyChild.getAsset().getUUID();
        auto asset         = repo.createAsset<TestPayloadWithDependency>("TestName", createData);
        assetUUID          = asset.getAsset().getUUID();
    }

    Repository repo2(Mode::Editor, "test_assets");
    repo2.registerDriver<TestDriver>(TestTypeTag);
    repo2.registerDriver<TestDriverWithDep>(TestTypeWithDepTag);

    ASSERT_TRUE(repo2.loadRepository());
    auto asset = repo2.getTypedAsset<TestPayloadWithDependency>(assetUUID);

    ASSERT_TRUE(asset.isValid());
    EXPECT_EQ(asset.getState(), State::Loaded);
    EXPECT_EQ(asset->localData, "test_data");
    EXPECT_EQ(asset->dependency.getState(), State::Loaded);
    EXPECT_EQ(asset->dependency.get().data, "child_data");
    EXPECT_NE(asset->dependencyLazy.getState(), State::Loaded);
    EXPECT_EQ(asset->dependencyLazy.get().data, "lazy_child_data"); // lazy loads on access
}

TEST_F(RepositoryTest, DependencyLists) {
    util::UUID assetUUID;

    {
        Repository repo(Mode::Editor, "test_assets");
        repo.registerDriver<TestDriver>(TestTypeTag);
        repo.registerDriver<TestDriverWithDep>(TestTypeWithDepTag);

        auto child  = repo.createAsset<TestPayload>("ChildName", TestCreateContext("child_data"));
        auto child1 = repo.createAsset<TestPayload>("ChildName1", TestCreateContext("child_data1"));
        auto child2 = repo.createAsset<TestPayload>("ChildName2", TestCreateContext("child_data2"));

        TestCreateContext createParams("test_data", child.getAsset().getUUID());
        createParams.depList = {child1.getAsset().getUUID(), child2.getAsset().getUUID()};
        auto asset = repo.createAsset<TestPayloadWithDependency>("TestName", createParams);

        assetUUID = asset.getAsset().getUUID();
    }

    Repository repo(Mode::Editor, "test_assets");
    repo.registerDriver<TestDriver>(TestTypeTag);
    repo.registerDriver<TestDriverWithDep>(TestTypeWithDepTag);

    ASSERT_TRUE(repo.loadRepository());

    auto asset = repo.getTypedAsset<TestPayloadWithDependency>(assetUUID);
    ASSERT_TRUE(asset.isValid());
    EXPECT_EQ(asset.getState(), State::Loaded);
    EXPECT_EQ(asset->localData, "test_data");
    EXPECT_EQ(asset->dependency.getState(), State::Loaded);
    EXPECT_EQ(asset->dependency.get().data, "child_data");
    EXPECT_EQ(asset->dependencyList.getSize(), 2);
    EXPECT_EQ(asset->dependencyList.get(0).data, "child_data1");
    EXPECT_EQ(asset->dependencyList.get(1).data, "child_data2");
}

TEST_F(RepositoryTest, GetAsset) {
    Repository repo(Mode::Editor, "test_assets");
    repo.registerDriver<TestDriver>(TestTypeTag);

    auto asset   = repo.createAsset<TestPayload>("TestName", TestCreateContext("test_data"));
    auto fetched = repo.getTypedAsset<TestPayload>(asset.getAsset().getUUID());

    ASSERT_TRUE(fetched.isValid());
    EXPECT_EQ(fetched.getCount(), 2);
    EXPECT_EQ(fetched->data, "test_data");
    EXPECT_EQ(fetched.getAsset().getType(), TestTypeTag);
    EXPECT_EQ(fetched.getAsset().getState(), State::Loaded);
    EXPECT_EQ(fetched.getAsset().getMetadata().getDisplayName(), "TestName");
}

TEST_F(RepositoryTest, Drivers) {
    Repository repo(Mode::Editor, "test_assets");
    repo.registerDriver<TestDriver>(TestTypeTag);
    repo.registerDriver<TestDriverWithDep>(TestTypeWithDepTag);

    auto* testDriver = repo.getDriver(TestTypeTag);
    auto* depDriver  = repo.getDriver(TestTypeWithDepTag);
    auto* badDriver  = repo.getDriver("unknown");

    EXPECT_NE(testDriver, nullptr);
    EXPECT_NE(depDriver, nullptr);
    EXPECT_EQ(badDriver, nullptr);
}

TEST_F(RepositoryTest, EditorSaveLoad) {
    util::UUID uuid;
    {
        Repository repo(Mode::Editor, "test_assets");
        repo.registerDriver<TestDriver>(TestTypeTag);

        auto asset = repo.createAsset<TestPayload>("TestName", TestCreateContext("test_data"));
        uuid       = asset.getAsset().getUUID();
        ASSERT_TRUE(asset.isValid());
    }

    Repository repo(Mode::Editor, "test_assets");
    repo.registerDriver<TestDriver>(TestTypeTag);

    ASSERT_TRUE(repo.loadRepository());

    auto asset = repo.getTypedAsset<TestPayload>(uuid);
    ASSERT_TRUE(asset.isValid());
    EXPECT_EQ(asset->data, "test_data");
}

TEST_F(RepositoryTest, EditorLoadMissingDependency) {
    util::UUID uuid;
    {
        Repository repo(Mode::Editor, "test_assets");
        repo.registerDriver<TestDriver>(TestTypeTag);

        auto asset = repo.createAsset<TestPayload>("TestName", TestCreateContext("test_data"));
        uuid       = asset.getAsset().getUUID();
        ASSERT_TRUE(asset.isValid());
    }

    std::filesystem::rename("test_assets/Assets/TestName", "test_assets/Assets/renamed");

    Repository repo(Mode::Editor, "test_assets");
    repo.registerDriver<TestDriver>(TestTypeTag);

    ASSERT_TRUE(repo.loadRepository());

    auto asset = repo.getTypedAsset<TestPayload>(uuid);
    ASSERT_TRUE(asset.isValid());
    EXPECT_EQ(asset->data, "test_data");
    EXPECT_EQ(asset.getAsset().getMetadata().getDisplayName(), "renamed");
}

TEST_F(RepositoryTest, AutoLoad) {
    util::UUID uuid;
    util::UUID autoLoadUUID;
    {
        Repository repo(Mode::Editor, "test_assets");
        repo.registerDriver<TestDriver>(TestTypeTag);

        auto asset = repo.createAsset<TestPayload>("TestName", TestCreateContext("test_data"));
        uuid       = asset.getAsset().getUUID();

        auto autoLoadAsset =
            repo.createAsset<TestPayload>("AutoLoadName", TestCreateContext("auto_load_data"));
        autoLoadAsset.getAsset().getMetadata().setIsAutoLoaded(true);
        autoLoadUUID = autoLoadAsset.getAsset().getUUID();
    }

    Repository repo(Mode::Editor, "test_assets");
    repo.registerDriver<TestDriver>(TestTypeTag);

    ASSERT_TRUE(repo.loadRepository());

    auto asset = repo.getAsset(uuid, State::Unloaded);
    ASSERT_TRUE(asset.isValid());
    EXPECT_EQ(asset.getState(), State::Unloaded);

    auto autoLoadAsset = repo.getAsset(autoLoadUUID, State::Unloaded);
    ASSERT_TRUE(autoLoadAsset.isValid());
    EXPECT_EQ(autoLoadAsset.getState(), State::Loaded);
}

TEST_F(RepositoryTest, ReleaseUnused) {
    Repository repo(Mode::Editor, "test_assets");
    repo.registerDriver<TestDriver>(TestTypeTag);

    auto asset    = repo.createAsset<TestPayload>("TestName", TestCreateContext("test_data"));
    Asset* sneaky = &asset.getAsset();
    asset.release();

    EXPECT_FALSE(asset.isValid());
    EXPECT_EQ(sneaky->getState(), State::Loaded);

    repo.releaseUnused();
    EXPECT_EQ(sneaky->getState(), State::Unloaded);

    asset = repo.getAsset(sneaky->getUUID());
    ASSERT_TRUE(asset.isValid());
    EXPECT_EQ(asset.getState(), State::Loaded);
    EXPECT_EQ(asset->data, "test_data");
}

TEST_F(RepositoryTest, FindMissing) {
    util::UUID uuid1;
    util::UUID uuid2;
    {
        Repository repo(Mode::Editor, "test_assets");
        repo.registerDriver<TestDriver>(TestTypeTag);

        auto asset1 = repo.createAsset<TestPayload>("TestName", TestCreateContext("test_data"));
        uuid1       = asset1.getAsset().getUUID();

        auto asset2 = repo.createAsset<TestPayload>("TestName2", TestCreateContext("test_data2"));
        uuid2       = asset2.getAsset().getUUID();
    }

    // delete asset1
    util::FileUtil::deleteDirectory("test_assets/Assets/TestName");

    // move asset2
    util::FileUtil::moveDirectory("test_assets/Assets/TestName2",
                                  "test_assets/Assets/TestFolder/TestName2Renamed");

    Repository repo(Mode::Editor, "test_assets");
    repo.registerDriver<TestDriver>(TestTypeTag);

    ASSERT_TRUE(repo.loadRepository());

    auto asset1 = repo.getAsset(uuid1);
    EXPECT_EQ(asset1.getAsset().getState(), State::Missing);

    auto asset2 = repo.getAsset(uuid2);
    EXPECT_TRUE(asset2.isValid());
    EXPECT_EQ(asset2.getAsset().getState(), State::Loaded);
    EXPECT_EQ(asset2->getMetadata().getDisplayName(), "TestName2Renamed");
    EXPECT_EQ(asset2->getMetadata().getPath(), "TestFolder");
}

TEST_F(RepositoryTest, SourceLinkAssets) {
    util::UUID uuid;
    {
        Repository repo(Mode::Editor, "test_assets");
        repo.registerDriver<TestDriver>(TestTypeTag);

        auto asset = repo.getAssetFromSourcePath<TestPayload>("/path/asset.png");
        uuid       = asset.getAsset().getUUID();

        ASSERT_TRUE(asset.isValid());
        EXPECT_EQ(asset->data, "/path/asset.png");
    }

    Repository repo(Mode::Editor, "test_assets");
    repo.registerDriver<TestDriver>(TestTypeTag);

    ASSERT_TRUE(repo.loadRepository());

    auto asset = repo.getAssetFromSourcePath<TestPayload>("/path/asset.png");
    ASSERT_TRUE(asset.isValid());
    EXPECT_EQ(asset->data, "/path/asset.png");
    EXPECT_EQ(asset.getAsset().getUUID(), uuid);

    auto badTypeFetch = repo.getAssetFromSourcePath("unknown", "/path/asset.png");
    EXPECT_FALSE(badTypeFetch.isValid());
}

TEST_F(RepositoryTest, SourceFileInfo) {
    ASSERT_TRUE(util::FileUtil::createDirectory("test_assets_source"));

    sf::Image image;
    image.resize({10, 10}, sf::Color::Red);
    ASSERT_TRUE(image.saveToFile("test_assets_source/test_image.png"));

    util::FileUtil::FileInfo info;
    ASSERT_TRUE(util::FileUtil::queryFileInfo("test_assets_source/test_image.png", info));

    Repository repo(Mode::Editor, "test_assets");
    auto asset =
        repo.getAssetFromSourcePath<asi::ImagePayload>("test_assets_source/test_image.png");

    EXPECT_TRUE(asset.isValid());
    EXPECT_EQ(asset.getAsset().getMetadata().getSourceFileInfo()->lastModified, info.modifiedTime);
    EXPECT_EQ(asset.getAsset().getMetadata().getSourceFileInfo()->path,
              "test_assets_source/test_image.png");
    EXPECT_EQ(asset->get().getSize().x, 10);
    EXPECT_EQ(asset->get().getSize().y, 10);
    EXPECT_EQ(asset->get().getPixel({0, 0}), sf::Color::Red);

    // update the image and reload from source
    sf::sleep(sf::milliseconds(1200));
    const auto prevTime = asset.getAsset().getMetadata().getSourceFileInfo()->lastModified;
    image.resize({20, 20}, sf::Color::Blue);
    ASSERT_TRUE(image.saveToFile("test_assets_source/test_image.png"));

    ASSERT_TRUE(asset.getAsset().reloadFromSource());
    EXPECT_EQ(asset->get().getSize().x, 20);
    EXPECT_EQ(asset->get().getSize().y, 20);
    EXPECT_EQ(asset->get().getPixel({0, 0}), sf::Color::Blue);
    EXPECT_GT(asset.getAsset().getMetadata().getSourceFileInfo()->lastModified, prevTime);
}

} // namespace unittest
} // namespace as
} // namespace bl
