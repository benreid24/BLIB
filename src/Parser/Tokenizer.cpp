#include <BLIB/Parser/Tokenizer.hpp>
#include <iostream>

namespace bl
{
namespace parser
{
Tokenizer::Tokenizer(ISkipper::Ptr skipper)
: skipper(skipper) {}

void Tokenizer::addTokenType(Node::Type type, const std::string& regex) {
    matchers.push_front(std::make_pair(std::regex(regex.c_str()), type));
}

std::vector<Node::Ptr> Tokenizer::tokenize(Stream& input) const {
    if (!skipper) return {};

    std::vector<Node::Ptr> tokens;
    std::string current;
    while (input.valid()) {
        skipper->skip(input);
        current.push_back(input.get());
        for (const auto& matcher : matchers) {
            std::smatch result;
            if (std::regex_match(current, result, matcher.first)) {
                Node::Ptr token(new Node());
                token->data         = result[0].str();
                token->sourceLine   = input.currentLine();
                token->sourceColumn = input.currentColumn();
                token->type         = matcher.second;
                tokens.push_back(token);
                current.clear();
                break;
            }
        }
    }

    if (!current.empty()) {
        std::cerr << "Reached end of input with unparsed data: '" << current << "'\n";
        return {};
    }
    return tokens;
}

} // namespace parser
} // namespace bl