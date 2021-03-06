#ifndef BLIB_UTIL_NONCOPYABLE_HPP
#define BLIB_UTIL_NONCOPYABLE_HPP

namespace bl
{
namespace util
{
/**
 * @brief Helper class to make derived classes impossible for user to copy directly
 *
 * @ingroup Util
 */
class NonCopyable {
protected:
    NonCopyable()  = default;
    ~NonCopyable() = default;

private:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};

} // namespace util
} // namespace bl

#endif
