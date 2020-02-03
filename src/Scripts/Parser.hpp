#ifndef BLIB_SCRIPTS_PARSER_HPP
#define BLIB_SCRIPTS_PARSER_HPP

#include <BLIB/Parser.hpp>

namespace bl
{
namespace scripts
{
/**
 * @brief Implementation of a Parser for the scripting language
 * @ingroup Scripts
 *
 */
struct Parser {
    /**
     * @brief Parses the input script into a parse tree
     *
     * @param input The script to parse
     * @return parser::Node::Ptr The root node, or nullptr on error
     */
    static parser::Node::Ptr parse(const std::string& input);

    /**
     * @brief Builds and returns the tokenizer for scripts
     *
     * @return parser::Tokenizer
     */
    static const parser::Tokenizer& getTokenizer();

    /**
     * @brief Builds and returns the Grammar for scripts
     *
     * @return parser::Grammar An uncompiled grammar w/o root node set
     */
    static parser::Grammar getGrammar();

    enum Grammar : parser::Node::Type {
        // Terminals
        NumLit,    // -123.5
        StringLit, // "String"
        Def,       // def
        If,        // if
        While,     // while
        Return,    // return
        And,       // and
        Or,        // or
        Not,       // not
        LParen,    // (
        RParen,    // )
        LBrkt,     // [
        RBrkt,     // ]
        LBrc,      // {
        RBrc,      // }
        Assign,    // =
        Eq,        // ==
        Ne,        // !=
        Gt,        // >
        Ge,        // >=
        Lt,        // <
        Le,        // >=
        Amp,       // &
        Dot,       // .
        Plus,      // +
        Minus,     // -
        Mult,      // *
        Div,       // /
        Hat,       // ^
        Comma,     // ,
        Term,      // ;
        Id,        // varname

        // Non-Terminals

        // Writeables
        ArrayDef, // LBrc RBrc. LRrc ValueList RBrc
        Property, // Id Dot Id
        ArrayAcc, // RValue LBrkt Value RBrkt
        RValue,   // Id. ArrayAcc. Property

        // Arithmetic
        TValue,  // RValue. NumLit. StringLit. Call. PGroup
        Exp,     // TValue. TValue Hat Value
        Product, // Exp. Product Mult Product. Product Div Product
        Sum,     // Product. Sum Plus Sum. Sum Minus Sum

        // Comparisons
        Cmp, // Sum. Cmp [Eq, Ne, Gt, Ge, Le, Lt] Cmp

        // Boolean Logic and top Value
        Negation, // Cmp. Not Negation
        AndGrp,   // Negation. AndGrp And AndGrp
        OrGrp,    // AndGrp. OrGrp Or OrGrp
        PGroup,   // LParen Value RParen
        Value,    // OrGrp

        // Assignment
        Ref,        // Amp RValue
        Assignment, // RValue Assign Value. RValue Assign Ref

        // Function Call
        ValueList, // Value. ValueList Comma Value
        Call,      // Id LParen ValueList RParen. Id LParen RParen

        // Conditional and Loop
        CondHead,    // If PGroup
        Conditional, // CondHead Statement. CondHead StmtBlock
        LoopHead,    // While PGroup
        Loop,        // LoopHead Statement. LoopHead StmtBlock

        // Statements
        Statement, // Return Value Term. Call Term. Conditional. Loop. Assignment Term. FDef
        StmtList,  // Statement. StmtList Statement
        StmtBlock, // LBrc StmtList RBrc

        // Function Definition
        Param,     // Comma Id
        ParamList, // Id Param. ParamList Param
        FName,     // Def Id
        FHead, // FName LParen ParamList RParen. FName LParen Id RParen. FName LParen RParen
        Fdef,  // FHead StmtList

        // Program
        Program // StmtList
    };
};

} // namespace scripts
} // namespace bl

#endif