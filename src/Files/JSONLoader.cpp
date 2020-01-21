#include <Files/JSONLoader.hpp>

#include <fstream>
#include <iostream>

namespace bl
{
namespace json
{

Loader::Loader(const std::string& filename) : filename(filename) {
    std::ifstream data(filename.c_str());
    std::string buffer;

    data.seekg(0, std::ios::end);
    buffer.reserve(data.tellg());
    data.seekg(0, std::ios::beg);
    buffer.assign((std::istreambuf_iterator<char>(data)), std::istreambuf_iterator<char>());

    input.str(buffer);
    skipWhitespace();
}

Loader::Loader(std::istream& stream) : filename("memory") {
    std::string buffer;
    stream.seekg(0, std::ios::end);
    buffer.reserve(stream.tellg());
    stream.seekg(0, std::ios::beg);
    buffer.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

    input.str(buffer);
    skipWhitespace();
}

Group Loader::load() {
    return loadGroup();
}

std::ostream& Loader::error() {
    valid = false;
    std::cerr << "Error: file '" << filename << "' line " << currentLine << ": ";
    return std::cerr;
}

void Loader::skipWhitespace() {
    while (isValid() && isWhitespace(input.peek())) {
        if (input.get() == '\n')
            currentLine += 1;
    }
}

void Loader::skipSymbol() {
    if (isValid()) {
        skipWhitespace();
        input.get();
        skipWhitespace();
    }
}

bool Loader::isValid() const {
    return input.good() && valid;
}

bool Loader::isWhitespace(char c) const {
    return c=='\n' || c==' ' || c=='\r' || c=='\t';
}

bool Loader::isNumber(char c) const {
    return c>='0' && c<='9';
}

bool Loader::isNumeric(char c) const {
    return isNumber(c) || c=='-';
}

Value Loader::loadValue() {
    if (isNumeric(input.peek()))
        return loadNumeric();
    if (input.peek() == '"')
        return loadString();
    if (input.peek() == 't' || input.peek() == 'f')
        return loadBool();
    if (input.peek() == '{')
        return loadGroup();
    if (input.peek() == '[')
        return loadList();
    error() << "Unexpected character '" << input.peek() << "'\n";
    return false;
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
                error() << "Expected boolean value but too many characters" << std::endl;
                valid = false;
                return false;
            }
        }
        if (input.good()) {
            error() << "'" << word << "' is not a boolean value" << std::endl;
            valid = false;
            return false;
        }
    }
    else {
        valid = false;
        error() << "Unexpected character '" << input.peek() << "'" << std::endl;
        return false;
    }
    valid = false;
    error() << "Unexpected end of file while parsing boolean" << std::endl;
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
                    error() << "Too many decimal points in number" << std::endl;
                    return 0;
                }
                else if (input.peek() == '.')
                    decimal = true;

                num.push_back(input.get());
                if (!input.good()) {
                    error() << "Unexpected end of file";
                    valid = false;
                    return 0;
                }
            }
            skipWhitespace();
            return std::stod(num);
        }
        else {
            error() << "Invalid numeric symbol " << c << std::endl;
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
                if (input.peek() == '\n')
                    currentLine += 1;

                ret.push_back(input.get());
                if (!input.good()) {
                    valid = false;
                    std::cerr << "Unexpected end of file" << std::endl;
                    return "";
                }
            }
            skipSymbol(); // closing quote
            return ret;
        }
        error() << "Unxpected symbol '" << input.peek() << "' expecting '\"'" << std::endl;
        valid = false;
    }
}

List Loader::loadList() {
    List ret;
    if (!isValid())
        return ret;
    
    if (input.peek() != '[') {
        valid = false;
        error() << "Expected '[' got " << input.peek() << std::endl;
        return ret;
    }
    skipSymbol();

    while (input.peek() != ']' && isValid()) {
        ret.push_back(loadValue());
        if (input.peek() == ',')
            skipSymbol();
        else if (input.peek() != ']') {
            valid = false;
            error() << "Expecting ',' got '" << input.peek() << "'\n";
            return ret;
        }
    }
    skipSymbol();
    return ret;
}

Group Loader::loadGroup() {
    Group group;
    if (!isValid())
        return group;

    if (input.peek() != '{') {
        valid = false;
        error() << "Expecting '{' but got '" << input.peek() << "'\n";
        return group;
    }
    skipSymbol();

    while (input.peek() != '}' && isValid()) {
        const std::string name = loadString();
        if (input.peek() != ':') {
            valid = false;
            error() << "Expecting ':' after field name got '" << input.peek() << "'\n";
            return group;
        }
        skipSymbol();
        group.addField(name, loadValue());
        if (input.peek() == ',')
            skipSymbol();
        else if (input.peek() != '}') {
            valid = false;
            error() << "Expecting ',' got '" << input.peek() << "'\n";
            return group;
        }
    }
    return group;
}

}
}