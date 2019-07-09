#include "rlbot/renderer.h"

#include "rlbot/interface.h"

#define CLOSE_TO_MAX_RENDER_MESSAGE_SIZE 30000

const int n_sphere = 16;
std::vector < vec3 > sphere_points = []{
  float k = 6.28319f / n_sphere;
  std::vector < vec3 > points(6 * n_sphere);
  for (int i = 0; i < n_sphere; i++) {
    float c1 = cos(i * k);
    float s1 = sin(i * k);
    float c2 = cos((i+1) * k);
    float s2 = sin((i+1) * k);
  
    points[6*i + 0] = vec3{0.0f, c1, s1};
    points[6*i + 1] = vec3{0.0f, c2, s2};

    points[6*i + 2] = vec3{c1, 0.0f, s1};
    points[6*i + 3] = vec3{c2, 0.0f, s2};

    points[6*i + 4] = vec3{c1, s1, 0.0f};
    points[6*i + 5] = vec3{c2, s2, 0.0f};
  }
  return points;
}();

Renderer::Renderer(int _index) : builder(CLOSE_TO_MAX_RENDER_MESSAGE_SIZE) {
  index = _index;
  int approx_max_messages = CLOSE_TO_MAX_RENDER_MESSAGE_SIZE / 44;
  messages.resize(approx_max_messages);
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

  messages.push_back(rlbot::flat::CreateRenderMessage(
      builder, rlbot::flat::RenderType_DrawLine3D, coloroffset, &flat_start,
      &flat_end));

  if (builder.GetSize() > CLOSE_TO_MAX_RENDER_MESSAGE_SIZE) Finish();
}

void Renderer::DrawSphere(Color c, sphere s) {
  vec3 p = s.center;
  float r = s.radius;
  for (int i = 0; i < 3 * n_sphere; i++) {
    DrawLine3D(c, p + r*sphere_points[2*i+0], p + r*sphere_points[2*i+1]);
  }
}

void Renderer::DrawOBB(Color c, obb box) {
  vec3 p = box.center;
  vec3 hw = box.half_width;
  mat3 o = box.orientation;
  dot(o, vec3{ hw[0],  hw[1],  hw[2]});
  DrawLine3D(c, p + dot(o, vec3{-hw[0], -hw[1], -hw[2]}),
                p + dot(o, vec3{ hw[0], -hw[1], -hw[2]}));
  DrawLine3D(c, p + dot(o, vec3{-hw[0], -hw[1],  hw[2]}),
                p + dot(o, vec3{ hw[0], -hw[1],  hw[2]}));
  DrawLine3D(c, p + dot(o, vec3{-hw[0],  hw[1], -hw[2]}),
                p + dot(o, vec3{ hw[0],  hw[1], -hw[2]}));
  DrawLine3D(c, p + dot(o, vec3{-hw[0],  hw[1],  hw[2]}),
                p + dot(o, vec3{ hw[0],  hw[1],  hw[2]}));

  DrawLine3D(c, p + dot(o, vec3{-hw[0], -hw[1], -hw[2]}),
                p + dot(o, vec3{-hw[0],  hw[1], -hw[2]}));
  DrawLine3D(c, p + dot(o, vec3{-hw[0], -hw[1],  hw[2]}),
                p + dot(o, vec3{-hw[0],  hw[1],  hw[2]}));
  DrawLine3D(c, p + dot(o, vec3{ hw[0], -hw[1], -hw[2]}),
                p + dot(o, vec3{ hw[0],  hw[1], -hw[2]}));
  DrawLine3D(c, p + dot(o, vec3{ hw[0], -hw[1],  hw[2]}),
                p + dot(o, vec3{ hw[0],  hw[1],  hw[2]}));

  DrawLine3D(c, p + dot(o, vec3{-hw[0], -hw[1], -hw[2]}),
                p + dot(o, vec3{-hw[0], -hw[1],  hw[2]}));
  DrawLine3D(c, p + dot(o, vec3{ hw[0], -hw[1], -hw[2]}),
                p + dot(o, vec3{ hw[0], -hw[1],  hw[2]}));
  DrawLine3D(c, p + dot(o, vec3{-hw[0],  hw[1], -hw[2]}),
                p + dot(o, vec3{-hw[0],  hw[1],  hw[2]}));
  DrawLine3D(c, p + dot(o, vec3{ hw[0],  hw[1], -hw[2]}),
                p + dot(o, vec3{ hw[0],  hw[1],  hw[2]}));
}


void Renderer::DrawPolyLine3D(Color color, std::vector<vec3> points) {
  if (points.size() >= 2) {
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
      messages.push_back(rlbot::flat::CreateRenderMessage(
          builder, rlbot::flat::RenderType_DrawLine3D, coloroffset, &flat_start,
          &flat_end));
    }

    if (builder.GetSize() > CLOSE_TO_MAX_RENDER_MESSAGE_SIZE) Finish();
  }
}

void Renderer::DrawString2D(Color color, std::string text, vec2 top_left,
                            int scaleX, int scaleY) {
  auto colorbuilder = rlbot::flat::ColorBuilder(builder);
  colorbuilder.add_a(color.a);
  colorbuilder.add_r(color.r);
  colorbuilder.add_g(color.g);
  colorbuilder.add_b(color.b);
  auto coloroffset = colorbuilder.Finish();

  auto stringoffset = builder.CreateString(text.c_str());
  rlbot::flat::Vector3 flat_top_left{top_left[0], top_left[1], 0.0f};

  messages.push_back(rlbot::flat::CreateRenderMessage(
      builder, rlbot::flat::RenderType_DrawString2D, coloroffset,
      &flat_top_left, 0, scaleX, scaleY, stringoffset));

  if (builder.GetSize() > CLOSE_TO_MAX_RENDER_MESSAGE_SIZE) Finish();
}

void Renderer::DrawString3D(Color color, std::string text, vec3 top_left,
                            int scaleX, int scaleY) {
  auto colorbuilder = rlbot::flat::ColorBuilder(builder);
  colorbuilder.add_a(color.a);
  colorbuilder.add_r(color.r);
  colorbuilder.add_g(color.g);
  colorbuilder.add_b(color.b);
  auto coloroffset = colorbuilder.Finish();

  auto stringoffset = builder.CreateString(text.c_str());
  rlbot::flat::Vector3 flat_top_left{top_left[0], top_left[1], top_left[2]};

  messages.push_back(rlbot::flat::CreateRenderMessage(
      builder, rlbot::flat::RenderType_DrawString3D, coloroffset,
      &flat_top_left, 0, scaleX, scaleY, stringoffset));

  if (builder.GetSize() > CLOSE_TO_MAX_RENDER_MESSAGE_SIZE) Finish();
}

void Renderer::Start() {
  builder.Clear();
  messages.clear();
}

void Renderer::Finish() {
  auto messageoffsets = builder.CreateVector(messages);

  auto groupbuilder = rlbot::flat::RenderGroupBuilder(builder);
  groupbuilder.add_id(index);
  groupbuilder.add_renderMessages(messageoffsets);

  auto packet = groupbuilder.Finish();

  builder.Finish(packet);

  Interface::RenderGroup(builder.GetBufferPointer(), builder.GetSize());
}
