#ifndef BLIB_MODELS_IMPORTER_HPP
#define BLIB_MODELS_IMPORTER_HPP

#include <assimp/Importer.hpp>

namespace bl
{
namespace mdl
{
class Model;

/**
 * @brief Wrapper class that performs Assimp imports with a simplified interface
 *
 * @ingroup Models
 */
class Importer {
public:
    /**
     * @brief Creates a new importer
     */
    Importer();

    /**
     * @brief Imports a model from the given path
     *
     * @param path The path to import from
     * @param result The model to populate with the imported data
     * @return True if the import was successful, false on error
     */
    bool import(const std::string& path, Model& result);

    /**
     * @brief Imports a model from memory
     *
     * @param buffer The memory to load from
     * @param len The number of bytes available in buffer
     * @param result The model to populate with the imported data
     * @return True if the import was successful, false on error
     */
    bool import(const char* buffer, std::size_t len, Model& result);

private:
    Assimp::Importer importer;
};

} // namespace mdl
} // namespace bl

#endif
