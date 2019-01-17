#include "Drive.h"
#include "CompositeArc.h"

#include <cmath>
#include <numeric>

#include "Field.h"
#include "circles.h"

const float Drive::max_speed = 2300.0f;
const float Drive::max_throttle_speed = 1410.0f;
const float Drive::boost_accel = 991.667f;
const float Drive::brake_accel = 3500.0f;
const float Drive::coasting_accel = 525.0f;

float Drive::throttle_accel(float speed) {
	const int n = 3;
	float values[n][2] = { {   0.0f, 1600.0f},
						  {1400.0f,  160.0f},
						  {1410.0f,    0.0f} };

	float input = clip(fabs(speed), 0.0f, 1410.0f);

	for (int i = 0; i < (n - 1); i++) {
		if (values[i][0] <= input && input < values[i + 1][0]) {
			float u = (input - values[i][0]) / (values[i + 1][0] - values[i][0]);
			return lerp(values[i][1], values[i + 1][1], u);
		}
	}

	return -1.0f;
}

float Drive::max_turning_curvature(float speed) {
	const int n = 6;
	float values[n][2] = { {   0.0f, 0.00690f},
						  { 500.0f, 0.00398f},
						  {1000.0f, 0.00235f},
						  {1500.0f, 0.00138f},
						  {1750.0f, 0.00110f},
						  {2300.0f, 0.00088f} };

	float input = clip(fabs(speed), 0.0f, 2300.0f);

	for (int i = 0; i < (n - 1); i++) {
		if (values[i][0] <= input && input < values[i + 1][0]) {
			float u = (input - values[i][0]) / (values[i + 1][0] - values[i][0]);
			return lerp(values[i][1], values[i + 1][1], u);
		}
	}

	return -1.0f;
}

float Drive::max_turning_speed(float curvature) {
	const int n = 6;
	float values[n][2] = { {0.00088f, 2300.0f},
						  {0.00110f, 1750.0f},
						  {0.00138f, 1500.0f},
						  {0.00235f, 1000.0f},
						  {0.00398f,  500.0f},
						  {0.00690f,    0.0f} };

	float input = clip(fabs(curvature), values[0][0], values[n - 1][0]);

	for (int i = 0; i < (n - 1); i++) {
		if (values[i][0] <= input && input <= values[i + 1][0]) {
			float u = (input - values[i][0]) / (values[i + 1][0] - values[i][0]);
			return clip(lerp(values[i][1], values[i + 1][1], u), 0.0f, 2300.0f);
		}
	}

	return -1.0f;
}

Drive::Drive(Car & c) : car(c) {

	int n = 6;
	error_history = std::list< float >(n, 0);

	target = { NAN, NAN, NAN };

	path = Curve();

	controls = Input();
	finished = false;

	kp = (1.0f / 0.03f);
	ki = 0.0f;
	kd = 0.0f;

}

void Drive::step(float dt) {

	debug = std::vector < vec3 >();

	//  float t_react = 0.4f;
	//
	//  // idealize the car's position and orientation as well as the target
	//  ray s_car_info = Field::snap(car.x);
	//  ray s_target_info = Field::snap(target);
	//
	//  vec3 s_pos = s_car_info.start;
	//  vec3 s_up = s_car_info.direction;
	//  vec3 s_left = normalize(cross(s_up, car.forward()));
	//  vec3 s_forward = normalize(cross(s_left, s_up));
	//  vec3 s_target = s_target_info.start;
	//  vec3 s_normal = s_target_info.direction;
	//
	//  mat3 s_orientation = {
	//    {s_forward[0], s_left[0], s_up[0]},
	//    {s_forward[1], s_left[1], s_up[1]},
	//    {s_forward[2], s_left[2], s_up[2]}
	//  };
	//
	//  // pass the idealized information to the unrolling transformation tool
	//  transform.setup(s_pos, s_orientation, s_target, s_normal);

	recalculate_path();

	s = path.find_nearest(car.x);
	vf = dot(car.v, car.forward());

	steer_controller(dt);

	speed_controller(dt);

	if (norm(car.x - target) < 100.0f) {
		finished = true;
	}

}

void Drive::steer_controller(float dt) {

	float t_react = 0.4f;

	// the point that car is currently chasing
	vec3 chase = path.point_at(s - fmaxf(fabs(vf) * t_react, 150.0f));

	// that same point, but in local coordinates
	//vec3 chase_local = dot(chase - car.x, s_orientation);
	vec3 chase_local = dot(chase - car.x, car.o);

	//  // if the target is out of the plane of the car, or it
	//  // has a different normal vector, then it should be unrolled
	//  if (dot(s_orientation, s_normal)[2] < 0.95f) {
	//    hare_local = transform.unroll_point(hare_local);
	//  }

	debug.push_back(dot(car.o, chase_local) + car.x);

	// angle between car's forward direction and hare_local
	float angle = atan2(chase_local[1], chase_local[0]);

	controls.steer = clip(3.0f * angle, -1.0f, 1.0f);

}

void Drive::speed_controller(float dt) {

	float dodge_delay = 0.10f;
	float dodge_duration = 1.00f;
	float dodge_dv = clip(max_speed - vf, 0.0f, 500.0f);

	float t_near = 0.30f;

	float ds_near = fmaxf(fabs(vf) * t_near, 150.0f);
	float ds_far = vf * dodge_duration + dodge_dv * (dodge_duration - dodge_delay);

	vec3 point_now = path.point_at(s);
	vec3 point_near = path.point_at(s + ds_near);
	vec3 point_far = path.point_at(s + ds_far);

	vec3 tangent_now = path.tangent_at(s);
	vec3 tangent_near = path.tangent_at(s + ds_near);
	vec3 tangent_far = path.tangent_at(s + ds_far);

	float speed;

	if (arrival_time) {

		T = fmaxf(arrival_time.value() - car.time, 0.008f);

		if (arrival_speed) {

			vec2 corrections = {
			  arrival_speed.value() - vf, // how close we are to satisfying the speed constraint
			  s - vf * T                  // how close we are to completing the path on time
			};

			mat2 invA = {
			  {-2.0f / T      ,  6.0f / (T * T)},
			  { 3.0f / (T * T), -6.0f / (T * T * T)}
			};

			speed = dot(vec2{ dt, dt*dt }, invA, corrections);

		}
		else {

			speed = s / T;

		}

	}
	else {

		if (arrival_speed) {

			speed = arrival_speed.value();

		}
		else {

			speed = max_throttle_speed;

		}

	}

	// speed constraints coming from path curvature get precedence
	for (float t = 0.0f; t < 0.6f; t += 0.1f) {
		float ds = fmaxf(fabs(vf), 50.0f) * t;
		float kappa = path.curvature_at(s - ds);
		std::cout << "path curvature: " << kappa << std::endl;
		float speed_limit = max_turning_speed(kappa);
		std::cout << "speed limit: " << speed_limit << std::endl;
		if (fabs(speed) > speed_limit) {
			speed = sgn(vf) * speed_limit;
		}
	}

	float acceleration = 0.0f;

	acceleration += kp * (speed - vf);
	acceleration += ki * std::accumulate(error_history.begin(), error_history.end(), 0.0f);
	acceleration += kd * ((speed - vf) - error_history.front());

	std::cout << vf << " " << speed << " " << acceleration << " ";

	float brake_coast_transition = -(1.25f * brake_accel + 0.00f * coasting_accel);
	float coasting_throttle_transition = -1.0f * coasting_accel;
	float throttle_boost_transition = 1.0f * throttle_accel(vf) + 1.25f * boost_accel;

	// apply brakes when the desired acceleration is negative and large enough
	if (acceleration <= brake_coast_transition) {

		std::cout << "braking" << std::endl;
		controls.throttle = -1.0f;
		controls.boost = 0;

		// let the car coast when the acceleration is negative and small
	}
	else if ((brake_coast_transition < acceleration) &&
		(acceleration <= coasting_throttle_transition)) {

		std::cout << "coasting" << std::endl;
		controls.throttle = 0.0f;
		controls.boost = 0;

		// for small positive accelerations, use throttle only
	}
	else if ((coasting_throttle_transition < acceleration) &&
		(acceleration <= throttle_boost_transition)) {

		std::cout << "throttling" << std::endl;
		controls.throttle = clip(acceleration / throttle_accel(vf), 0.02f, 1.0f);
		controls.boost = 0;

		// if the desired acceleration is big enough, use boost
	}
	else if (throttle_boost_transition < acceleration) {

		std::cout << "boosting" << std::endl;
		controls.throttle = 1.0f;
		controls.boost = 1;

	}

	error_history.push_front(speed - vf);
	error_history.pop_back();

}



bool Drive::plan_path(vec3 _target,
	float _arrival_time,
	float _arrival_speed) {

	target = _target;
	//  arrival_time = _arrival_time;
	//  arrival_speed = _arrival_speed;
	//
	//  // if the target is clearly impossible to reach, exit early
	//  float time_left = fmaxf(0.0f, arrival_time.value() - car.time);
	//  if (norm(target - car.x) > max_speed * time_left) {
	//    return false;
	//  }
	//
	//  float rmax = radius_from(vec2(car.x), normalize(vec2(car.left())), vec2(target));
	//
	//  std::cout << rmax << std::endl;
	//
	//  // if the turning circle required to reach this point is
	//  // too small, we don't try to plan a path for it
	//  if (fabs(rmax) < 300.0f) {
	//    return false;
	//  }

	float r = 1.1f / max_turning_curvature(fmaxf(norm(car.v), 500.0f));

	path = Curve(CompositeArc(200.0f,
		vec2(car.x),
		normalize(vec2(car.forward())),
		r,
		300.0f,
		vec2(target),
		vec2{ 1.0f, 0.0f },
		500.0f));

	return true;

}

void Drive::recalculate_path(bool force) {

#if 0
	// if the user has changed the requirements of the path, recalculate
	if ((norm(prev_target - target) > 5.0f) ||
		(norm(prev_tangent - tangent) > 0.1f) ||
		(fabs(prev_speed - speed) > 5.0f) || force) {

		// First, transform everything into local coordinates    
		// 
		// Rather than using the car's orientation (which can
		// be disturbed by recently contacting a ball or player),
		// we construct a more ideal orientation matrix, based
		// on the point on the field nearest to our car 
		// (along with the surface normal at that point)
		ray s_car = Field::snap(car.x);

		vec3 up = s_car.direction;
		vec3 left = normalize(cross(up, car.forward()));
		vec3 forward = normalize(cross(left, up));

		mat3 s_orientation = {
		  {forward[0], left[0], up[0]},
		  {forward[1], left[1], up[1]},
		  {forward[2], left[2], up[2]}
		};

		vec3 p1 = { 0.0f, 0.0f, 0.0f }; // position
		vec3 t1 = { 1.0f, 0.0f, 0.0f }; // tangent
		vec3 n1 = { 0.0f, 0.0f, 1.0f }; // normal

		// also snap the target to the boundary
		ray s_target = snap_to_boundary(target);

		vec3 p2 = dot(s_target.start - s_car.start, s_orientation);
		vec3 t2 = dot(tangent, s_orientation);
		vec3 n2 = dot(s_target.direction, s_orientation);

		// try to tolerate little mistakes in tangent vectors the user gives us
		t2 = normalize(t2 - dot(t2, n2) * n2);

		// if the target is out of the plane of the car, and it
		// has a different normal vector, then it should be unrolled
		//bool unroll = (p2[2] > (0.5 * r) && dot(n1, n2) < 0.95f);
		bool unroll = dot(n1, n2) < 0.95f;
		if (unroll) {
			p2 = transform.unroll_point(p2);
			t2 = transform.unroll_direction(t2);
			n2 = transform.unroll_direction(n2);
		}

		float best_time = 100.0f;
		ArcLineArc best_path;

		float v1 = norm(car.v);
		float v2 = speed;
		float v_avg = 0.5f * (v1 + v2);

		float r1 = 1.1f / max_curvature(v_avg);
		float r2 = 1.1f / max_curvature(v2);

		if (norm(t2) == 0.0f) {
			r2 = 0.0;
		}

		float t_offset = 0.25f;

		for (const int & s1 : { -1, 1 }) {
			float offset1 = v1 * t_offset;

			for (const int & s2 : { -1, 1 }) {
				float offset2 = v2 * t_offset;

				auto some_path = ArcLineArc(vec2(p1 + offset1 * t1),
					vec2(t1),
					r1*s1,
					vec2(p2 - offset2 * t2),
					vec2(t2),
					r2*s2);

				some_path.length += offset1 + offset2;

				float time = 1.1f * some_path.length / v_avg;

				if (time < best_time) {
					best_time = time;
					best_path = some_path;
				}
			}
		}

		size_t n = 32;

		std::vector < vec3 > points;

		points.push_back(vec3(p1));

		mat2 Q = rotation(sgn(best_path.r1) * best_path.phi1 / n);
		vec2 delta = best_path.p1 - best_path.o1;
		for (size_t i = 0; i < n; i++) {
			points.push_back(vec3(best_path.o1 + delta));
			delta = dot(Q, delta);
		}

		for (size_t i = 1; i < (n - 1); i++) {
			float s = float(i) / float(n - 1);
			points.push_back(vec3((1.0f - s) * best_path.q1 + s * best_path.q2));
		}

		Q = rotation(sgn(best_path.r2) * best_path.phi2 / n);
		delta = best_path.q2 - best_path.o2;
		for (size_t i = 0; i < n; i++) {
			points.push_back(vec3(best_path.o2 + delta));
			delta = dot(Q, delta);
		}

		points.push_back(vec3(p2));

		// if we unrolled earlier, we have to roll back up now
		if (unroll) {
			for (int i = 0; i < points.size(); i++) {
				points[i] = transform.roll_point(points[i]);
			}
		}

		// finally, transform all the points back into world coordinates
		for (int i = 0; i < points.size(); i++) {
			points[i] = dot(s_orientation, points[i]) + s_car.start;
		}

		path = Curve(points);

	}

	prev_target = target;
	prev_tangent = tangent;
	prev_speed = speed;

#endif
}

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
void init_drive(pybind11::module & m) {
	pybind11::class_<Drive>(m, "Drive")
		.def(pybind11::init<Car &>())
		.def_readwrite("target", &Drive::target)
		.def_readwrite("arrival_speed", &Drive::arrival_speed)
		.def_readwrite("arrival_time", &Drive::arrival_time)
		.def_readwrite("arrival_tangent", &Drive::arrival_tangent)
		.def_readwrite("finished", &Drive::finished)
		.def_readwrite("controls", &Drive::controls)
		.def_readwrite("path", &Drive::path)
		.def_readwrite("debug", &Drive::debug)
		.def_readwrite("kp", &Drive::kp)
		.def_readwrite("ki", &Drive::ki)
		.def_readwrite("kd", &Drive::kd)
		.def_readonly_static("max_speed", &Drive::max_speed)
		.def_readonly_static("max_throttle_speed", &Drive::max_throttle_speed)
		.def_readonly_static("boost_accel", &Drive::boost_accel)
		.def_readonly_static("brake_accel", &Drive::brake_accel)
		.def_readonly_static("coasting_accel", &Drive::coasting_accel)
		.def_static("max_turning_speed", &Drive::max_turning_speed)
		.def_static("max_turning_curvature", &Drive::max_turning_curvature)
		.def_static("throttle_accel", &Drive::throttle_accel)
		.def("step", &Drive::step)
		.def("plan_path", &Drive::plan_path)
		.def("recalculate_path", &Drive::recalculate_path, pybind11::arg("force") = false);
}
#endif
