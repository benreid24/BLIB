#include <BLIB/Systems/Transform2DDescriptorSystem.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace sys
{
Transform2DDescriptorSystem::Transform2DDescriptorSystem()
: interpView(nullptr)
, posOnlyView(nullptr) {}

void Transform2DDescriptorSystem::update(std::mutex&, float, float, float lag, float) {
    posOnlyView->forEach([](PosTags::TComponentSet& cset) {
        auto& t = *cset.get<com::Transform2D>();
        if (t.requiresRefresh()) { t.refresh(); }
    });
    interpView->forEach([lag](InterpTags::TComponentSet& cset) {
        com::Transform2D t = *cset.get<com::Transform2D>();
        cset.get<com::Velocity2D>()->apply(t, lag);
        t.refresh();
    });
}

void Transform2DDescriptorSystem::init(engine::Engine& e) {
    interpView  = InterpTags::getView(e.ecs());
    posOnlyView = PosTags::getView(e.ecs());
}

} // namespace sys
} // namespace bl
