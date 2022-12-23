#ifndef INDEXBUFFER_HPP
#define INDEXBUFFER_HPP

#include "VertexBuffer.hpp"

template<typename T>
class IndexBuffer : public VertexBuffer<T> {
public:
    IndexBuffer(Renderer& owner);
    IndexBuffer(Renderer& owner, unsigned int vertexCount, unsigned int indiceCount);
    virtual ~IndexBuffer();

    void create(unsigned int vertexCount, unsigned int indiceCount);

    unsigned int indiceCount() const;
    std::uint32_t& indice(unsigned int i);
    std::uint32_t* indiceData();

    void sendToGPU();
    void render(VkCommandBuffer commandBuffer);

private:
    std::vector<std::uint32_t> indices;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    void create(unsigned int); // to shadow and make VertexBuffer::create unavailable

    virtual void destroy() override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
IndexBuffer<T>::IndexBuffer(Renderer& owner)
: VertexBuffer(owner) {}

template<typename T>
IndexBuffer<T>::IndexBuffer(Renderer& owner, unsigned int vc, unsigned int ic)
: IndexBuffer(owner) {
    create(vc, ic);
}

template<typename T>
IndexBuffer<T>::~IndexBuffer() {
    vkDeviceWaitIdle(owner.state.device);
    destroy();
}

template<typename T>
void IndexBuffer<T>::create(unsigned int vc, unsigned int ic) {
    VertexBuffer<T>::create(vc);

    indices.resize(ic);
    VkDeviceSize bufferSize = sizeof(std::uint32_t) * ic;
    owner.state.createBuffer(bufferSize,
                             VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             indexBuffer,
                             indexBufferMemory);
}

template<typename T>
void IndexBuffer<T>::sendToGPU() {
    VertexBuffer<T>::sendToGPU();

    VkDeviceSize bufferSize = sizeof(std::uint32_t) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    owner.state.createBuffer(bufferSize,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             stagingBuffer,
                             stagingBufferMemory);

    void* data;
    vkMapMemory(owner.state.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(owner.state.device, stagingBufferMemory);

    owner.state.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(owner.state.device, stagingBuffer, nullptr);
    vkFreeMemory(owner.state.device, stagingBufferMemory, nullptr);
}

template<typename T>
unsigned int IndexBuffer<T>::indiceCount() const {
    return indices.size();
}

template<typename T>
std::uint32_t& IndexBuffer<T>::indice(unsigned int i) {
    return indices[i];
}

template<typename T>
std::uint32_t* IndexBuffer<T>::indiceData() {
    return indices.data();
}

template<typename T>
void IndexBuffer<T>::render(VkCommandBuffer commandBuffer) {
    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[]   = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}

template<typename T>
void IndexBuffer<T>::destroy() {
    VertexBuffer<T>::destroy();

    if (!indexBuffer || !indexBufferMemory) return;
    vkDestroyBuffer(owner.state.device, indexBuffer, nullptr);
    vkFreeMemory(owner.state.device, indexBufferMemory, nullptr);
    indexBuffer       = nullptr;
    indexBufferMemory = nullptr;
}

#endif
