#ifndef BLIB_PARSER_ISKIPPER_HPP
#define BLIB_PARSER_ISKIPPER_HPP

#include <BENG/Parser/Stream.hpp>
#include <memory>

namespace bg
{
namespace parser
{
/**
 * @brief Interface for custom parsers to skip past unwanted characters such as whitespace or
 * comments
 * @ingroup Parser
 *
 */
class ISkipper {
public:
    typedef std::shared_ptr<ISkipper> Ptr;

    /**
     * @brief Skip past unwanted characters in the stream
     *
     * @param stream The stream to perform skip operations on
     */
    virtual void skip(Stream& stream) const = 0;
};

} // namespace parser
} // namespace bg

#endif