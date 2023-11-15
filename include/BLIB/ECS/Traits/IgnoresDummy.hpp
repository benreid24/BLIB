#ifndef BLIB_ECS_TRAITS_IGNORESDUMMY_HPP
#define BLIB_ECS_TRAITS_IGNORESDUMMY_HPP

namespace bl
{
namespace ecs
{
namespace trait
{
/**
 * @brief Empty trait that marks a component as ignoring dummy entities. When dummies are ignored
 *        the parent/child traits will skip over dummy entities and connect the entities above/below
 *        them. See OverlayObject for an example on how this is used
 *
 * @ingroup ECS
 */
struct IgnoresDummy {};

} // namespace trait
} // namespace ecs
} // namespace bl

#endif
