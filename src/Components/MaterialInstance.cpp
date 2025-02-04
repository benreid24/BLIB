#include <BLIB/Components/MaterialInstance.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace com
{
MaterialInstance::MaterialInstance(rc::Renderer& renderer, rc::rcom::DrawableBase& drawComponent)
: renderer(renderer)
, drawable(drawComponent)
, pipeline(
      &renderer.materialPipelineCache().getPipeline(drawComponent.getDefaultMaterialPipelineId())) {
}

MaterialInstance::MaterialInstance(rc::Renderer& renderer, rc::rcom::DrawableBase& drawComponent,
                                   std::uint32_t materialPipelineId)
: renderer(renderer)
, drawable(drawComponent)
, pipeline(&renderer.materialPipelineCache().getPipeline(materialPipelineId)) {}

MaterialInstance::MaterialInstance(rc::Renderer& renderer, rc::rcom::DrawableBase& drawComponent,
                                   rc::mat::MaterialPipeline* pipeline)
: renderer(renderer)
, drawable(drawComponent)
, pipeline(pipeline) {}

MaterialInstance::MaterialInstance(rc::Renderer& renderer, rc::rcom::DrawableBase& drawComponent,
                                   rc::mat::MaterialPipeline* pipeline,
                                   const rc::res::MaterialRef& material)
: renderer(renderer)
, drawable(drawComponent)
, pipeline(pipeline)
, material(material) {}

MaterialInstance::MaterialInstance(rc::Renderer& renderer, rc::rcom::DrawableBase& drawComponent,
                                   std::uint32_t materialPipelineId,
                                   const rc::res::MaterialRef& material)
: renderer(renderer)
, drawable(drawComponent)
, pipeline(&renderer.materialPipelineCache().getPipeline(materialPipelineId))
, material(material) {}

void MaterialInstance::setMaterial(const rc::res::MaterialRef& m) {
    material = m;
    onMaterialChange();
}

void MaterialInstance::setPipeline(rc::mat::MaterialPipeline* p) {
    pipeline = p;
    onPipelineChange();
}

void MaterialInstance::setPipeline(std::uint32_t pipelineId) {
    pipeline = &renderer.materialPipelineCache().getPipeline(pipelineId);
    onPipelineChange();
}

void MaterialInstance::setPipeline(rc::mat::MaterialPipelineSettings&& settings) {
    pipeline = &renderer.materialPipelineCache().getOrCreatePipeline(std::move(settings));
    onPipelineChange();
}

MaterialInstance& MaterialInstance::operator=(const MaterialInstance& copy) {
    pipeline = copy.pipeline;
    material = copy.material;
    onPipelineChange();
    onMaterialChange();
    return *this;
}

void MaterialInstance::onPipelineChange() { drawable.rebucket(); }

void MaterialInstance::onMaterialChange() { markDirty(); }

void MaterialInstance::refreshDescriptor(rc::mat::MaterialDescriptor& d) {
    if (material) {
        // id method is always safe on ref even if invalid
        d.diffuseTextureId = material->getTexture().id();
        d.normalTextureId  = material->getNormalMap().id();
        d.uvTextureId      = material->getUVMap().id();
    }
}

void MaterialInstance::refreshDescriptor(std::uint32_t& tid) {
    if (material) { tid = material->getTexture().id(); }
}

} // namespace com
} // namespace bl
