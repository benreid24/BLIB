# BLIB


BLIB started as a collection of code from various projects over the years suited for reuse. Since then it has been expanded into a large collection
of code, both new and reused, that I intend to use as a small game engine for personal projects. Some of the key features include:
- An Engine interface with state management and a configurable main game loop
- An Entity Component System
- A full GUI system as well as a Menu system suitable for controllers
- A full fledged logging system
- Animations, an animation editor, and a collection of custom renderable shapes
- Utilities for encoding and decoding formatted binary data and JSON
- A full script interpreter and a generic, reusable shift-reduce parser
- Thread safe resource management with automatic cleanup based on time since last used
- Support for asynchronous resource loading with progress callbacks
- Event queues with dispatches and scope guards
- A collection of utility classes such as Signal, Timer, random number generator, and more

Visit the [projects page](https://github.com/benreid24/BLIB/projects) for future feature development plans.

## Documentation

- [Examples](examples)
- [bScript language reference](docs/wiki/bscript_reference.md)
- [Full source documentation](https://benreid24.github.io/BLIB/modules.html)

## Build Status
![Windows](https://github.com/benreid24/BLIB/workflows/windows-verify/badge.svg?branch=master) 
![macOS](https://github.com/benreid24/BLIB/workflows/macos-verify/badge.svg?branch=master) 
![Linux](https://github.com/benreid24/BLIB/workflows/linux-verify/badge.svg?branch=master)
