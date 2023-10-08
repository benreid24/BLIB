#include <BLIB/Render/Primitives/SlideshowVertex.hpp>

namespace bl
{
namespace rc
{
namespace prim
{
SlideshowVertex::SlideshowVertex()
: pos(0.f, 0.f, 0.f)
, color(1.f, 1.f, 1.f, 1.f)
, slideshowIndex(0) {}

SlideshowVertex::SlideshowVertex(const glm::vec3& pos)
: pos(pos)
, color(1.f, 1.f, 1.f, 1.f)
, slideshowIndex(0) {}

SlideshowVertex::SlideshowVertex(const glm::vec3& pos, const glm::vec4& color)
: pos(pos)
, color(color)
, slideshowIndex(0) {}

SlideshowVertex::SlideshowVertex(const glm::vec3& pos, std::uint32_t index)
: pos(pos)
, color(1.f, 1.f, 1.f, 1.f)
, slideshowIndex(index) {}

SlideshowVertex::SlideshowVertex(const glm::vec3& pos, const glm::vec4& color,
                                 std::uint32_t slideshowIndex)
: pos(pos)
, color(color)
, slideshowIndex(slideshowIndex) {}

VkVertexInputBindingDescription SlideshowVertex::bindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding   = 0;
    bindingDescription.stride    = sizeof(SlideshowVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> SlideshowVertex::attributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

    // pos attribute
    attributeDescriptions[0].binding  = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset   = offsetof(SlideshowVertex, pos);

    // color attribute
    attributeDescriptions[1].binding  = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[1].offset   = offsetof(SlideshowVertex, color);

    // player attribute
    attributeDescriptions[2].binding  = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format   = VK_FORMAT_R32_UINT;
    attributeDescriptions[2].offset   = offsetof(SlideshowVertex, slideshowIndex);

    return attributeDescriptions;
}

} // namespace prim
} // namespace rc
} // namespace bl
