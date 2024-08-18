#ifndef BLIB_COMPONENTS_SHAPE2D_HPP
#define BLIB_COMPONENTS_SHAPE2D_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Component that allows the renderer to render a 2d shape, or any arbitrary index buffer,
 *        for the owning entity
 *
 * @ingroup Components
 */
struct Shape2D : public rc::rcom::DrawableBase {
    rc::buf::IndexBuffer indexBuffer;

    /**
     * @brief Updates the draw parameters, syncs with the scene, and uploads the new vertices
     */
    void commit();

    /**
     * @brief Returns the default pipeline for regular scenes
     */
    virtual std::uint32_t getDefaultScenePipelineId() const override {
        return rc::Config::PipelineIds::Lit2DGeometry;
    }

    /**
     * @brief Returns the default pipeline for overlays
     */
    virtual std::uint32_t getDefaultOverlayPipelineId() const override {
        return rc::Config::PipelineIds::Unlit2DGeometry;
    }
};

} // namespace com
} // namespace bl

#endif
