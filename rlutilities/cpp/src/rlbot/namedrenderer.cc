#include "rlbot/namedrenderer.h"

#include "rlbot/interface.h"
#include "flatbuffers/flatbuffers.h"

NamedRenderer::NamedRenderer(std::string name)
    : Renderer(std::hash<std::string>()(name)) {}

void NamedRenderer::StartPacket() {
  flatbuffers::FlatBufferBuilder flatBufferBuilder(1000);
}

void NamedRenderer::FinishAndSend() {
  Finish();
  Interface::RenderGroup(flatBufferBuilder.GetBufferPointer(),
                         flatBufferBuilder.GetSize());
}

