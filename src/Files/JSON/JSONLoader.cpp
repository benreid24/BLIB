#include <Files/JSON/JSONLoader.hpp>

#include <BLIB/Logging.hpp>
#include <fstream>

namespace bl
{
namespace file
{
namespace json
{
Loader::Loader(const std::string& filename)
: valid(true)
, filename(filename)
, currentLine(0) {
    std::ifstream data(filename.c_str());
    std::string buffer;

    data.seekg(0, std::ios::end);
    buffer.reserve(data.tellg());
    data.seekg(0, std::ios::beg);
    buffer.assign((std::istreambuf_iterator<char>(data)), std::istreambuf_iterator<char>());

    input.str(buffer);
    skipWhitespace();
}

Loader::Loader(std::istream& stream)
: valid(true)
, filename("memory")
, currentLine(0) {
    std::string buffer;
    stream.seekg(0, std::ios::end);
    buffer.reserve(stream.tellg());
    stream.seekg(0, std::ios::beg);
    buffer.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

    input.str(buffer);
    skipWhitespace();
}

Group Loader::load() { return loadGroup(); }

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

Value Loader::loadValue() {
    const SourceInfo source = {filename, currentLine};

    if (isNumeric(input.peek())) {
        Value val = loadNumeric();
        val.setSource(source);
        return val;
    }
    if (input.peek() == '"') {
        Value val = loadString();
        val.setSource(source);
        return val;
    }
    if (input.peek() == 't' || input.peek() == 'f') {
        Value val = Value(loadBool());
        val.setSource(source);
        return val;
    }
    if (input.peek() == '{') {
        Value val = loadGroup();
        val.setSource(source);
        return val;
    }
    if (input.peek() == '[') {
        Value val = loadList();
        val.setSource(source);
        return val;
    }
    BL_LOG_ERROR << error() << "Unexpected character '" << input.peek() << "'\n";
    return Value(false);
}

bool Loader::loadBool() {
    if (input.peek() == 't' || input.peek() == 'f') {
        std::string word;
        while (!isWhitespace(input.peek()) && input.good()) {
            word.push_back(input.get());
            if (word == "true") {
                skipWhitespace();
                return true;
            }
            if (word == "false") {
                skipWhitespace();
                return false;
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

float Loader::loadNumeric() {
    if (isValid()) {
        const char c = input.peek();
        if (c == '-' || (c >= '0' && c <= '9')) {
            std::string num;
            num.push_back(input.get());
            bool decimal = false;

            while (isNumber(input.peek()) || input.peek() == '.') {
                if (input.peek() == '.' && decimal) {
                    BL_LOG_ERROR << error() << "Too many decimal points in number";
                    return 0;
                }
                else if (input.peek() == '.')
                    decimal = true;

                num.push_back(input.get());
                if (!input.good()) {
                    BL_LOG_ERROR << error() << "Unexpected end of file";
                    valid = false;
                    return 0;
                }
            }
            skipWhitespace();
            return std::stod(num);
        }
        else {
            BL_LOG_ERROR << error() << "Invalid numeric symbol " << c;
            valid = false;
        }
    }
    return 0;
}

std::string Loader::loadString() {
    if (isValid()) {
        if (input.peek() == '"') {
            std::string ret;
            input.get();
            while (input.peek() != '"') {
                if (input.peek() == '\n') currentLine += 1;

                ret.push_back(input.get());
                if (!input.good()) {
                    valid = false;
                    BL_LOG_ERROR << "Unexpected end of file";
                    return "";
                }
            }
            skipSymbol(); // closing quote
            return ret;
        }
        BL_LOG_ERROR << error() << "Unxpected symbol '" << input.peek() << "' expecting '\"'";
        valid = false;
    }
    return "";
}

List Loader::loadList() {
    List ret;
    if (!isValid()) return ret;

    if (input.peek() != '[') {
        valid = false;
        BL_LOG_ERROR << error() << "Expected '[' got " << input.peek();
        return ret;
    }
    skipSymbol();

    while (input.peek() != ']' && isValid()) {
        ret.push_back(loadValue());
        if (input.peek() == ',')
            skipSymbol();
        else if (input.peek() != ']') {
            valid = false;
            BL_LOG_ERROR << error() << "Expecting ',' got '" << input.peek() << "'\n";
            return ret;
        }
    }
    skipSymbol();

    return ret;
}

Group Loader::loadGroup() {
    Group group;
    if (!isValid()) return group;

    if (input.peek() != '{') {
        valid = false;
        BL_LOG_ERROR << error() << "Expecting '{' but got '" << input.peek() << "'\n";
        return group;
    }
    skipSymbol();

    while (input.peek() != '}' && isValid()) {
        const std::string name = loadString();
        if (input.peek() != ':') {
            valid = false;
            BL_LOG_ERROR << error() << "Expecting ':' after field name got '" << input.peek()
                         << "'\n";
            return group;
        }
        skipSymbol();

        group.addField(name, loadValue());
        if (input.peek() == ',')
            skipSymbol();
        else if (input.peek() != '}') {
            valid = false;
            BL_LOG_ERROR << error() << "Expecting ',' got '" << input.peek() << "'\n";
            return group;
        }
    }
    skipSymbol();

    return group;
}

} // namespace json
} // namespace file
} // namespace bl
