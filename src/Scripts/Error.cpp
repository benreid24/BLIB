#include <BLIB/Scripts/Error.hpp>
#include <sstream>

namespace bl
{
namespace scripts
{
namespace
{
std::vector<std::string> makeStack(const std::string& msg,
                                   const std::vector<std::string>& current) {
    std::vector<std::string> v = current;
    v.push_back(msg);
    return v;
}

std::string buildMsg(const std::string& err, parser::Node::Ptr src) {
    std::stringstream ss;
    ss << "Line " << src->sourceLine << " position " << src->sourceColumn << ": " << err;
    return ss.str();
}

std::string buildStackMsg(const std::vector<std::string>& stack) {
    if (stack.empty()) return "";
    std::stringstream ss;
    auto i = stack.rbegin();
    ss << *i << '\n';
    for (++i; i != stack.rend(); ++i) { ss << "    " << *i << '\n'; }
    return ss.str();
}
} // namespace

Error::Error(const std::string& msg)
: error(msg)
, stack(msg) {}

Error::Error(const std::string& msg, parser::Node::Ptr src)
: error(buildMsg(msg, src))
, stack(error) {}

Error::Error(const std::string& msg, parser::Node::Ptr src, const Error& prev)
: error(buildMsg(msg, src))
, stack(error + "\n    " + prev.stack) {}

const std::string& Error::message() const { return error; }

const std::string& Error::stacktrace() const { return stack; }

} // namespace scripts
} // namespace bl