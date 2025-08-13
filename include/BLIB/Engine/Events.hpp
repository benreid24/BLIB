#ifndef BLIB_ENGINE_EVENTS_HPP
#define BLIB_ENGINE_EVENTS_HPP

/**
 * @addtogroup EngineEvents
 * @ingroup Engine
 * @brief Collection of event classes that fire as the Engine runs
 */

#include <BLIB/Engine/Events/Paused.hpp>
#include <BLIB/Engine/Events/Players.hpp>
#include <BLIB/Engine/Events/Resumed.hpp>
#include <BLIB/Engine/Events/Shutdown.hpp>
#include <BLIB/Engine/Events/Startup.hpp>
#include <BLIB/Engine/Events/StateChange.hpp>
#include <BLIB/Engine/Events/WindowResize.hpp>
#include <BLIB/Engine/Events/Worlds.hpp>

/// Key for accessing the engine signal channel from the global signal table
constexpr const char* SignalChannelKey = "BLIB.Engine.Signals";

#endif
