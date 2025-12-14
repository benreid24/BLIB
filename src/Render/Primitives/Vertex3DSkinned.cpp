#include <BLIB/Render/Primitives/Vertex3DSkinned.hpp>

namespace bl
{
namespace rc
{
namespace prim
{
Vertex3DSkinned::Vertex3DSkinned()
: pos()
, color(1.f)
, texCoord()
, tangent()
, normal() {}

Vertex3DSkinned::Vertex3DSkinned(const glm::vec3& pos, const glm::vec2& texCoord)
: pos(pos)
, texCoord(texCoord)
, color(1.f)
, tangent()
, normal() {}

Vertex3DSkinned::Vertex3DSkinned(const glm::vec3& pos, const glm::vec4& color)
: pos(pos)
, texCoord()
, color(color)
, tangent()
, normal() {}

Vertex3DSkinned& Vertex3DSkinned::operator=(const mdl::Vertex& copy) {
    pos      = copy.pos;
    color    = copy.color;
    texCoord = copy.texCoord;
    tangent  = copy.tangent;
    normal   = copy.normal;
    for (unsigned int i = 0; i < 4; ++i) {
        if (copy.boneIds[i] < 0) {
            boneIndices[i] = 0;
            boneWeights[i] = 0.f;
        }
        else {
            boneIndices[i] = copy.boneIds[i];
            boneWeights[i] = copy.boneWeights[i];
        }
    }
    return *this;
}

VkVertexInputBindingDescription Vertex3DSkinned::bindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding   = 0;
    bindingDescription.stride    = sizeof(Vertex3DSkinned);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 7> Vertex3DSkinned::attributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 7> attributeDescriptions{};

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
    attributeDescriptions[0].offset   = offsetof(Vertex3DSkinned, pos);

    // color attributes
    attributeDescriptions[1].binding  = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[1].offset   = offsetof(Vertex3DSkinned, color);

    // texCoord
    attributeDescriptions[2].binding  = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset   = offsetof(Vertex3DSkinned, texCoord);

    // tangent
    attributeDescriptions[3].binding  = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[3].offset   = offsetof(Vertex3DSkinned, tangent);

    // normal
    attributeDescriptions[4].binding  = 0;
    attributeDescriptions[4].location = 4;
    attributeDescriptions[4].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[4].offset   = offsetof(Vertex3DSkinned, normal);

    // bone indices
    attributeDescriptions[5].binding  = 0;
    attributeDescriptions[5].location = 5;
    attributeDescriptions[5].format   = VK_FORMAT_R32G32B32A32_UINT;
    attributeDescriptions[5].offset   = offsetof(Vertex3DSkinned, boneIndices);

    // bone weights
    attributeDescriptions[6].binding  = 0;
    attributeDescriptions[6].location = 6;
    attributeDescriptions[6].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[6].offset   = offsetof(Vertex3DSkinned, boneWeights);

    return attributeDescriptions;
}

std::array<VkVertexInputAttributeDescription, 1>
Vertex3DSkinned::attributeDescriptionsPositionsOnly() {
    std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};

    // pos attributes
    attributeDescriptions[0].binding  = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset   = offsetof(Vertex3DSkinned, pos);

    return attributeDescriptions;
}

} // namespace prim
} // namespace rc
} // namespace bl
