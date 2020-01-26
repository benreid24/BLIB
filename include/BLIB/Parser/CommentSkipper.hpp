#ifndef BLIB_PARSER_COMMENTSKIPPER_HPP
#define BLIB_PARSER_COMMENTSKIPPER_HPP

#include <BLIB/Parser/WhitespaceSkipper.hpp>

namespace bl
{
namespace parser
{
/**
 * @brief Basic comment and whitespace skipper for more common use cases
 * @ingroup parser
 *
 */
class CommentSkipper : public WhitespaceSkipper {
public:
    /**
     * @brief Construct a new Comment Skipper object. Leave arguments blank to ignore
     *
     * @param lineCommentOpen String that identifies the opening of a line comment
     * @param blockCommentOpen String that opens a block comment
     * @param blockCommentClose String that closes a block comment
     */
    CommentSkipper(const std::string& lineCommentOpen, const std::string& blockCommentOpen,
                   const std::string& blockCommentClose);

    /**
     * @brief Helper function to create a CommentSkipper wrapped in an ISkipper::Ptr
     * @see CommentSkiper()
     */
    static ISkipper::Ptr create(const std::string& lineCommentOpen,
                                const std::string& blockCommentOpen,
                                const std::string& blockCommentClose);

    /**
     * @brief Skips whitespace and comments
     *
     * @param stream The stream to update
     */
    virtual void skip(Stream& stream) const override;

private:
    const std::string lineOpen;
    const std::string blockOpen;
    const std::string blockClose;
};
} // namespace parser
} // namespace bl

#endif