#include <Scripts/Parser.hpp>
#include <cassert>

namespace bg
{
namespace scripts
{
namespace
{
parser::ISkipper::Ptr getskipper() { return parser::CommentSkipper::create("//", "/*", "*/"); }

parser::Tokenizer buildTokenizer() {
    using G = Parser::Grammar;
    parser::Tokenizer tokenizer(getskipper());

    tokenizer.addSkipperToggleChar('"');
    tokenizer.addEscapeSequence("\\n", '\n');

    tokenizer.addTokenType(G::NumLit, "([0-9]+(\\.[0-9]+)?)");
    tokenizer.addTokenType(G::StringLit, "\"([^\"]*)\"");
    tokenizer.addTokenType(G::LParen, "\\(");
    tokenizer.addTokenType(G::RParen, "\\)");
    tokenizer.addTokenType(G::LBrkt, "\\[");
    tokenizer.addTokenType(G::RBrkt, "\\]");
    tokenizer.addTokenType(G::LBrc, "\\{");
    tokenizer.addTokenType(G::RBrc, "\\}");
    tokenizer.addTokenType(G::Assign, "=");
    tokenizer.addTokenType(G::Eq, "==");
    tokenizer.addTokenType(G::Ne, "!=");
    tokenizer.addTokenType(G::Gt, ">");
    tokenizer.addTokenType(G::Ge, ">=");
    tokenizer.addTokenType(G::Lt, "<");
    tokenizer.addTokenType(G::Le, "<=");
    tokenizer.addTokenType(G::Amp, "&");
    tokenizer.addTokenType(G::Dot, "\\.");
    tokenizer.addTokenType(G::Plus, "\\+");
    tokenizer.addTokenType(G::Minus, "\\-");
    tokenizer.addTokenType(G::Mult, "\\*");
    tokenizer.addTokenType(G::Div, "/");
    tokenizer.addTokenType(G::Hat, "\\^");
    tokenizer.addTokenType(G::Comma, ",");
    tokenizer.addTokenType(G::Term, ";");
    tokenizer.addTokenType(G::Colon, ":");
    tokenizer.addTokenType(G::Id, "[a-zA-Z]+[a-zA-Z0-9]*");
    tokenizer.addKeyword(G::Id, G::Def, "def");
    tokenizer.addKeyword(G::Id, G::If, "if");
    tokenizer.addKeyword(G::Id, G::Elif, "elif");
    tokenizer.addKeyword(G::Id, G::Else, "else");
    tokenizer.addKeyword(G::Id, G::While, "while");
    tokenizer.addKeyword(G::Id, G::Return, "return");
    tokenizer.addKeyword(G::Id, G::And, "and");
    tokenizer.addKeyword(G::Id, G::Or, "or");
    tokenizer.addKeyword(G::Id, G::Not, "not");
    tokenizer.addKeyword(G::Id, G::True, "true");
    tokenizer.addKeyword(G::Id, G::False, "false");
    tokenizer.addKeyword(G::Id, G::For, "for");
    tokenizer.addKeyword(G::Id, G::In, "in");

    return tokenizer;
}

parser::Grammar buildGrammar() {
    using G = Parser::Grammar;

    parser::Grammar grammar;

    grammar.addTerminal(G::NumLit);
    grammar.addTerminal(G::StringLit);
    grammar.addTerminal(G::True);
    grammar.addTerminal(G::False);
    grammar.addTerminal(G::Def);
    grammar.addTerminal(G::If);
    grammar.addTerminal(G::Elif);
    grammar.addTerminal(G::Else);
    grammar.addTerminal(G::While);
    grammar.addTerminal(G::For);
    grammar.addTerminal(G::In);
    grammar.addTerminal(G::Return);
    grammar.addTerminal(G::And);
    grammar.addTerminal(G::Or);
    grammar.addTerminal(G::Not);
    grammar.addTerminal(G::LParen);
    grammar.addTerminal(G::RParen);
    grammar.addTerminal(G::LBrkt);
    grammar.addTerminal(G::RBrkt);
    grammar.addTerminal(G::LBrc);
    grammar.addTerminal(G::RBrc);
    grammar.addTerminal(G::Assign);
    grammar.addTerminal(G::Eq);
    grammar.addTerminal(G::Ne);
    grammar.addTerminal(G::Gt);
    grammar.addTerminal(G::Ge);
    grammar.addTerminal(G::Lt);
    grammar.addTerminal(G::Le);
    grammar.addTerminal(G::Amp);
    grammar.addTerminal(G::Dot);
    grammar.addTerminal(G::Plus);
    grammar.addTerminal(G::Minus);
    grammar.addTerminal(G::Mult);
    grammar.addTerminal(G::Div);
    grammar.addTerminal(G::Hat);
    grammar.addTerminal(G::Comma);
    grammar.addTerminal(G::Term);
    grammar.addTerminal(G::Colon);
    grammar.addTerminal(G::Id);

    grammar.addNonTerminal(G::ArrayDef);
    grammar.addNonTerminal(G::Property);
    grammar.addNonTerminal(G::ArrayAcc);
    grammar.addNonTerminal(G::RValue);
    grammar.addNonTerminal(G::UNeg);
    grammar.addNonTerminal(G::TValue);
    grammar.addNonTerminal(G::Exp);
    grammar.addNonTerminal(G::Product);
    grammar.addNonTerminal(G::Sum);
    grammar.addNonTerminal(G::Cmp);
    grammar.addNonTerminal(G::Negation);
    grammar.addNonTerminal(G::AndGrp);
    grammar.addNonTerminal(G::OrGrp);
    grammar.addNonTerminal(G::PGroup);
    grammar.addNonTerminal(G::Value);
    grammar.addNonTerminal(G::Ref);
    grammar.addNonTerminal(G::Assignment);
    grammar.addNonTerminal(G::ValueList);
    grammar.addNonTerminal(G::ArgList);
    grammar.addNonTerminal(G::Call);
    grammar.addNonTerminal(G::IfHead);
    grammar.addNonTerminal(G::ElifHead);
    grammar.addNonTerminal(G::IfBlock);
    grammar.addNonTerminal(G::ElifBlock);
    grammar.addNonTerminal(G::ElseBlock);
    grammar.addNonTerminal(G::ElifChain);
    grammar.addNonTerminal(G::ElseCond);
    grammar.addNonTerminal(G::Conditional);
    grammar.addNonTerminal(G::LoopHead);
    grammar.addNonTerminal(G::Loop);
    grammar.addNonTerminal(G::ForHead);
    grammar.addNonTerminal(G::ForLoop);
    grammar.addNonTerminal(G::Ret);
    grammar.addNonTerminal(G::Statement);
    grammar.addNonTerminal(G::StmtList);
    grammar.addNonTerminal(G::StmtBlock);
    grammar.addNonTerminal(G::ParamList);
    grammar.addNonTerminal(G::FName);
    grammar.addNonTerminal(G::FHead);
    grammar.addNonTerminal(G::FDef);
    grammar.addNonTerminal(G::Program);

    // Writeables
    grammar.addRule(G::ArrayDef, {G::LBrkt, G::RBrkt});
    grammar.addRule(G::ArrayDef, {G::LBrkt, G::ValueList, G::RBrkt});
    grammar.addRule(G::ArrayAcc, {G::RValue, G::LBrkt, G::Value, G::RBrkt});
    grammar.addRule(G::Property, {G::RValue, G::Dot, G::Id});
    grammar.addRule(G::RValue, G::Id);
    grammar.addRule(G::RValue, G::ArrayAcc);
    grammar.addRule(G::RValue, G::Property);

    // Arithmetic
    grammar.addRule(G::PGroup, {G::LParen, G::Value, G::RParen});
    grammar.addRule(G::UNeg, {G::Minus, G::TValue});
    grammar.addRule(G::TValue, G::UNeg);
    grammar.addRule(G::TValue, G::PGroup);
    grammar.addRule(G::TValue, G::RValue);
    grammar.addRule(G::TValue, G::NumLit);
    grammar.addRule(G::TValue, G::StringLit);
    grammar.addRule(G::TValue, G::Call);
    grammar.addRule(G::TValue, G::ArrayDef);
    grammar.addRule(G::TValue, G::True);
    grammar.addRule(G::TValue, G::False);
    grammar.addRule(G::Exp, G::TValue);
    grammar.addRule(G::Exp, {G::Exp, G::Hat, G::TValue});
    grammar.addRule(G::Product, G::Exp);
    grammar.addRule(G::Product, {G::Product, G::Mult, G::Exp});
    grammar.addRule(G::Product, {G::Product, G::Div, G::Exp});
    grammar.addRule(G::Sum, G::Product);
    grammar.addRule(G::Sum, {G::Sum, G::Plus, G::Product});
    grammar.addRule(G::Sum, {G::Sum, G::Minus, G::Product});

    // Comparisons
    grammar.addRule(G::Cmp, G::Sum);
    grammar.addRule(G::Cmp, {G::Sum, G::Eq, G::Sum});
    grammar.addRule(G::Cmp, {G::Sum, G::Ne, G::Sum});
    grammar.addRule(G::Cmp, {G::Sum, G::Gt, G::Sum});
    grammar.addRule(G::Cmp, {G::Sum, G::Ge, G::Sum});
    grammar.addRule(G::Cmp, {G::Sum, G::Lt, G::Sum});
    grammar.addRule(G::Cmp, {G::Sum, G::Le, G::Sum});

    // Boolean Logic
    grammar.addRule(G::Negation, G::Cmp);
    grammar.addRule(G::Negation, {G::Not, G::Negation});
    grammar.addRule(G::AndGrp, G::Negation);
    grammar.addRule(G::AndGrp, {G::AndGrp, G::And, G::Negation});
    grammar.addRule(G::OrGrp, G::AndGrp);
    grammar.addRule(G::OrGrp, {G::OrGrp, G::Or, G::AndGrp});

    // Value
    grammar.addRule(G::Value, G::OrGrp);

    // Assignment
    grammar.addRule(G::Ref, {G::Amp, G::RValue});
    grammar.addRule(G::Assignment, {G::RValue, G::Assign, G::Value, G::Term});
    grammar.addRule(G::Assignment, {G::RValue, G::Assign, G::Ref, G::Term});

    // Function call
    grammar.addRule(G::ValueList, G::Value);
    grammar.addRule(G::ValueList, {G::ValueList, G::Comma, G::Value});
    grammar.addRule(G::ArgList, {G::LParen, G::ValueList, G::RParen});
    grammar.addRule(G::Call, {G::RValue, G::ArgList});
    grammar.addRule(G::Call, {G::RValue, G::LParen, G::RParen});

    // Conditional
    grammar.addRule(G::IfHead, {G::If, G::PGroup});
    grammar.addRule(G::ElifHead, {G::Elif, G::PGroup});
    grammar.addRule(G::IfBlock, {G::IfHead, G::Statement});
    grammar.addRule(G::IfBlock, {G::IfHead, G::StmtBlock});
    grammar.addRule(G::ElifBlock, {G::ElifHead, G::Statement});
    grammar.addRule(G::ElifBlock, {G::ElifHead, G::StmtBlock});
    grammar.addRule(G::ElseBlock, {G::Else, G::Statement});
    grammar.addRule(G::ElseBlock, {G::Else, G::StmtBlock});
    grammar.addRule(G::ElifChain, G::IfBlock);
    grammar.addRule(G::ElifChain, {G::ElifChain, G::ElifBlock});
    grammar.addRule(G::ElseCond, {G::ElifChain, G::ElseBlock});
    grammar.addRule(G::Conditional, G::ElifChain);
    grammar.addRule(G::Conditional, G::ElseCond);

    // Loop
    grammar.addRule(G::LoopHead, {G::While, G::PGroup});
    grammar.addRule(G::Loop, {G::LoopHead, G::Statement});
    grammar.addRule(G::Loop, {G::LoopHead, G::StmtBlock});
    grammar.addRule(G::ForHead, {G::For, G::LParen, G::Id, G::In, G::Value, G::RParen});
    grammar.addRule(G::ForLoop, {G::ForHead, G::Statement});
    grammar.addRule(G::ForLoop, {G::ForHead, G::StmtBlock});

    // Statements
    grammar.addRule(G::Ret, {G::Return, G::Term});
    grammar.addRule(G::Ret, {G::Return, G::Value, G::Term});
    grammar.addRule(G::Statement, G::Ret);
    grammar.addRule(G::Statement, {G::Call, G::Term});
    grammar.addRule(G::Statement, G::Assignment);
    grammar.addRule(G::Statement, G::Conditional);
    grammar.addRule(G::Statement, G::Loop);
    grammar.addRule(G::Statement, G::ForLoop);
    grammar.addRule(G::Statement, G::FDef);
    grammar.addRule(G::StmtList, G::Statement);
    grammar.addRule(G::StmtList, {G::StmtList, G::Statement});
    grammar.addRule(G::StmtBlock, {G::LBrc, G::StmtList, G::RBrc});

    // Function definition
    grammar.addRule(G::ParamList, {G::Id, G::Comma, G::Id});
    grammar.addRule(G::ParamList, {G::ParamList, G::Comma, G::Id});
    grammar.addRule(G::FName, {G::Def, G::Id});
    grammar.addRule(G::FHead, {G::FName, G::LParen, G::RParen});
    grammar.addRule(G::FHead, {G::FName, G::LParen, G::Id, G::RParen});
    grammar.addRule(G::FHead, {G::FName, G::LParen, G::ParamList, G::RParen});
    grammar.addRule(G::FDef, {G::FHead, G::StmtBlock});

    // Program
    grammar.addRule(G::Program, G::StmtList);

    return grammar;
}

bg::Parser buildParser() {
    parser::Grammar grammar = buildGrammar();
    grammar.setStart(Parser::Grammar::Program);
    return bg::Parser(grammar, Parser::getTokenizer());
}

} // namespace

const parser::Tokenizer& Parser::getTokenizer() {
    static const parser::Tokenizer tokenizer = buildTokenizer();
    return tokenizer;
}

parser::Grammar Parser::getGrammar() {
    static const parser::Grammar grammar = buildGrammar();
    return grammar;
}

parser::Node::Ptr Parser::parse(const std::string& input) {
    static const bg::Parser parser = buildParser();
    return parser.parse(input);
}

} // namespace scripts
} // namespace bg