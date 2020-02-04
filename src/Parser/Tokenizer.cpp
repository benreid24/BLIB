#include <BLIB/Parser/Tokenizer.hpp>
#include <iostream>

namespace bl
{
namespace parser
{
Tokenizer::Tokenizer(ISkipper::Ptr skipper)
: skipper(skipper) {}

void Tokenizer::addTokenType(Node::Type type, const std::string& regex) {
    matchers[regex] = std::make_pair(std::regex(regex.c_str()), type);
    recomputeAmbiguous();
}

void Tokenizer::addEscapeSequence(const std::string& sequence, char c) {
    escapeSequences.push_back(std::make_pair(sequence, c));
}

void Tokenizer::addSkipperToggleChar(char c) { togglers.push_back(c); }

void Tokenizer::addKeyword(Node::Type src, Node::Type res, const std::string& kword) {
    kwords[src].push_back({kword, res});
}

std::vector<Node::Ptr> Tokenizer::tokenize(Stream& input) const {
    std::vector<Node::Ptr> tokens;
    std::string current;
    int fLine = 0;
    int fCol  = 0;

    bool skipperActive = true;
    while (input.valid()) {
        if (skipperActive && skipper) skipper->skip(input);
        if (input.peek() == EOF) break;
        if (std::find(togglers.begin(), togglers.end(), input.peek()) != togglers.end())
            skipperActive = !skipperActive;

        if (current.empty()) {
            fLine = input.currentLine();
            fCol  = input.currentColumn();
        }
        current.push_back(input.get());
        for (const auto& escape : escapeSequences) {
            const size_t i = current.find(escape.first);
            if (i != std::string::npos) {
                current.replace(i, escape.first.size(), std::string(1, escape.second));
            }
        }

        for (const auto& matcher : matchers) {
            std::smatch result;
            if (std::regex_match(current, result, matcher.second.first)) {
                bool cont = false;
                for (int i = 1; i <= 3; ++i) {
                    const std::string temp = current + input.peekN(i);
                    if (std::regex_match(temp, matcher.second.first)) {
                        cont = true;
                        break;
                    }
                }
                if (cont) continue;

                auto a = ambiguous.find(matcher.first);
                if (a != ambiguous.end()) {
                    bool skip = false;
                    for (const std::string& k : a->second) {
                        const std::string check =
                            current + input.peekN(k.size() - current.size());
                        const std::regex& m = matchers.find(k)->second.first;
                        if (std::regex_match(check, m)) {
                            skip = true;
                            break;
                        }
                    }
                    if (skip) continue;
                }

                Node::Ptr token(new Node());
                if (result.size() > 1)
                    token->data = result[1].str();
                else
                    token->data = result[0].str();
                token->sourceLine   = input.currentLine();
                token->sourceColumn = input.currentColumn();
                token->type         = matcher.second.second;

                auto kiter = kwords.find(token->type);
                if (kiter != kwords.end()) {
                    for (const auto& t : kiter->second) {
                        if (t.first == token->data) {
                            token->type = t.second;
                            break;
                        }
                    }
                }

                tokens.push_back(token);
                current.clear();
                break;
            }
        }
    }

    if (!current.empty()) {
        std::cerr << "Unexpected character '" << current[0] << "' on line " << fLine
                  << " at position " << fCol << std::endl;
        return {};
    }
    return tokens;
}

void Tokenizer::recomputeAmbiguous() {
    ambiguous.clear();
    for (const auto& check : matchers) {
        for (const auto& sub : matchers) {
            if (sub.first == check.first) continue;
            if (check.first.find(sub.first) != std::string::npos)
                ambiguous[sub.first].push_back(check.first);
        }
    }
}

} // namespace parser
} // namespace bl