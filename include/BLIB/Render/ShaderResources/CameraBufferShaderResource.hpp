#ifndef BLIB_RENDER_SHADERRESOURCES_CAMERABUFFERSHADERRESOURCE_HPP
#define BLIB_RENDER_SHADERRESOURCES_CAMERABUFFERSHADERRESOURCE_HPP

#include <BLIB/Cameras/Camera.hpp>
#include <BLIB/Render/Buffers/UniformBuffer.hpp>
#include <BLIB/Render/ShaderResources/BufferShaderResource.hpp>
#include <BLIB/Render/ShaderResources/Key.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
class RenderTarget;
class Overlay;

/// Namespace containing implementations of specific shader resources
namespace sri
{
/**
 * @brief Uniform structure for camera data
 *
 * @ingroup Renderer
 */
struct CameraBufferUniform {
    glm::mat4 projection;
    glm::mat4 view;
    alignas(16) glm::vec3 cameraPosition;
};

/**
 * @brief Shader resource for the camera uniform buffer
 *
 * @ingroup Renderer
 */
class CameraBufferShaderResource
: public sr::BufferShaderResource<buf::UniformBuffer<CameraBufferUniform>, 1> {
public:
    /**
     * @brief Creates the shader resource
     *
     * @param owner The render target that owns this resource
     */
    CameraBufferShaderResource(RenderTarget& owner);

    /**
     * @brief Creates the shader resource for an overlay camera
     *
     * @param owner The render target that owns this resource
     * @param overlay An overlay that this resource is for
     */
    CameraBufferShaderResource(RenderTarget& owner, Overlay& overlay);

    /**
     * @brief Creates the GPU buffer
     *
     * @param engine The game engine instance
     */
    virtual void init(engine::Engine& engine) override;

    /**
     * @brief Copies the camera data into the local buffer
     */
    virtual void copyFromSource() override;

private:
    RenderTarget& owner;
    bool isOverlay;
};

/**
 * @brief Key for the built-in camera buffer shader resource
 *
 * @ingroup Renderer
 */
constexpr sr::Key<CameraBufferShaderResource> CameraBufferKey("__builtin_CameraBuffer");

} // namespace sri
} // namespace rc
} // namespace bl

#endif
