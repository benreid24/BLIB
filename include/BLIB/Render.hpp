#ifndef BLIB_RENDER_HPP
#define BLIB_RENDER_HPP

/**
 * @defgroup Renderer
 * @brief A streamlined rendering system for the game engine
 */

#include <BLIB/Engine/Engine.hpp>

#include <BLIB/Render/Buffers/DynamicSSBO.hpp>
#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Buffers/StaticSSBO.hpp>
#include <BLIB/Render/Buffers/UniformBuffer.hpp>
#include <BLIB/Render/Components/DescriptorComponentBase.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Components/SceneObjectRef.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/Builtin/Object2DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Object2DInstance.hpp>
#include <BLIB/Render/Descriptors/Builtin/Object3DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Object3DInstance.hpp>
#include <BLIB/Render/Descriptors/Builtin/PostFXFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene2DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene2DInstance.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene3DFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene3DInstance.hpp>
#include <BLIB/Render/Descriptors/Builtin/TexturePoolFactory.hpp>
#include <BLIB/Render/Descriptors/Builtin/TexturePoolInstance.hpp>
#include <BLIB/Render/Descriptors/DescriptorComponentStorage.hpp>
#include <BLIB/Render/Descriptors/DescriptorComponentStorageCache.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Drawables/Components/Textured.hpp>
#include <BLIB/Render/Drawables/Components/Transform2D.hpp>
#include <BLIB/Render/Drawables/Sprite.hpp>
#include <BLIB/Render/Drawables/Text.hpp>
#include <BLIB/Render/Drawables/Text/BasicText.hpp>
#include <BLIB/Render/Drawables/Text/VulkanFont.hpp>
#include <BLIB/Render/Events/OverlayEntityScaled.hpp>
#include <BLIB/Render/Events/SceneDestroyed.hpp>
#include <BLIB/Render/Observer.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Render/Overlays/OverlayCamera.hpp>
#include <BLIB/Render/Overlays/OverlayObject.hpp>
#include <BLIB/Render/Overlays/Viewport.hpp>
#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <BLIB/Render/Primitives/Vertex.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Resources/BindlessTextureArray.hpp>
#include <BLIB/Render/Resources/Material.hpp>
#include <BLIB/Render/Resources/MaterialPool.hpp>
#include <BLIB/Render/Resources/PipelineCache.hpp>
#include <BLIB/Render/Resources/PipelineLayoutCache.hpp>
#include <BLIB/Render/Resources/RenderPassCache.hpp>
#include <BLIB/Render/Resources/ScenePool.hpp>
#include <BLIB/Render/Resources/ShaderModuleCache.hpp>
#include <BLIB/Render/Resources/TexturePool.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <BLIB/Render/Scenes/BasicScene.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <BLIB/Render/Scenes/PostFX.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Scenes/SceneObjectStorage.hpp>
#include <BLIB/Render/Scenes/SceneRenderContext.hpp>
#include <BLIB/Render/Systems/CameraUpdateSystem.hpp>
#include <BLIB/Render/Systems/DescriptorComponentSystem.hpp>
#include <BLIB/Render/Systems/OverlayScaler.hpp>
#include <BLIB/Render/Systems/RenderSystem.hpp>
#include <BLIB/Render/Systems/TextSyncSystem.hpp>
#include <BLIB/Render/Transfers/TransferContext.hpp>
#include <BLIB/Render/Transfers/TransferEngine.hpp>
#include <BLIB/Render/Transfers/Transferable.hpp>
#include <BLIB/Render/UpdateSpeed.hpp>
#include <BLIB/Render/Vulkan/AlignedBuffer.hpp>
#include <BLIB/Render/Vulkan/AttachmentBuffer.hpp>
#include <BLIB/Render/Vulkan/AttachmentSet.hpp>
#include <BLIB/Render/Vulkan/Buffer.hpp>
#include <BLIB/Render/Vulkan/CleanupManager.hpp>
#include <BLIB/Render/Vulkan/CommonSamplers.hpp>
#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <BLIB/Render/Vulkan/Framebuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/PerFrameVector.hpp>
#include <BLIB/Render/Vulkan/PerSwapFrame.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <BLIB/Render/Vulkan/PipelineLayout.hpp>
#include <BLIB/Render/Vulkan/PipelineParameters.hpp>
#include <BLIB/Render/Vulkan/RenderPass.hpp>
#include <BLIB/Render/Vulkan/RenderPassParameters.hpp>
#include <BLIB/Render/Vulkan/RenderTexture.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>
#include <BLIB/Render/Vulkan/Swapchain.hpp>
#include <BLIB/Render/Vulkan/Texture.hpp>
#include <BLIB/Render/Vulkan/VkCheck.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>

// Inline methods in the below
#include <BLIB/Render/Drawables/Components/OverlayScalable.hpp>
#include <BLIB/Render/Drawables/Drawable.hpp>
#include <BLIB/Render/Systems/BuiltinDescriptorComponentSystems.hpp>
#include <BLIB/Render/Systems/BuiltinDrawableSystems.hpp>
#include <BLIB/Render/Systems/DrawableSystem.hpp>

#endif
