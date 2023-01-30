target_sources(BLIB PRIVATE
    DescriptorPoolRequirements.cpp
    Framebuffer.cpp
    Pipeline.cpp
    PipelineParameters.cpp
    RenderPass.cpp
    RenderPassParameters.cpp
    StandardImageBuffer.cpp
    Swapchain.cpp
    Transferable.cpp
    TransferEngine.cpp
    VulkanState.cpp
)

add_subdirectory(Utils)
