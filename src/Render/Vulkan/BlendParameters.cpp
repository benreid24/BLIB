#include <BLIB/Render/Vulkan/BlendParameters.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
BlendParameters::BlendParameters()
: colorBlendBehavior(ColorBlendBehavior::AlphaBlend)
, simpleColorBlendAttachmentCount(1) {
    colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.logicOp           = VK_LOGIC_OP_COPY; // Optional
    colorBlending.blendConstants[0] = 0.0f;             // Optional
    colorBlending.blendConstants[1] = 0.0f;             // Optional
    colorBlending.blendConstants[2] = 0.0f;             // Optional
    colorBlending.blendConstants[3] = 0.0f;             // Optional
}

BlendParameters::BlendParameters(const BlendParameters& copy) { *this = copy; }

BlendParameters& BlendParameters::operator=(const BlendParameters& copy) {
    colorBlendBehavior              = copy.colorBlendBehavior;
    simpleColorBlendAttachmentCount = copy.simpleColorBlendAttachmentCount;
    colorAttachmentBlendStates      = copy.colorAttachmentBlendStates;
    colorBlending.pAttachments      = colorAttachmentBlendStates.data();
    colorBlending.attachmentCount   = colorAttachmentBlendStates.size();
    return *this;
}

BlendParameters& BlendParameters::withSimpleColorBlendState(ColorBlendBehavior blend,
                                                            std::uint32_t ac) {
    colorBlendBehavior              = blend;
    simpleColorBlendAttachmentCount = ac;
    return *this;
}

BlendParameters& BlendParameters::addColorAttachmentBlendState(
    const VkPipelineColorBlendAttachmentState& ca) {
    colorAttachmentBlendStates.emplace_back(ca);
    return *this;
}

BlendParameters& BlendParameters::withColorBlendStateConfig(VkLogicOp operation,
                                                            float blendConstant0,
                                                            float blendConstant1,
                                                            float blendConstant2,
                                                            float blendConstant3) {
    colorBlending.logicOpEnable     = VK_TRUE;
    colorBlending.logicOp           = operation;
    colorBlending.blendConstants[0] = blendConstant0;
    colorBlending.blendConstants[1] = blendConstant1;
    colorBlending.blendConstants[2] = blendConstant2;
    colorBlending.blendConstants[3] = blendConstant3;
    return *this;
}

void BlendParameters::build() {
    if (colorAttachmentBlendStates.empty() && colorBlendBehavior != ColorBlendBehavior::None) {
        for (std::uint32_t i = 0; i < simpleColorBlendAttachmentCount; ++i) {
            colorAttachmentBlendStates.emplace_back();
            auto& ca          = colorAttachmentBlendStates.back();
            ca.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            ca.blendEnable = VK_TRUE;

            switch (colorBlendBehavior) {
            case ColorBlendBehavior::Overwrite:
                ca.blendEnable = VK_FALSE;
                break;

            case ColorBlendBehavior::Add:
                ca.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
                ca.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
                ca.colorBlendOp        = VK_BLEND_OP_ADD;
                ca.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                ca.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                ca.alphaBlendOp        = VK_BLEND_OP_MAX;
                break;

            case ColorBlendBehavior::AlphaBlend:
            default:
                ca.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                ca.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                ca.colorBlendOp        = VK_BLEND_OP_ADD;
                ca.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                ca.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                ca.alphaBlendOp        = VK_BLEND_OP_MAX;
                break;
            }
        }
    }
    colorBlending.pAttachments    = colorAttachmentBlendStates.data();
    colorBlending.attachmentCount = colorAttachmentBlendStates.size();
}

bool BlendParameters::operator==(const BlendParameters& right) const {
    if (colorAttachmentBlendStates.size() != right.colorAttachmentBlendStates.size()) {
        return false;
    }
    for (unsigned int i = 0; i < colorAttachmentBlendStates.size(); ++i) {
        if (colorAttachmentBlendStates[i].blendEnable !=
            right.colorAttachmentBlendStates[i].blendEnable) {
            return false;
        }
        if (colorAttachmentBlendStates[i].srcColorBlendFactor !=
            right.colorAttachmentBlendStates[i].srcColorBlendFactor) {
            return false;
        }
        if (colorAttachmentBlendStates[i].dstColorBlendFactor !=
            right.colorAttachmentBlendStates[i].dstColorBlendFactor) {
            return false;
        }
        if (colorAttachmentBlendStates[i].colorBlendOp !=
            right.colorAttachmentBlendStates[i].colorBlendOp) {
            return false;
        }
        if (colorAttachmentBlendStates[i].srcAlphaBlendFactor !=
            right.colorAttachmentBlendStates[i].srcAlphaBlendFactor) {
            return false;
        }
        if (colorAttachmentBlendStates[i].dstAlphaBlendFactor !=
            right.colorAttachmentBlendStates[i].dstAlphaBlendFactor) {
            return false;
        }
        if (colorAttachmentBlendStates[i].alphaBlendOp !=
            right.colorAttachmentBlendStates[i].alphaBlendOp) {
            return false;
        }
        if (colorAttachmentBlendStates[i].colorWriteMask !=
            right.colorAttachmentBlendStates[i].colorWriteMask) {
            return false;
        }
    }

    if (colorBlending.flags != right.colorBlending.flags) { return false; }
    if (colorBlending.logicOpEnable != right.colorBlending.logicOpEnable) { return false; }
    if (colorBlending.logicOp != right.colorBlending.logicOp) { return false; }
    for (unsigned int i = 0; i < 4; ++i) {
        if (colorBlending.blendConstants[i] != right.colorBlending.blendConstants[i]) {
            return false;
        }
    }

    return true;
}

} // namespace vk
} // namespace rc
} // namespace bl
