#ifndef BLIB_GRAPHICS_SHAPES2D_ICONBASE_HPP
#define BLIB_GRAPHICS_SHAPES2D_ICONBASE_HPP

#include <BLIB/Graphics/Shapes2D/SingleShape2D.hpp>
#include <type_traits>

namespace bl
{
namespace gfx
{
namespace s2d
{
template<typename B>
class IconBase : public B {
    static_assert(std::is_base_of_v<Shape2D, B>, "B must derive from Shape2D");

public:
    /**
     * @brief The various icons that can be rendered
     */
    enum struct Type {
        /// A basic arrow that points up
        Arrow,

        /// A basic folder icon
        Folder
    };

    /**
     * @brief Creates the icon
     *
     * @param type The type of icon to render
     * @param size The size of the icon
     */
    IconBase(Type type, const glm::vec2& size);

    /**
     * @brief Destroys the icon
     */
    virtual ~IconBase() = default;

    /**
     * @brief Sets the local size of the icon in world units. This is the size prior to the
     *        transform. The vertices of the icon are updated
     *
     * @param size The local size of the icon in world units
     */
    void setSize(const glm::vec2& size);

    /**
     * @brief Returns the local size of the icon
     */
    const glm::vec2& getSize() const;

protected:
    const Type iconType;

private:
    glm::vec2 size;

    virtual unsigned int getVertexCount() const override;
    virtual void populateVertex(unsigned int index, rc::prim::Vertex& vertex) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename B>
IconBase<B>::IconBase(Type type, const glm::vec2& size)
: iconType(type)
, size(size) {}

template<typename B>
void IconBase<B>::setSize(const glm::vec2& s) {
    size = s;
    B::notifyDirty();
}

template<typename B>
const glm::vec2& IconBase<B>::getSize() const {
    return size;
}

template<typename B>
unsigned int IconBase<B>::getVertexCount() const {
    switch (iconType) {
    case Type::Arrow:
        return 7;
    case Type::Folder:
        // TODO - folder icon
        return 0;
    default:
        BL_LOG_ERROR << "Unknown icon type: " << iconType;
        return 0;
    }
}

template<typename B>
void IconBase<B>::populateVertex(unsigned int index, rc::prim::Vertex& vertex) {
    constexpr float ArrowHeadHeight = 0.4f;
    constexpr float ArrowBodyWidth  = 0.3f;

    switch (iconType) {
    case Type::Arrow:
        switch (index) {
        case 0:
            vertex.pos.x = 0.f;
            vertex.pos.y = size.y * ArrowHeadHeight;
            break;
        case 1:
            vertex.pos.x = size.x * 0.5f;
            vertex.pos.y = 0.f;
            break;
        case 2:
            vertex.pos.x = size.x;
            vertex.pos.y = size.y * ArrowHeadHeight;
            break;
        case 3:
            vertex.pos.x = size.x * (0.5f + ArrowBodyWidth * 0.5f);
            vertex.pos.y = size.y * ArrowHeadHeight;
            break;
        case 4:
            vertex.pos.x = size.x * (0.5f + ArrowBodyWidth * 0.5f);
            vertex.pos.y = size.y;
            break;
        case 5:
            vertex.pos.x = size.x * (0.5f - ArrowBodyWidth * 0.5f);
            vertex.pos.y = size.y;
            break;
        case 6:
            vertex.pos.x = size.x * (0.5f - ArrowBodyWidth * 0.5f);
            vertex.pos.y = size.y * ArrowHeadHeight;
            break;
        }
        break;

    case Type::Folder:
        // TODO - folder icon
        break;
    }
}

} // namespace s2d
} // namespace gfx
} // namespace bl

#endif
