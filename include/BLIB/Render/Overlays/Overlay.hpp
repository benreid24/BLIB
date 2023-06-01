#ifndef BLIB_RENDER_OVERLAYS_OVERLAY_HPP
#define BLIB_RENDER_OVERLAYS_OVERLAY_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Overlays/OverlayObject.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace render
{
class Renderer;

/// Classes for Observer overlays
namespace ovy
{
class Overlay {
public:
    //

private:
    Renderer& renderer;
    ds::DescriptorSetFactoryCache& descriptorFactories;
    ds::DescriptorSetInstanceCache descriptorSets;
    std::vector<OverlayObject> objects;
    util::IdAllocator<std::uint32_t> staticIds;
    util::IdAllocator<std::uint32_t> dynamicIds;
    std::vector<std::uint32_t> roots;
    std::vector<std::uint32_t> parentMap;

    Overlay(Renderer& renderer, std::uint32_t maxStatic, std::uint32_t maxDynamic);
    ~Overlay();
};

} // namespace ovy
} // namespace render
} // namespace bl

#endif
