#include <BLIB/Parser/CommentSkipper.hpp>

namespace bl
{
namespace parser
{
CommentSkipper::CommentSkipper(const std::string& lineCommentOpen,
                               const std::string& blockCommentOpen,
                               const std::string& blockCommentClose)
: lineOpen(lineCommentOpen)
, blockOpen(blockCommentOpen)
, blockClose(blockCommentClose) {}

ISkipper::Ptr CommentSkipper::create(const std::string& lineCommentOpen,
                                     const std::string& blockCommentOpen,
                                     const std::string& blockCommentClose) {
    return ISkipper::Ptr(
        new CommentSkipper(lineCommentOpen, blockCommentOpen, blockCommentClose));
}

void CommentSkipper::skip(Stream& input) const {
    WhitespaceSkipper::skip(input);
    while (input.peekN(lineOpen.size()) == lineOpen ||
           input.peekN(blockOpen.size()) == blockOpen) {
        while (input.peekN(lineOpen.size()) == lineOpen) {
            input.getN(lineOpen.size());
            while (input.peek() != '\n') {
                if (input.peek() == EOF) return;
                input.get();
            }
            WhitespaceSkipper::skip(input);
        }
        while (input.peekN(blockOpen.size()) == blockOpen) {
            input.getN(blockOpen.size());
            while (input.peekN(blockClose.size()) != blockClose) {
                if (input.peek() == EOF) return;
                input.get();
            }
            input.getN(blockClose.size());
            WhitespaceSkipper::skip(input);
        }
    }
}

} // namespace parser
} // namespace bl