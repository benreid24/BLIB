#ifndef BLIB_RENDER_PRIMITIVES_INDEXBUFFER_HPP
#define BLIB_RENDER_PRIMITIVES_INDEXBUFFER_HPP

#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <BLIB/Render/Primitives/Vertex.hpp>
#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/Vulkan/Buffer.hpp>
#include <cstring>
#include <vector>

namespace bl
{
namespace rc
{
namespace buf
{

/**
 * @brief Basic index buffer class built on top of vk::Buffer. Intended for static mesh data
 *
 * @tparam T The type of vertex to index into. Defaults to Vertex
 * @ingroup Renderer
 */
template<typename T>
class IndexBufferT : public tfr::Transferable {
public:
    static constexpr VkIndexType IndexType = VK_INDEX_TYPE_UINT32;

    /**
     * @brief Creates the vertex and index buffers
     *
     * @param vulkanState The renderer vulkan state
     * @param vertexCount The number of vertices
     * @param indexCount The number of indices
     */
    void create(vk::VulkanState& vulkanState, std::uint32_t vertexCount, std::uint32_t indexCount);

    /**
     * @brief Frees both the vertex buffer and index buffer
     */
    void destroy();

    /**
     * @brief Returns the vertex buffer
     */
    constexpr std::vector<T>& vertices();

    /**
     * @brief Returns the vertex buffer
     */
    constexpr const std::vector<T>& vertices() const;

    /**
     * @brief Returns the index buffer
     */
    constexpr std::vector<std::uint32_t>& indices();

    /**
     * @brief Returns the index buffer
     */
    constexpr const std::vector<std::uint32_t>& indices() const;

    /**
     * @brief Builds and returns the parameters required to render the index buffer
     */
    prim::DrawParameters getDrawParameters() const;

    /**
     * @brief Returns the number of indices in the buffer
     */
    constexpr std::uint32_t indexCount() const;

    /**
     * @brief Returns the number of vertices in the buffer
     */
    constexpr std::uint32_t vertexCount() const;

    /**
     * @brief Returns the Vulkan handle of the vertex buffer
     */
    constexpr VkBuffer vertexBufferHandle() const;

    /**
     * @brief Returns the Vulkan handle of the index buffer
     */
    constexpr VkBuffer indexBufferHandle() const;

private:
    std::vector<prim::Vertex> cpuVertexBuffer;
    std::vector<std::uint32_t> cpuIndexBuffer;
    vk::Buffer gpuVertexBuffer;
    vk::Buffer gpuIndexBuffer;

    virtual void executeTransfer(VkCommandBuffer commandBuffer,
                                 tfr::TransferContext& context) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
void IndexBufferT<T>::create(vk::VulkanState& vs, std::uint32_t vc, std::uint32_t ic) {
    vulkanState = &vs;
    cpuVertexBuffer.resize(vc);
    cpuIndexBuffer.resize(ic, 0);
    gpuVertexBuffer.create(vs,
                           vc * sizeof(T),
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                           VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                           0);
    gpuIndexBuffer.create(vs,
                          ic * sizeof(std::uint32_t),
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                          VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                          0);
}

template<typename T>
void IndexBufferT<T>::destroy() {
    cpuVertexBuffer.clear();
    cpuIndexBuffer.clear();
    gpuVertexBuffer.destroy();
    gpuIndexBuffer.destroy();
}

template<typename T>
constexpr std::vector<T>& IndexBufferT<T>::vertices() {
    return cpuVertexBuffer;
}

template<typename T>
constexpr const std::vector<T>& IndexBufferT<T>::vertices() const {
    return cpuVertexBuffer;
}

template<typename T>
constexpr std::vector<std::uint32_t>& IndexBufferT<T>::indices() {
    return cpuIndexBuffer;
}

template<typename T>
constexpr const std::vector<std::uint32_t>& IndexBufferT<T>::indices() const {
    return cpuIndexBuffer;
}

template<typename T>
prim::DrawParameters IndexBufferT<T>::getDrawParameters() const {
    prim::DrawParameters params;
    params.indexBuffer   = gpuIndexBuffer.getBuffer();
    params.indexCount    = cpuIndexBuffer.size();
    params.indexOffset   = 0;
    params.vertexBuffer  = gpuVertexBuffer.getBuffer();
    params.vertexOffset  = 0;
    params.firstInstance = 0;
    params.instanceCount = 1;
    return params;
}

template<typename T>
constexpr std::uint32_t IndexBufferT<T>::indexCount() const {
    return cpuIndexBuffer.size();
}

template<typename T>
constexpr std::uint32_t IndexBufferT<T>::vertexCount() const {
    return cpuVertexBuffer.size();
}

template<typename T>
constexpr VkBuffer IndexBufferT<T>::vertexBufferHandle() const {
    return gpuVertexBuffer.getBuffer();
}

template<typename T>
constexpr VkBuffer IndexBufferT<T>::indexBufferHandle() const {
    return gpuIndexBuffer.getBuffer();
}

template<typename T>
void IndexBufferT<T>::executeTransfer(VkCommandBuffer commandBuffer,
                                      tfr::TransferContext& context) {
    // vertex buffer
    VkBuffer stagingBuf;
    void* stagingMem;
    context.createTemporaryStagingBuffer(gpuVertexBuffer.getSize(), stagingBuf, &stagingMem);
    std::memcpy(stagingMem, cpuVertexBuffer.data(), gpuVertexBuffer.getSize());

    VkBufferCopy copyCmd{};
    copyCmd.srcOffset = 0;
    copyCmd.dstOffset = 0;
    copyCmd.size      = gpuVertexBuffer.getSize();
    vkCmdCopyBuffer(commandBuffer, stagingBuf, gpuVertexBuffer.getBuffer(), 1, &copyCmd);

    VkBufferMemoryBarrier barrier{};
    barrier.sType         = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.buffer        = gpuVertexBuffer.getBuffer();
    barrier.offset        = 0;
    barrier.size          = gpuVertexBuffer.getSize();
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    context.registerBufferBarrier(barrier);

    // index buffer
    context.createTemporaryStagingBuffer(gpuIndexBuffer.getSize(), stagingBuf, &stagingMem);
    std::memcpy(stagingMem, cpuIndexBuffer.data(), gpuIndexBuffer.getSize());

    copyCmd.size = gpuIndexBuffer.getSize();
    vkCmdCopyBuffer(commandBuffer, stagingBuf, gpuIndexBuffer.getBuffer(), 1, &copyCmd);

    barrier.buffer = gpuIndexBuffer.getBuffer();
    barrier.size   = gpuIndexBuffer.getSize();
    context.registerBufferBarrier(barrier);
}

/**
 * @brief Convenience alias for a standard index buffer
 * @ingroup Renderer
 */
using IndexBuffer = IndexBufferT<prim::Vertex>;

} // namespace buf
} // namespace rc
} // namespace bl

#endif
