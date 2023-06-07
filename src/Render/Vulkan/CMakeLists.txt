target_sources(BLIB PRIVATE
    AttachmentBuffer.cpp
    CommonSamplers.cpp
    DescriptorPool.cpp
    Framebuffer.cpp
    Pipeline.cpp
    PipelineParameters.cpp
    RenderPass.cpp
    RenderPassParameters.cpp
    StandardAttachmentBuffers.cpp
    Swapchain.cpp
    VulkanState.cpp
)

add_subdirectory(Utils)
