#ifndef BLIB_GRAPHICS_RECTANGLE_HPP
#define BLIB_GRAPHICS_RECTANGLE_HPP

#include <BLIB/Graphics/Shape2D.hpp>

namespace bl
{
namespace gfx
{
class Rectangle : public Shape2D {
public:
    Rectangle();

    void create(engine::Engine& engine, const glm::vec2& size);

    void setSize(const glm::vec2& size);

    const glm::vec2& getSize() const;

private:
    glm::vec2 size;
    // TODO - color overrides for gradients

    virtual unsigned int getVertexCount() const override;
    virtual void populateVertex(unsigned int index, rc::prim::Vertex& vertex) override;
};

} // namespace gfx
} // namespace bl

#endif
