#ifndef BLIB_ASSETS_BUILTIN_MODELPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_MODELPAYLOAD_HPP

#include <BLIB/Assets/PayloadGeneric.hpp>
#include <BLIB/Models/Model.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Payload for a model asset
 *
 * @ingroup Assets
 */
using ModelPayload = as::PayloadGeneric<mdl::Model>;

} // namespace asi
} // namespace bl

#endif
