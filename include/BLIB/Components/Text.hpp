#ifndef BLIB_COMPONENTS_TEXT_HPP
#define BLIB_COMPONENTS_TEXT_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>

namespace bl
{
namespace com
{
struct Text : public rc::rcom::DrawableBase {
    // TODO - switch to vertex buffer when added
    rc::buf::IndexBuffer vertices;

    /**
     * @brief Updates the draw parameters to render the entire vertex buffer
     */
    void updateDrawParams();
};

} // namespace com
} // namespace bl

#endif
