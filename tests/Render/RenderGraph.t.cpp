#include <BLIB/Render/Graph/RenderGraph.hpp>
#include <gtest/gtest.h>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Graph/AssetFactory.hpp>
#include <BLIB/Render/Graph/AssetTags.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace rg
{
namespace unittest
{
namespace
{

////////////////////////////////////// ASSETS ////////////////////////////////////////////

constexpr std::string_view ShadowLightsTag = "shadow-lights";
constexpr std::string_view ShadowMapTag    = "shadowmap";
constexpr std::string_view SharedTag       = "shared";

struct TestAsset : public Asset {
    bool created;
    bool preparedForInput;
    bool preparedForOutput;

    TestAsset(std::string_view tag)
    : Asset(tag)
    , created(false)
    , preparedForInput(false)
    , preparedForOutput(false) {}

    virtual void doCreate(engine::Engine&, Renderer&, RenderTarget*) override { created = true; }

    virtual void doPrepareForInput(const ExecutionContext&) override {
        EXPECT_TRUE(created);
        preparedForInput = true;
    }

    virtual void doPrepareForOutput(const ExecutionContext&) override {
        EXPECT_TRUE(created);
        preparedForOutput = true;
    }
};

struct SceneObjects : public TestAsset {
    SceneObjects()
    : TestAsset(AssetTags::SceneObjectsInput) {}
};

struct ShadowLights : public TestAsset {
    ShadowLights()
    : TestAsset(ShadowLightsTag) {}
};

struct ShadowMap : public TestAsset {
    bool rendered;

    ShadowMap()
    : TestAsset(ShadowMapTag)
    , rendered(false) {}
};

struct SceneRenderOutput : public TestAsset {
    bool rendered;

    SceneRenderOutput()
    : TestAsset(AssetTags::RenderedSceneOutput)
    , rendered(false) {}
};

struct PostFXOutput : public TestAsset {
    bool rendered;

    PostFXOutput()
    : TestAsset(AssetTags::PostFXOutput)
    , rendered(false) {}
};

struct BloomAsset : public TestAsset {
    BloomAsset()
    : TestAsset(AssetTags::BloomColorAttachmentPair) {}
};

struct Swapframe : public TestAsset {
    bool rendered;

    Swapframe()
    : TestAsset(AssetTags::FinalFrameOutput)
    , rendered(false) {}
};

template<typename T>
struct TestProvider : public AssetProvider {
    virtual Asset* create(std::string_view) override { return new T(); }
};

void setupFactory(AssetFactory& factory) {
    factory.addProvider<TestProvider<SceneObjects>>(AssetTags::SceneObjectsInput);
    factory.addProvider<TestProvider<ShadowLights>>(ShadowLightsTag);
    factory.addProvider<TestProvider<ShadowMap>>(ShadowMapTag);
    factory.addProvider<TestProvider<SceneRenderOutput>>(AssetTags::RenderedSceneOutput);
    factory.addProvider<TestProvider<PostFXOutput>>(AssetTags::PostFXOutput);
    factory.addProvider<TestProvider<Swapframe>>(AssetTags::FinalFrameOutput);
    factory.addProvider<TestProvider<BloomAsset>>(AssetTags::BloomColorAttachmentPair);
}

/////////////////////////////////////// TASKS ////////////////////////////////////////////

constexpr unsigned int NotRan = std::numeric_limits<unsigned int>::max();

struct TestTask : public Task {
    bool created;
    bool graphInit;

    TestTask()
    : created(false)
    , graphInit(false) {}

    virtual void create(engine::Engine&, Renderer&, Scene*) override { created = true; }

    virtual void execute(const ExecutionContext&) override {
        ASSERT_TRUE(assets.output->asset.valid());
        TestAsset* asset = dynamic_cast<TestAsset*>(&assets.output->asset.get());
        ASSERT_NE(asset, nullptr);
        ASSERT_TRUE(asset->preparedForOutput);

        for (auto& input : assets.requiredInputs) {
            ASSERT_TRUE(input->asset.valid());
            asset = dynamic_cast<TestAsset*>(&input->asset.get());
            ASSERT_NE(asset, nullptr);
            EXPECT_TRUE(asset->preparedForInput);
        }
        for (auto& input : assets.optionalInputs) {
            if (input) {
                ASSERT_TRUE(input->asset.valid());
                asset = dynamic_cast<TestAsset*>(&input->asset.get());
                ASSERT_NE(asset, nullptr);
                EXPECT_TRUE(asset->preparedForInput);
            }
        }

        onExecute();
    }

    virtual void onExecute() = 0;
};

struct ShadowMapTask : public TestTask {
    ShadowMapTask()
    : TestTask() {
        assetTags.createdOutputs.emplace_back(ShadowMapTag);
        assetTags.requiredInputs.emplace_back(AssetTags::SceneObjectsInput);
        assetTags.requiredInputs.emplace_back(ShadowLightsTag);
    }

    virtual void onGraphInit() override {
        graphInit = true;

        ASSERT_EQ(assets.output->asset->getTag(), ShadowMapTag);
        ASSERT_NE(dynamic_cast<ShadowMap*>(&assets.output->asset.get()), nullptr);
        ASSERT_EQ(assets.requiredInputs.size(), 2);
        ASSERT_TRUE(assets.requiredInputs[0]->asset.valid());
        ASSERT_TRUE(assets.requiredInputs[1]->asset.valid());
        EXPECT_NE(dynamic_cast<SceneObjects*>(&assets.requiredInputs[0]->asset.get()), nullptr);
        EXPECT_NE(dynamic_cast<ShadowLights*>(&assets.requiredInputs[1]->asset.get()), nullptr);
    }

    virtual void onExecute() override {
        dynamic_cast<ShadowMap*>(&assets.output->asset.get())->rendered = true;
    }
};

struct SceneRenderTask : public TestTask {
    SceneRenderTask()
    : TestTask() {
        assetTags.concreteOutputs.emplace_back(AssetTags::FinalFrameOutput);
        assetTags.createdOutputs.emplace_back(AssetTags::RenderedSceneOutput);
        assetTags.requiredInputs.emplace_back(AssetTags::SceneObjectsInput);
        assetTags.optionalInputs.emplace_back(ShadowMapTag);
    }

    virtual void onGraphInit() override {
        graphInit = true;

        if (assets.output->asset->getTag() == AssetTags::FinalFrameOutput) {
            ASSERT_NE(dynamic_cast<Swapframe*>(&assets.output->asset.get()), nullptr);
        }
        else {
            ASSERT_EQ(assets.output->asset->getTag(), AssetTags::RenderedSceneOutput);
            ASSERT_NE(dynamic_cast<SceneRenderOutput*>(&assets.output->asset.get()), nullptr);
        }

        if (assets.optionalInputs.size() > 0 && assets.optionalInputs[0]) {
            ASSERT_NE(dynamic_cast<ShadowMap*>(&assets.optionalInputs[0]->asset.get()), nullptr);
        }

        ASSERT_EQ(assets.requiredInputs.size(), 1);
        EXPECT_NE(dynamic_cast<SceneObjects*>(&assets.requiredInputs[0]->asset.get()), nullptr);
    }

    virtual void onExecute() override {
        if (assets.output->asset->getTag() == AssetTags::FinalFrameOutput) {
            dynamic_cast<Swapframe*>(&assets.output->asset.get())->rendered = true;
        }
        else { dynamic_cast<SceneRenderOutput*>(&assets.output->asset.get())->rendered = true; }

        if (assets.optionalInputs.size() > 0 && assets.optionalInputs[0]) {
            EXPECT_TRUE(dynamic_cast<ShadowMap*>(&assets.optionalInputs[0]->asset.get())->rendered);
        }
    }
};

struct PostFXTask : public TestTask {
    PostFXTask()
    : TestTask() {
        assetTags.concreteOutputs.emplace_back(AssetTags::FinalFrameOutput);
        assetTags.createdOutputs.emplace_back(AssetTags::PostFXOutput);
        assetTags.requiredInputs.emplace_back(
            TaskInput{AssetTags::RenderedSceneOutput, AssetTags::PostFXOutput});
    }

    virtual void onGraphInit() override {
        graphInit = true;

        if (assets.output->asset->getTag() == AssetTags::FinalFrameOutput) {
            ASSERT_NE(dynamic_cast<Swapframe*>(&assets.output->asset.get()), nullptr);
            dynamic_cast<Swapframe*>(&assets.output->asset.get())->rendered = true;
        }
        else {
            ASSERT_EQ(assets.output->asset->getTag(), AssetTags::PostFXOutput);
            ASSERT_NE(dynamic_cast<PostFXOutput*>(&assets.output->asset.get()), nullptr);
            dynamic_cast<PostFXOutput*>(&assets.output->asset.get())->rendered = true;
        }

        ASSERT_EQ(assets.requiredInputs.size(), 1);
        ASSERT_TRUE(assets.requiredInputs[0]->asset.valid());
        if (assets.requiredInputs[0]->asset->getTag() == AssetTags::RenderedSceneOutput) {
            EXPECT_NE(dynamic_cast<SceneRenderOutput*>(&assets.requiredInputs[0]->asset.get()),
                      nullptr);
        }
        else {
            EXPECT_NE(dynamic_cast<PostFXOutput*>(&assets.requiredInputs[0]->asset.get()), nullptr);
        }
    }

    virtual void onExecute() override {
        if (assets.requiredInputs[0]->asset->getTag() == AssetTags::RenderedSceneOutput) {
            EXPECT_TRUE(
                dynamic_cast<SceneRenderOutput*>(&assets.requiredInputs[0]->asset.get())->rendered);
        }
        else {
            EXPECT_TRUE(
                dynamic_cast<PostFXOutput*>(&assets.requiredInputs[0]->asset.get())->rendered);
        }

        if (assets.output->asset->getTag() == AssetTags::FinalFrameOutput) {
            dynamic_cast<Swapframe*>(&assets.output->asset.get())->rendered = true;
        }
        else { dynamic_cast<PostFXOutput*>(&assets.output->asset.get())->rendered = true; }
    }
};

struct PostFXBloomTask : public TestTask {
    PostFXBloomTask()
    : TestTask() {
        assetTags.concreteOutputs.emplace_back(AssetTags::FinalFrameOutput);
        assetTags.createdOutputs.emplace_back(AssetTags::PostFXOutput);
        assetTags.requiredInputs.emplace_back(
            TaskInput{AssetTags::RenderedSceneOutput, TaskInput::Shared});
        assetTags.requiredInputs.emplace_back(TaskInput{AssetTags::BloomColorAttachmentPair});
    }

    virtual void onGraphInit() override {
        graphInit = true;

        ASSERT_NE(dynamic_cast<Swapframe*>(&assets.output->asset.get()), nullptr);
        dynamic_cast<Swapframe*>(&assets.output->asset.get())->rendered = true;

        ASSERT_EQ(assets.requiredInputs.size(), 2);
        ASSERT_TRUE(assets.requiredInputs[0]->asset.valid());
        ASSERT_TRUE(assets.requiredInputs[1]->asset.valid());
        ASSERT_TRUE(assets.requiredInputs[0]->asset->getTag() == AssetTags::RenderedSceneOutput ||
                    assets.requiredInputs[0]->asset->getTag() ==
                        AssetTags::BloomColorAttachmentPair);
        ASSERT_TRUE(assets.requiredInputs[1]->asset->getTag() == AssetTags::RenderedSceneOutput ||
                    assets.requiredInputs[1]->asset->getTag() ==
                        AssetTags::BloomColorAttachmentPair);
    }

    virtual void onExecute() override {}
};

struct BloomTask : public TestTask {
    BloomTask()
    : TestTask() {
        assetTags.createdOutputs.emplace_back(AssetTags::BloomColorAttachmentPair);
        assetTags.requiredInputs.emplace_back(
            TaskInput{AssetTags::RenderedSceneOutput, TaskInput::Shared});
    }

    virtual void onGraphInit() override {
        graphInit = true;

        ASSERT_EQ(assets.output->asset->getTag(), AssetTags::BloomColorAttachmentPair);
        ASSERT_NE(dynamic_cast<BloomAsset*>(&assets.output->asset.get()), nullptr);

        ASSERT_EQ(assets.requiredInputs.size(), 1);
        ASSERT_TRUE(assets.requiredInputs[0]->asset.valid());
        EXPECT_NE(dynamic_cast<SceneRenderOutput*>(&assets.requiredInputs[0]->asset.get()),
                  nullptr);
    }

    virtual void onExecute() override {}
};

} // namespace

TEST(RenderGraph, BasicSceneRender) {
    AssetFactory factory;
    setupFactory(factory);

    AssetPool pool(factory, nullptr);
    pool.putAsset<SceneObjects>();
    Swapframe* swapframe = pool.putAsset<Swapframe>();

    engine::Engine engine(engine::Settings{});
    RenderGraph graph(engine, engine.renderer(), pool, nullptr, nullptr);

    graph.putTask<SceneRenderTask>();

    graph.execute(nullptr, 0, false);
    graph.executeFinal(nullptr, 0, false);
    EXPECT_TRUE(swapframe->rendered);
}

TEST(RenderGraph, SceneWithPostFX) {
    AssetFactory factory;
    setupFactory(factory);

    AssetPool pool(factory, nullptr);
    pool.putAsset<SceneObjects>();
    Swapframe* swapframe = pool.putAsset<Swapframe>();

    engine::Engine engine(engine::Settings{});
    RenderGraph graph(engine, engine.renderer(), pool, nullptr, nullptr);

    graph.putTask<SceneRenderTask>();
    graph.putTask<PostFXTask>();

    graph.execute(nullptr, 0, false);
    graph.executeFinal(nullptr, 0, false);
    EXPECT_TRUE(swapframe->rendered);
}

TEST(RenderGraph, SceneWithPostFXAndShadows) {
    AssetFactory factory;
    setupFactory(factory);

    AssetPool pool(factory, nullptr);
    pool.putAsset<SceneObjects>();
    Swapframe* swapframe = pool.putAsset<Swapframe>();
    pool.putAsset<ShadowLights>();

    engine::Engine engine(engine::Settings{});
    RenderGraph graph(engine, engine.renderer(), pool, nullptr, nullptr);

    graph.putTask<SceneRenderTask>();
    graph.putTask<PostFXTask>();
    graph.putTask<ShadowMapTask>();

    graph.execute(nullptr, 0, false);
    graph.executeFinal(nullptr, 0, false);
    EXPECT_TRUE(swapframe->rendered);
}

TEST(RenderGraph, SceneWithPostFXChainAndShadows) {
    AssetFactory factory;
    setupFactory(factory);

    AssetPool pool(factory, nullptr);
    pool.putAsset<SceneObjects>();
    Swapframe* swapframe = pool.putAsset<Swapframe>();
    pool.putAsset<ShadowLights>();

    engine::Engine engine(engine::Settings{});
    RenderGraph graph(engine, engine.renderer(), pool, nullptr, nullptr);

    graph.putTask<SceneRenderTask>();
    graph.putTask<PostFXTask>();
    graph.putTask<PostFXTask>();
    graph.putTask<PostFXTask>();
    graph.putTask<ShadowMapTask>();

    graph.execute(nullptr, 0, false);
    graph.executeFinal(nullptr, 0, false);
    EXPECT_TRUE(swapframe->rendered);
}

TEST(RenderGraph, MultipleGraphsSharedAssets) {
    AssetFactory factory;
    setupFactory(factory);

    AssetPool pool(factory, nullptr);
    pool.putAsset<SceneObjects>();
    Swapframe* swapframe = pool.putAsset<Swapframe>();

    engine::Engine engine(engine::Settings{});

    RenderGraph graph1(engine, engine.renderer(), pool, nullptr, nullptr);
    RenderGraph graph2(engine, engine.renderer(), pool, nullptr, nullptr);

    graph1.putTask<SceneRenderTask>();
    graph1.putTask<PostFXTask>();

    graph1.execute(nullptr, 0, false);
    graph1.executeFinal(nullptr, 0, false);
    EXPECT_TRUE(swapframe->rendered);

    graph2.putTask<SceneRenderTask>();
    graph2.putTask<PostFXTask>();

    swapframe->rendered = false;
    graph2.execute(nullptr, 0, false);
    graph2.executeFinal(nullptr, 0, false);
    EXPECT_TRUE(swapframe->rendered);
}

TEST(RenderGraph, TasksSharingInputs) {
    AssetFactory factory;
    setupFactory(factory);

    AssetPool pool(factory, nullptr);
    pool.putAsset<SceneObjects>();
    Swapframe* swapframe = pool.putAsset<Swapframe>();

    engine::Engine engine(engine::Settings{});
    RenderGraph graph(engine, engine.renderer(), pool, nullptr, nullptr);

    graph.putTask<SceneRenderTask>();
    graph.putTask<PostFXBloomTask>();
    graph.putTask<BloomTask>();

    graph.execute(nullptr, 0, false);
    graph.executeFinal(nullptr, 0, false);
    EXPECT_TRUE(swapframe->rendered);
}

} // namespace unittest
} // namespace rg
} // namespace rc
} // namespace bl
