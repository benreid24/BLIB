#include <BLIB/Assets/Metadata.hpp>

#include <BLIB/Assets/Asset.hpp>

namespace bl
{
namespace as
{
Metadata::Metadata()
: displayName("New Asset")
, description("")
, creationTime(std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::utc_clock::now().time_since_epoch())
                   .count())
, isAutoLoaded(false)
, owner(nullptr) {}

Metadata::Metadata(const Metadata& copy)
: displayName(copy.displayName)
, description(copy.description)
, creationTime(copy.creationTime)
, owner(nullptr) {}

std::chrono::sys_time<std::chrono::seconds> Metadata::getCreationTime() const {
    const auto utcTime =
        std::chrono::utc_time<std::chrono::seconds>{std::chrono::seconds{creationTime}};
    return std::chrono::utc_clock::to_sys(utcTime);
}

void Metadata::setDisplayName(const std::string& name) {
    if (owner) { owner->handleDisplayNameChange(displayName, name); }
    displayName = name;
}

void Metadata::setDescription(const std::string& description) { this->description = description; }

void Metadata::setPath(const std::string& p) {
    if (owner) { owner->handlePathChange(path, p); }
    path = p;
}

void Metadata::setCreationTime(std::uint64_t time) { creationTime = time; }

void Metadata::setIsAutoLoaded(bool al) {
    isAutoLoaded = al;
    if (owner) { owner->handleAutoLoadChange(); }
}

} // namespace as
} // namespace bl