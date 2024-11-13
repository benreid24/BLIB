#ifndef BLIB_ECS_HPP
#define BLIB_ECS_HPP

/**
 * @defgroup ECS
 * @brief Collection of classes for managing entities via the ECS pattern
 *
 */

#include <BLIB/ECS/Cleaner.hpp>
#include <BLIB/ECS/ComponentMask.hpp>
#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/ECS/ComponentSet.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/EntityBacked.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/ECS/Registry.hpp>
#include <BLIB/ECS/Transaction.hpp>
#include <BLIB/ECS/View.hpp>

#include <BLIB/ECS/Traits/ChildAware.hpp>
#include <BLIB/ECS/Traits/ParentAware.hpp>
#include <BLIB/ECS/Traits/ParentAwareVersioned.hpp>
#include <BLIB/ECS/Traits/Versioned.hpp>

#endif
