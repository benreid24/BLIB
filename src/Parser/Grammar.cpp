#include <BLIB/Parser/Grammar.hpp>

#include <iostream>
#include <map>

namespace bl
{
namespace parser
{
namespace
{
std::vector<Node::Sequence> genSubSequences(const Node::Sequence& sequence) {
    if (sequence.empty()) return {};

    std::vector<Node::Sequence> sequences;
    Node::Sequence current;
    for (Node::Type t : sequence) {
        current.push_back(t);
        sequences.push_back(current);
    }
    return sequences;
}
} // namespace
void Grammar::setStart(Node::Type s) { start = s; }

Node::Type Grammar::getStart() const { return start; }

void Grammar::addRule(Node::Type result, const Node::Sequence& sequence) {
    rules.push_back(std::make_pair(result, sequence));
}

void Grammar::addRule(Node::Type result, Node::Type node) {
    const Node::Sequence sequence(1, node);
    addRule(result, sequence);
}

Grammar::Reduction Grammar::reductionLookup(const Node::Sequence& sequence) const {
    auto i = reductionTable.find(sequence);
    if (i != reductionTable.end()) return i->second;
    return {Node::None, 0};
}

bool Grammar::compile() {
    reductionTable.clear();
    const Reduction defaultReduction = {Node::None, 0};

    for (const auto& rule : rules) {
        const std::vector<Node::Sequence> subkeys = genSubSequences(rule.second);
        for (const auto& key : subkeys) {
            auto i = reductionTable.find(key);
            if (i == reductionTable.end())
                i = reductionTable.insert(std::make_pair(key, defaultReduction)).first;
            i->second.reductionsPossible += 1;
            if (key == rule.second) {
                if (i->second.result != Node::None) {
                    std::cerr << "Error: Grammar is ambiguous. Rule " << i->second.result
                              << " and rule " << rule.first
                              << " are generated by the same token sequence" << std::endl;
                    reductionTable.clear();
                    return false;
                }
                i->second.result = rule.first;
            }
        }
    }

    return true;
}

} // namespace parser

} // namespace bl