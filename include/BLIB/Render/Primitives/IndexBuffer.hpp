#ifndef BLIB_RENDER_PRIMITIVES_INDEXBUFFER_HPP
#define BLIB_RENDER_PRIMITIVES_INDEXBUFFER_HPP

#include <BLIB/Render/Primitives/GenericBuffer.hpp>
#include <BLIB/Render/Primitives/Vertex.hpp>
#include <BLIB/Render/Scenes/DrawParameters.hpp>

namespace bl
{
namespace render
{
namespace prim
{

/**
 * @brief Basic index buffer class built on top of GenericBuffer. Intended for static mesh data
 *
 * @tparam T The type of vertex to index into. Defaults to Vertex
 * @ingroup Renderer
 */
template<typename T>
class IndexBufferT {
public:
    static constexpr VkIndexType IndexType = VK_INDEX_TYPE_UINT32;
    using TVertexBuffer = GenericBuffer<T, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false>;
    using TIndexBuffer  = GenericBuffer<std::uint32_t, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false>;

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
    constexpr TVertexBuffer& vertices();

    /**
     * @brief Returns the vertex buffer
     */
    constexpr const TVertexBuffer& vertices() const;

    /**
     * @brief Returns the index buffer
     */
    constexpr TIndexBuffer& indices();

    /**
     * @brief Returns the index buffer
     */
    constexpr const TIndexBuffer& indices() const;

    /**
     * @brief Sends both buffers to the GPU
     */
    void sendToGPU();

    /**
     * @brief Builds and returns the parameters required to render the index buffer
     */
    DrawParameters getDrawParameters() const;

private:
    TVertexBuffer vertexBuffer;
    TIndexBuffer indexBuffer;
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
constexpr IndexBufferT<T>::TVertexBuffer& IndexBufferT<T>::vertices() {
    return vertexBuffer;
}

template<typename T>
constexpr const IndexBufferT<T>::TVertexBuffer& IndexBufferT<T>::vertices() const {
    return vertexBuffer;
}

template<typename T>
constexpr IndexBufferT<T>::TIndexBuffer& IndexBufferT<T>::indices() {
    return indexBuffer;
}

template<typename T>
constexpr const IndexBufferT<T>::TIndexBuffer& IndexBufferT<T>::indices() const {
    return indexBuffer;
}

template<typename T>
void IndexBufferT<T>::sendToGPU() {
    vertexBuffer.queueTransfer(tfr::Transferable::SyncRequirement::DeviceIdle);
    indexBuffer.queueTransfer(tfr::Transferable::SyncRequirement::DeviceIdle);
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

} // namespace prim
} // namespace render
} // namespace bl

#endif
