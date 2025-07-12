#include "AttachmentSet.hpp"
#include <BLIB/Render/Vulkan/AttachmentSet.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
AttachmentSet::AttachmentSet()
: extent{0, 0}
, images{}
, views{}
, n(0)
, layerCount(1)
, outputIndex(0) {}

AttachmentSet::AttachmentSet(Image& image)
: AttachmentSet() {
    init(image);
}

AttachmentSet::AttachmentSet(std::uint32_t attachmentCount, VkImage* imagesIn, VkImageView* viewsIn,
                             VkImageAspectFlags* aspectsIn, VkExtent2D extentIn, std::uint32_t lc)
: AttachmentSet() {
    init(attachmentCount, imagesIn, viewsIn, aspectsIn, extentIn, lc);
}

void AttachmentSet::init(std::uint32_t attachmentCount, VkImage* imagesIn, VkImageView* viewsIn,
                         VkImageAspectFlags* aspectsIn, VkExtent2D extentIn, std::uint32_t lc) {
    n          = attachmentCount;
    layerCount = lc;
    extent     = extentIn;
    for (std::uint32_t i = 0; i < attachmentCount; ++i) {
        images[i]  = imagesIn[i];
        views[i]   = viewsIn[i];
        aspects[i] = aspects[i];
    }
}

void AttachmentSet::init(Image& image) {
    n          = 1;
    layerCount = image.getLayerCount();
    extent     = image.getSize();
    images[0]  = image.getImage();
    views[0]   = image.getView();
    aspects[0] = image.getAspect();
}

void AttachmentSet::setAttachmentCount(std::uint32_t count) { n = count; }

void AttachmentSet::setAttachment(std::uint32_t index, VkImage image, VkImageView view) {
    images[index] = image;
    views[index]  = view;
    n             = std::max(n, index + 1);
}

void AttachmentSet::setAttachmentAspect(std::uint32_t index, VkImageAspectFlags aspect) {
    aspects[index] = aspect;
}

void AttachmentSet::setAttachment(std::uint32_t index, Image& image) {
    images[index]  = image.getImage();
    views[index]   = image.getView();
    aspects[index] = image.getAspect();
}

void AttachmentSet::setAttachments(std::uint32_t index, AttachmentImageSet& imagesIn) {
    copy(imagesIn.attachmentSet(), imagesIn.getAttachmentCount(), index);
}

void AttachmentSet::copy(const AttachmentSet& other, std::uint32_t count, std::uint32_t baseIndex) {
    for (std::uint32_t i = 0; i < count; ++i) {
        images[baseIndex + i]  = other.images[i];
        views[baseIndex + i]   = other.views[i];
        aspects[baseIndex + i] = other.aspects[i];
    }
}

void AttachmentSet::setOutputIndex(std::uint32_t index) { outputIndex = index; }

void AttachmentSet::setRenderExtent(const VkExtent2D& e) { extent = e; }

} // namespace vk
} // namespace rc
} // namespace bl