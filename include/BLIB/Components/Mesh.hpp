#ifndef BLIB_COMPONENTS_MESH_HPP
#define BLIB_COMPONENTS_MESH_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Primitives/Vertex3D.hpp>
#include <BLIB/Render/Primitives/Vertex3DSkinned.hpp>

namespace bl
{
/// Collection of built-in ECS components
namespace com
{
/**
 * @brief ECS component for a basic mesh from an index buffer
 *
 * @tparam TVertex The vertex type to use in the mesh
 * @tparam MaterialPipelineId The material pipeline to use for rendering
 * @ingroup Components
 */
template<typename TVertex, std::uint32_t MaterialPipelineId>
struct Mesh : public rc::rcom::DrawableBase {
    rc::buf::IndexBufferT<TVertex> gpuBuffer;

    /**
     * @brief Helper method to initialize all the mesh data
     *
     * @param vulkanState Renderer Vulkan state
     * @param vertexCount Number of vertices to create
     * @param indexCount Number of indices to create
     */
    void create(rc::vk::VulkanState& vulkanState, std::uint32_t vertexCount,
                std::uint32_t indexCount) {
        gpuBuffer.create(vulkanState, vertexCount, indexCount);
        drawParams = gpuBuffer.getDrawParameters();
        gpuBuffer.queueTransfer();
    }

    /**
     * @brief Helper method to initialize all the mesh data
     *
     * @param vulkanState Renderer Vulkan state
     * @param vertices An existing vertex buffer to take over
     * @param indices An existing index buffer to take over
     */
    void create(rc::vk::VulkanState& vulkanState, std::vector<TVertex>&& vertices,
                std::vector<std::uint32_t>&& indices) {
        gpuBuffer.create(vulkanState, std::move(vertices), std::move(indices));
        drawParams = gpuBuffer.getDrawParameters();
        gpuBuffer.queueTransfer();
    }

    /**
     * @brief Returns the default material pipeline for rendering
     */
    virtual std::uint32_t getDefaultMaterialPipelineId() const override {
        return MaterialPipelineId;
    }
};

/**
 * @brief A basic 3d mesh component
 *
 * @ingroup Components
 */
using BasicMesh = Mesh<rc::prim::Vertex3D, rc::Config::MaterialPipelineIds::Mesh3DMaterial>;

/**
 * @brief Mesh component for skinned 3d models
 *
 * @ingroup Components
 */
using SkinnedMesh = Mesh<rc::prim::Vertex3DSkinned, rc::Config::MaterialPipelineIds::Mesh3DSkinned>;

} // namespace com
} // namespace bl

#endif
