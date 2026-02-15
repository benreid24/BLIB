#ifndef BLIB_ASSETS_LOADER_HPP
#define BLIB_ASSETS_LOADER_HPP

#include <BLIB/Assets/Payload.hpp>
#include <memory>

namespace bl
{
namespace as
{
class Loader {
public:
    struct LoadContext {
        // TODO - stuff here on what is being loaded and helper methods to access nested files and
        // such. May be worth pulling out into separate class. How/where to handle editor vs game?
    };

    virtual ~Loader() = default;

    virtual std::unique_ptr<Payload> load() = 0;
};

} // namespace as
} // namespace bl

#endif
