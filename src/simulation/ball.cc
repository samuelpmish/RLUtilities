#include "simulation/ball.h"
#include "simulation/field.h"

#include "misc/json.h"

const float Ball::restitution = 0.6f;
const float Ball::drag = -0.0305f;
const float Ball::mu = 2.0f;

const float Ball::v_max = 4000.0f;
const float Ball::w_max = 6.0f;

const float Ball::m = 30.0f;
const float Ball::soccar_radius = 91.25f;
const float Ball::hoops_radius = 91.25f;
const float Ball::dropshot_radius = 100.45f;
const float Ball::soccar_collision_radius = 93.15f;
const float Ball::hoops_collision_radius = 93.15f;
const float Ball::dropshot_collision_radius = 103.6f;

float Ball::radius = Ball::soccar_radius;
float Ball::collision_radius = Ball::soccar_collision_radius;
float Ball::I = 0.4f * Ball::m * Ball::radius * Ball::radius;

Ball::Ball() {
	time = 0.0f;
	position = vec3{ 0.0f, 0.0f, 1.1f * collision_radius };
	velocity = vec3{ 0.0f, 0.0f, 0.0f };
	angular_velocity = vec3{ 0.0f, 0.0f, 0.0f };
}

sphere Ball::hitbox() {
	return sphere{ position, collision_radius };
}

void Ball::step(float dt) {
	const vec3 gravity = vec3{ 0.0, 0.0, -650.0f };

	ray contact = Field::collide(hitbox());

	if (norm(contact.direction) > 0.0) {

		vec3 p = contact.start;
		vec3 n = contact.direction;

		vec3 L = p - position;

		float m_reduced = 1.0f / ((1.0f / m) + dot(L, L) / I);

		vec3 v_perp = fminf(dot(velocity, n), 0.0f) * n;
		vec3 v_para = velocity - v_perp - cross(L, angular_velocity);

		float ratio = norm(v_perp) / fmaxf(norm(v_para), 0.0001f);

		vec3 J_perp = -(1.0f + restitution) * m * v_perp;
		vec3 J_para = -fminf(1.0f, mu * ratio) * m_reduced * v_para;

		vec3 J = J_perp + J_para;

		angular_velocity += cross(L, J) / I;
		velocity += (J / m) + drag * velocity * dt;
		position += velocity * dt;

		float penetration = collision_radius - dot(position - p, n);
		if (penetration > 0.0f) {
			position += 1.001f * penetration * n;
		}

	}
	else {

		velocity += (drag * velocity + gravity) * dt;
		position += velocity * dt;

	}

	angular_velocity *= fminf(1.0, w_max / norm(angular_velocity));
	velocity *= fminf(1.0, v_max / norm(velocity));
	time += dt;
}

vec3 closest_point_on_obb(const vec3 & v, const obb & hitbox) {
	vec3 v_local = dot(v - hitbox.center, hitbox.orientation);
	v_local[0] = clip(v_local[0], -hitbox.half_width[0], hitbox.half_width[0]);
	v_local[1] = clip(v_local[1], -hitbox.half_width[1], hitbox.half_width[1]);
	v_local[2] = clip(v_local[2], -hitbox.half_width[2], hitbox.half_width[2]);
	return dot(hitbox.orientation, v_local) + hitbox.center;
}

float scale(float dv) {
	const int n = 6;
	float values[n][2] = { {   0.0f, 0.65f},
						  { 500.0f, 0.65f},
						  {2300.0f, 0.55f},
						  {4600.0f, 0.30f} };

	float input = clip(dv, 0.0f, 4600.0f);

	for (int i = 0; i < (n - 1); i++) {
		if (values[i][0] <= input && input < values[i + 1][0]) {
			float u = (input - values[i][0]) / (values[i + 1][0] - values[i][0]);
			return lerp(values[i][1], values[i + 1][1], u);
		}
	}

	return -1.0f;
}

void Ball::step(float dt, const Car & c) {

	vec3 p = closest_point_on_obb(position, c.hitbox());

	if (norm(p - position) < collision_radius) {

    vec3 cx = c.position;
    vec3 cv = c.velocity;
    vec3 cw = c.angular_velocity;
    mat3 co = c.orientation;

		vec3 n1 = normalize(p - position);

		mat3 L_b = antisym(p - position);
		mat3 L_c = antisym(p - c.position);

		mat3 invI_c = dot(co, dot(c.invI, transpose(co)));

		mat3 M = inv(((1.0f / m) + (1.0f / c.m)) * eye<3>() - (dot(L_b, L_b) / I) - dot(L_c, dot(invI_c, L_c)));

		vec3 Delta_V = (cv - dot(L_c, cw)) - (velocity - dot(L_b, angular_velocity));

		// compute the impulse that is consistent with an inelastic collision 
		vec3 J1 = dot(M, Delta_V);

		vec3 J1_perp = fminf(dot(J1, n1), -1.0) * n1;
		vec3 J1_para = J1 - J1_perp;

		float ratio = norm(J1_perp) / fmaxf(norm(J1_para), 0.001f);

		// scale the parallel component of J1 such that the
		// Coulomb friction model is satisfied
		J1 = J1_perp + fminf(1.0f, mu * ratio) * J1_para;


		vec3 f = c.forward();
		vec3 n2 = position - cx;
		n2[2] *= 0.35f;
		n2 = normalize(n2 - 0.35f * dot(n2, f) * f);

		float dv = fminf(norm(velocity - cv), 4600.0f);
		vec3 J2 = m * dv * scale(dv) * n2;

		angular_velocity += dot(L_b, J1) / I;
		velocity += (J1 + J2) / m;

	}

	step(dt);

}
