#include <BLIB/Parser/Parser.hpp>

#include <iostream>

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

Node::Ptr Parser::parse(Stream& input) const {
    std::vector<Node::Ptr> parsed = tokenizer.tokenize(input);
    if (parsed.empty()) return nullptr;
    std::list<Node::Ptr> tokens(parsed.begin(), parsed.end());

    std::vector<Node::Ptr> stack;
    stack.push_back(tokens.front());
    tokens.pop_front();

    while (!tokens.empty()) {
        while (tryReduction(stack)) {}
        stack.push_back(tokens.front());
        tokens.pop_front();
    }
    while (tryReduction(stack)) {}

    if (stack.size() > 1) {
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

bool Parser::tryReduction(std::vector<Node::Ptr>& stack) const {
    for (unsigned int len = 1; len <= stack.size(); ++len) {
        std::vector<Node::Ptr> substr;
        substr.reserve(len);
        for (unsigned int s = 0; s < stack.size() - len; ++s) {
            substr.clear();
            for (unsigned int i = s; i < stack.size() && i < s + len; ++i) {
                substr.push_back(stack[i]);
            }
            const Node::Sequence key = genKey(substr);
            Grammar::Reduction r     = grammar.reductionLookup(key);
            if (r.result != Node::None) {
                if (r.reductionsPossible == 1) {
                    doReduction(stack, s, len, r.result);
                    return true;
                }
                if (s + len <= stack.size() - 1) {
                    substr.push_back(stack[s + len]);
                    const Node::Sequence nextKey = genKey(substr);
                    const Node::Type t           = r.result;
                    r                            = grammar.reductionLookup(nextKey);
                    if (r.reductionsPossible == 0) {
                        doReduction(stack, s, len, t);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

} // namespace bl