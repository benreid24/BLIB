#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Particle.hpp"
#include <BLIB/Render/Buffers/VertexBuffer.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Renderer.hpp>

struct ExampleComponent : public bl::rc::rcom::DrawableBase {
    bl::rc::buf::VertexBuffer vertexBuffer;

    ExampleComponent(bl::rc::Renderer& renderer, const glm::vec2 size, bool transparency,
                     std::uint32_t pipeline) {
        vertexBuffer.create(renderer.vulkanState(), 1);

        auto& vertices       = vertexBuffer.vertices();
        vertices[0].pos      = {0.f, 0.f, 0.f};
        vertices[0].texCoord = {0.f, 0.f};
        vertices[0].color    = {1.f, 1.f, 1.f, 1.f};

        vertexBuffer.queueTransfer(bl::rc::tfr::Transferable::SyncRequirement::Immediate);

        drawParams               = vertexBuffer.getDrawParameters();
        drawParams.instanceCount = 0;
        drawParams.firstInstance = 0;
        containsTransparency     = transparency;
        this->pipeline           = pipeline;
    }
};

#endif
