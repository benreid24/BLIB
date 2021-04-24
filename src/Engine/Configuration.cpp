#include <BLIB/Engine/Configuration.hpp>
#include <fstream>

namespace bl
{
namespace engine
{
namespace
{
template<typename T>
void parseValue(const std::string& type, const std::string& value, const std::string& key,
                unsigned int line) {
    T parsed = T();
    std::stringstream parser(value);
    parser >> parsed;
    if (!parser.eof()) {
        BL_LOG_WARN << "Bad " << type << " for key'" << key << "' on line " << line
                    << ". value: " << value;
    }
    else {
        Configuration::set<T>(key, parsed, true);
    }
}

template<typename T>
void saveValues(
    std::ofstream& output, char code,
    std::unordered_map<std::type_index, std::unordered_map<std::string, std::any>>& config) {
    const auto tkey = std::type_index(typeid(T));
    auto it         = config.find(tkey);
    if (it != config.end()) {
        for (const auto& pair : it->second) {
            output << code << pair.first << "=" << std::any_cast<T>(pair.second) << "\n";
        }
    }
}

template<typename T>
void logValues(
    std::unordered_map<std::type_index, std::unordered_map<std::string, std::any>>& config) {
    const auto tkey = std::type_index(typeid(T));
    auto it         = config.find(tkey);
    if (it != config.end()) {
        for (const auto& pair : it->second) {
            BL_LOG_INFO << "Config " << pair.first << "=" << std::any_cast<T>(pair.second);
        }
    }
}

} // namespace

Configuration& Configuration::get() {
    static Configuration configuration;
    return configuration;
}

bool Configuration::load(const std::string& file) {
    BL_LOG_INFO << "Loading configuration from " << file;

    std::ifstream input(file.c_str(), std::ios::in);
    if (!input.good()) return false;

    unsigned int lineNo = 1;
    while (input.good()) {
        std::string lineStr;
        std::getline(input, lineStr);
        if (lineStr.empty()) break;

        std::stringstream line(lineStr);
        std::string key, value;
        const char t = line.get();
        if (line.eof()) return false;
        std::getline(line, key, '=');
        if (line.eof()) return false;
        std::getline(line, value);

        switch (t) {
        case 'b':
            if (value == "1") { Configuration::set<bool>(key, true); }
            else if (value == "0") {
                Configuration::set<bool>(key, false, true);
            }
            else {
                BL_LOG_WARN << "Bad bool value for key '" << key << "' value: " << value;
            }
            break;

        case 'i':
            parseValue<int>("integer", value, key, lineNo);
            break;

        case 'u':
            parseValue<unsigned int>("unsigned integer", value, key, lineNo);
            break;

        case 'f':
            parseValue<float>("float", value, key, lineNo);
            break;

        case 's':
            Configuration::set<std::string>(key, value, true);
            break;

        default:
            BL_LOG_WARN << "Invalid type '" << t << "'"
                        << "on line " << lineNo << " (" << lineStr << ")";
            break;
        }
        ++lineNo;
    }

    return true;
}

bool Configuration::save(const std::string& file) {
    std::ofstream output(file.c_str(), std::ios::out);
    if (!output.good()) return false;

    saveValues<bool>(output, 'b', get().config);
    saveValues<int>(output, 'i', get().config);
    saveValues<unsigned int>(output, 'u', get().config);
    saveValues<float>(output, 'f', get().config);
    saveValues<std::string>(output, 's', get().config);

    return output.good();
}

void Configuration::log() {
    logValues<bool>(get().config);
    logValues<int>(get().config);
    logValues<unsigned>(get().config);
    logValues<float>(get().config);
    logValues<std::string>(get().config);
}

void Configuration::clear() { get().config.clear(); }

} // namespace engine
} // namespace bl
