#ifndef BLIB_GRAPHICS_SHAPE_HPP
#define BLIB_GRAPHICS_SHAPE_HPP

#include <BLIB/Components/Shape2D.hpp>
#include <BLIB/Graphics/Components/OverlayScalable.hpp>
#include <BLIB/Graphics/Components/Transform2D.hpp>
#include <BLIB/Graphics/Drawable.hpp>

namespace bl
{
namespace gfx
{
class Shape2D
: public Drawable<com::Shape2D> // TODO - add Textured and set pipeline on change
, public bcom::OverlayScalable {
public:
    void update(); // TODO - figure out how to avoid calling this manually?
    // TODO - one time run system for tasks to register? runs at certain (arbitrary) frame stage

protected:
    Shape2D();

    virtual unsigned int getVertexCount() const = 0;

    virtual void populateVertex(unsigned int index, rc::prim::Vertex& vertex) = 0;

private:
    glm::vec4 fillColor;
    glm::vec4 outlineColor;
    float outlineThickness;
    bool dirty;
};

} // namespace gfx
} // namespace bl

#endif
