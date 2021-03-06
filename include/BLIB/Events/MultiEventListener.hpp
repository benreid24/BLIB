#ifndef BLIB_EVENTS_MULTIEVENTLISTENER_HPP
#define BLIB_EVENTS_MULTIEVENTLISTENER_HPP

namespace bl
{
template<typename T>
class MultiEventListenerBase {
public:
    virtual ~MultiEventListenerBase() = default;

    virtual void observe(const T& event) = 0;
};

template<typename... TEvents>
class MultiEventListener : public MultiEventListenerBase<TEvents>... {
public:
    virtual ~MultiEventListener() = default;
};

} // namespace bl

#endif
