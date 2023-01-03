#ifndef BLIB_RENDER_RENDERER_SCENE_HPP
#define BLIB_RENDER_RENDERER_SCENE_HPP

#include <BLIB/Render/Ref.hpp>
#include <BLIB/Render/Renderer/Object.hpp>
#include <BLIB/Render/Resources/Material.hpp>
#include <glad/vulkan.h>

namespace bl
{
namespace render
{
class Scene {
public:
    //

private:
    std::vector<Object> objects;
};

} // namespace render
} // namespace bl

#endif
