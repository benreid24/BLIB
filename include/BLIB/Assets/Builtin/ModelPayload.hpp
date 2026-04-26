#ifndef BLIB_ASSETS_BUILTIN_MODELPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_MODELPAYLOAD_HPP

#include <BLIB/Assets/Builtin/Animation3DPayload.hpp>
#include <BLIB/Assets/Builtin/MaterialPayload.hpp>
#include <BLIB/Assets/Builtin/TexturePayload.hpp>
#include <BLIB/Assets/Dependency.hpp>
#include <BLIB/Assets/DependencyList.hpp>
#include <BLIB/Assets/Payload.hpp>
#include <BLIB/Models/Model.hpp>
#include <BLIB/Serialization.hpp>

namespace bl
{
namespace asi
{
class ModelDriver;

/**
 * @brief Payload for a model asset
 *
 * @ingroup Assets
 */
class ModelPayload : public as::Payload {
public:
    /**
     * @brief Creates an empty model payload
     *
     * @param ctx The context to construct with
     */
    ModelPayload(const as::Payload::ConstructContext& ctx);

    /**
     * @brief Destroys the payload
     */
    virtual ~ModelPayload() = default;

    /**
     * @brief Returns the root node of the model. Only call after loading
     */
    const mdl::Node& getRoot() const { return nodes.getNode(0); }

    /**
     * @brief Returns the set of nodes in the model
     */
    const mdl::NodeSet& getNodes() const { return nodes; }

    /**
     * @brief Returns the bones of the model
     */
    const mdl::BoneSet& getBones() const { return bones; }

    /**
     * @brief Returns the set of meshes in the model
     */
    const mdl::MeshSet& getMeshes() const { return meshes; }

    /**
     * @brief Returns the material at the given index
     *
     * @param i The index of the material to fetch
     * @return The material at the given index
     */
    const MaterialPayload& getMaterial(std::size_t i) const { return materials.get(i); }

    /**
     * @brief Returns the animation at the given index
     * @param i The index of the animation to fetch
     * @return The animation at the given index
     */
    const Animation3DPayload& getAnimation(std::size_t i) const { return animations.get(i); }

private:
    mdl::NodeSet nodes;
    mdl::MeshSet meshes;
    mdl::BoneSet bones;
    as::DependencyList<MaterialPayload> materials;
    as::DependencyList<Animation3DPayload> animations;

    friend class ModelDriver;
    friend struct serial::SerializableObject<ModelPayload>;
};

} // namespace asi

namespace serial
{
template<>
struct SerializableObject<asi::ModelPayload> : public SerializableObjectBase {
    SerializableField<1, asi::ModelPayload, mdl::NodeSet> nodes;
    SerializableField<2, asi::ModelPayload, mdl::MeshSet> meshes;
    SerializableField<3, asi::ModelPayload, mdl::BoneSet> bones;

    SerializableObject()
    : SerializableObjectBase("ModelPayload")
    , nodes("nodes", *this, &asi::ModelPayload::nodes, SerializableFieldBase::Required{})
    , meshes("meshes", *this, &asi::ModelPayload::meshes, SerializableFieldBase::Required{})
    , bones("bones", *this, &asi::ModelPayload::bones, SerializableFieldBase::Required{}) {}
};

} // namespace serial
} // namespace bl

#endif
