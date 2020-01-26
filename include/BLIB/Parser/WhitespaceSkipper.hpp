#ifndef BLIB_PARSER_WHITESPACESKIPPER_HPP
#define BLIB_PARSER_WHITESPACESKIPPER_HPP

#include <BLIB/Parser/ISkipper.hpp>
#include <cctype>

namespace bl
{
namespace parser
{
/**
 * @brief Default skipper that skips over all whitespace beginning with the next token
 *        Note that this will not work with any tokens that begin with whitespace
 *
 */
class WhitespaceSkipper : public ISkipper {
public:
    static ISkipper::Ptr create() {
        return ISkipper::Ptr(new WhitespaceSkipper());
    }

    virtual void skip(Stream& stream) const override {
        while (std::isspace(stream.peek())) stream.get();
    }
};

} // namespace parser

} // namespace bl

#endif