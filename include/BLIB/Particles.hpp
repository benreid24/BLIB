#ifndef BLIB_PARTICLES_HPP
#define BLIB_PARTICLES_HPP

/**
 * @defgroup Particles
 * @brief A robust particle system supporting arbitrary particle types and user defined emitters,
 *        affectors, sinks, and renderer integration
 *
 */

#include <BLIB/Particles/System.hpp>

#include <BLIB/Particles/Affector.hpp>
#include <BLIB/Particles/DescriptorSetFactory.hpp>
#include <BLIB/Particles/DescriptorSetInstance.hpp>
#include <BLIB/Particles/Emitter.hpp>
#include <BLIB/Particles/Link.hpp>
#include <BLIB/Particles/ParticleManager.hpp>
#include <BLIB/Particles/ParticleManagerBase.hpp>
#include <BLIB/Particles/ParticleSystem.hpp>
#include <BLIB/Particles/Renderer.hpp>
#include <BLIB/Particles/Sink.hpp>

// Keep separate
#include <BLIB/Particles/DescriptorSetFactory.inl>
#include <BLIB/Particles/DescriptorSetInstance.inl>
#include <BLIB/Particles/Renderer.inl>

#endif
