#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <BLIB/Resources.hpp>
#include <glad/vulkan.h>

class Renderer;
struct TextureLoader;

class Texture {
public:
    Texture();
    ~Texture();

    void createSampler(VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                       VkFilter magFilter               = VK_FILTER_LINEAR,
                       VkFilter minFilter               = VK_FILTER_LINEAR);

    void createSampler(VkSamplerAddressMode addressModeU, VkSamplerAddressMode addressModeV,
                       VkSamplerAddressMode addressModeW, VkFilter magFilter = VK_FILTER_LINEAR,
                       VkFilter minFilter = VK_FILTER_LINEAR);

private:
    static VkDevice device;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;

    friend struct TextureLoader;
};

using TextureManager = bl::resource::ResourceManager<Texture>;

// TODO - may be prudent to only load textures through Renderer and to use the Resources module only
// for sf::Image. That way multiple renderers may be used
struct TextureLoader : public bl::resource::LoaderBase<Texture> {
    Renderer& renderer;

    TextureLoader(Renderer& renderer);
    virtual ~TextureLoader() = default;
    virtual bool load(const std::string&, const char* buffer, std::size_t len, std::istream&,
                      Texture& result) override;
};

#endif
