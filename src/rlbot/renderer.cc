#include "rlbot/renderer.h"

#include "rlbot/interface.h"

#define CLOSE_TO_MAX_RENDER_MESSAGE_SIZE 10000

Renderer::Renderer() { 
  index = 0;
}

void Renderer::DrawLine3D(Color color, vec3 start, vec3 end) {
  auto colorbuilder = rlbot::flat::ColorBuilder(builder);
  colorbuilder.add_a(color.a);
  colorbuilder.add_r(color.r);
  colorbuilder.add_g(color.g);
  colorbuilder.add_b(color.b);
  auto coloroffset = colorbuilder.Finish();

  rlbot::flat::Vector3 flat_start{start[0], start[1], start[2]};
  rlbot::flat::Vector3 flat_end{end[0], end[1], end[2]};

  renderMessages.push_back(rlbot::flat::CreateRenderMessage(
      builder, rlbot::flat::RenderType_DrawLine3D, coloroffset,
      &flat_start, &flat_end));

  //std::cout << builder.GetSize() << std::endl;
  if (builder.GetSize() > CLOSE_TO_MAX_RENDER_MESSAGE_SIZE) Finish();

}

void Renderer::DrawPolyLine3D(Color color, std::vector<vec3> points) {
  auto colorbuilder = rlbot::flat::ColorBuilder(builder);
  colorbuilder.add_a(color.a);
  colorbuilder.add_r(color.r);
  colorbuilder.add_g(color.g);
  colorbuilder.add_b(color.b);
  auto coloroffset = colorbuilder.Finish();

  for (int i = 0; i < points.size() - 1; i++) {
    rlbot::flat::Vector3 flat_start{points[i][0], points[i][1], points[i][2]};
    rlbot::flat::Vector3 flat_end{points[i + 1][0], points[i + 1][1],
                                  points[i + 1][2]};
    renderMessages.push_back(rlbot::flat::CreateRenderMessage(
        builder, rlbot::flat::RenderType_DrawLine3D, coloroffset,
        &flat_start, &flat_end));
  }
}

void Renderer::DrawString2D(std::string text, Color color, vec2 top_left, int scaleX,
                            int scaleY) {
  auto colorbuilder = rlbot::flat::ColorBuilder(builder);
  colorbuilder.add_a(color.a);
  colorbuilder.add_r(color.r);
  colorbuilder.add_g(color.g);
  colorbuilder.add_b(color.b);
  auto coloroffset = colorbuilder.Finish();

  auto stringoffset = builder.CreateString(text.c_str());
  rlbot::flat::Vector3 flat_top_left{top_left[0], top_left[1], 0.0f};

  renderMessages.push_back(rlbot::flat::CreateRenderMessage(
      builder, rlbot::flat::RenderType_DrawString3D, coloroffset,
      &flat_top_left, 0, scaleX, scaleY, stringoffset));
}

void Renderer::DrawString3D(std::string text, Color color, vec3 top_left,
                            int scaleX, int scaleY) {
  auto colorbuilder = rlbot::flat::ColorBuilder(builder);
  colorbuilder.add_a(color.a);
  colorbuilder.add_r(color.r);
  colorbuilder.add_g(color.g);
  colorbuilder.add_b(color.b);
  auto coloroffset = colorbuilder.Finish();

  auto stringoffset = builder.CreateString(text.c_str());
  rlbot::flat::Vector3 flat_top_left{top_left[0], top_left[1], top_left[2]};

  renderMessages.push_back(rlbot::flat::CreateRenderMessage(
      builder, rlbot::flat::RenderType_DrawString2D, coloroffset,
      &flat_top_left, 0, scaleX, scaleY, stringoffset));
}

void Renderer::Clear() {

  // TODO

}

void Renderer::Finish() {
  auto messageoffsets = builder.CreateVector(renderMessages);

  auto groupbuilder = rlbot::flat::RenderGroupBuilder(builder);
  groupbuilder.add_id(index++);
  groupbuilder.add_renderMessages(messageoffsets);

  auto packet = groupbuilder.Finish();

  builder.Finish(packet);

  std::cout << Interface::RenderGroup(builder.GetBufferPointer(), builder.GetSize()) << std::endl;

  renderMessages.clear();
  builder.Clear();
}
