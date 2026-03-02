#include <BLIB/Assets/Asset.hpp>

#include <BLIB/Assets/Repository.hpp>
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

bool Asset::create(const CreateContext::CustomData& createData) {
    state                      = State::Unloaded;
    detail::DriverBase* driver = repo.getDriver(type);
    if (!driver) {
        BL_LOG_ERROR << "Attempted to create asset with type " << type
                     << " but no driver is registered for that type";
        return false;
    }
    state = State::Loading;
    CreateContext context(repo, *this, createData);
    payload = driver->create(context);
    if (!payload) {
        BL_LOG_ERROR << "Driver failed to create asset of type " << type;
        state = State::Failed;
        return false;
    }
    state = State::Loaded;
    return true;
}

bool Asset::load() {
    if (state == State::Loaded) { return true; }
    if (state == State::Failed) { return false; }
    detail::DriverBase* driver = repo.getDriver(type);
    if (!driver) {
        BL_LOG_ERROR << "Attempted to load asset with type " << type
                     << " but no driver is registered for that type";
        state = State::Failed;
        return false;
    }
    state = State::Loading;
    ReadContext context(repo, *this);
    payload = driver->read(context);
    if (!payload) {
        BL_LOG_ERROR << "Driver failed to read asset of type " << type;
        state = State::Failed;
        return false;
    }
    state = State::Loaded;
    return true;
}

bool Asset::unload(bool force) {
    if (state != State::Loaded) { return true; }
    if (refCount > 0 && !force) {
        BL_LOG_WARN << "Attempted to unload asset with " << refCount << " outstanding references";
        return false;
    }
    payload.reset();
    state = State::Unloaded;
    return true;
}

} // namespace as
} // namespace bl
