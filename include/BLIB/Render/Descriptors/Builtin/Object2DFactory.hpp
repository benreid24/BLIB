#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT2DFACTORY_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_OBJECT2DFACTORY_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Descriptor set factory used by 2d objects rendered using default built-in pipelines
 *
 * @ingroup Renderer
 */
class Object2DFactory : public DescriptorSetFactory {
public:
    /**
     * @brief Destroys the factory
     */
    virtual ~Object2DFactory() = default;

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
     * @return A Object2DInstance instance
     */
    virtual std::unique_ptr<DescriptorSetInstance> createDescriptorSet() const override;

private:
    engine::Engine* engine;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
