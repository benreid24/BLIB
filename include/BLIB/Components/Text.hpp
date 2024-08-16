#ifndef BLIB_COMPONENTS_TEXT_HPP
#define BLIB_COMPONENTS_TEXT_HPP

#include <BLIB/Render/Buffers/VertexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Component for rendered text
 *
 * @ingroup Components
 */
struct Text : public rc::rcom::DrawableBase {
    rc::buf::VertexBuffer vertices;

    /**
     * @brief Creates an empty component
     */
    Text();

    /**
     * @brief Updates the draw parameters to render the entire vertex buffer
     *
     * @param vertexCount The number of vertices to actually render
     */
    void updateDrawParams(std::uint32_t vertexCount);

    /**
     * @brief Returns the default pipeline for regular scenes
     */
    virtual std::uint32_t getDefaultScenePipelineId() const override {
        return rc::Config::PipelineIds::Text;
    }

    /**
     * @brief Returns the default pipeline for overlays
     */
    virtual std::uint32_t getDefaultOverlayPipelineId() const override {
        return rc::Config::PipelineIds::Text;
    }
};

} // namespace com
} // namespace bl

#endif
