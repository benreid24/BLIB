#ifndef BLIB_ASSETS_BUILTIN_MODELPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_MODELPAYLOAD_HPP

#include <BLIB/Assets/Builtin/Animation3DPayload.hpp>
#include <BLIB/Assets/Builtin/MaterialPayload.hpp>
#include <BLIB/Assets/Builtin/TexturePayload.hpp>
#include <BLIB/Assets/Dependency.hpp>
#include <BLIB/Assets/DependencyList.hpp>
#include <BLIB/Assets/Payload.hpp>
#include <BLIB/Models/Model.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>

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
     * @brief Flips the V texture coordinates of all meshes in the model
     */
    void flipUVs();

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
     * @brief Returns the material at the given index
     *
     * @param i The index of the material to fetch
     * @return A ref to the asset of the material at the given index
     */
    as::TypedRef<MaterialPayload> getMaterialRef(std::size_t i) const {
        return materials.getRef(i);
    }

    /**
     * @brief Returns the number of materials in the model
     */
    unsigned int getMaterialCount() const { return materials.getSize(); }

    /**
     * @brief Returns the animation at the given index
     * @param i The index of the animation to fetch
     * @return The animation at the given index
     */
    const Animation3DPayload& getAnimation(std::size_t i) const { return animations.get(i); }

    /**
     * @brief Returns the animation at the given index
     *
     * @param i The index of the animation to fetch
     * @return A ref to the asset of the animation at the given index
     */
    as::TypedRef<Animation3DPayload> getAnimationRef(std::size_t i) const {
        return animations.getRef(i);
    }

    /**
     * @brief Returns the number of animations in the model
     */
    unsigned int getAnimationCount() const { return animations.getSize(); }

private:
    mdl::NodeSet nodes;
    mdl::MeshSet meshes;
    mdl::BoneSet bones;
    as::DependencyList<MaterialPayload> materials;
    as::DependencyList<Animation3DPayload> animations;

    friend class ModelDriver;
    friend struct refl::ReflectedObject<ModelPayload>;
};

} // namespace asi

namespace refl
{
template<>
struct ReflectedObject<asi::ModelPayload> {
    inline static const auto spec = makeSpec<asi::ModelPayload>(
        "ModelPayload", memberList(defineMember(1, "nodes", &asi::ModelPayload::nodes),
                                   defineMember(2, "meshes", &asi::ModelPayload::meshes),
                                   defineMember(3, "bones", &asi::ModelPayload::bones)));
};
} // namespace refl

} // namespace bl

#endif
