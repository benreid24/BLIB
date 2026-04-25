#ifndef BLIB_UTIL_TEMPLATESTRING_HPP
#define BLIB_UTIL_TEMPLATESTRING_HPP

#include <string_view>

namespace bl
{
namespace util
{
struct TemplateString {
    char data[256];
    unsigned int size;

    constexpr TemplateString()
    : data{}
    , size(0) {}

    template<std::size_t N>
    constexpr TemplateString(const char (&str)[N])
    : data{}
    , size(N - 1) {
        static_assert(N <= sizeof(data), "String literal too long for TemplateString");
        for (unsigned int i = 0; i < N - 1; ++i) { data[i] = str[i]; }
    }

    constexpr std::string_view toStringView() const { return std::string_view(data, size); }
};

} // namespace util
} // namespace bl

#endif
