#ifndef BLIB_RENDER_GRAPH_PURPOSE_HPP
#define BLIB_RENDER_GRAPH_PURPOSE_HPP

#include <string_view>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Collection of built-in purposes for render graph assets
 *
 * @ingroup Renderer
 */
struct Purpose {
    static constexpr std::string_view SSAOBuffer = "SSAOBuffer";
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
