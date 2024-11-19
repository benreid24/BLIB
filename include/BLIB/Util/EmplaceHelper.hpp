#ifndef BLIB_UTIL_EMPLACEHELPER_HPP
#define BLIB_UTIL_EMPLACEHELPER_HPP

namespace bl
{
namespace util
{
/**
 * @brief Helper to emplace non-constructible objects (private constructor, factory methods, etc)
 *
 * @tparam F Function type of the factory method
 * @ingroup Util
 */
template<typename F>
class EmplaceHelper {
public:
    /**
     * @brief Creates the helper with the given object factory
     *
     * @param f Callback to create the desired object
     */
    EmplaceHelper(F&& f)
    : factory(f) {}

    /**
     * @brief Called by emplace methods to create objects
     */
    operator auto() { return factory(); }

private:
    F& factory;
};

} // namespace util
} // namespace bl

#endif
