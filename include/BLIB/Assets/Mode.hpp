#ifndef BLIB_ASSETS_MODE_HPP
#define BLIB_ASSETS_MODE_HPP

namespace bl
{
namespace as
{
/**
 * @brief Represents the mode the asset system is running in
 *
 * @ingroup Assets
 */
enum struct Mode {
    /// Assets are stored in source control friendly formats and loaded directly from files
    Editor,

    /// Assets are stored in optimized formats and loaded from optimized bundles
    Game,

    /// Special mode for bundle creation. Same as Game but used when writing
    BundleCreation = Game
};

} // namespace as
} // namespace bl

#endif
