target_sources(BLIB PRIVATE
    CommonDescriptorSetLayouts.cpp
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
