#ifndef BLIB_MODELS_VISITOR_HPP
#define BLIB_MODELS_VISITOR_HPP

#include <functional>
#include <string>

namespace bl
{
namespace mdl
{
/**
 * @brief Signature of a visitor of model dependency files. Takes in the original file path and
 *        returns a new file path. This can be used to perform processing of dependencies as well as
 *        to rewrite dependency paths if a model is moved
 *
 * @ingroup Models
 */
using DependencyVisitor = std::function<std::string(const std::string&)>;

} // namespace mdl
} // namespace bl

#endif
