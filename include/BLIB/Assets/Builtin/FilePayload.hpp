#ifndef BLIB_ASSETS_BUILTIN_FILEPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_FILEPAYLOAD_HPP

#include <BLIB/Assets/Payload.hpp>
#include <vector>

namespace bl
{
namespace asi
{
/**
 * @brief Generic payload container for single files
 *
 * @ingroup Assets
 */
class FilePayload : public as::Payload {
public:
    /**
     * @brief Creates the payload
     *
     * @param ctx The repo context
     */
    FilePayload(const Payload::ConstructContext& ctx)
    : Payload(ctx) {}

    /**
     * @brief Destroys the payload
     */
    virtual ~FilePayload() = default;

    /**
     * @brief Flushes the payload data to the asset repository. Only needs to be called if mutated
     *        after creation
     */
    void saveChangesToAssetRepo() { flush(); }

    /**
     * @brief Returns the file data
     */
    std::vector<char>& getData() { return data; }

    /**
     * @brief Returns the file data
     */
    const std::vector<char>& getData() const { return data; }

private:
    std::vector<char> data;
};

} // namespace asi
} // namespace bl

#endif
