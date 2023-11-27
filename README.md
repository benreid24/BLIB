# BLIB

BLIB started as a collection of code from various projects over the years suited for reuse. Since then it has been expanded into a large collection
of code, both new and reused, that I intend to use as a small game engine for personal projects. Some of the key features include:
- Engine interface with state management and a configurable main game loop
- Framework for implementing game systems that are parallelized by default
- Entity Component System with entity parenting and dependencies supported out of the box
- A Vulkan-based renderer backed by the ECS
- Classical GUI framework and a Menu system suitable for controllers
- 2D Animations, an animation editor, and a collection of custom renderable shapes
- Utilities for encoding and decoding formatted binary data and JSON
- A robust logging system
- A custom script interpreter and a generic, reusable shift-reduce parser
- Thread safe resource management with automatic cleanup based on time since last used
- A collection of specialized containers and data structures useful for games
- Support for asynchronous resource loading with progress callbacks
- An event bus capable of efficiently dispatching any type of event struct across listeners
- A collection of other utilities such as signals, random number generation, a thread pool, and more

Visit the [project page](https://github.com/users/benreid24/projects/2) for future feature development plans.

## Documentation

- [Examples](examples)
- [bScript language reference](docs/wiki/bscript_reference.md)
- [Full source documentation](https://benreid24.github.io/BLIB/modules.html)

## Showcase

- [Peoplemon](https://github.com/benreid24/Peoplemon-3)

## Build Status
![Windows](https://github.com/benreid24/BLIB/workflows/windows-verify/badge.svg?branch=master) 
![macOS](https://github.com/benreid24/BLIB/workflows/macos-verify/badge.svg?branch=master) 
![Linux](https://github.com/benreid24/BLIB/workflows/linux-verify/badge.svg?branch=master)
