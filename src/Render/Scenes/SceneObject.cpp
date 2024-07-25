#include <BLIB/Render/Scenes/SceneObject.hpp>

#include <BLIB/Render/Components/SceneObjectRef.hpp>

namespace bl
{
namespace rc
{
namespace scene
{
SceneObject::SceneObject()
: hidden(false)
, drawParams{}
, refToThis(nullptr) {}

void SceneObject::updateRefToThis() {
    if (refToThis) { refToThis->object = this; }
}

} // namespace scene
} // namespace rc
} // namespace bl
