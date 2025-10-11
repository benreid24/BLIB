#ifndef BLIB_RENDER_DESCRIPTORS_SHADERINPUTS_ASSETBUFFERSHADERINPUT_HPP
#define BLIB_RENDER_DESCRIPTORS_SHADERINPUTS_ASSETBUFFERSHADERINPUT_HPP

#include <BLIB/Engine/HeaderHelpers.hpp>
#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Events/GraphEvents.hpp>
#include <BLIB/Render/Graph/Assets/GenericBufferAsset.hpp>
#include <BLIB/Render/Graph/GraphAsset.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/ShaderResources/ShaderResource.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

namespace bl
{
namespace rc
{
namespace sr
{
/**
 * @brief Shader input that provides access to generic buffer assets from the render graph
 *
 * @tparam TAsset The graph asset type to provide access to
 * @ingroup Renderer
 */
template<typename TAsset>
class AssetBufferShaderResource
: public ShaderResource
, public sig::Listener<event::SceneGraphAssetInitialized> {
    static_assert(std::is_base_of_v<rgi::GenericBufferAsset, TAsset>,
                  "TAsset must derive from GenericBufferAsset");

public:
    /**
     * @brief Initializes the asset buffer shader input
     */
    AssetBufferShaderResource()
    : asset(nullptr)
    , dirtyFrames(0) {}

    /**
     * @brief Destroys the shader input
     */
    virtual ~AssetBufferShaderResource() = default;

    /**
     * @brief Subscribes to the renderer event channel
     *
     * @param engine The engine instance
     */
    virtual void init(engine::Engine& engine) override {
        /*
        TODO - replace this hack
        DETAILS: Descriptor sets are per-scene, but some descriptor sets conceptually exist as per
                 observer. SceneDescriptorSetInstance is an example of this. We should insert a new
                 base class ObserverAwareDescriptorSetInstance that sits below
                 SceneDescriptorSetInstance and provides a mechnanism for per-observer data
                 (templated?). Could even just replace SceneDescriptorSetInstance with this.
        */

        // handle the case where the descriptor set is created after the asset is initialized
        auto& assets = engine::HeaderHelpers::getRenderer(engine).getObserver().getAssetPool();
        asset        = assets.getAsset<TAsset>();
        if (asset) { dirtyFrames = cfg::Limits::MaxConcurrentFrames; }
        else { subscribe(engine::HeaderHelpers::getEngineSignalChannel(engine)); }
    }

    /**
     * @brief Does nothing
     */
    virtual void cleanup() override {}

    /**
     * @brief Does nothing, derived classes or owners are responsible for transferring updated
     *        contents of the buffer
     */
    virtual void performTransfer() override { --dirtyFrames; }

    /**
     * @brief Does nothing. Derived classes may copy source data into the buffer here
     */
    virtual void copyFromSource() override {}

    /**
     * @brief Returns true for Limits::MaxConcurrentFrames frames after a buffer change
     */
    virtual bool dynamicDescriptorUpdateRequired() const override { return dirtyFrames > 0; }

    /**
     * @brief Returns true for Limits::MaxConcurrentFrames frames after a buffer change
     */
    virtual bool staticDescriptorUpdateRequired() const override { return dirtyFrames > 0; }

    /**
     * @brief Returns the underlying buffer
     */
    vk::Buffer& getBuffer() {
        if (!asset) {
            throw std::runtime_error("getBuffer called before a matching asset was found");
        }
        return asset->getBuffer();
    }

protected:
    rgi::GenericBufferAsset* asset;
    std::uint32_t dirtyFrames;

    virtual void process(const event::SceneGraphAssetInitialized& e) override {
        TAsset* a = dynamic_cast<TAsset*>(&e.asset->asset.get());
        if (a && asset != a) {
            asset       = a;
            dirtyFrames = cfg::Limits::MaxConcurrentFrames;
        }
    }
};

} // namespace sr
} // namespace rc
} // namespace bl

#endif
