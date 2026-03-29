#ifndef BLIB_ASSETS_DRIVERS_IMAGE_HPP
#define BLIB_ASSETS_DRIVERS_IMAGE_HPP

#include <BLIB/Assets/Driver.hpp>
#include <BLIB/Assets/PayloadGeneric.hpp>
#include <SFML/Graphics/Image.hpp>

namespace bl
{
/// Contains built-in payloads and drivers
namespace asi
{
using ImagePayload = as::PayloadGeneric<sf::Image>;

class ImageDriver : public as::Driver<ImagePayload> {
public:
    ImageDriver() = default;

    virtual ~ImageDriver() = default;

    virtual bool doCreate(const as::CreateContext& ctx, ImagePayload& payload) override;

    virtual bool doRead(const as::ReadContext& ctx, ImagePayload& payload) override;

    virtual bool doWrite(const as::WriteContext& ctx, const ImagePayload& payload) override;

private:
    //
};

} // namespace asi
} // namespace bl

#endif
