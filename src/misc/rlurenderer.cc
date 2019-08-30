#include "misc/rlurenderer.h"
#include "misc/timer.h"

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

rlbot::flat::Vector3 vec3_to_Vector3(vec3 v)
{
  return {v[0], v[1], v[2]};
}

RLURenderer::RLURenderer(std::string name) : scoped_renderer(name)
{
}

void RLURenderer::DrawLine3D(rlbot::Color color, vec3 start, vec3 end)
{
  scoped_renderer.DrawLine3D(color, vec3_to_Vector3(start), vec3_to_Vector3(end));
}

void RLURenderer::DrawOBB(rlbot::Color color, obb box)
{
  vec3 p = box.center;
  vec3 hw = box.half_width;
  mat3 o = box.orientation;
  dot(o, vec3{ hw[0],  hw[1],  hw[2]});
  DrawLine3D(color, p + dot(o, vec3{-hw[0], -hw[1], -hw[2]}),
                    p + dot(o, vec3{ hw[0], -hw[1], -hw[2]}));
  DrawLine3D(color, p + dot(o, vec3{-hw[0], -hw[1],  hw[2]}),
                    p + dot(o, vec3{ hw[0], -hw[1],  hw[2]}));
  DrawLine3D(color, p + dot(o, vec3{-hw[0],  hw[1], -hw[2]}),
                    p + dot(o, vec3{ hw[0],  hw[1], -hw[2]}));
  DrawLine3D(color, p + dot(o, vec3{-hw[0],  hw[1],  hw[2]}),
                    p + dot(o, vec3{ hw[0],  hw[1],  hw[2]}));

  DrawLine3D(color, p + dot(o, vec3{-hw[0], -hw[1], -hw[2]}),
                    p + dot(o, vec3{-hw[0],  hw[1], -hw[2]}));
  DrawLine3D(color, p + dot(o, vec3{-hw[0], -hw[1],  hw[2]}),
                    p + dot(o, vec3{-hw[0],  hw[1],  hw[2]}));
  DrawLine3D(color, p + dot(o, vec3{ hw[0], -hw[1], -hw[2]}),
                    p + dot(o, vec3{ hw[0],  hw[1], -hw[2]}));
  DrawLine3D(color, p + dot(o, vec3{ hw[0], -hw[1],  hw[2]}),
                    p + dot(o, vec3{ hw[0],  hw[1],  hw[2]}));

  DrawLine3D(color, p + dot(o, vec3{-hw[0], -hw[1], -hw[2]}),
                    p + dot(o, vec3{-hw[0], -hw[1],  hw[2]}));
  DrawLine3D(color, p + dot(o, vec3{ hw[0], -hw[1], -hw[2]}),
                    p + dot(o, vec3{ hw[0], -hw[1],  hw[2]}));
  DrawLine3D(color, p + dot(o, vec3{-hw[0],  hw[1], -hw[2]}),
                    p + dot(o, vec3{-hw[0],  hw[1],  hw[2]}));
  DrawLine3D(color, p + dot(o, vec3{ hw[0],  hw[1], -hw[2]}),
                    p + dot(o, vec3{ hw[0],  hw[1],  hw[2]}));
}

void RLURenderer::DrawSphere(rlbot::Color color, sphere s)
{
  vec3 p = s.center;
  float r = s.radius;
  for (int i = 0; i < 3 * n_sphere; i++) {
    DrawLine3D(color, p + r*sphere_points[2*i+0], p + r*sphere_points[2*i+1]);
  }
}

void RLURenderer::DrawPolyLine3D(rlbot::Color color, std::vector<vec3> vertices)
{
  std::vector<const rlbot::flat::Vector3 *> pointers;

  for (size_t i = 0; i < vertices.size(); i++)
  {
    pointers.push_back(new rlbot::flat::Vector3(vec3_to_Vector3(vertices[i])));
  }
  
  scoped_renderer.DrawPolyLine3D(color, pointers);

  for(auto p : pointers)
  {
    delete p;
  }
}

void RLURenderer::DrawString2D(rlbot::Color color, std::string text, vec2 topleft, int scaleX, int scaleY)
{
  scoped_renderer.DrawString2D(text, color, vec3_to_Vector3(topleft), scaleX, scaleY);
}

void RLURenderer::DrawString3D(rlbot::Color color, std::string text, vec3 topleft, int scaleX, int scaleY)
{
  scoped_renderer.DrawString3D(text, color, vec3_to_Vector3(topleft), scaleX, scaleY);
}
