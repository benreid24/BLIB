#include <BLIB/Components/Animation2D.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Systems/Animation2DSystem.hpp>

namespace bl
{
namespace com
{
Animation2D::Animation2D()
: DrawableBase()
, systemHandle(nullptr)
, player(nullptr) {}

void Animation2D::create(engine::Engine& engine, const Animation2DPlayer& anim) {
    sys::Animation2DSystem& system = engine.systems().getSystem<sys::Animation2DSystem>();
    player                         = &anim;
    system.createNonSlideshow(*this, anim);
}

void Animation2D::initDrawParams(const rc::prim::DrawParameters& params) {
    drawParams = params;
    syncDrawParamsToScene();
}

void Animation2D::updateDrawParams(std::uint32_t indexStart, std::uint32_t indexCount) {
    drawParams.indexOffset = indexStart;
    drawParams.indexCount  = indexCount;
    syncDrawParamsToScene();
}

} // namespace com
} // namespace bl
