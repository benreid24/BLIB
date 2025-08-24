#include <BLIB/Render/Descriptors/Builtin/GlobalDataFactory.hpp>

#include <BLIB/Render/Descriptors/Builtin/GlobalDataInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
void GlobalDataFactory::init(engine::Engine&, Renderer& renderer) {
    globalData          = &renderer.getGlobalDescriptorData();
    descriptorSetLayout = renderer.getGlobalDescriptorData().getDescriptorLayout();
}

std::unique_ptr<ds::DescriptorSetInstance> GlobalDataFactory::createDescriptorSet() const {
    return std::make_unique<GlobalDataInstance>(*globalData);
}

std::type_index GlobalDataFactory::creates() const { return typeid(GlobalDataInstance); }

} // namespace dsi
} // namespace rc
} // namespace bl
