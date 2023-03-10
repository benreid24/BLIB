#ifndef BLIB_RENDER_DESCRIPTORS_BUILTIN_DEFAULTOBJECTDESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_BUILTIN_DEFAULTOBJECTDESCRIPTORSETINSTANCE_HPP

#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

namespace bl
{
namespace render
{
namespace ds
{
class DefaultObjectDescriptorSetInstance : public DescriptorSetInstance {
public:
    virtual ~DefaultObjectDescriptorSetInstance() = default;

private:
    // TODO - rethink buffer interfaces
    std::vector<glm::mat4> transforms;
    std::vector<std::uint32_t> textures;
};

} // namespace ds
} // namespace render
} // namespace bl

#endif
