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
        const std::string name =
            ctx.getAsset().getMetadata().getDisplayName() + "_material_" + std::to_string(i);

        if (!ctx.getCustomData().getIsSourceReload() || i >= payload.materials.getSize()) {
            auto materialAsset =
                ctx.getRepository().createAsset<MaterialPayload>(name, matCreateData);
            if (!materialAsset) {
                BL_LOG_ERROR << "Failed to create material asset for model " << ctx.getAsset();
                return false;
            }

            payload.materials.addDependency(materialAsset->getAsset().getUUID());
        }
        else {
            auto& mat = payload.materials.getAsset(i);
            if (!mat.forceRecreatePayload(matCreateData)) {
                BL_LOG_ERROR << "Failed to force recreate material asset for model "
                             << ctx.getAsset();
                return false;
            }
        }
    }

    // remove extra materials not needed after recreate
    if (ctx.getCustomData().getIsSourceReload() &&
        payload.materials.getSize() > data.getMaterials().numMaterials()) {
        while (payload.materials.getSize() > data.getMaterials().numMaterials()) {
            payload.materials.removeDependency(data.getMaterials().numMaterials());
        }
    }

    unsigned int i = 0;
    for (const mdl::Animation& anim : data.getAnimations().getAnimations()) {
        Animation3DDriver::CreateParams animCreateData{anim};
        const std::string name =
            ctx.getAsset().getMetadata().getDisplayName() + "_animation_" + std::to_string(i);

        if (!ctx.getCustomData().getIsSourceReload() || i >= payload.animations.getSize()) {
            auto animationAsset =
                ctx.getRepository().createAsset<Animation3DPayload>(name, animCreateData);
            if (!animationAsset) {
                BL_LOG_ERROR << "Failed to create animation asset for model " << ctx.getAsset();
                return false;
            }
            payload.animations.addDependency(animationAsset->getAsset().getUUID());
        }
        else {
            // TODO - how did we get two animations?
            auto& anim = payload.animations.getAsset(i);
            if (!anim.forceRecreatePayload(animCreateData)) {
                BL_LOG_ERROR << "Failed to force recreate animation asset for model "
                             << ctx.getAsset();
                return false;
            }
        }

        ++i;
    }

    // remove extra animations not needed after recreate
    if (ctx.getCustomData().getIsSourceReload() &&
        payload.animations.getSize() > data.getAnimations().getAnimations().size()) {
        while (payload.animations.getSize() > data.getAnimations().getAnimations().size()) {
            payload.animations.removeDependency(data.getAnimations().getAnimations().size());
        }
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
