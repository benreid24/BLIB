#include <BLIB/Parser/Parser.hpp>

#include <Scripts/Parser.hpp>
#include <iostream>

std::string toStr(bl::parser::Node::Type t) {
    using T = bl::scripts::Parser::Grammar;
    switch (t) {
    case T::NumLit:
        return "NumLit";
    case T::TValue:
        return "TValue";
    case T::RValue:
        return "RValue";
    case T::Exp:
        return "Exp";
    case T::Hat:
        return "Hat";
    case T::Plus:
        return "Plus";
    case T::Minus:
        return "Minus";
    case T::Mult:
        return "Mult";
    case T::Div:
        return "Div";
    case T::LParen:
        return "LParen";
    case T::RParen:
        return "RParen";
    case T::Id:
        return "Id";
    case T::Product:
        return "Product";
    case T::Sum:
        return "Sum";
    case T::Cmp:
        return "Cmp";
    case T::Negation:
        return "Negation";
    case T::AndGrp:
        return "AndGrp";
    case T::OrGrp:
        return "OrGrp";
    case T::PGroup:
        return "PGroup";
    case T::Value:
        return "Value";
    case T::ValueList:
        return "ValueList";
    case T::ArgList:
        return "ArgList";
    case T::Call:
        return "Call";
    case T::Comma:
        return "Comma";
    default:
        return std::to_string(t);
    }
}

namespace bl
{
using namespace parser;
namespace
{
std::ostream& error(Node::Ptr node) {
    std::cerr << "Error: Line " << node->sourceLine << " column " << node->sourceColumn
              << ": ";
    return std::cerr;
}

std::ostream& operator<<(std::ostream& s, const Node::Sequence& nodes) {
    s << "[";
    if (!nodes.empty()) s << toStr(nodes[0]);
    for (unsigned int i = 1; i < nodes.size(); ++i) { s << ", " << toStr(nodes[i]); }
    s << "]";
    return s;
}

Node::Sequence genKey(const std::vector<Node::Ptr>& nodes) {
    Node::Sequence key;
    key.reserve(nodes.size());
    for (const Node::Ptr& node : nodes) key.push_back(node->type);
    return key;
}

void doReduction(std::vector<Node::Ptr>& stack, unsigned int i, unsigned int len,
                 Node::Type type) {
    Node::Ptr node(new Node());
    node->type = type;
    node->children.reserve(len);
    for (unsigned int j = i; j < i + len; ++j) {
        node->children.push_back(stack[j]);
        node->data += stack[j]->data;
        stack[j]->parent = node;
    }
    node->sourceLine   = stack[i]->sourceLine;
    node->sourceColumn = stack[i]->sourceColumn;
    stack.erase(stack.begin() + i, stack.begin() + i + len);
    stack.insert(stack.begin() + i, node);
}

} // namespace

Parser::Parser(const Grammar& grammar, const Tokenizer& tokenizer)
: grammar(grammar)
, tokenizer(tokenizer) {}

Node::Ptr Parser::parse(const std::string& input) const {
    Stream stream(input);
    return parse(stream);
}

Node::Ptr Parser::parse(Stream& input) const {
    std::vector<Node::Ptr> parsed = tokenizer.tokenize(input);
    if (parsed.empty()) return nullptr;
    std::list<Node::Ptr> tokens(parsed.begin(), parsed.end());

    std::vector<Node::Ptr> stack;
    stack.push_back(tokens.front());
    tokens.pop_front();

    bool unchanged = false;
    while (!tokens.empty() || !unchanged) {
        bool r = false;
        while (tryReduction(stack, !tokens.empty(), false)) { r = true; }
        if (!r) {
            if (unchanged) {
                tryReduction(stack, true, true);
                unchanged = false;
            }
            else
                unchanged = true;
        }
        else
            unchanged = false;
        if (!tokens.empty()) {
            stack.push_back(tokens.front());
            tokens.pop_front();
        }
    }
    while (tryReduction(stack, false, false)) {}
    while (tryReduction(stack, false, true)) {}

    if (stack.size() > 1) {
        if (stack.size() > 2) {
            for (unsigned int l = 0; l < stack.size(); ++l) {
                unsigned int len = stack.size() - l;
                std::vector<Node::Ptr> substr;
                substr.reserve(len);
                for (unsigned int s = 0; s <= stack.size() - len; ++s) {
                    substr.clear();
                    for (unsigned int i = s; i < stack.size() && i < s + len; ++i) {
                        substr.push_back(stack[i]);
                    }
                    Node::Sequence key   = genKey(substr);
                    Grammar::Reduction r = grammar.reductionLookup(key);
                    if (r.reductionsPossible > 0 && s + len < stack.size()) {
                        key.push_back(stack[s + len]->type);
                        r = grammar.reductionLookup(key);
                        if (r.reductionsPossible == 0) {
                            error(stack[s + len])
                                << "Unexpected symbol '" << stack[s + len]->data << "'\n";
                            goto errorPrinted;
                        }
                    }
                }
            }
        errorPrinted:;
        }
        else
            error(stack[1]) << "Unexpected symbol '" << stack[1]->data << "'\n";
        return nullptr;
    }
    if (stack.empty()) {
        std::cerr << "Internal parser error: No tree\n";
        return nullptr;
    }

    Node::Ptr root = stack[0];
    if (root->type != grammar.getStart()) {
        error(root) << "Input is malformed. Expected start node " << grammar.getStart()
                    << " got " << root->type << std::endl;
        return nullptr;
    }
    return root;
}

bool Parser::tryReduction(std::vector<Node::Ptr>& stack, bool inputRemaining,
                          bool unchanged) const {
    unsigned int nextStart = 0;

    std::cout << "Stack: " << genKey(stack) << std::endl;

    for (unsigned int l = 0; l < stack.size(); ++l) {
        const unsigned int len = stack.size() - l;
        std::vector<Node::Ptr> substr;
        substr.reserve(len);
        unsigned int s = nextStart;
        for (nextStart = 0; s <= stack.size() - len; ++s) {
            substr.clear();
            for (unsigned int i = s; i < stack.size() && i < s + len; ++i) {
                substr.push_back(stack[i]);
            }
            const Node::Sequence key = genKey(substr);
            Grammar::Reduction r     = grammar.reductionLookup(key);
            if (r.result != Node::None) {
                if (r.reductionsPossible == 1) {
                    std::cout << "Reducing[#r=1] " << key << " -> " << toStr(r.result)
                              << std::endl;
                    doReduction(stack, s, len, r.result);
                    return true;
                }
                else if (unchanged && stack.size() - s - len >= grammar.longestRule()) {
                    std::cout << "Reducing[sgre] " << key << " -> " << toStr(r.result)
                              << std::endl;
                    doReduction(stack, s, len, r.result);
                    return true;
                }
                else if (!inputRemaining && (s + len >= stack.size())) {
                    std::cout << "Reducing[endi] " << key << " -> " << toStr(r.result)
                              << std::endl;
                    doReduction(stack, s, len, r.result);
                    return true;
                }
                else if (s + len < stack.size()) {
                    std::vector<Node::Ptr> next = {stack[s + len]};
                    substr.push_back(stack[s + len]);
                    Grammar::Reduction tr = grammar.reductionLookup(genKey(substr));
                    if (tr.reductionsPossible == 0) {
                        tr = grammar.reductionLookup(genKey(next));
                        if (tr.reductionsPossible == 0) {
                            std::cout << "Reducing[ekey] " << key << " -> " << toStr(r.result)
                                      << std::endl;
                            doReduction(stack, s, len, r.result);
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

} // namespace bl