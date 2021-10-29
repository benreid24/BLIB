#include <BLIB/Media/Graphics/VertexBuffer.hpp>

namespace bl
{
namespace gfx
{
VertexBuffer::VertexBuffer(sf::PrimitiveType p, sf::VertexBuffer::Usage u, unsigned int c)
: count(c)
, array(p, c)
, buffer(p, u) {
    array.resize(c);
    if (sf::VertexBuffer::isAvailable()) { buffer.create(c); }
}

void VertexBuffer::resize(unsigned int c) {
    count = c;
    array.resize(c);
    if (sf::VertexBuffer::isAvailable()) { buffer.create(c); }
}

unsigned int VertexBuffer::size() const { return count; }

sf::Vertex& VertexBuffer::operator[](unsigned int i) { return array[i]; }

const sf::Vertex& VertexBuffer::operator[](unsigned int i) const { return array[i]; }

void VertexBuffer::update(unsigned int i, unsigned int c) {
    if (sf::VertexBuffer::isAvailable()) { buffer.update(&array[i], c, i); }
}

void VertexBuffer::update() {
    if (sf::VertexBuffer::isAvailable()) { buffer.update(&array[0]); }
}

void VertexBuffer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (sf::VertexBuffer::isAvailable()) { target.draw(buffer, states); }
    else {
        target.draw(array, states);
    }
}

} // namespace gfx
} // namespace bl
