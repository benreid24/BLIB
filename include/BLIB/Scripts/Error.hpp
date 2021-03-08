#ifndef BLIB_SCRIPTS_ERROR_HPP
#define BLIB_SCRIPTS_ERROR_HPP

#include <BLIB/Parser/Node.hpp>
#include <string>

namespace bl
{
namespace script
{
/**
 * @brief Script utility class to represent an error. Can be used to build a call stack for
 *        script errors
 * @ingroup Scripts
 *
 */
class Error {
public:
    /**
     * @brief Constructs a root Error with an empty call stack
     *
     * @param error Error message
     */
    Error(const std::string& error);

    /**
     * @brief Construct a new Error object with the given message and source location
     *
     * @param error Error message
     * @param location Node where the error occured
     */
    Error(const std::string& error, parser::Node::Ptr location);

    /**
     * @brief Construct a new Error object from a message, current location, and existing Error
     * stack
     *
     * @param error Error message
     * @param location Where the error occured
     * @param stack An Error object containing the stack levels below this
     */
    Error(const std::string& error, parser::Node::Ptr location, const Error& stack);

    /**
     * @brief Returns the contained error message
     *
     */
    const std::string& message() const;

    /**
     * @brief Returns the full stack trace, formatted
     *
     */
    const std::string& stacktrace() const;

private:
    const std::string error;
    const std::string stack;
};

/**
 * @brief Special Exception for Scripts to exit without an Error
 * @ingroup Scripts
 *
 */
struct Exit {};

} // namespace script

} // namespace bl

#endif