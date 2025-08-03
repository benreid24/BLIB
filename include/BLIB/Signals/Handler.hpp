#ifndef BLIB_SIGNALS_HANDLER_HPP
#define BLIB_SIGNALS_HANDLER_HPP

namespace bl
{
namespace sig
{
template<typename T>
class Handler {
public:
    virtual ~Handler() = default;

    virtual void process(const T& signal) = 0;
};
} // namespace sig
} // namespace bl

#endif
