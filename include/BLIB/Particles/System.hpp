#ifndef BLIB_PARTICLES_SYSTEM_HPP
#define BLIB_PARTICLES_SYSTEM_HPP

#include <BLIB/Containers/DynamicObjectPool.hpp>
#include <cmath>
#include <functional>

namespace bl
{
/// A simple bare bones particle system
namespace particle
{
template<typename T>
class System {
public:
    using CreateFunction = std::function<void(T*)>;
    using UpdateFunction = std::function<bool(T&)>;
    using RenderFunction = std::function<void(const T&)>;

    System(CreateFunction createFunction, unsigned int target, float createPerSecond);

    void setTargetCount(unsigned int target);

    void setCreateRate(float createPerSecond);

    unsigned int particleCount() const;

    void update(const UpdateFunction& updateCb, float dt);

    void render(const RenderFunction& renderCb) const;

private:
    struct Instance {
        char buf[sizeof(T)];
        T* cast() { return static_cast<T*>(static_cast<void*>(buf)); }
        const T* cast() const { return static_cast<const T*>(static_cast<const void*>(buf)); }
        Instance() = default;
        Instance(Instance&& c) { new (buf) T(std::move(*c.cast())); }
        ~Instance() { cast()->~T(); }
    };
    mutable container::DynamicObjectPool<Instance> particles;
    unsigned int target;
    float rate;
    float toCreate;
    CreateFunction createFunction;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
System<T>::System(CreateFunction cf, unsigned int target, float createRate)
: target(target)
, rate(createRate)
, toCreate(0.f)
, createFunction(cf) {
    particles.reserve(target);
}

template<typename T>
void System<T>::setTargetCount(unsigned int t) {
    target = t;
}

template<typename T>
void System<T>::setCreateRate(float r) {
    rate = r;
}

template<typename T>
unsigned int System<T>::particleCount() const {
    return particles.size();
}

template<typename T>
void System<T>::update(const UpdateFunction& cb, float dt) {
    for (auto it = particles.begin(); it != particles.end(); ++it) {
        if (!cb(*it->cast())) { particles.erase(it); }
    }

    toCreate                  = std::max(toCreate + rate * dt, static_cast<float>(target));
    const unsigned int create = std::floor(toCreate);
    toCreate -= create;

    for (unsigned int i = 0; i < create; ++i) {
        auto it = particles.emplace();
        createFunction(it->cast());
    }
}

template<typename T>
void System<T>::render(const RenderFunction& cb) const {
    for (const auto& inst : particles) { cb(*inst.cast()); }
}

} // namespace particle
} // namespace bl

#endif
