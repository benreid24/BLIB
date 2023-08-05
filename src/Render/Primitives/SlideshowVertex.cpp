#include <BLIB/Render/Primitives/SlideshowVertex.hpp>

namespace bl
{
namespace rc
{
namespace prim
{
SlideshowVertex::SlideshowVertex()
: pos(0.f, 0.f, 0.f)
, slideshowIndex(0) {}

SlideshowVertex::SlideshowVertex(const glm::vec3& pos)
: pos(pos)
, slideshowIndex(0) {}

SlideshowVertex::SlideshowVertex(const glm::vec3& pos, std::uint32_t index)
: pos(pos)
, slideshowIndex(index) {}

VkVertexInputBindingDescription SlideshowVertex::bindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding   = 0;
    bindingDescription.stride    = sizeof(SlideshowVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> SlideshowVertex::attributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

    // pos attribute
    attributeDescriptions[0].binding  = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset   = offsetof(SlideshowVertex, pos);

    // player attribute
    attributeDescriptions[1].binding  = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format   = VK_FORMAT_R32_UINT;
    attributeDescriptions[1].offset   = offsetof(SlideshowVertex, slideshowIndex);

    return attributeDescriptions;
}

} // namespace prim
} // namespace rc
} // namespace bl
