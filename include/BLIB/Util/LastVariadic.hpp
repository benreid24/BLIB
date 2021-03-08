#ifndef BLIB_UTIL_LASTVARIADIC_HPP
#define BLIB_UTIL_LASTVARIADIC_HPP

namespace bl
{
namespace util
{
/**
 * @brief Provides a typedef for the last type in a variadic template parameter list
 *
 * @tparam Ts List of types to select the last from
 * @ingroup Util
 */
template<typename... Ts>
class LastVariadic {
    template<typename T>
    struct tag {
        using type = T;
    };

public:
    /// The last type in the provided list of types
    using Last = typename decltype((tag<Ts>{}, ...))::type;
};

} // namespace util
} // namespace bl

#endif
