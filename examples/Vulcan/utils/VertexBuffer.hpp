#ifndef VERTEXBUFFER_HPP
#define VERTEXBUFFER_HPP

#include <glad/vulkan.h>
#include <initializer_list>
#include <vector>

#include <BLIB/Logging.hpp>

class Renderer;

class VertexBufferBase {
public:
    VertexBufferBase(Renderer& owner);
    virtual ~VertexBufferBase();

    void render(VkCommandBuffer commandBuffer);
    virtual void destroy();

protected:
    Renderer& owner;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    std::uint32_t vertexCount;
};

template<typename T>
class VertexBuffer : public VertexBufferBase {
public:
    VertexBuffer(Renderer& owner);
    VertexBuffer(Renderer& owner, unsigned int size);
    virtual ~VertexBuffer() = default;

    void create(unsigned int size);

    unsigned int size() const;

    T* data();
    const T* data() const;

    T& operator[](unsigned int i);
    const T& operator[](unsigned int i) const;

    void sendToGPU();

private:
    std::vector<T> vertices;
    // TODO - add local transform
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
VertexBuffer<T>::VertexBuffer(Renderer& owner)
: VertexBufferBase(owner) {}

template<typename T>
VertexBuffer<T>::VertexBuffer(Renderer& owner, unsigned int size)
: VertexBuffer(owner) {
    create(size);
}

template<typename T>
void VertexBuffer<T>::create(unsigned int size) {
    vertexCount = size;
    vertices.resize(size);

    VkDeviceSize bufferSize = sizeof(T) * vertices.size();
    owner.state.createBuffer(bufferSize,
                             VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             vertexBuffer,
                             vertexBufferMemory);
}

template<typename T>
unsigned int VertexBuffer<T>::size() const {
    return vertices.size();
}

template<typename T>
T* VertexBuffer<T>::data() {
    return vertices.data();
}

template<typename T>
const T* VertexBuffer<T>::data() const {
    return vertices.data();
}

template<typename T>
T& VertexBuffer<T>::operator[](unsigned int i) {
    return vertices[i];
}

template<typename T>
const T& VertexBuffer<T>::operator[](unsigned int i) const {
    return vertices[i];
}

template<typename T>
void VertexBuffer<T>::sendToGPU() {
    VkDeviceSize bufferSize = sizeof(T) * vertices.size();

    // create staging buffer for transfer to faster, device-local, buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    owner.state.createBuffer(bufferSize,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             stagingBuffer,
                             stagingBufferMemory);

    // copy to staging buffer
    void* data;
    vkMapMemory(owner.state.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(owner.state.device, stagingBufferMemory);

    // copy from staging to device memory
    owner.state.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    // destroy staging buffer
    vkDestroyBuffer(owner.state.device, stagingBuffer, nullptr);
    vkFreeMemory(owner.state.device, stagingBufferMemory, nullptr);
}

#endif
