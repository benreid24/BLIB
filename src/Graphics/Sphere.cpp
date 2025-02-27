#include <BLIB/Graphics/Sphere.hpp>

#include <array>

namespace bl
{
namespace gfx
{
namespace
{

// https://gamedev.stackexchange.com/questions/31308/algorithm-for-creating-spheres
constexpr std::array<std::uint32_t, 60> SourceIndices = {
    0,  4, 1, 0, 9, 4, 9, 5,  4, 4, 5,  8,  4,  8, 1, 8,  10, 1,  8, 3,
    10, 5, 3, 8, 5, 2, 3, 2,  7, 3, 7,  10, 3,  7, 6, 10, 7,  11, 6, 11,
    0,  6, 0, 1, 6, 6, 1, 10, 9, 0, 11, 9,  11, 2, 9, 2,  5,  7,  2, 11};

constexpr float X = 0.525731112119133606f;
constexpr float Z = 0.850650808352039932f;

constexpr std::array<glm::vec3, 12> SourceVertices = {glm::vec3(-X, 0.f, Z),
                                                      glm::vec3(X, 0.f, Z),
                                                      glm::vec3(-X, 0.f, -Z),
                                                      glm::vec3(X, 0.f, -Z),
                                                      glm::vec3(0.f, Z, X),
                                                      glm::vec3(0.f, Z, -X),
                                                      glm::vec3(0.f, -Z, X),
                                                      glm::vec3(0.f, -Z, -X),
                                                      glm::vec3(Z, X, 0.f),
                                                      glm::vec3(-Z, X, 0.f),
                                                      glm::vec3(Z, -X, 0.f),
                                                      glm::vec3(-Z, -X, 0.f)};

constexpr std::uint32_t NoMidpoint = std::numeric_limits<std::uint32_t>::max();

std::size_t midpointKey(std::uint32_t i1, std::uint32_t i2, int lod) {
    if (i1 > i2) { std::swap(i1, i2); }
    return SourceIndices.size() * lod * i1 + i2;
}

std::uint32_t findPoint(const std::vector<rc::prim::Vertex3D>& vertices,
                        const rc::prim::Vertex3D& point) {
    for (const auto& v : vertices) {
        if (v.pos == point.pos) { return &v - vertices.data(); }
    }
    return NoMidpoint;
}

std::uint32_t getMidpoint(std::unordered_map<std::uint32_t, std::uint32_t>& midpointMap,
                          std::vector<rc::prim::Vertex3D>& vertices, std::uint32_t i1,
                          std::uint32_t i2, int lod) {
    const std::uint32_t mpKey = midpointKey(i1, i2, lod);
    auto it                   = midpointMap.find(mpKey);
    if (it != midpointMap.end()) { return it->second; }

    const auto& v1 = vertices[i1];
    const auto& v2 = vertices[i2];
    const rc::prim::Vertex3D mpv((v1.pos + v2.pos) * 0.5f);
    const std::uint32_t i = findPoint(vertices, mpv);
    if (i != NoMidpoint) { return i; }
    else {
        const std::uint32_t mp = vertices.size();
        vertices.emplace_back(mpv);
        midpointMap[mpKey] = mp;
        return mp;
    }
}

} // namespace

Sphere::Sphere() {}

Sphere::Sphere(engine::World& world, float size, int detailLevel,
               const bl::rc::res::MaterialRef& material, std::uint32_t materialPipelineId) {
    create(world, size, detailLevel, material, materialPipelineId);
}

void Sphere::create(engine::World& world, float size, int detailLevel,
                    const bl::rc::res::MaterialRef& material, std::uint32_t materialPipelineId) {
    Drawable::createWithMaterial(world, materialPipelineId, material);
    Transform3D::create(world.engine().ecs(), entity());

    std::vector<rc::prim::Vertex3D> vertices;
    std::vector<std::uint32_t> indices;
    std::vector<std::uint32_t> splitIndices;
    std::unordered_map<std::uint32_t, std::uint32_t> midpointMap;
    vertices.reserve(SourceVertices.size() * detailLevel * 3);
    indices.reserve(SourceIndices.size() * (detailLevel * 4));
    splitIndices.reserve(SourceIndices.size() * (detailLevel * 4));

    std::transform(SourceVertices.begin(),
                   SourceVertices.end(),
                   std::back_inserter(vertices),
                   [](const glm::vec3& v) { return rc::prim::Vertex3D(v); });
    std::copy(SourceIndices.begin(), SourceIndices.end(), std::back_inserter(indices));

    // make sphere
    for (int lod = 1; lod <= detailLevel; ++lod) {
        for (std::uint32_t i = 0; i < indices.size(); i += 3) {
            const std::uint32_t i0  = indices[i + 0];
            const std::uint32_t i1  = indices[i + 1];
            const std::uint32_t i2  = indices[i + 2];
            const std::uint32_t m01 = getMidpoint(midpointMap, vertices, i0, i1, lod);
            const std::uint32_t m12 = getMidpoint(midpointMap, vertices, i1, i2, lod);
            const std::uint32_t m02 = getMidpoint(midpointMap, vertices, i0, i2, lod);
            const std::array<std::uint32_t, 12> newIndices = {
                i0, m01, m02, i1, m12, m01, i2, m02, m12, m02, m01, m12};
            std::copy(newIndices.begin(), newIndices.end(), std::back_inserter(splitIndices));
        }
        indices.swap(splitIndices);
        splitIndices.clear();
        midpointMap.clear();
    }

    // normalize vertices to make sphere
    for (auto& v : vertices) { v.pos = size * glm::normalize(v.pos); }

    // preprocessing
    rc::prim::Vertex3D::computeTBN(component().gpuBuffer.vertices().data(),
                                   component().gpuBuffer.indices().data(),
                                   component().gpuBuffer.indices().size());

    // create mesh
    component().create(
        world.engine().renderer().vulkanState(), std::move(vertices), std::move(indices));
}

void Sphere::setColor(const rc::Color& c) {
    for (auto& v : component().gpuBuffer.vertices()) { v.color = c.toVec4(); }
    component().gpuBuffer.queueTransfer();
}

void Sphere::scaleToSize(const glm::vec2&) {
    // noop
}

} // namespace gfx
} // namespace bl
