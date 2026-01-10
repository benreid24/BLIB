#ifndef BLIB_COMPONENTS_BATCHEDSHAPES2D_HPP
#define BLIB_COMPONENTS_BATCHEDSHAPES2D_HPP

#include <BLIB/Render/Buffers/BatchIndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Config/MaterialPipelineIds.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Component that allows the renderer to render a batch of 2d shapes
 *
 * @ingroup Components
 */
struct BatchedShapes2D : public rc::rcom::DrawableBase {
    rc::buf::BatchIndexBuffer indexBuffer;

    /**
     * @brief Creates the index buffer with the given starting capacities
     *
     * @param renderer The renderer instance
     * @param vertexCapacity The number of vertices to allocate
     * @param indexCapacity The number of indices to allocate
     */
    BatchedShapes2D(rc::Renderer& renderer, std::uint32_t vertexCapacity,
                    std::uint32_t indexCapacity);

    /**
     * @brief Updates the draw parameters and syncs with the scene
     */
    void commit();

    /**
     * @brief Returns the default material pipeline for rendering
     */
    virtual std::uint32_t getDefaultMaterialPipelineId() const override {
        return rc::cfg::MaterialPipelineIds::Geometry2D;
    }
};

} // namespace com
} // namespace bl

#endif
