#ifndef BLIB_MENU_ITEMS_IMAGEITEM_HPP
#define BLIB_MENU_ITEMS_IMAGEITEM_HPP

#include <BLIB/Graphics/Sprite.hpp>
#include <BLIB/Interfaces/Menu/Item.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief RenderItem for displaying sprites in a Menu
 *
 * @ingroup Menu
 *
 */
class ImageItem : public Item {
public:
    typedef std::shared_ptr<ImageItem> Ptr;

    /**
     * @brief Create the render item from a sprite
     *
     * @param texture The texture to use
     */
    static Ptr create(const rc::res::TextureRef& texture);

    /**
     * @brief Destroy the Image Item object
     */
    virtual ~ImageItem() = default;

    /**
     * @brief Returns a modifiable reference to the contained sprite
     */
    gfx::Sprite& getSprite();

    /**
     * @brief Changes the texture of this image. Does not trigger a menu refresh
     *
     * @param texture The texture to use
     */
    void setTexture(const rc::res::TextureRef& texture);

    /**
     * @see Item::getSize
     */
    virtual glm::vec2 getSize() const override;

protected:
    /**
     * @brief Called at least once when the item is added to a menu. Should create required graphics
     *        primitives and return the transform to use
     *
     * @param world The world to create entities in
     */
    virtual void doCreate(engine::World& world) override;

    /**
     * @brief Called when the item should be added to the scene
     *
     * @param scene The scene to add to
     */
    virtual void doSceneAdd(rc::Scene* scene) override;

    /**
     * @brief Called when the item should be removed from the scene
     */
    virtual void doSceneRemove() override;

    /**
     * @brief Returns the entity (or top level entity) of the item
     */
    virtual ecs::Entity getEntity() const override;

    /**
     * @brief Manually draw the item
     *
     * @param ctx The render context
     */
    virtual void draw(rc::scene::CodeScene::RenderContext& ctx) override;

private:
    rc::res::TextureRef texture;
    gfx::Sprite sprite;

    ImageItem(const rc::res::TextureRef& texture);
};

} // namespace menu
} // namespace bl

#endif
