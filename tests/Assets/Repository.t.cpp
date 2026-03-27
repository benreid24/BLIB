#include <BLIB/Assets/Repository.hpp>
#include <gtest/gtest.h>

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
};

struct TestPayload : public Payload {
    TestPayload(const Payload::ConstructContext& ctx)
    : Payload(ctx) {}

    void init(const TestCreateContext& createData) { data = createData.data; }

    void init(const std::string& str) { data = str; }

    std::string data;
};

struct TestDriver : public Driver<TestPayload> {
    using PayloadType = TestPayload;

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
        std::vector<char> buf;
        if (!ctx.readFile("payload.txt", buf)) { return false; }
        payload.data = std::string(buf.begin(), buf.end());
        return true;
    }

    virtual bool doWrite(const WriteContext& ctx, const TestPayload& payload) override {
        return ctx.writeFile("payload.txt",
                             std::span<const char>(payload.data.data(), payload.data.size()));
    }
};

struct TestPayloadWithDependency : public Payload {
    TestPayloadWithDependency(const Payload::ConstructContext& ctx)
    : Payload(ctx)
    , dependency(ctx.repo, *this, "depTag") {}

    bool init(const CreateContext& ctx) {
        if (const TestCreateContext* createData = ctx.getCustomDataAsMaybe<TestCreateContext>()) {
            localData = createData->data;
            return dependency.init(createData->dep);
        }
        return false;
    }

    std::string localData;
    Dependency<TestPayload> dependency;
};

struct TestDriverWithDep : public Driver<TestPayloadWithDependency> {
    using PayloadType = TestPayloadWithDependency;

    virtual ~TestDriverWithDep() = default;

    virtual bool doCreate(const CreateContext& ctx, TestPayloadWithDependency& payload) override {
        return payload.init(ctx);
    }

    virtual bool doRead(const ReadContext& ctx, TestPayloadWithDependency& payload) override {
        std::vector<char> buf;
        if (!ctx.readFile("payloadAdvanced.txt", buf)) { return false; }
        payload.localData = std::string(buf.begin(), buf.end());
        return true;
    }

    virtual bool doWrite(const WriteContext& ctx,
                         const TestPayloadWithDependency& payload) override {
        return ctx.writeFile(
            "payloadAdvanced.txt",
            std::span<const char>(payload.localData.data(), payload.localData.size()));
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
    auto asset = repo.createAsset<TestPayloadWithDependency>(
        "TestName", TestCreateContext("test_data", child.getAsset().getUUID()));
    auto badAsset = repo.createAsset<TestPayloadWithDependency>(
        "BadTestName", TestCreateContext("test_data", util::UUID()));

    ASSERT_TRUE(asset.isValid());
    EXPECT_EQ(asset.getState(), State::Loaded);
    EXPECT_EQ(asset->localData, "test_data");
    EXPECT_EQ(asset->dependency.getState(), State::Loaded);
    EXPECT_EQ(asset->dependency.get().data, "child_data");
    EXPECT_FALSE(badAsset.isValid());
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
        ASSERT_TRUE(repo.saveRepository());
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
        ASSERT_TRUE(repo.saveRepository());
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

        ASSERT_TRUE(repo.saveRepository());
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
        ASSERT_TRUE(repo.saveRepository());

        auto asset2 = repo.createAsset<TestPayload>("TestName2", TestCreateContext("test_data2"));
        uuid2       = asset2.getAsset().getUUID();
        // do not save repo
    }

    // delete asset1
    util::FileUtil::deleteDirectory("test_assets/Assets/TestName");

    Repository repo(Mode::Editor, "test_assets");
    repo.registerDriver<TestDriver>(TestTypeTag);

    ASSERT_TRUE(repo.loadRepository());

    auto asset1 = repo.getAsset(uuid1);
    EXPECT_EQ(asset1.getAsset().getState(), State::Missing);

    auto asset2 = repo.getAsset(uuid2);
    EXPECT_TRUE(asset2.isValid());
    EXPECT_EQ(asset2.getAsset().getState(), State::Loaded);
}

TEST_F(RepositoryTest, StaticAssets) {
    util::UUID uuid;
    {
        Repository repo(Mode::Editor, "test_assets");
        repo.registerDriver<TestDriver>(TestTypeTag);

        auto asset = repo.getStaticAsset<TestPayload>("/path/asset.png");
        uuid       = asset.getAsset().getUUID();

        ASSERT_TRUE(asset.isValid());
        EXPECT_EQ(asset->data, "/path/asset.png");
        ASSERT_TRUE(repo.saveRepository());
    }

    Repository repo(Mode::Editor, "test_assets");
    repo.registerDriver<TestDriver>(TestTypeTag);

    ASSERT_TRUE(repo.loadRepository());

    auto asset = repo.getStaticAsset<TestPayload>("/path/asset.png");
    ASSERT_TRUE(asset.isValid());
    EXPECT_EQ(asset->data, "/path/asset.png");
    EXPECT_EQ(asset.getAsset().getUUID(), uuid);

    auto badTypeFetch = repo.getStaticAsset("unknown", "/path/asset.png");
    EXPECT_FALSE(badTypeFetch.isValid());
}

} // namespace unittest
} // namespace as
} // namespace bl
