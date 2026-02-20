#include <BLIB/Assets/Metadata.hpp>

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
, isAutoLoaded(false) {}

Metadata::Metadata(const Metadata& copy)
: displayName(copy.displayName)
, description(copy.description)
, creationTime(copy.creationTime) {}

std::chrono::sys_time<std::chrono::seconds> Metadata::getCreationTime() const {
    const auto utcTime =
        std::chrono::utc_time<std::chrono::seconds>{std::chrono::seconds{creationTime}};
    return std::chrono::utc_clock::to_sys(utcTime);
}

void Metadata::setDisplayName(const std::string& name) { displayName = name; }

void Metadata::setDescription(const std::string& description) { this->description = description; }

void Metadata::setCreationTime(std::uint64_t time) { creationTime = time; }

void Metadata::setIsAutoLoaded(bool al) { isAutoLoaded = al; }

} // namespace as
} // namespace bl