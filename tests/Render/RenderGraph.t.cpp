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

    virtual void doStartOutput(const ExecutionContext&) override {
        EXPECT_TRUE(created);
        preparedForOutput = true;
    }

    virtual void doEndOutput(const ExecutionContext&) override {
        EXPECT_TRUE(created);
        EXPECT_TRUE(preparedForOutput);
    }
};

struct SceneObjects : public TestAsset {
    SceneObjects()
    : TestAsset(AssetTags::SceneInput) {}
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
    factory.addProvider<TestProvider<SceneObjects>>(AssetTags::SceneInput);
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

    TestTask(std::string_view id)
    : Task(id)
    , created(false)
    , graphInit(false) {}

    virtual void create(engine::Engine&, Renderer&, Scene*) override { created = true; }

    virtual void execute(const ExecutionContext&, Asset* output) override {
        ASSERT_TRUE(assets.outputs[0]->asset.valid());
        TestAsset* asset = dynamic_cast<TestAsset*>(output);
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
    : TestTask("ShadowMapTask") {
        assetTags.outputs.emplace_back(TaskOutput(ShadowMapTag, TaskOutput::CreatedByTask));
        assetTags.requiredInputs.emplace_back(AssetTags::SceneInput);
        assetTags.requiredInputs.emplace_back(ShadowLightsTag);
    }

    virtual void onGraphInit() override {
        graphInit = true;

        ASSERT_EQ(assets.outputs[0]->asset->getTag(), ShadowMapTag);
        ASSERT_NE(dynamic_cast<ShadowMap*>(&assets.outputs[0]->asset.get()), nullptr);
        ASSERT_EQ(assets.requiredInputs.size(), 2);
        ASSERT_TRUE(assets.requiredInputs[0]->asset.valid());
        ASSERT_TRUE(assets.requiredInputs[1]->asset.valid());
        EXPECT_NE(dynamic_cast<SceneObjects*>(&assets.requiredInputs[0]->asset.get()), nullptr);
        EXPECT_NE(dynamic_cast<ShadowLights*>(&assets.requiredInputs[1]->asset.get()), nullptr);
    }

    virtual void onExecute() override {
        dynamic_cast<ShadowMap*>(&assets.outputs[0]->asset.get())->rendered = true;
    }
};

struct SceneRenderTask : public TestTask {
    SceneRenderTask()
    : TestTask("SceneRenderTask") {
        assetTags.outputs.emplace_back(
            TaskOutput({AssetTags::RenderedSceneOutput, AssetTags::FinalFrameOutput},
                       {TaskOutput::CreatedByTask, TaskOutput::CreatedExternally}));
        assetTags.requiredInputs.emplace_back(AssetTags::SceneInput);
        assetTags.optionalInputs.emplace_back(ShadowMapTag);
    }

    virtual void onGraphInit() override {
        graphInit = true;

        if (assets.outputs[0]->asset->getTag() == AssetTags::FinalFrameOutput) {
            ASSERT_NE(dynamic_cast<Swapframe*>(&assets.outputs[0]->asset.get()), nullptr);
        }
        else {
            ASSERT_EQ(assets.outputs[0]->asset->getTag(), AssetTags::RenderedSceneOutput);
            ASSERT_NE(dynamic_cast<SceneRenderOutput*>(&assets.outputs[0]->asset.get()), nullptr);
        }

        if (assets.optionalInputs.size() > 0 && assets.optionalInputs[0]) {
            ASSERT_NE(dynamic_cast<ShadowMap*>(&assets.optionalInputs[0]->asset.get()), nullptr);
        }

        ASSERT_EQ(assets.requiredInputs.size(), 1);
        EXPECT_NE(dynamic_cast<SceneObjects*>(&assets.requiredInputs[0]->asset.get()), nullptr);
    }

    virtual void onExecute() override {
        if (assets.outputs[0]->asset->getTag() == AssetTags::FinalFrameOutput) {
            dynamic_cast<Swapframe*>(&assets.outputs[0]->asset.get())->rendered = true;
        }
        else { dynamic_cast<SceneRenderOutput*>(&assets.outputs[0]->asset.get())->rendered = true; }

        if (assets.optionalInputs.size() > 0 && assets.optionalInputs[0]) {
            EXPECT_TRUE(dynamic_cast<ShadowMap*>(&assets.optionalInputs[0]->asset.get())->rendered);
        }
    }
};

struct PostFXTask : public TestTask {
    PostFXTask(int* counter = nullptr)
    : TestTask("PostFXTask")
    , executedAt(-1)
    , counter(counter) {
        assetTags.outputs.emplace_back(
            TaskOutput({AssetTags::PostFXOutput, AssetTags::FinalFrameOutput},
                       {TaskOutput::CreatedByTask, TaskOutput::CreatedExternally},
                       {TaskOutput::Exclusive, TaskOutput::Shared}));
        assetTags.requiredInputs.emplace_back(TaskInput(
            {AssetTags::RenderedSceneOutput, AssetTags::PostFXOutput}, rg::TaskInput::Exclusive));
    }

    virtual void onGraphInit() override {
        graphInit = true;

        if (assets.outputs[0]->asset->getTag() == AssetTags::FinalFrameOutput) {
            ASSERT_NE(dynamic_cast<Swapframe*>(&assets.outputs[0]->asset.get()), nullptr);
            dynamic_cast<Swapframe*>(&assets.outputs[0]->asset.get())->rendered = true;
        }
        else {
            ASSERT_EQ(assets.outputs[0]->asset->getTag(), AssetTags::PostFXOutput);
            ASSERT_NE(dynamic_cast<PostFXOutput*>(&assets.outputs[0]->asset.get()), nullptr);
            dynamic_cast<PostFXOutput*>(&assets.outputs[0]->asset.get())->rendered = true;
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
        if (counter) {
            executedAt = *counter;
            *counter += 1;
        }
        if (assets.requiredInputs[0]->asset->getTag() == AssetTags::RenderedSceneOutput) {
            EXPECT_TRUE(
                dynamic_cast<SceneRenderOutput*>(&assets.requiredInputs[0]->asset.get())->rendered);
        }
        else {
            EXPECT_TRUE(
                dynamic_cast<PostFXOutput*>(&assets.requiredInputs[0]->asset.get())->rendered);
        }

        if (assets.outputs[0]->asset->getTag() == AssetTags::FinalFrameOutput) {
            dynamic_cast<Swapframe*>(&assets.outputs[0]->asset.get())->rendered = true;
        }
        else { dynamic_cast<PostFXOutput*>(&assets.outputs[0]->asset.get())->rendered = true; }
    }

    int executedAt;
    int* counter;
};

struct PostFXBloomTask : public TestTask {
    PostFXBloomTask()
    : TestTask("PostFXBloomTask") {
        assetTags.outputs.emplace_back(
            TaskOutput({AssetTags::PostFXOutput, AssetTags::FinalFrameOutput},
                       {TaskOutput::CreatedByTask, TaskOutput::CreatedExternally}));
        assetTags.requiredInputs.emplace_back(TaskInput{AssetTags::RenderedSceneOutput});
        assetTags.requiredInputs.emplace_back(TaskInput{AssetTags::BloomColorAttachmentPair});
    }

    virtual void onGraphInit() override {
        graphInit = true;

        ASSERT_NE(dynamic_cast<Swapframe*>(&assets.outputs[0]->asset.get()), nullptr);
        dynamic_cast<Swapframe*>(&assets.outputs[0]->asset.get())->rendered = true;

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
    : TestTask("BloomTask") {
        assetTags.outputs.emplace_back(
            TaskOutput(AssetTags::BloomColorAttachmentPair, TaskOutput::CreatedByTask));
        assetTags.requiredInputs.emplace_back(TaskInput{AssetTags::RenderedSceneOutput});
    }

    virtual void onGraphInit() override {
        graphInit = true;

        ASSERT_EQ(assets.outputs[0]->asset->getTag(), AssetTags::BloomColorAttachmentPair);
        ASSERT_NE(dynamic_cast<BloomAsset*>(&assets.outputs[0]->asset.get()), nullptr);

        ASSERT_EQ(assets.requiredInputs.size(), 1);
        ASSERT_TRUE(assets.requiredInputs[0]->asset.valid());
        EXPECT_NE(dynamic_cast<SceneRenderOutput*>(&assets.requiredInputs[0]->asset.get()),
                  nullptr);
    }

    virtual void onExecute() override {}
};

struct ExternalSharingTask : public TestTask {
    ExternalSharingTask(TaskOutput::Order order, int& counter)
    : TestTask("ExternalSharingTask")
    , counter(counter)
    , executedAtCount(10000) {
        assetTags.outputs.emplace_back(TaskOutput(
            AssetTags::FinalFrameOutput, TaskOutput::CreatedExternally, TaskOutput::Shared, order));
        assetTags.requiredInputs.emplace_back(TaskInput{AssetTags::SceneInput});
    }

    virtual void onGraphInit() override {
        graphInit = true;

        ASSERT_NE(dynamic_cast<Swapframe*>(&assets.outputs[0]->asset.get()), nullptr);
        dynamic_cast<Swapframe*>(&assets.outputs[0]->asset.get())->rendered = true;

        ASSERT_EQ(assets.requiredInputs.size(), 1);
        ASSERT_TRUE(assets.requiredInputs[0]->asset.valid());
        ASSERT_TRUE(assets.requiredInputs[0]->asset->getTag() == AssetTags::SceneInput);
    }

    virtual void onExecute() override { executedAtCount = counter++; }

    int& counter;
    int executedAtCount;
};

struct SharedTaskForExclusive : public TestTask {
    SharedTaskForExclusive(TaskOutput::Order order, int& counter)
    : TestTask("SharedTaskForExclusive")
    , counter(counter)
    , executedAtCount(10000) {
        assetTags.outputs.emplace_back(
            TaskOutput({AssetTags::RenderedSceneOutput, AssetTags::FinalFrameOutput},
                       {TaskOutput::CreatedByTask, TaskOutput::CreatedExternally},
                       {TaskOutput::Shared},
                       order));
    }

    virtual void onGraphInit() override { graphInit = true; }

    virtual void onExecute() override {
        if (assets.outputs[0]->asset->getTag() == AssetTags::FinalFrameOutput) {
            dynamic_cast<Swapframe*>(&assets.outputs[0]->asset.get())->rendered = true;
        }
        else { dynamic_cast<SceneRenderOutput*>(&assets.outputs[0]->asset.get())->rendered = true; }

        executedAtCount = counter++;
    }

    int& counter;
    int executedAtCount;
};

struct ExclusivelySharedTask : public TestTask {
    ExclusivelySharedTask(TaskOutput::Order order, int& counter)
    : TestTask("ExclusivelySharedTask")
    , counter(counter)
    , executedAtCount(10000) {
        assetTags.outputs.emplace_back(
            TaskOutput({AssetTags::RenderedSceneOutput, AssetTags::FinalFrameOutput},
                       {TaskOutput::CreatedByOtherTask, TaskOutput::CreatedExternally},
                       {TaskOutput::Shared},
                       order,
                       {"SharedTaskForExclusive"}));
    }

    virtual void onGraphInit() override { graphInit = true; }

    virtual void onExecute() override { executedAtCount = counter++; }

    int& counter;
    int executedAtCount;
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

    SceneRenderTask* task = graph.putTask<SceneRenderTask>();

    graph.execute(nullptr, 0, false);
    EXPECT_TRUE(swapframe->rendered);
    EXPECT_TRUE(task->graphInit);
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
    EXPECT_TRUE(swapframe->rendered);

    graph2.putTask<SceneRenderTask>();
    graph2.putTask<PostFXTask>();

    swapframe->rendered = false;
    graph2.execute(nullptr, 0, false);
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
    EXPECT_TRUE(swapframe->rendered);
}

TEST(RenderGraph, TasksSharingExternalInputsAndOutputs) {
    AssetFactory factory;
    setupFactory(factory);

    AssetPool pool(factory, nullptr);
    pool.putAsset<SceneObjects>();
    Swapframe* swapframe = pool.putAsset<Swapframe>();

    engine::Engine engine(engine::Settings{});
    RenderGraph graph(engine, engine.renderer(), pool, nullptr, nullptr);

    int counter = 0;
    auto* task1 = graph.putTask<ExternalSharingTask>(TaskOutput::First, counter);
    auto* task2 = graph.putTask<ExternalSharingTask>(TaskOutput::Last, counter);

    graph.execute(nullptr, 0, false);
    EXPECT_TRUE(swapframe->rendered);
    EXPECT_EQ(counter, 2);
    EXPECT_EQ(task1->executedAtCount, 0);
    EXPECT_EQ(task2->executedAtCount, 1);
}

TEST(RenderGraph, TaskSharedOnlyOutput) {
    AssetFactory factory;
    setupFactory(factory);

    AssetPool pool(factory, nullptr);
    pool.putAsset<SceneObjects>();
    Swapframe* swapframe = pool.putAsset<Swapframe>();

    engine::Engine engine(engine::Settings{});
    RenderGraph graph(engine, engine.renderer(), pool, nullptr, nullptr);

    int counter = 0;
    auto* task2 = graph.putTask<ExclusivelySharedTask>(TaskOutput::Last, counter);
    auto* task1 = graph.putTask<SharedTaskForExclusive>(TaskOutput::First, counter);
    auto* task3 = graph.putTask<PostFXTask>(&counter);

    graph.execute(nullptr, 0, false);
    EXPECT_TRUE(swapframe->rendered);
    EXPECT_EQ(counter, 3);
    EXPECT_EQ(task1->executedAtCount, 0);
    EXPECT_EQ(task2->executedAtCount, 1);
    EXPECT_EQ(task3->executedAt, 2);
}

TEST(RenderGraph, TaskSharedOnlyOutputExternal) {
    AssetFactory factory;
    setupFactory(factory);

    AssetPool pool(factory, nullptr);
    pool.putAsset<SceneObjects>();
    Swapframe* swapframe = pool.putAsset<Swapframe>();

    engine::Engine engine(engine::Settings{});
    RenderGraph graph(engine, engine.renderer(), pool, nullptr, nullptr);

    int counter = 0;
    auto* task2 = graph.putTask<ExclusivelySharedTask>(TaskOutput::Last, counter);
    auto* task1 = graph.putTask<SharedTaskForExclusive>(TaskOutput::First, counter);

    graph.execute(nullptr, 0, false);
    EXPECT_TRUE(swapframe->rendered);
    EXPECT_EQ(counter, 2);
    EXPECT_EQ(task1->executedAtCount, 0);
    EXPECT_EQ(task2->executedAtCount, 1);
}

} // namespace unittest
} // namespace rg
} // namespace rc
} // namespace bl
