#ifndef BLIB_RENDER_SHADERRESOURCES_SSAOBUFFERSHADERRESOURCE_HPP
#define BLIB_RENDER_SHADERRESOURCES_SSAOBUFFERSHADERRESOURCE_HPP

#include <BLIB/Render/Buffers/BufferSingleDeviceLocalSourced.hpp>
#include <BLIB/Render/Events/SettingsChanged.hpp>
#include <BLIB/Render/ShaderResources/BufferShaderResource.hpp>
#include <BLIB/Render/ShaderResources/Key.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
class Settings;

namespace sri
{
/**
 * @brief Uniform payload for the SSAO algorithm
 *
 * @ingroup Renderer
 */
struct SSAOBufferPayload {
    glm::vec4 randomRotations[4][4];
    glm::vec4 samples[128];
    std::uint32_t sampleCount;
    float radius;
    float bias;
    float exponent;
};

/**
 * @brief Shader resource for the global SSAO settings
 *
 * @ingroup Renderer
 */
class SSAOBufferShaderResource
: public sr::BufferShaderResource<buf::BufferSingleDeviceLocalSourcedUBO<SSAOBufferPayload>, 1>
, public sig::Listener<event::SettingsChanged> {
public:
    /**
     * @brief Creates the shader resource
     */
    SSAOBufferShaderResource() = default;

    /**
     * @brief Destroys the shader resource
     */
    virtual ~SSAOBufferShaderResource() = default;

private:
    Settings* settings;

    virtual void init(engine::Engine& engine, RenderTarget& owner) override;
    virtual void process(const event::SettingsChanged& event) override;

    void populateAndTransfer();
};

/**
 * @brief The key for the global SSAO settings shader buffer resource
 *
 * @ingroup Renderer
 */
constexpr sr::Key<SSAOBufferShaderResource> SSAOBufferShaderResourceKey{"__builtin_SSAOBuffer"};

} // namespace sri
} // namespace rc
} // namespace bl

#endif
