#include <BLIB/Render/Renderables/Renderable.hpp>
#include <BLIB/Render/Renderer/Object.hpp>

namespace bl
{
namespace render
{
Object::Object(Renderable* o)
: owner(o)
, hidden(false)
, drawParams{} {
    frameData.transform = glm::mat4(1.f);
    frameData.index     = 0; // TODO - which value for null?
}

} // namespace render
} // namespace bl
