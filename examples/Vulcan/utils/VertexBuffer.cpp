#include "VertexBuffer.hpp"
#include "../Renderer.hpp"

VertexBufferBase::VertexBufferBase(Renderer& owner)
: owner(owner) {
    owner.registerVertexBuffer(this);
}

VertexBufferBase::~VertexBufferBase() {
    vkDeviceWaitIdle(owner.state.device);
    destroy();
    owner.unregisterVertexBuffer(this);
}

void VertexBufferBase::render(VkCommandBuffer commandBuffer) {
    // TODO - apply local transform
    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[]   = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

void VertexBufferBase::destroy() {
    if (!vertexBuffer || !vertexBufferMemory) return;
    vkDestroyBuffer(owner.state.device, vertexBuffer, nullptr);
    vkFreeMemory(owner.state.device, vertexBufferMemory, nullptr);
    vertexBuffer       = nullptr;
    vertexBufferMemory = nullptr;
}
