#include <BLIB/Render/Primitives/Vertex.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace render
{
namespace prim
{
Vertex::Vertex()
: pos()
, color(1.f, 1.f, 1.f, 1.f)
, texCoord() {}

Vertex::Vertex(const glm::vec3& pos)
: pos(pos)
, color(1.f, 1.f, 1.f, 1.f)
, texCoord() {}

Vertex::Vertex(const glm::vec3& position, const glm::vec4& color)
: pos(position)
, color(color)
, texCoord() {}

Vertex::Vertex(const glm::vec3& position, const glm::vec2& texCoord)
: pos(position)
, color(1.f, 1.f, 1.f, 1.f)
, texCoord(texCoord) {}

Vertex::Vertex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& texCoord)
: pos(position)
, color(color)
, texCoord(texCoord) {}

Vertex::Vertex(std::initializer_list<float> fl)
: Vertex() {
    switch (fl.size()) {
    case 3: // position only
        pos.x = *fl.begin();
        pos.y = *(fl.begin() + 1);
        pos.z = *(fl.begin() + 2);
        break;

    case 5: // position + texture coord
        pos.x      = *fl.begin();
        pos.y      = *(fl.begin() + 1);
        pos.z      = *(fl.begin() + 2);
        texCoord.x = *(fl.begin() + 3);
        texCoord.y = *(fl.begin() + 4);
        break;

    case 7: // position + color
        pos.x   = *fl.begin();
        pos.y   = *(fl.begin() + 1);
        pos.z   = *(fl.begin() + 2);
        color.x = *(fl.begin() + 3);
        color.y = *(fl.begin() + 4);
        color.z = *(fl.begin() + 5);
        color.w = *(fl.begin() + 6);
        break;

    case 9: // position + color + texture coord
        pos.x      = *fl.begin();
        pos.y      = *(fl.begin() + 1);
        pos.z      = *(fl.begin() + 2);
        color.x    = *(fl.begin() + 3);
        color.y    = *(fl.begin() + 4);
        color.z    = *(fl.begin() + 5);
        color.w    = *(fl.begin() + 6);
        texCoord.x = *(fl.begin() + 7);
        texCoord.y = *(fl.begin() + 8);
        break;

    default:
        BL_LOG_ERROR << "Invalid number of elements to construct vertex: " << fl.size();
        break;
    }
}

VkVertexInputBindingDescription Vertex::bindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding   = 0;
    bindingDescription.stride    = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> Vertex::attributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

    /*
    float: VK_FORMAT_R32_SFLOAT
    vec2: VK_FORMAT_R32G32_SFLOAT
    vec3: VK_FORMAT_R32G32B32_SFLOAT
    vec4: VK_FORMAT_R32G32B32A32_SFLOAT
    */

    // pos attributes
    attributeDescriptions[0].binding  = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT; // use colors even for vecs
    attributeDescriptions[0].offset   = offsetof(Vertex, pos);

    // color attributes
    attributeDescriptions[1].binding  = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[1].offset   = offsetof(Vertex, color);

    // texCoord
    attributeDescriptions[2].binding  = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset   = offsetof(Vertex, texCoord);

    return attributeDescriptions;
}

} // namespace prim
} // namespace render
} // namespace bl
