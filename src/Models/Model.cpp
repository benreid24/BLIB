#include <BLIB/Models/Model.hpp>

namespace bl
{
namespace mdl
{
namespace
{
std::pair<glm::vec3, glm::vec3> computeNodeBox(const Node& node, const Model& model,
                                               const glm::mat4& globalTransform,
                                               const std::pair<glm::vec3, glm::vec3>& parentBox) {
    std::pair<glm::vec3, glm::vec3> box = parentBox;
    const glm::mat4 nodeTransform       = globalTransform * node.getTransform();

    for (unsigned int meshIndex : node.getMeshes()) {
        const Mesh& mesh = model.getMeshes().getMesh(meshIndex);
        for (const Vertex& vertex : mesh.getVertices()) {
            const glm::vec4 transformedVertex = nodeTransform * glm::vec4(vertex.pos, 1.0f);
            box.first                         = glm::min(box.first, glm::vec3(transformedVertex));
            box.second                        = glm::max(box.second, glm::vec3(transformedVertex));
        }
    }

    for (std::uint32_t childIndex : node.getChildren()) {
        const Node& childNode = model.getNodes().getNode(childIndex);
        box                   = computeNodeBox(childNode, model, nodeTransform, box);
    }

    return box;
}
} // namespace

void Model::populate(const aiScene* scene, const std::string& path) {
    bones.populate(scene);
    nodes.reserveSpace(scene);
    nodes.addNode(0).populate(nodes, scene, scene->mRootNode, bones);
    materials.populate(scene, path);
    meshes.populate(scene, bones);
    animations.populate(scene, nodes, bones);
    flipUVs();
}

void Model::visitDependencies(const DependencyVisitor& visitor) {
    materials.visitDependencies(visitor);
}

void Model::flipUVs() { meshes.flipUVs(); }

std::pair<glm::vec3, glm::vec3> Model::getBoundingBox() const {
    std::pair<glm::vec3, glm::vec3> box = {glm::vec3(std::numeric_limits<float>::max()),
                                           glm::vec3(std::numeric_limits<float>::lowest())};
    return computeNodeBox(getRoot(), *this, glm::identity<glm::mat4>(), box);
}

} // namespace mdl
} // namespace bl
