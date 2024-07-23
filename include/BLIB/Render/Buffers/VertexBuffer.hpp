#ifndef BLIB_RENDER_PRIMITIVES_VERTEXBUFFER_HPP
#define BLIB_RENDER_PRIMITIVES_VERTEXBUFFER_HPP

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
 * @brief Basic vertex buffer class built on top of vk::Buffer. Intended for static mesh data
 *
 * @tparam T The type of vertex to index into
 * @ingroup Renderer
 */
template<typename T>
class VertexBufferT : public tfr::Transferable {
public:
    /**
     * @brief Defers destruction of the buffers
     */
    virtual ~VertexBufferT();

    /**
     * @brief Creates the vertex and index buffers
     *
     * @param vulkanState The renderer vulkan state
     * @param vertexCount The number of vertices
     */
    void create(vk::VulkanState& vulkanState, std::uint32_t vertexCount);

    /**
     * @brief Resizes to the given vertex count. Must not be called before create()
     *
     * @param vertexCount The new number of vertices
     * @param copyOldVertices Whether to copy the old buffer into the new
     */
    void resize(std::uint32_t vertexCount, bool copyOldVertices = false);

    /**
     * @brief Frees the vertex buffer
     */
    void destroy();

    /**
     * @brief Queues the buffer to be erased in a deferred manner
     */
    void deferDestruction();

    /**
     * @brief Returns the vertex buffer
     */
    constexpr std::vector<T>& vertices();

    /**
     * @brief Returns the vertex buffer
     */
    constexpr const std::vector<T>& vertices() const;

    /**
     * @brief Builds and returns the parameters required to render the index buffer
     */
    prim::DrawParameters getDrawParameters() const;

    /**
     * @brief Returns the number of vertices in the buffer
     */
    constexpr std::uint32_t vertexCount() const;

    /**
     * @brief Returns the Vulkan handle of the vertex buffer
     */
    constexpr VkBuffer bufferHandle() const;

private:
    std::vector<prim::Vertex> cpuVertexBuffer;
    vk::Buffer gpuVertexBuffer;

    virtual void executeTransfer(VkCommandBuffer commandBuffer,
                                 tfr::TransferContext& context) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
inline VertexBufferT<T>::~VertexBufferT() {
    deferDestruction();
}

template<typename T>
void VertexBufferT<T>::create(vk::VulkanState& vs, std::uint32_t vc) {
    vulkanState = &vs;
    cpuVertexBuffer.resize(vc);
    gpuVertexBuffer.create(vs,
                           vc * sizeof(T),
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                           VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                           0);
}

template<typename T>
inline void VertexBufferT<T>::resize(std::uint32_t vc, bool copy) {
    cpuVertexBuffer.resize(vc);
    gpuVertexBuffer.ensureSize(vc * sizeof(T), !copy);
}

template<typename T>
void VertexBufferT<T>::destroy() {
    cpuVertexBuffer.clear();
    gpuVertexBuffer.destroy();
}

template<typename T>
inline void VertexBufferT<T>::deferDestruction() {
    cpuVertexBuffer.clear();
    gpuVertexBuffer.deferDestruction();
}

template<typename T>
constexpr std::vector<T>& VertexBufferT<T>::vertices() {
    return cpuVertexBuffer;
}

template<typename T>
constexpr const std::vector<T>& VertexBufferT<T>::vertices() const {
    return cpuVertexBuffer;
}

template<typename T>
prim::DrawParameters VertexBufferT<T>::getDrawParameters() const {
    prim::DrawParameters params;
    params.type          = prim::DrawParameters::DrawType::VertexBuffer;
    params.vertexBuffer  = gpuVertexBuffer.getBuffer();
    params.vertexCount   = vertexCount();
    params.vertexOffset  = 0;
    params.firstInstance = 0;
    params.instanceCount = 1;
    return params;
}

template<typename T>
constexpr std::uint32_t VertexBufferT<T>::vertexCount() const {
    return cpuVertexBuffer.size();
}

template<typename T>
constexpr VkBuffer VertexBufferT<T>::bufferHandle() const {
    return gpuVertexBuffer.getBuffer();
}

template<typename T>
void VertexBufferT<T>::executeTransfer(VkCommandBuffer commandBuffer,
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
}

/**
 * @brief Convenience alias for a standard vertex buffer
 * @ingroup Renderer
 */
using VertexBuffer = VertexBufferT<prim::Vertex>;

} // namespace buf
} // namespace rc
} // namespace bl

#endif
