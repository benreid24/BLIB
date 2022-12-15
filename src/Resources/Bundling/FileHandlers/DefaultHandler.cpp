#include <BLIB/Resources/Bundling/FileHandlers/DefaultHandler.hpp>
#include <fstream>

namespace bl
{
namespace resource
{
namespace bundle
{
bool DefaultHandler::processFile(const std::string& path, std::ostream& output,
                                 FileHandlerContext&) {
    std::ifstream input(path.c_str(), std::ios::binary);
    if (!input.good()) return false;
    std::istreambuf_iterator<char> begin_source(input);
    std::istreambuf_iterator<char> end_source;
    std::ostreambuf_iterator<char> begin_dest(output);
    std::copy(begin_source, end_source, begin_dest);
    return true;
}

} // namespace bundle
} // namespace resource
} // namespace bl
