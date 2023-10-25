#ifndef BLIB_GUI_RENDERER_RENDERER_HPP
#define BLIB_GUI_RENDERER_RENDERER_HPP

#include <BLIB/Interfaces/GUI/Renderer/FactoryTable.hpp>

namespace bl
{
namespace gui
{
namespace rdr
{
class Renderer {
public:
    Renderer(FactoryTable& factory);

private:
    FactoryTable& factory;
};

} // namespace rdr
} // namespace gui
} // namespace bl

#endif
