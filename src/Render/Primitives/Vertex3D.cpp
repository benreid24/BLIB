#include <BLIB/Render/Primitives/Vertex3D.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace rc
{
namespace prim
{
namespace
{
void computeSingleTBN(Vertex3D& v1, Vertex3D& v2, Vertex3D& v3) {
    const glm::vec3 edge1    = v2.pos - v1.pos;
    const glm::vec3 edge2    = v3.pos - v1.pos;
    const glm::vec2 deltaUV1 = v2.texCoord - v1.texCoord;
    const glm::vec2 deltaUV2 = v3.texCoord - v2.texCoord;

    const float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    glm::vec3 tangent;
    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    glm::vec3 bitangent;
    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    const glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
    v1.normal              = normal;
    v2.normal              = normal;
    v3.normal              = normal;

    v1.tangent = tangent;
    v2.tangent = tangent;
    v3.tangent = tangent;

    v1.bitangent = bitangent;
    v2.bitangent = bitangent;
    v3.bitangent = bitangent;
}
} // namespace

Vertex3D::Vertex3D()
: pos()
, color(1.f)
, texCoord()
, tangent()
, bitangent()
, normal() {}

Vertex3D::Vertex3D(const glm::vec3& pos, const glm::vec2& texCoord)
: pos(pos)
, texCoord(texCoord)
, color(1.f)
, tangent()
, bitangent()
, normal() {}

Vertex3D::Vertex3D(const glm::vec3& pos, const glm::vec4& color)
: pos(pos)
, texCoord()
, color(color)
, tangent()
, bitangent()
, normal() {}

void Vertex3D::computeTBN(Vertex3D* vertices, std::size_t n) {
    if (n % 3 != 0) { BL_LOG_ERROR << "Number of vertices is not divisible by 3"; }

    for (std::size_t i = 0; i < n; i += 3) {
        computeSingleTBN(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
}

void Vertex3D::computeTBN(Vertex3D* vertices, std::uint32_t* indices, std::size_t n) {
    if (n % 3 != 0) { BL_LOG_ERROR << "Number of indices is not divisible by 3"; }

    for (std::size_t i = 0; i < n; i += 3) {
        computeSingleTBN(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
    }
}

VkVertexInputBindingDescription Vertex3D::bindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding   = 0;
    bindingDescription.stride    = sizeof(Vertex3D);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 6> Vertex3D::attributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 6> attributeDescriptions{};

    /*
    float: VK_FORMAT_R32_SFLOAT
    vec2: VK_FORMAT_R32G32_SFLOAT
    vec3: VK_FORMAT_R32G32B32_SFLOAT
    vec4: VK_FORMAT_R32G32B32A32_SFLOAT
    */

    // pos attributes
    attributeDescriptions[0].binding  = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset   = offsetof(Vertex3D, pos);

    // color attributes
    attributeDescriptions[1].binding  = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[1].offset   = offsetof(Vertex3D, color);

    // texCoord
    attributeDescriptions[2].binding  = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset   = offsetof(Vertex3D, texCoord);

    // tangent
    attributeDescriptions[3].binding  = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[3].offset   = offsetof(Vertex3D, tangent);

    // bitangent
    attributeDescriptions[4].binding  = 0;
    attributeDescriptions[4].location = 4;
    attributeDescriptions[4].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[4].offset   = offsetof(Vertex3D, bitangent);

    // normal
    attributeDescriptions[5].binding  = 0;
    attributeDescriptions[5].location = 5;
    attributeDescriptions[5].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[5].offset   = offsetof(Vertex3D, normal);

    return attributeDescriptions;
}

} // namespace prim
} // namespace rc
} // namespace bl
