#include <BLIB/Assets/Drivers/ModelDriver.hpp>

#include <BLIB/Assets/Drivers/Animation3DDriver.hpp>
#include <BLIB/Models/Importer.hpp>
#include <BLIB/Serialization.hpp>

namespace bl
{
namespace asi
{
namespace
{
std::pair<glm::vec3, glm::vec3> computeNodeBox(const mdl::Node& node, const mdl::NodeSet& nodes,
                                               const mdl::MeshSet& meshes,
                                               const glm::mat4& globalTransform,
                                               const std::pair<glm::vec3, glm::vec3>& parentBox) {
    std::pair<glm::vec3, glm::vec3> box = parentBox;
    const glm::mat4 nodeTransform       = globalTransform * node.getTransform();

    for (unsigned int meshIndex : node.getMeshes()) {
        const mdl::Mesh& mesh = meshes.getMesh(meshIndex);
        for (const mdl::Vertex& vertex : mesh.getVertices()) {
            const glm::vec4 transformedVertex = nodeTransform * glm::vec4(vertex.pos, 1.0f);
            box.first                         = glm::min(box.first, glm::vec3(transformedVertex));
            box.second                        = glm::max(box.second, glm::vec3(transformedVertex));
        }
    }

    for (std::uint32_t childIndex : node.getChildren()) {
        const mdl::Node& childNode = nodes.getNode(childIndex);
        box                        = computeNodeBox(childNode, nodes, meshes, nodeTransform, box);
    }

    return box;
}
} // namespace

ModelDriver::ModelDriver()
: Driver(
      as::bdl::AssetBundleConfig{.affinity  = as::bdl::AssetBundleConfig::Affinity::Parent,
                                 .selection = as::bdl::AssetBundleConfig::Selection::NonRoot,
                                 .onMount = as::bdl::AssetBundleConfig::OnMount::WhenRequested}) {}

bool ModelDriver::doCreate(as::CreateContext& ctx, ModelPayload& payload) {
    const CreateParams* params = ctx.getCustomDataAsMaybe<CreateParams>();
    if (params) {
        std::pair<glm::vec3, glm::vec3> box = {glm::vec3(std::numeric_limits<float>::max()),
                                               glm::vec3(std::numeric_limits<float>::lowest())};
        box                                 = computeNodeBox(
            params->nodes.getNode(0), params->nodes, params->meshes, glm::mat4(1.0f), box);

        payload.bounds = box;
        payload.nodes  = params->nodes;
        payload.meshes = params->meshes;
        payload.bones  = params->bones;
        for (const auto& mat : params->materials) {
            if (!mat.isValid()) {
                BL_LOG_ERROR << "Invalid material ref in ModelDriver::CreateParams";
                return false;
            }
            payload.materials.addDependency(mat.getAsset().getUUID());
        }
        for (const auto& anim : params->animations) {
            if (!anim.isValid()) {
                BL_LOG_ERROR << "Invalid animation ref in ModelDriver::CreateParams";
                return false;
            }
            payload.animations.addDependency(anim.getAsset().getUUID());
        }
        return true;
    }

    if (ctx.getCustomData().getPath().empty()) { return false; }

    mdl::Importer importer;
    mdl::Model data;
    if (!importer.import(ctx.getCustomData().getPath(), data)) {
        BL_LOG_ERROR << "Failed to import model from path " << ctx.getCustomData().getPath();
        return false;
    }

    payload.bounds = data.getBoundingBox();
    payload.nodes  = data.getNodes();
    payload.meshes = data.getMeshes();
    payload.bones  = data.getBones();

    for (unsigned int i = 0; i < data.getMaterials().numMaterials(); ++i) {
        MaterialPayload::CreateData matCreateData{data.getMaterials().getMaterial(i)};
        auto materialAsset = ctx.getRepository().createAsset<MaterialPayload>(
            ctx.getAsset().getMetadata().getDisplayName() + "_material_" + std::to_string(i),
            matCreateData);
        if (!materialAsset) {
            BL_LOG_ERROR << "Failed to create material asset for model "
                         << ctx.getAsset().getUUID();
            return false;
        }
        payload.materials.addDependency(materialAsset->getAsset().getUUID());
    }

    unsigned int i = 0;
    for (const mdl::Animation& anim : data.getAnimations().getAnimations()) {
        Animation3DDriver::CreateParams animCreateData{anim};
        auto animationAsset = ctx.getRepository().createAsset<Animation3DPayload>(
            ctx.getAsset().getMetadata().getDisplayName() + "_animation_" + std::to_string(i++),
            animCreateData);
        if (!animationAsset) {
            BL_LOG_ERROR << "Failed to create animation asset for model "
                         << ctx.getAsset().getUUID();
            return false;
        }
        payload.animations.addDependency(animationAsset->getAsset().getUUID());
    }

    return true;
}

bool ModelDriver::doRead(as::ReadContext& ctx, ModelPayload& payload) {
    stream::InputStream input;
    if (!ctx.setupReadStream("model.bmdl", input)) { return false; }
    return serial::binary::Serializer<ModelPayload>::deserialize(input, payload);
}

bool ModelDriver::doWrite(as::WriteContext& ctx, const ModelPayload& payload) {
    stream::OutputStream output;
    if (!ctx.setupWriteStream("model.bmdl", output)) { return false; }
    return serial::binary::Serializer<ModelPayload>::serialize(output, payload);
}

} // namespace asi
} // namespace bl
