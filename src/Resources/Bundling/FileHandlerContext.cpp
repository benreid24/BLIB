#include <BLIB/Resources/Bundling/FileHandlerContext.hpp>

namespace bl
{
namespace resource
{
namespace bundle
{
FileHandlerContext::FileHandlerContext(std::unordered_set<std::string>& excludes,
                                       std::stack<std::string, std::vector<std::string>>& fileStack)
: excludes(excludes)
, fileList(fileStack) {}

void FileHandlerContext::addDependencyFile(const std::string& path) { fileList.emplace(path); }

void FileHandlerContext::addExcludeFile(const std::string& path) { excludes.emplace(path); }

} // namespace bundle
} // namespace resource
} // namespace bl
