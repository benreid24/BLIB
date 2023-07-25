#ifndef BLIB_PARTICLES_SYSTEM_HPP
#define BLIB_PARTICLES_SYSTEM_HPP

#include <BLIB/Containers/FastEraseVector.hpp>
#include <cmath>
#include <functional>

namespace bl
{
/// A simple bare bones particle system
namespace particle
{
/**
 * @brief A very simple particle system. Particles not provided
 *
 * @tparam T The type of particle to manage
 * @ingroup Particles
 */
template<typename T>
class System { // TODO - flesh this out into something with more options and features. has potential
public:
    /**
     * @brief Function callback to use when creating particles
     *
     * @param T* A pointer to an uninitialized particle. Must use in place new
     *
     */
    using CreateFunction = std::function<void(T*)>;

    /**
     * @brief Function callback to update a particle
     *
     * @param T& The particle to update
     */
    using UpdateFunction = std::function<bool(T&)>;

    /**
     * @brief Function callback to render a particle
     *
     * @param const T& The particle to render
     *
     */
    using RenderFunction = std::function<void(const T&)>;

    /**
     * @brief Construct a new particle system
     *
     * @param createFunction The callback to use to create new particles
     * @param target The number of particles to build up to
     * @param createPerSecond How many particles to create per second
     */
    System(CreateFunction createFunction, unsigned int target, float createPerSecond);

    /**
     * @brief Set the target number of particles. Does not destroy existing particles if target is
     *        under current count
     *
     * @param target Number of particles to create and manage
     */
    void setTargetCount(unsigned int target);

    /**
     * @brief Sets the rate at which particles are created until target is reached
     *
     * @param createPerSecond The number of particles to create each second
     */
    void setCreateRate(float createPerSecond);

    /**
     * @brief Sets whether or not destroyed particles should be replaced immediately. Default is
     *        false. If not replaced when destroyed particles are created at the creation rate
     *
     * @param replace True to replace immediately, false to create over time
     */
    void setReplaceDestroyed(bool replace);

    /**
     * @brief Returns the number of particles currently alive
     *
     */
    unsigned int particleCount() const;

    /**
     * @brief Updates all the particles and spawns new ones as necessary
     *
     * @param updateCb Function to update each particle with
     * @param dt Time elapsed since last call to update, in seconds
     */
    void update(const UpdateFunction& updateCb, float dt);

    /**
     * @brief Renders all active particles
     *
     * @param renderCb Function to render each particle with
     */
    void render(const RenderFunction& renderCb) const;

private:
    ctr::FastEraseVector<T> particles;
    unsigned int target;
    float rate;
    float toCreate;
    bool replace;
    CreateFunction createFunction;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
System<T>::System(CreateFunction cf, unsigned int target, float createRate)
: target(target)
, rate(createRate)
, toCreate(0.f)
, replace(false)
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
void System<T>::setReplaceDestroyed(bool r) {
    replace = r;
}

template<typename T>
unsigned int System<T>::particleCount() const {
    return particles.size();
}

template<typename T>
void System<T>::update(const UpdateFunction& cb, float dt) {
    for (unsigned int i = 0; i < particles.size(); ++i) {
        if (!cb(particles[i])) {
            if (replace && target >= particles.size()) { createFunction(&particles[i]); }
            else {
                particles.erase(i);
                --i;
            }
        }
    }

    if (target > particleCount()) {
        toCreate = std::min(toCreate + rate * dt, static_cast<float>(target - particleCount()));
        const unsigned int create = std::floor(toCreate);
        toCreate -= create;

        for (unsigned int i = 0; i < create; ++i) {
            particles.emplace_back();
            createFunction(&particles.back());
        }
    }
}

template<typename T>
void System<T>::render(const RenderFunction& cb) const {
    for (const auto& particle : particles) { cb(particle); }
}

} // namespace particle
} // namespace bl

#endif
