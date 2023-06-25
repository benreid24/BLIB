#ifndef BLIB_UTIL_VISITOR_HPP
#define BLIB_UTIL_VISITOR_HPP

namespace bl
{
namespace util
{
/**
 * @brief Utility class to wrap a collection of lambdas for std::visit
 *
 * @ingroup Util
 */
template<class... Ts>
struct Visitor : Ts... {
    using Ts::operator()...;
};

// explicit deduction guide
template<class... Ts>
Visitor(Ts...) -> Visitor<Ts...>;

} // namespace util
} // namespace bl

#endif
