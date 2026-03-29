#ifndef BLIB_STREAMS_OUTPUTSTREAM_HPP
#define BLIB_STREAMS_OUTPUTSTREAM_HPP

#include <BLIB/Streams/Mode.hpp>
#include <fstream>
#include <string_view>
#include <variant>
#include <vector>

namespace bl
{
namespace stream
{
class OutputStream {
public:
    OutputStream();

    OutputStream(std::string_view path);

    OutputStream(std::ostream& stream);

    OutputStream(std::size_t initialSize);

    ~OutputStream() = default;

    Mode getMode() const;

    bool isValid() const;

    operator bool() const { return isValid(); }

    bool open(std::ostream& stream);

    void open(std::size_t initialSize);

    void open(std::ostream& stream);

    bool write(const char* data, std::size_t len);

private:
    std::variant<std::monostate, std::ofstream, std::ostream*, std::vector<char>> stream;
};

} // namespace stream
} // namespace bl

#endif
