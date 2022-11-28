#include <BLIB/Serialization/JSON/JSONLoader.hpp>

#include <BLIB/Logging.hpp>
#include <fstream>

namespace bl
{
namespace serial
{
namespace json
{
Loader::Loader(const std::string& filename)
: input(fileInput)
, valid(true)
, filename(filename)
, currentLine(0) {
    fileInput.open(filename.c_str());
    skipWhitespace();
}

Loader::Loader(std::istream& stream)
: input(stream)
, valid(true)
, filename("memory")
, currentLine(0) {
    skipWhitespace();
}

std::string Loader::error() {
    valid = false;
    std::stringstream ss;
    ss << "File '" << filename << "' line " << currentLine << ": ";
    return ss.str();
}

void Loader::skipWhitespace() {
    while (isValid() && isWhitespace(input.peek())) {
        if (input.get() == '\n') currentLine += 1;
    }
}

void Loader::skipSymbol() {
    if (isValid()) {
        skipWhitespace();
        input.get();
        skipWhitespace();
    }
}

bool Loader::isValid() const { return input.good() && valid; }

bool Loader::isWhitespace(char c) const { return c == '\n' || c == ' ' || c == '\r' || c == '\t'; }

bool Loader::isNumber(char c) const { return c >= '0' && c <= '9'; }

bool Loader::isNumeric(char c) const { return isNumber(c) || c == '-'; }

bool Loader::loadValue(Value& result) {
    const SourceInfo source = {filename, currentLine};
    result.setSource(source);

    if (isNumeric(input.peek())) { return loadNumeric(result); }
    if (input.peek() == '"') {
        std::string str;
        if (!loadString(str)) return false;
        result = str;
        return true;
    }
    if (input.peek() == 't' || input.peek() == 'f') {
        bool rb = false;
        if (!loadBool(rb)) return false;
        result = rb;
        return true;
    }
    if (input.peek() == '{') {
        Group grp;
        if (!loadGroup(grp)) return false;
        result = grp;
        return true;
    }
    if (input.peek() == '[') {
        List list;
        if (!loadList(list)) return false;
        result = list;
        return true;
    }
    BL_LOG_ERROR << error() << "Unexpected character '" << input.peek() << "'\n";
    return false;
}

bool Loader::loadBool(bool& result) {
    if (input.peek() == 't' || input.peek() == 'f') {
        std::string word;
        word.reserve(5);
        while (std::isalpha(input.peek()) && input.good()) {
            word.push_back(input.get());
            if (word == "true") {
                skipWhitespace();
                result = true;
                return true;
            }
            if (word == "false") {
                skipWhitespace();
                result = false;
                return true;
            }
            if (word.size() > 5) {
                BL_LOG_ERROR << error() << "Expected boolean value but too many characters";
                valid = false;
                return false;
            }
        }
        if (input.good()) {
            BL_LOG_ERROR << error() << "'" << word << "' is not a boolean value";
            valid = false;
            return false;
        }
    }
    else {
        valid = false;
        BL_LOG_ERROR << error() << "Unexpected character '" << input.peek() << "'";
        return false;
    }
    valid = false;
    BL_LOG_ERROR << error() << "Unexpected end of file while parsing boolean";
    return false;
}

bool Loader::loadNumeric(Value& val) {
    if (isValid()) {
        const char c = input.peek();
        if (c == '-' || (c >= '0' && c <= '9')) {
            std::string num;
            num.push_back(input.get());
            bool decimal = false;

            while (isNumber(input.peek()) || input.peek() == '.') {
                if (input.peek() == '.' && decimal) {
                    BL_LOG_ERROR << error() << "Too many decimal points in number";
                    return false;
                }
                else if (input.peek() == '.')
                    decimal = true;

                num.push_back(input.get());
                if (!input.good()) {
                    BL_LOG_ERROR << error() << "Unexpected end of file";
                    valid = false;
                    return false;
                }
            }
            skipWhitespace();
            if (decimal) { val = std::stof(num.c_str()); }
            else { val = std::stol(num.c_str()); }
            return true;
        }
        else {
            BL_LOG_ERROR << error() << "Invalid numeric symbol " << c;
            valid = false;
            return false;
        }
    }
    return false;
}

bool Loader::loadString(std::string& result) {
    if (isValid()) {
        if (input.peek() == '"') {
            input.get();
            while (input.peek() != '"') {
                if (input.peek() == '\n') currentLine += 1;

                if (input.peek() == '\\') {
                    const char c = input.get();
                    switch (c) {
                    case 'n':
                        result.push_back('\n');
                        break;
                    case 't':
                        result.push_back('\t');
                        break;
                    default:
                        result.push_back(c);
                        break;
                    }
                }
                else { result.push_back(input.get()); }
                if (!input.good()) {
                    valid = false;
                    BL_LOG_ERROR << "Unexpected end of file";
                    return false;
                }
            }
            skipSymbol(); // closing quote
            return true;
        }
        BL_LOG_ERROR << error() << "Unxpected symbol '" << input.peek() << "' expecting '\"'";
        valid = false;
    }
    return false;
}

bool Loader::loadList(List& result) {
    if (!isValid()) return false;

    if (input.peek() != '[') {
        valid = false;
        BL_LOG_ERROR << error() << "Expected '[' got " << input.peek();
        return false;
    }
    skipSymbol();

    while (input.peek() != ']' && isValid()) {
        result.emplace_back(0);
        if (!loadValue(result.back())) return false;
        if (input.peek() == ',')
            skipSymbol();
        else if (input.peek() != ']') {
            valid = false;
            BL_LOG_ERROR << error() << "Expecting ',' got '" << input.peek() << "'\n";
            return false;
        }
    }
    skipSymbol();

    return true;
}

bool Loader::loadGroup(Group& result) {
    if (!isValid()) return false;

    if (input.peek() != '{') {
        valid = false;
        BL_LOG_ERROR << error() << "Expecting '{' but got '" << input.peek() << "'\n";
        return false;
    }
    skipSymbol();

    while (input.peek() != '}' && isValid()) {
        std::string name;
        if (!loadString(name)) return false;
        if (input.peek() != ':') {
            valid = false;
            BL_LOG_ERROR << error() << "Expecting ':' after field name got '" << input.peek()
                         << "'\n";
            return false;
        }
        skipSymbol();

        result.addField(name, Value{false});
        Value& val = *result.getField(name);
        if (!loadValue(val)) return false;
        if (input.peek() == ',')
            skipSymbol();
        else if (input.peek() != '}') {
            valid = false;
            BL_LOG_ERROR << error() << "Expecting ',' got '" << input.peek() << "'\n";
            return false;
        }
    }
    skipSymbol();

    return true;
}

} // namespace json
} // namespace serial
} // namespace bl
