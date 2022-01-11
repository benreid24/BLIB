#include <BLIB/Scripts/Error.hpp>
#include <sstream>

namespace bl
{
namespace script
{
namespace
{
std::string buildMsg(const std::string& err, const parser::Node::Ptr& src) {
    std::stringstream ss;
    ss << "Line " << src->sourceLine << " position " << src->sourceColumn << ": " << err;
    return ss.str();
}
} // namespace

Error::Error(const std::string& msg)
: error(msg)
, stack(msg) {}

Error::Error(const std::string& msg, const parser::Node::Ptr& src)
: error(buildMsg(msg, src))
, stack(error) {}

Error::Error(const std::string& msg, const parser::Node::Ptr& src, const Error& prev)
: error(buildMsg(msg, src))
, stack(error + "\n    " + prev.stack) {}

const std::string& Error::message() const { return error; }

const std::string& Error::stacktrace() const { return stack; }

} // namespace script
} // namespace bl
