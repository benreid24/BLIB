#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Particle.hpp"
#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Renderer.hpp>

struct Component : public bl::rc::rcom::DrawableBase {
    bl::rc::buf::IndexBuffer indexBuffer;

    Component(bl::rc::Renderer& renderer, const glm::vec2 size, bool transparency,
              std::uint32_t pipeline) {
        indexBuffer.create(renderer.vulkanState(), 4, 6);

        auto& vertices       = indexBuffer.vertices();
        vertices[0].pos      = {0.f, 0.f, 0.f};
        vertices[0].texCoord = {0.f, 0.f};
        vertices[0].color    = {1.f, 1.f, 1.f, 1.f};

        vertices[1].pos      = {size.x, 0.f, 0.f};
        vertices[1].texCoord = {1.f, 0.f};
        vertices[1].color    = {1.f, 1.f, 1.f, 1.f};

        vertices[2].pos      = {size.x, size.y, 0.f};
        vertices[2].texCoord = {1.f, 1.f};
        vertices[2].color    = {1.f, 1.f, 1.f, 1.f};

        vertices[3].pos      = {0.f, size.y, 0.f};
        vertices[3].texCoord = {0.f, 1.f};
        vertices[3].color    = {1.f, 1.f, 1.f, 1.f};

        indexBuffer.queueTransfer(bl::rc::tfr::Transferable::SyncRequirement::Immediate);

        drawParams               = indexBuffer.getDrawParameters();
        drawParams.instanceCount = 0;
        drawParams.firstInstance = 0;
        containsTransparency     = transparency;
        this->pipeline           = pipeline;
    }
};

#endif
