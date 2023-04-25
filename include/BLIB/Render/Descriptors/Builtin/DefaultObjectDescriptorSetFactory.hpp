#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_DEFAULTOBJECTDESCRIPTORSETFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_DEFAULTOBJECTDESCRIPTORSETFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

namespace bl
{
namespace render
{
namespace ds
{
/**
 * @brief Descriptor set factory used by objects rendered using default built-in pipelines
 *
 * @ingroup Renderer
 */
class DefaultObjectDescriptorSetFactory : public DescriptorSetFactory {
public:
    /**
     * @brief Destroys the factory
     */
    virtual ~DefaultObjectDescriptorSetFactory();

    /**
     * @brief Initializes the factory and creates the descriptor set layout
     *
     * @param engine Game engine instance
     * @param renderer Renderer instance
     */
    virtual void init(engine::Engine& engine, Renderer& renderer) override;

    /**
     * @brief Creates an instance of the descriptor set for this factory
     *
     * @return A DefaultObjectDescriptorSetInstance instance
     */
    virtual std::unique_ptr<DescriptorSetInstance> createDescriptorSet() const override;

private:
    engine::Engine* engine;
};

} // namespace ds
} // namespace render
} // namespace bl

#endif
