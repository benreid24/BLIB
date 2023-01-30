#ifndef BLIB_RENDER_PRIMITIVES_INDEXBUFFER_HPP
#define BLIB_RENDER_PRIMITIVES_INDEXBUFFER_HPP

#include <BLIB/Render/Primitives/Buffer.hpp>
#include <BLIB/Render/Primitives/Vertex.hpp>
#include <BLIB/Render/Renderer/DrawParameters.hpp>

namespace bl
{
namespace render
{

/**
 * @brief Basic index buffer class built on top of Buffer
 *
 * @tparam T The type of vertex to index into. Defaults to Vertex
 * @ingroup Renderer
 */
template<typename T>
class IndexBufferT {
public:
    /**
     * @brief Creates the vertex and index buffers
     *
     * @param vulkanState The renderer vulkan state
     * @param vertexCount The number of vertices
     * @param indexCount The number of indices
     */
    void create(VulkanState& vulkanState, std::uint32_t vertexCount, std::uint32_t indexCount);

    /**
     * @brief Frees both the vertex buffer and index buffer
     */
    void destroy();

    /**
     * @brief Returns the vertex buffer
     */
    constexpr Buffer<T>& vertices();

    /**
     * @brief Returns the vertex buffer
     */
    constexpr const Buffer<T>& vertices() const;

    /**
     * @brief Returns the index buffer
     */
    constexpr Buffer<std::uint32_t>& indices();

    /**
     * @brief Returns the index buffer
     */
    constexpr const Buffer<std::uint32_t>& indices() const;

    /**
     * @brief Sends both buffers to the GPU
     */
    void sendToGPU();

    /**
     * @brief Builds and returns the parameters required to render the index buffer
     */
    DrawParameters getDrawParameters() const;

private:
    Buffer<T> vertexBuffer;
    Buffer<std::uint32_t> indexBuffer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
void IndexBufferT<T>::create(VulkanState& vulkanState, std::uint32_t vertexCount,
                            std::uint32_t indexCount) {
    vertexBuffer.create(vulkanState, vertexCount, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    indexBuffer.create(vulkanState, indexCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

template<typename T>
void IndexBufferT<T>::destroy() {
    vertexBuffer.destroy();
    indexBuffer.destroy();
}

template<typename T>
constexpr Buffer<T>& IndexBufferT<T>::vertices() {
    return vertexBuffer;
}

template<typename T>
constexpr const Buffer<T>& IndexBufferT<T>::vertices() const {
    return vertexBuffer;
}

template<typename T>
constexpr Buffer<std::uint32_t>& IndexBufferT<T>::indices() {
    return indexBuffer;
}

template<typename T>
constexpr const Buffer<std::uint32_t>& IndexBufferT<T>::indices() const {
    return indexBuffer;
}

template<typename T>
void IndexBufferT<T>::sendToGPU() {
    vertexBuffer.queueTransfer();
    indexBuffer.queueTransfer();
}

template<typename T>
inline DrawParameters IndexBufferT<T>::getDrawParameters() const {
    DrawParameters params;
    params.indexBuffer  = indexBuffer.handle();
    params.indexCount   = indexBuffer.size();
    params.indexOffset  = 0;
    params.vertexBuffer = vertexBuffer.handle();
    params.vertexOffset = 0;
    return params;
}

/**
 * @brief Convenience alias for a standard index buffer
 * @ingroup Renderer
*/
using IndexBuffer = IndexBufferT<Vertex>;

} // namespace render
} // namespace bl

#endif
