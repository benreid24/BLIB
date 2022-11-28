#ifndef BLIB_FILES_JSONLOADER_HPP
#define BLIB_FILES_JSONLOADER_HPP

#include <BLIB/Serialization/JSON/JSON.hpp>
#include <fstream>
#include <sstream>

namespace bl
{
namespace serial
{
namespace json
{
/**
 * @brief Utility class to load JSON objects from input streams or files. Each call consumes from
 *        the given file or stream, so only call if you know that content will be present. Prefer
 *        the global read/write methods or Serializer, this is mainly for internal use
 *
 * @ingroup JSON
 */
class Loader {
public:
    /**
     * @brief Construct a new Loader from a file
     *
     * @param filename The file to open
     */
    Loader(const std::string& filename);

    /**
     * @brief Construct a new Loader from a stream
     *
     * @param stream The stream to read from
     */
    Loader(std::istream& stream);

    /**
     * @brief Loads a JSON value from the underlying stream
     *
     * @param result The Value to store the result in
     * @return True if the value could be loaded, false otherwise
     */
    bool loadValue(Value& result);

    /**
     * @brief Loads a string value from the underlying stream
     *
     * @param result The string to read into
     * @return True if read successfully, false otherwise
     */
    bool loadString(std::string& result);

    /**
     * @brief Loads a numeric value from the underlying stream
     *
     * @param val Value to store the result in
     * @return True if a number was read, false otherwise
     */
    bool loadNumeric(Value& val);

    /**
     * @brief Loads a boolean value from the stream
     *
     * @param result Boolean to place the read value in
     * @return True if read successfully, false otherwise
     */
    bool loadBool(bool& result);

    /**
     * @brief Loads a list from the underlying stream
     *
     * @param result List to read into
     * @return True if a list was read, false otherwise
     */
    bool loadList(List& result);

    /**
     * @brief Loads a group from the underlying stream
     *
     * @param group Group to load into
     * @return True if a group could be read, false otherwise
     */
    bool loadGroup(Group& group);

private:
    std::ifstream fileInput;
    std::istream& input;
    bool valid;

    const std::string filename;
    int currentLine;

    bool isValid() const;

    void skipWhitespace();
    void skipSymbol();

    bool isNumber(char c) const;
    bool isNumeric(char c) const;
    bool isWhitespace(char c) const;

    std::string error();
};

} // namespace json
} // namespace serial
} // namespace bl

#endif
