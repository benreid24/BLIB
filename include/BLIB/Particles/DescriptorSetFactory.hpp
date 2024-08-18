#ifndef BLIB_PARTICLES_DESCRIPTORSETFACTORY_HPP
#define BLIB_PARTICLES_DESCRIPTORSETFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

namespace bl
{
namespace pcl
{
/**
 * @brief Descriptor set factory for the default descriptor set used by the default renderer.
 *        Provides an array of particle structs as well as a singular global object of config data
 *        to the shaders. Types must be specified in a specialization of RenderConfigMap
 *
 * @tparam T The particle type
 * @tparam GpuT The particle type to use in the GPU-visible buffers. Must be assignable from T
 * @ingroup Particles
 */
template<typename T, typename GpuT>
class DescriptorSetFactory : public rc::ds::DescriptorSetFactory {
public:
    /**
     * @brief Destroys the factory
     */
    virtual ~DescriptorSetFactory() = default;

private:
    bl::engine::Engine* engine;

    virtual void init(bl::engine::Engine& e, bl::rc::Renderer& renderer) override;
    virtual std::unique_ptr<rc::ds::DescriptorSetInstance> createDescriptorSet() const override;
    virtual std::type_index creates() const override;
};

} // namespace pcl
} // namespace bl

#endif
