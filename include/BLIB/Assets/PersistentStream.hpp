#ifndef BLIB_ASSETS_PERSISTENTSTREAM_HPP
#define BLIB_ASSETS_PERSISTENTSTREAM_HPP

#include <BLIB/Streams/InputStream.hpp>
#include <BLIB/Streams/SfInputStreamAdaptor.hpp>
#include <BLIB/Util/NonCopyable.hpp>

namespace bl
{
namespace as
{
class StreamCache;

/**
 * @brief Represents a persistent stream to asset data
 *
 * @ingroup Assets
 */
class PersistentStream : private util::NonCopyable {
public:
    /**
     * @brief Creates an unitialized persistent stream
     */
    PersistentStream()
    : stream()
    , adaptor(stream) {}

    /**
     * @brief Returns the underlying stream
     */
    stream::InputStream& getStream() { return stream; }

    /**
     * @brief Returns an SFML compatible stream
     */
    stream::SfInputStreamAdaptor& getSFMLAdaptor() { return adaptor; }

private:
    stream::InputStream stream;
    stream::SfInputStreamAdaptor adaptor;

    friend class StreamCache;
};

} // namespace as
} // namespace bl

#endif
