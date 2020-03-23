#include <BLIB/Parser/Grammar.hpp>

#include <iostream>
#include <map>

namespace bl
{
namespace parser
{
void Grammar::setStart(Node::Type s) {
    if (!nonterminal(s)) {
        std::cout << "Invalid start symbol " << s << std::endl;
        return;
    }
    start = s;
}

Node::Type Grammar::getStart() const { return start; }

bool Grammar::addRule(Node::Type result, const Node::Sequence& sequence) {
    if (!nonterminal(result)) {
        std::cout << "Tried to add production to non nonterminal " << result << std::endl;
        return false;
    }
    for (Node::Type t : sequence) {
        if (!nonterminal(t) && !terminal(t)) {
            std::cout << "Unrecognized node type in production: " << t << std::endl;
            return false;
        }
    }
    productions.push_back({result, sequence});
}

bool Grammar::addRule(Node::Type result, Node::Type node) {
    const Node::Sequence sequence(1, node);
    addRule(result, sequence);
}

Grammar::ItemSet Grammar::closure(const Grammar::Item& item) const {
    ItemSet result;
    result.add(item);
    if (item.cursor >= item.production.set.size()) return result;

    if (nonterminal(item.production.set[item.cursor])) {
        const std::list<Production> subprods = producing(item.production.set[item.cursor]);
        for (const Production& prod : subprods) {
            const ItemSet subclosure = closure(Item(prod));
            for (const Item& item : subclosure.items()) {
                if (!result.contains(item)) result.add(item);
            }
        }
    }
    return result;
}

Grammar::ItemSet Grammar::closure(const Grammar::ItemSet& set) const {
    ItemSet result;
    for (const Item& item : set.items()) {
        ItemSet c = closure(item);
        for (const Item& i : c.items()) result.add(i);
    }
    return result;
}

std::list<Grammar::Production> Grammar::producing(Node::Type t) const {
    std::list<Production> result;
    for (const Production& prod : productions) {
        if (prod.result == t) result.push_back(prod);
    }
    return result;
}

bool Grammar::terminal(Node::Type t) const { return terminals.find(t) != terminals.end(); }

bool Grammar::nonterminal(Node::Type t) const {
    return nonterminals.find(t) != nonterminals.end();
}

Node::Sequence Grammar::followSet(Node::Type t) const {
    Node::Sequence result;
    for (const Production& p : productions) {
        for (unsigned int i = 0; i < p.set.size(); ++i) {
            if (p.set[i] == t) {
                // Add follow set of the following symbol
                if (i < p.set.size() - 1) {
                    Node::Sequence pt = deepFollow(p.set[i + 1]);
                    result.insert(result.begin(), pt.begin(), pt.end());
                }
                // Add follow set of the resulting nonterminal
                else {
                    Node::Sequence pt = deepFollow(p.result);
                    result.insert(result.begin(), pt.begin(), pt.end());
                }
            }
        }
    }
    if (t == start) result.push_back(Node::EOI);

    // dedup
    Node::Sequence ret;
    for (Node::Type node : result) {
        if (std::find(ret.begin(), ret.end(), node) == ret.end()) ret.push_back(node);
    }
    return ret;
}

Node::Sequence Grammar::deepFollow(Node::Type t) const {
    if (terminal(t)) return {t};

    Node::Sequence result;
    for (const Production& p : productions) {
        if (p.result == t) {
            Node::Sequence s = deepFollow(p.set[0]);
            result.insert(result.begin(), s.begin(), s.end());
        }
    }
    return result;
}

bool Grammar::ItemSet::contains(const Grammar::Item& item) const {
    return std::find(set.begin(), set.end(), item) != set.end();
}

void Grammar::ItemSet::add(const Grammar::Item& item) {
    if (!contains(item)) set.push_back(item);
}

const std::list<Grammar::Item>& Grammar::ItemSet::items() const { return set; }

bool Grammar::ItemSet::operator==(const Grammar::ItemSet& rhs) const {
    if (set.size() != rhs.set.size()) return false;
    std::list<Item> check = set;
    for (const Item& item : rhs.set) {
        auto i = std::find(check.begin(), check.end(), item);
        if (i != check.end()) check.erase(i);
    }
    return check.empty();
}

bool Grammar::Production::operator==(const Grammar::Production& rhs) const {
    if (result != rhs.result) return false;
    if (set.size() != rhs.set.size()) return false;
    for (unsigned int i = 0; i < set.size(); ++i) {
        if (set[i] != rhs.set[i]) return false;
    }
    return true;
}

Grammar::Item::Item(const Grammar::Production& prod)
: Item(prod, 0) {}

Grammar::Item::Item(const Grammar::Production& prod, int c)
: production(prod)
, cursor(c) {}

bool Grammar::Item::operator==(const Item& rhs) const {
    if (cursor != rhs.cursor) return false;
    return production == rhs.production;
}

bool Grammar::Item::final() const { return cursor == production.set.size(); }

Grammar::Item Grammar::Item::next() const { return Item(production, cursor + 1); }

} // namespace parser

} // namespace bl