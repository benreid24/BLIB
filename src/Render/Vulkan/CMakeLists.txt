target_sources(BLIB PRIVATE
    AttachmentBuffer.cpp
    CommonSamplers.cpp
    DescriptorPool.cpp
    Framebuffer.cpp
    Pipeline.cpp
    PipelineLayout.cpp
    PipelineParameters.cpp
    RenderPass.cpp
    RenderPassParameters.cpp
    RenderTexture.cpp
    StandardAttachmentBuffers.cpp
    Swapchain.cpp
    Texture.cpp
    VulkanState.cpp
)

add_subdirectory(Utils)
