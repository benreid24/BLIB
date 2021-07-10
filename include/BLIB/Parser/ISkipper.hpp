#ifndef BLIB_PARSER_ISKIPPER_HPP
#define BLIB_PARSER_ISKIPPER_HPP

#include <BLIB/Parser/Stream.hpp>
#include <memory>

namespace bl
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
     * @brief Destroy the ISkipper object
     *
     */
    virtual ~ISkipper() = default;

    /**
     * @brief Skip past unwanted characters in the stream
     *
     * @param stream The stream to perform skip operations on
     */
    virtual void skip(Stream& stream) const = 0;
};

} // namespace parser
} // namespace bl

#endif