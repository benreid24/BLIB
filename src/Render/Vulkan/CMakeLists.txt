target_sources(BLIB PRIVATE
    Buffer.cpp
    CleanupManager.cpp
    CommonSamplers.cpp
    DedicatedCommandBuffers.cpp
    DescriptorPool.cpp
    DescriptorSet.cpp
    Framebuffer.cpp
    Image.cpp
    Pipeline.cpp
    PipelineInstance.cpp
    PipelineLayout.cpp
    PipelineParameters.cpp
    PipelineSpecialization.cpp
    RenderPass.cpp
    RenderPassParameters.cpp
    RenderTexture.cpp
    SharedCommandBuffer.cpp
    SharedCommandPool.cpp
    Swapchain.cpp
    Texture.cpp
    VulkanState.cpp
)

add_subdirectory(Utils)
