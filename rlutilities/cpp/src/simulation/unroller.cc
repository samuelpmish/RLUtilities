#include "simulation/unroller.h"

Unroller::Unroller() {
	r = 200.0f;

	car_pos = vec3{ 0.0f, 0.0f, 0.0f };
	car_orientation = eye<3>();

	target = vec3{ 0.0f, 0.0f, 0.0f };
	normal = vec3{ 0.0f, 0.0f, 0.0f };
}

void Unroller::setup(vec3 x, mat3 o, vec3 t, vec3 n) {
	car_pos = x;
	car_orientation = o;
	target = t;
	normal = n;

	// first, transform everything into local coordinates    
	vec3 n1 = { 0.0f, 0.0f, 1.0f }; // normal

	vec3 p2 = dot(target - car_pos, car_orientation);
	vec3 n2 = dot(normal, car_orientation);

	vec3 e2 = n1;
	vec3 e3 = normalize(cross(e2, n2));
	vec3 e1 = normalize(cross(e2, e3));

	theta = atan2(dot(n2, -e1), dot(n2, n1));
	h = r * (1.0f - cos(theta));

	O = (dot(p2, n2) + h) * vec3(normalize(vec2(n2)));
	S = axis_to_rotation(theta * e3);
	frame = mat3{
	  {e1[0], e2[0], e3[0]},
	  {e1[1], e2[1], e3[1]},
	  {e1[2], e2[2], e3[2]}
	};
}

vec3 Unroller::unroll_point(vec3 p) {
	vec3 q = dot(p - O, frame);

	if (q[0] > 0.0f) {
		float phi = (q[1] >= h) ? theta : acos(1.0f - clip(q[1] / r, 0.0f, 1.0f));
		float l = (q[1] >= h) ? (q[1] - h) / sin(theta) : 0.0f;

		q[0] = r * phi + l;
		q[1] = 0.0f;

		return dot(frame, q) + O;
	}
	else {
		return p;
	}
}

vec3 Unroller::unroll_direction(vec3 d) {
	return dot(d, S);
}

vec3 Unroller::roll_point(vec3 p) {
	vec3 q = dot(p - O, frame);

	if (q[0] > 0.0) {
		float phi = fminf(theta, q[0] / r);
		float l = fmaxf(0.0f, q[0] - r * theta);

		q[0] = r * sin(phi) + l * cos(phi);
		q[1] = r * (1.0f - cos(phi)) + l * sin(phi);

		return dot(frame, q) + O;
	}
	else {
		return p;
	}
}

vec3 Unroller::roll_direction(vec3 d) {
	return dot(S, d);
}
