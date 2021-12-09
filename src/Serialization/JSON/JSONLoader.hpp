#ifndef BLIB_FILES_JSONLOADER_HPP
#define BLIB_FILES_JSONLOADER_HPP

#include <BLIB/Serialization/JSON/JSON.hpp>
#include <sstream>

namespace bl
{
namespace serial
{
namespace json
{
/**
 * @brief Utility class to load JSON objects from input streams or files
 * @ingroup JSON
 *
 */
class Loader {
public:
    Loader(const std::string& filename);
    Loader(std::istream& stream);

    Group load();

private:
    std::stringstream input;
    bool valid;

    const std::string filename;
    int currentLine;

    bool isValid() const;

    void skipWhitespace();
    void skipSymbol();

    bool isNumber(char c) const;
    bool isNumeric(char c) const;
    bool isWhitespace(char c) const;

    Value loadValue();
    std::string loadString();
    float loadNumeric();
    bool loadBool();
    List loadList();
    Group loadGroup();

    std::string error();
};

} // namespace json
} // namespace file
} // namespace bl

#endif
