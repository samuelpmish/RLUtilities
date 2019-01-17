#include "AerialTurn.h"

#include "timer.h"

#include <fstream>

const float AerialTurn::scale = 10.5f;
const vec3 AerialTurn::angular_acceleration =
vec3{ -400.0f, -130.0f, 95.0f } / AerialTurn::scale;

const vec3 AerialTurn::angular_damping =
vec3{ -50.0f, -30.0f, -20.0f } / AerialTurn::scale;

const vec3 e[] = {
 {1.0f, 0.0f, 0.0f},
 {0.0f, 1.0f, 0.0f},
 {0.0f, 0.0f, 1.0f}
};

AerialTurn::AerialTurn(Car & c) : car(c) {
	target = eye<3>();
	eps_phi = 0.10f;
	eps_omega = 0.15f;
	horizon_time = 0.05f;

	finished = false;
	controls = Input();
}

// This matrix is used to relate the angular
// velocity to the time rate of the components
// in the axis-angle representation of the
// orientation error, phi. It is defined
// such that we can write dphi_dt in the
// following way:
//
//    vec3 dphi_dt = dot(Z(phi), omega)
//
mat3 AerialTurn::Z(const vec3 & q) {

	// in general, these coefficients are
	// given by:
	//
	//   c[i] = (B[i] / i!),
	//
	// where B[i] is the ith Bernoulli number
	float series_coefficients[] = {
	   1.0f,
	  -1.0f / 2.0f,
	   1.0f / 12.0f,
	   0.0f,
	   -1.0f / 720.0f,
	   0.0f,
	   1.0f / 30240.0f,
	   0.0f,
	   -1.0f / 1209600.0f
	};

	// we don't need to be exact, so we only take the 
	// first few terms in the series expansion.
	// 
	// In practice, I see good results at with the
	// linear approximation (i_max == 1)
	int i_max = 2;

	// Sum the first few terms in the series expansion
	mat3 Z_approx = eye<3>();
	mat3 Omega = antisym(q);
	mat3 Omega_to_the_ith_power = Omega;
	for (int i = 1; i <= i_max; i++) {
		if (series_coefficients[i] != 0.0f) {
			Z_approx = Z_approx + series_coefficients[i] * Omega_to_the_ith_power;
		}
		if (i != i_max) {
			Omega_to_the_ith_power = dot(Omega_to_the_ith_power, Omega);
		}
	}

	return Z_approx;
}

// This function provides a guideline for when 
// control swiching should take place. 
vec3 AerialTurn::G(const vec3 & q, const vec3 & dq_dt) {
	const vec3 T = angular_acceleration;
	const vec3 D = angular_damping;

	float G_roll = -sgn(dq_dt[0]) * (
		(fabs(dq_dt[0]) / D[0]) +
		(T[0] / (D[0] * D[0])) * log(T[0] / (T[0] + D[0] * fabs(dq_dt[0])))
		);

	float G_pitch = -sgn(dq_dt[1]) * dq_dt[1] * dq_dt[1] / (2.0f * T[1]);
	float G_yaw = sgn(dq_dt[2]) * dq_dt[2] * dq_dt[2] / (2.0f * T[2]);

	return vec3{ G_roll, G_pitch, G_yaw };
}

// the error between the predicted state and the precomputed return trajectories
vec3 AerialTurn::f(const vec3 & alpha_local, const float dt) {
	vec3 alpha = dot(theta, alpha_local);
	vec3 omega_pred = omega + alpha * dt;
	vec3 phi_pred = phi + dot(Z(phi), omega + 0.5f * alpha * dt) * dt;
	vec3 dphi_pred_dt = dot(Z(phi_pred), omega_pred);
	return -phi_pred - G(phi_pred, dphi_pred_dt);
}

// Let g(x) be the continuous piecewise linear function
// that interpolates the points:
// (-1.0, values[0]), (0.0, values[1]), (1.0, values[2])
// 
// solve_pwl() determines a value of x in [-1, 1] 
// such that || g(x) - y || is minimized.
float solve_pwl(float y, const vec3 & values) {
	float min_value = fminf(fminf(values[0], values[1]), values[2]);
	float max_value = fmaxf(fmaxf(values[0], values[1]), values[2]);
	float clipped_y = clip(y, min_value, max_value);

	// if the clipped value can be found in the interval [-1, 0]
	if ((fminf(values[0], values[1]) <= clipped_y) &&
		(clipped_y <= fmaxf(values[0], values[1]))) {
		if (fabs(values[1] - values[0]) > 0.0001f) {
			return (clipped_y - values[1]) / (values[1] - values[0]);
		}
		else {
			return -0.5f;
		}

		// if the clipped value can be found in the interval [0, 1]
	}
	else {
		if (fabs(values[2] - values[1]) > 0.0001f) {
			return (clipped_y - values[1]) / (values[2] - values[1]);
		}
		else {
			return 0.5f;
		}
	}
}

vec3 AerialTurn::find_controls_for(const vec3 & ideal_alpha) {
	const vec3 w = omega_local;
	const vec3 T = angular_acceleration;
	const vec3 D = angular_damping;

	vec3 rpy, alpha_values;

	// Note: these controls are calculated differently,
	// since Rocket League never disables roll damping.
	alpha_values = vec3{ -T[0] + D[0] * w[0], D[0] * w[0], T[0] + D[0] * w[0] };
	//std::cout << "possible alpha[0]: " << alpha_values << std::endl;
	rpy[0] = solve_pwl(ideal_alpha[0], alpha_values);

	alpha_values = vec3{ -T[1], D[1] * w[1], T[1] };
	//std::cout << "possible alpha[1]: " << alpha_values << std::endl;
	rpy[1] = solve_pwl(ideal_alpha[1], alpha_values);

	alpha_values = vec3{ -T[2], D[2] * w[2], T[2] };
	//std::cout << "possible alpha[2]: " << alpha_values << std::endl;
	rpy[2] = solve_pwl(ideal_alpha[2], alpha_values);

	return rpy;
}

void AerialTurn::step(float dt) {

	omega = dot(transpose(target), car.w);
	theta = dot(transpose(target), car.o);
	omega_local = dot(omega, theta);
	phi = rotation_to_axis(theta);
	dphi_dt = dot(Z(phi), omega);

	// Apply a few Newton iterations to find
	// local angular accelerations that try not to overshoot
	// the guideline trajectories defined by AerialTurn::G().
	// 
	// This helps to ensure monotonic convergence to the
	// desired orientation, when possible.
	int n_iter = 5;
	float eps = 0.001f;
	vec3 alpha{ 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < n_iter; i++) {
		vec3 f0 = f(alpha, horizon_time);

		mat3 J;
		for (int j = 0; j < 3; j++) {
			vec3 df_j = (f0 - f(alpha + eps * e[j], horizon_time)) / eps;
			J(0, j) = df_j[0]; J(1, j) = df_j[1]; J(2, j) = df_j[2];
		}

		alpha += dot(inv(J), f0);
	}

	vec3 rpy = find_controls_for(alpha);

	rpy[0] *= clip(0.5f * ((fabs(phi[0]) / eps_phi) + (fabs(omega[0]) / eps_omega)), 0.0f, 1.0f);
	rpy[1] *= clip(0.5f * ((fabs(phi[1]) / eps_phi) + (fabs(omega[1]) / eps_omega)), 0.0f, 1.0f);
	rpy[2] *= clip(0.5f * ((fabs(phi[2]) / eps_phi) + (fabs(omega[2]) / eps_omega)), 0.0f, 1.0f);

	controls.roll = rpy[0];
	controls.pitch = rpy[1];
	controls.yaw = rpy[2];

	finished = (norm(phi) < eps_phi) && (norm(omega) < eps_omega);

}

Car AerialTurn::simulate() {

	Car car_copy = Car(car);
	AerialTurn copy = AerialTurn(car_copy);
	copy.target = target;
	copy.eps_phi = eps_phi;
	copy.eps_omega = eps_omega;
	copy.horizon_time = horizon_time;

	float dt = 0.01666f;
	for (float t = dt; t < 2.0f; t += dt) {

		// get the new controls
		copy.step(dt);

		// and simulate their effect on the car
		car_copy.step(copy.controls, dt);

		if (copy.finished) break;
	}

	return car_copy;
}

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/pybind11.h>
void init_aerialturn(pybind11::module & m) {
	pybind11::class_<AerialTurn>(m, "AerialTurn")
		.def(pybind11::init<Car &>())
		.def_readwrite("target", &AerialTurn::target)
		.def_readwrite("eps_phi", &AerialTurn::eps_phi)
		.def_readwrite("eps_omega", &AerialTurn::eps_omega)
		.def_readwrite("horizon_time", &AerialTurn::horizon_time)
		.def_readwrite("finished", &AerialTurn::finished)
		.def_readwrite("controls", &AerialTurn::controls)
		.def("step", &AerialTurn::step)
		.def("simulate", &AerialTurn::simulate);
}
#endif
