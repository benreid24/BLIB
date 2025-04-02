target_sources(BLIB PRIVATE
    AttachmentBuffer.cpp
    Buffer.cpp
    CleanupManager.cpp
    CommonSamplers.cpp
    DedicatedCommandBuffers.cpp
    DescriptorPool.cpp
    DescriptorSet.cpp
    Framebuffer.cpp
    Pipeline.cpp
    PipelineLayout.cpp
    PipelineParameters.cpp
    RenderPass.cpp
    RenderPassParameters.cpp
    RenderTexture.cpp
    SharedCommandBuffer.cpp
    SharedCommandPool.cpp
    StandardAttachmentBuffers.cpp
    Swapchain.cpp
    Texture.cpp
    TextureBase.cpp
    VulkanState.cpp
)

add_subdirectory(Utils)
