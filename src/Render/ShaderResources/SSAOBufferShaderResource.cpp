#include <BLIB/Render/ShaderResources/SSAOBufferShaderResource.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Util/Random.hpp>

namespace bl
{
namespace rc
{
namespace sri
{
namespace
{
std::uint32_t getSampleCount(Settings::SSAO ssao) {
    switch (ssao) {
    case Settings::SSAO::Ultra:
        return 64;
    case Settings::SSAO::High:
        return 32;
    case Settings::SSAO::Medium:
        return 16;
    case Settings::SSAO::Low:
        return 8;
    case Settings::SSAO::None:
    default:
        return 0;
    }
}

float lerp(float a, float b, float f) { return a + f * (b - a); }
} // namespace

void SSAOBufferShaderResource::init(engine::Engine& engine, RenderTarget& owner) {
    BufferShaderResource::init(engine, owner);
    BufferShaderResource::getBuffer().setCopyFullRange(true);
    settings = &engine.renderer().getSettings();
    subscribe(engine.renderer().getSignalChannel());
    populateAndTransfer();
}

void SSAOBufferShaderResource::process(const event::SettingsChanged& e) {
    switch (e.setting) {
    case event::SettingsChanged::Setting::SSAO:
    case event::SettingsChanged::Setting::SSAOParams:
        populateAndTransfer();
        break;
    default:
        break;
    }
}

void SSAOBufferShaderResource::populateAndTransfer() {
    auto& params = buffer[0];

    // copy parameters
    params.bias     = settings->getSSAOBias();
    params.radius   = settings->getSSAORadius();
    params.exponent = settings->getSSAOExponent();

    // create sample direction vectors
    params.sampleCount = getSampleCount(settings->getSSAO());
    for (std::uint32_t i = 0; i < params.sampleCount; ++i) {
        // make random unit vector with positive z (facing outside of surface)
        glm::vec3 s(util::Random::get<float>(-1.f, 1.f),
                    util::Random::get<float>(-1.f, 1.f),
                    util::Random::get<float>(0.f, 1.f));
        s = glm::normalize(s);

        // scale by random factor
        s *= util::Random::get<float>(0.f, 1.f);

        // scale by lerp of i to gen more samples near to the surface
        float scale = static_cast<float>(i) / static_cast<float>(params.sampleCount);
        scale       = lerp(0.1f, 1.0f, scale * scale);
        s *= scale;

        params.samples[i] = glm::vec4(s, 0.f);
    }

    // create random noise rotation vectors
    for (std::uint32_t x = 0; x < 4; ++x) {
        for (std::uint32_t y = 0; y < 4; ++y) {
            params.randomRotations[x][y] = glm::vec4(
                util::Random::get<float>(-1.f, 1.f), util::Random::get<float>(-1.f, 1.f), 0.f, 0.f);
        }
    }

    buffer.queueTransfer();
}

} // namespace sri
} // namespace rc
} // namespace bl
