#ifndef BLIB_ASSETS_METADATA_HPP
#define BLIB_ASSETS_METADATA_HPP

#include <chrono>
#include <cstdint>
#include <string>

namespace bl
{
namespace as
{
/**
 * @brief Asset metadata storage
 *
 * @ingroup Assets
 */
class Metadata {
public:
    Metadata();

    Metadata(const Metadata& copy);

    const std::string& getDisplayName() const { return displayName; }

    const std::string& getDescription() const { return description; }

    std::uint64_t getCreationTimeRaw() const { return creationTime; }

    std::chrono::sys_time<std::chrono::seconds> getCreationTime() const;

    void setDisplayName(const std::string& name);

    void setDescription(const std::string& description);

    void setCreationTime(std::uint64_t time);

private:
    std::string displayName;
    std::string description;
    std::uint64_t creationTime;
};

} // namespace as
} // namespace bl

#endif
