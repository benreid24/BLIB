#ifndef BLIB_SERIALIZATION_TRAITS_HPP
#define BLIB_SERIALIZATION_TRAITS_HPP

namespace bl
{
namespace serial
{
/**
 * @brief Container for serilization reflection traits
 *
 * @ingroup Serialization
 */
struct Trait {
    /**
     * @brief Reflection trait that indicates a field is optional. If it is missing serialization
     *        may still succeed. Missing fields without this trait will cause serialization to fail
     *
     * @ingroup Serialization
     */
    struct Optional {};
};

} // namespace serial
} // namespace bl

#endif
