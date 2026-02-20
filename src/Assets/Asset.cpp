#include <BLIB/Assets/Asset.hpp>

#include <BLIB/Logging.hpp>
#include <stdexcept>

namespace bl
{
namespace as
{
Asset::Asset(Repository& repo)
: repo(repo)
, uuid()
, type()
, metadata()
, payload(nullptr)
, state(State::Unknown) {}

Payload& Asset::getPayload() {
    if (state != State::Loaded) {
        BL_LOG_ERROR << "Attempted to access asset payload when asset is not loaded";
        throw std::runtime_error("Asset payload is not loaded");
    }
    return *payload;
}

} // namespace as
} // namespace bl
