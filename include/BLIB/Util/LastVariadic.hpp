#ifndef BLIB_UTIL_LASTVARIADIC_HPP
#define BLIB_UTIL_LASTVARIADIC_HPP

namespace bl
{
template<typename... Ts>
class LastVariadic {
    template<typename T>
    struct tag {
        using type = T;
    };

public:
    using Last = typename decltype((tag<Ts>{}, ...))::type;
};

} // namespace bl

#endif
