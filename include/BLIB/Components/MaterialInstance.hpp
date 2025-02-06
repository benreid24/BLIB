#ifndef BLIB_COMPONENTS_MATERIALINSTANCE_HPP
#define BLIB_COMPONENTS_MATERIALINSTANCE_HPP

#include <BLIB/Render/Components/DescriptorComponentBase.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Materials/MaterialDescriptor.hpp>
#include <BLIB/Render/Materials/MaterialId.hpp>
#include <BLIB/Render/Materials/MaterialPipeline.hpp>
#include <BLIB/Render/Resources/MaterialPool.hpp>

namespace bl
{
namespace rc
{
class Renderer;
}

namespace com
{
/**
 * @brief Component that binds a material and pipeline to an entity
 *
 * @ingroup Components
 */
class MaterialInstance
: public rc::rcom::DescriptorComponentBase<MaterialInstance, rc::mat::MaterialId, std::uint32_t> {
public:
    /**
     * @brief Creates the material instance using the default pipeline from the drawable component
     *
     * @param renderer The renderer instance
     * @param drawComponent The drawable component for the entity
     */
    MaterialInstance(rc::Renderer& renderer, rc::rcom::DrawableBase& drawComponent);

    /**
     * @brief Creates the material instance from a pipeline
     *
     * @param renderer The renderer instance
     * @param drawComponent The drawable component for the entity
     * @param pipeline The pipeline to render with
     */
    MaterialInstance(rc::Renderer& renderer, rc::rcom::DrawableBase& drawComponent,
                     rc::mat::MaterialPipeline* pipeline);

    /**
     * @brief Creates the material instance from a pipeline id
     *
     * @param renderer The renderer instance
     * @param drawComponent The drawable component for the entity
     * @param materialPipelineId The pipeline id to render with
     */
    MaterialInstance(rc::Renderer& renderer, rc::rcom::DrawableBase& drawComponent,
                     std::uint32_t materialPipelineId);

    /**
     * @brief Creates the material instance from a pipeline and a material
     *
     * @param renderer The renderer instance
     * @param drawComponent The drawable component for the entity
     * @param pipeline The pipeline to render with
     * @param material The material to use
     */
    MaterialInstance(rc::Renderer& renderer, rc::rcom::DrawableBase& drawComponent,
                     rc::mat::MaterialPipeline* pipeline, const rc::res::MaterialRef& material);

    /**
     * @brief Creates the material instance from a pipeline and a material
     *
     * @param renderer The renderer instance
     * @param drawComponent The drawable component for the entity
     * @param materialPipelineId The material pipeline id to render with
     * @param material The material to use
     */
    MaterialInstance(rc::Renderer& renderer, rc::rcom::DrawableBase& drawComponent,
                     std::uint32_t materialPipelineId, const rc::res::MaterialRef& material);

    /**
     * @brief Returns the material used by the entity
     */
    const rc::res::MaterialRef& getMaterial() const;

    /**
     * @brief Returns the pipeline used by the entity
     */
    rc::mat::MaterialPipeline& getPipeline() const;

    /**
     * @brief Update the material used by the entity
     *
     * @param material The new material to use
     */
    void setMaterial(const rc::res::MaterialRef& material);

    /**
     * @brief Update the pipeline used by the entity
     *
     * @param pipeline The pipeline to use
     */
    void setPipeline(rc::mat::MaterialPipeline* pipeline);

    /**
     * @brief Update the pipeline used by the entity
     *
     * @param pipeline The id of the pipeline to use
     */
    void setPipeline(std::uint32_t pipelineId);

    /**
     * @brief Update the pipeline used by the entity
     *
     * @param pipeline The settings of the pipeline to use
     */
    void setPipeline(rc::mat::MaterialPipelineSettings&& settings);

    /**
     * @brief Copies the material settings from another instance
     *
     * @param copy The material instance to copy
     * @return A reference to this object
     */
    MaterialInstance& operator=(const MaterialInstance& copy);

private:
    rc::Renderer& renderer;
    rc::rcom::DrawableBase& drawable;
    rc::mat::MaterialPipeline* pipeline;
    rc::res::MaterialRef material;

    void onPipelineChange();
    void onMaterialChange();

    // from descriptor base
    virtual void refreshDescriptor(rc::mat::MaterialId& descriptor) override;
    virtual void refreshDescriptor(std::uint32_t& textureId) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline const rc::res::MaterialRef& MaterialInstance::getMaterial() const { return material; }

inline rc::mat::MaterialPipeline& MaterialInstance::getPipeline() const { return *pipeline; }

} // namespace com
} // namespace bl

#endif
