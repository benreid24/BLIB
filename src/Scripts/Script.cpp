#include <BLIB/Scripts.hpp>

#include <BLIB/Files/FileUtil.hpp>
#include <Scripts/Parser.hpp>
#include <fstream>
#include <streambuf>

namespace bl
{
using namespace scripts;

Script::Script(const std::string& data) {
    std::string input = data;
    if (FileUtil::exists(data)) {
        std::ifstream file(data.c_str());
        file.seekg(0, std::ios::end);
        input.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        input.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }
    root = scripts::Parser::parse(input);
}

} // namespace bl