import math

from RLUtilities.linear_algebra import *
from RLUtilities.simulation import Car, Input, Field, sphere


class DoNothing:

    def __init__(self):

        self.controls = Input()
        self.finished = True

    def step(self, dt):

        return self.finished


class Jump:

    def __init__(self, duration):

        self.duration = duration
        self.controls = Input()

        self.timer = 0
        self.counter = 0

        self.finished = False

    def step(self, dt):

        self.controls.jump = 1 if self.timer < self.duration else 0

        if self.controls.jump == 0:
            self.counter += 1

        self.timer += dt

        if self.counter >= 2:
            self.finished = True

        return self.finished


class AirDodge2:

    def __init__(self, car, jump_duration = 0.1, target = None, dodge_time = None):

        self.car = car
        self.jump_duration = jump_duration
        self.target = target
        self.controls = Input()

        self.jumped = car.on_g
        self.jump = Jump(duration)

        if duration <= 0:
            self.jump.finished = True

        self.counter = 0
        self.state_timer = 0.0
        self.total_timer = 0.0

        self.finished = False

    def step(self, dt):

        recovery_time = 0.0 if (self.target is None) else 0.4

        if not self.jump.finished:

            self.jump.step(dt)
            self.controls = self.jump.controls

        else:

            if self.counter == 0:

                # double jump
                if self.target is None:
                    self.controls.roll = 0
                    self.controls.pitch = 0
                    self.controls.yaw = 0

                # air dodge
                else:
                    target_local = dot(self.target - self.car.pos, self.car.theta)
                    target_local[2] = 0;

                    direction = normalize(target_local)

                    self.controls.roll = 0
                    self.controls.pitch = -direction[0]
                    self.controls.yaw = sgn(self.car.theta[2,2]) * direction[1]

            elif self.counter == 2:

                self.controls.jump = 1

            elif self.counter >= 4:

                self.controls.roll = 0
                self.controls.pitch = 0
                self.controls.yaw = 0
                self.controls.jump = 0

            self.counter += 1
            self.state_timer += dt

        self.finished = (self.jump.finished and
                         self.state_timer > recovery_time and
                         self.counter >= 6)



class AirDodge:

    def __init__(self, car, duration = 0.0, target = None, dodge_time = None):

        self.car = car
        self.target = target
        self.controls = Input()

        self.jump = Jump(duration)

        if duration <= 0:
            self.jump.finished = True

        self.counter = 0
        self.state_timer = 0.0
        self.total_timer = 0.0

        self.finished = False

    def step(self, dt):

        recovery_time = 0.0 if (self.target is None) else 0.4

        if not self.jump.finished:

            self.jump.step(dt)
            self.controls = self.jump.controls

        else:

            if self.counter == 0:

                # double jump
                if self.target is None:
                    self.controls.roll = 0
                    self.controls.pitch = 0
                    self.controls.yaw = 0

                # air dodge
                else:
                    target_local = dot(self.target - self.car.pos, self.car.theta)
                    target_local[2] = 0

                    direction = normalize(target_local)

                    self.controls.roll = 0
                    self.controls.pitch = -direction[0]
                    self.controls.yaw = sgn(self.car.theta[2,2]) * direction[1]

            elif self.counter == 2:

                self.controls.jump = 1

            elif self.counter >= 4:

                self.controls.roll = 0
                self.controls.pitch = 0
                self.controls.yaw = 0
                self.controls.jump = 0

            self.counter += 1
            self.state_timer += dt

        self.finished = (self.jump.finished and
                         self.state_timer > recovery_time and
                         self.counter >= 6)


# Solves a piecewise linear (PWL) equation of the form
#
# a x + b | x | + (or - ?) c == 0
#
# for -1 <= x <= 1. If no solution exists, this returns
# the x value that gets closest
def solve_PWL(a, b, c):

    xp = c/(a+b) if abs(a+b) > 10e-6 else -1
    xm = c/(a-b) if abs(a-b) > 10e-6 else  1

    if xm <= 0 <= xp:
        if abs(xp) < abs(xm):
            return clip(xp,  0, 1)
        else:
            return clip(xm, -1, 0)
    else:
        if 0 <= xp:
            return clip(xp,  0, 1)
        if xm <= 0:
            return clip(xm, -1, 0)

    return 0


# w0: beginning step angular velocity (world coordinates)
# w1: beginning step angular velocity (world coordinates)
# theta: orientation matrix
# dt: time step
def aerial_rpy(w0, w1, theta, dt):

    # car's moment of inertia (spherical symmetry)
    J = 10.5

    # aerial control torque coefficients
    T = vec3(-400.0, -130.0, 95.0)

    # aerial damping torque coefficients
    H = vec3(-50.0, -30.0, -20.0)

    # get angular velocities in local coordinates
    w0_local = dot(w0, theta)
    w1_local = dot(w1, theta)

    # PWL equation coefficients
    a = [T[i] * dt / J for i in range(0, 3)]
    b = [-w0_local[i] * H[i] * dt / J for i in range(0, 3)]
    c = [w1_local[i] - (1 + H[i] * dt / J) * w0_local[i] for i in range(0, 3)]

    # RL treats roll damping differently
    b[0] = 0

    return vec3(
      solve_PWL(a[0], b[0], c[0]),
      solve_PWL(a[1], b[1], c[1]),
      solve_PWL(a[2], b[2], c[2])
    )


class AerialTurn:

    ALPHA_MAX = 9.0

    def periodic(self, x):
        return ((x - math.pi) % (2 * math.pi)) + math.pi

    def q(self, x):
        return 1.0 - (1.0 / (1.0 + 500.0 * x * x))

    def r(self, delta, v):
        return delta - 0.5 * sgn(v) * v * v / self.ALPHA_MAX

    def controller(self, delta, v, dt):
        ri = self.r(delta, v)

        alpha = sgn(ri) * self.ALPHA_MAX

        rf = self.r(delta - v * dt, v + alpha * dt)

        # use a single step of secant method to improve
        # the acceleration when residual changes sign
        if ri * rf < 0.0:
            alpha *= (2.0 * (ri / (ri - rf)) - 1)

        return alpha

    def find_landing_orientation(self, num_points):

        f = vec3(0, 0, 0)
        l = vec3(0, 0, 0)
        u = vec3(0, 0, 0)

        dummy = Car(self.car)
        self.trajectory.append(vec3(dummy.pos))
        for i in range(0, num_points):
            dummy.step(Input(), 0.0333)
            self.trajectory.append(vec3(dummy.pos))
            u = Field.collide(sphere(dummy.pos, 100)).direction
            if norm(u) > 0.0 and i > 10:
                f = normalize(dummy.vel - dot(dummy.vel, u) * u)
                l = normalize(cross(u, f))
                self.found = True
                break

        if self.found:
            self.target = mat3(f[0], l[0], u[0],
                               f[1], l[1], u[1],
                               f[2], l[2], u[2])
        else:
            self.target = self.car.theta

    def __init__(self, car, target='Recovery', timeout=5.0):

        self.found = False
        self.car = car
        self.trajectory = []

        if target == 'Recovery':

            self.find_landing_orientation(200)

        else:

            self.target = target

        self.timeout = timeout

        self.epsilon_omega = 0.15
        self.epsilon_theta = 0.04

        self.controls = Input()

        self.timer = 0.0
        self.finished = False
        self.relative_rotation = vec3(0, 0, 0)
        self.geodesic_local = vec3(0, 0, 0)

    def step(self, dt):

        relative_rotation = dot(transpose(self.car.theta), self.target)
        geodesic_local = rotation_to_axis(relative_rotation)

        # figure out the axis of minimal rotation to target
        geodesic_world = dot(self.car.theta, geodesic_local)

        self.timer += dt

        if ((norm(self.car.omega) < self.epsilon_omega and
             norm(geodesic_world) < self.epsilon_theta) or
            self.timer >= self.timeout or self.car.on_ground):

            self.finished = True

            self.controls.roll = 0
            self.controls.pitch = 0
            self.controls.yaw = 0

        else:

            # get the angular acceleration
            alpha = vec3(
                self.controller(geodesic_world[0], self.car.omega[0], dt),
                self.controller(geodesic_world[1], self.car.omega[1], dt),
                self.controller(geodesic_world[2], self.car.omega[2], dt)
            )

            # reduce the corrections for when the solution is nearly converged
            for i in range(0, 3):
                error = abs(geodesic_world[i]) + abs(self.car.omega[i]);
                alpha[i] = self.q(error) * alpha[i]

            # set the desired next angular velocity
            omega_next = self.car.omega + alpha * dt

            # determine the controls that produce that angular velocity
            roll_pitch_yaw = aerial_rpy(self.car.omega, omega_next, self.car.theta, dt)

            self.controls.roll = roll_pitch_yaw[0]
            self.controls.pitch = roll_pitch_yaw[1]
            self.controls.yaw = roll_pitch_yaw[2]

        return self.finished


def solve_quadratic(a, b, c, interval=None):

    discriminant = b * b - 4 * a * c

    if discriminant < 0:

        return None

    else:

        x1 = (-b - sqrt(discriminant)) / (2 * a)
        x2 = (-b + sqrt(discriminant)) / (2 * a)

        if interval is None:
            return x1
        else:
            if interval[0] < x1 < interval[1]:
                return x1
            elif interval[0] < x2 < interval[1]:
                return x2
            else:
                return None


def look_at(direction, up=vec3(0, 0, 1)):

    f = normalize(direction)
    u = normalize(cross(f, cross(up, f)))
    l = normalize(cross(u, f))

    return mat3(f[0], l[0], u[0],
                f[1], l[1], u[1],
                f[2], l[2], u[2])


class Aerial:

    COUNTER = 0

    # states
    JUMP = 0
    AERIAL_APPROACH = 1

    # parameters
    jump_t = 0.25
    jump_dx = 100.0
    jump_dv = 600.0

    B = 1000.0  # boost acceleration
    g = -650.0  # gravitational acceleration
    a =    9.0  # maximum aerial angular acceleration

    def is_viable(self):

        # figure out what needs to be done in the maneuver
        self.calculate_course()
        # and if it requires an acceleration that the car
        # is unable to produce, then it is not viable
        return 0 <= self.B_avg < 0.95 * Aerial.B

    def calculate_course(self):

        z = vec3(0, 0, 1)
        P = self.target
        x0 = vec3(self.car.pos)
        v0 = vec3(self.car.vel)

        delta_t = self.t_arrival - self.car.time

        if self.car.on_ground:
            v0 += self.car.up() * Aerial.jump_dv
            x0 += self.car.up() * Aerial.jump_dx

        self.A = P - x0 - v0 * delta_t - 0.5 * Aerial.g * delta_t * delta_t * z

        self.f = normalize(self.A)

        # estimate the time required to turn
        phi = angle_between(self.car.theta, look_at(self.f, self.up))
        T = 0.7 * (2.0 * math.sqrt(phi / Aerial.a))

        # see if the boost acceleration needed to reach the target is achievable
        self.B_avg = 2.0 * norm(self.A) / ((delta_t - T) ** 2)

    def __init__(self, car, target, t_arrival, up=vec3(0, 0, 1)):

        self.car = car
        self.target = target
        self.t_arrival = t_arrival
        self.up = up

        self.controls = Input()

        self.action = None
        self.state = self.JUMP if car.on_ground else self.AERIAL_APPROACH

        self.jump_duration = 0.2
        self.double_jump = AirDodge(car, self.jump_duration)
        self.aerial_turn = AerialTurn(car, self.car.theta)

        self.A = vec3(0, 0, 0)
        self.f = vec3(0, 0, 0)
        self.B_avg = 0

        self.counter = 0
        self.state_timer = 0.0
        self.total_timer = 0.0
        self.boost_counter = 0.0

        self.finished = False

    def step(self, dt):

        old_state = self.state

        self.calculate_course()

        if norm(self.A) > 75.0:
            self.aerial_turn.target = look_at(self.f, self.up)
        else:
            self.aerial_turn.target = look_at(normalize(self.target-self.car.pos), self.up)

        self.aerial_turn.step(dt)

        if self.state == self.JUMP:

            self.double_jump.step(dt)

            self.controls = self.double_jump.controls
            if self.total_timer < self.jump_duration:
                self.controls.roll = self.aerial_turn.controls.roll
                self.controls.pitch = self.aerial_turn.controls.pitch
                self.controls.yaw = self.aerial_turn.controls.yaw

            if self.double_jump.finished:
                self.state = self.AERIAL_APPROACH

        elif self.state == self.AERIAL_APPROACH:

            # use the controls from the aerial turn correction
            self.controls = self.aerial_turn.controls

            use_boost = 0
            delta_t = self.t_arrival - self.car.time
            increment = clip(1.25 * (self.B_avg / Aerial.B), 0.0, 1.0)

            # and set the boost in a way that its duty cycle
            # approximates the desired average boost ratio
            if dot(self.car.forward(), normalize(self.f)) > 0.7:
                use_boost -= round(self.boost_counter)
                self.boost_counter += increment
                use_boost += round(self.boost_counter)

                corrected = self.A - 4 * Aerial.B * dt * delta_t * self.car.forward()

                if use_boost and norm(corrected) > norm(self.A):
                    use_boost = False
                    self.boost_counter -= increment

            #print(norm(self.A), use_boost, Aerial.B * dt * delta_t, self.car.forward(), self.car.vel)

            self.controls.boost = 1 if use_boost else 0



        if self.state == old_state:
            self.counter += 1
            self.state_timer += dt
        else:
            self.counter = 0
            self.state_timer = 0.0

        self.total_timer += dt

        self.finished = (self.car.time >= self.t_arrival)


class HalfFlip:

    def __init__(self, car, use_boost = False):
        self.car = car
        self.use_boost = use_boost
        self.controls = Input()

        duration = 0.12
        behind = car.pos - 1000.0 * car.forward()
        self.dodge = AirDodge(self.car, duration, behind)

        self.s = 0.95 * sgn(dot(self.car.omega, self.car.up()) + 0.01)

        self.timer = 0.0

        self.finished = False

    def step(self, dt):

        boost_delay = 0.4
        stall_start = 0.50
        stall_end = 0.70
        timeout = 2.0

        self.dodge.step(dt)
        self.controls = self.dodge.controls

        if stall_start < self.timer < stall_end:
            self.controls.roll  =  0.0
            self.controls.pitch = -1.0
            self.controls.yaw   =  0.0

        if self.timer > stall_end:
            self.controls.roll  =  self.s
            self.controls.pitch = -1.0
            self.controls.yaw   =  self.s

        if self.use_boost and self.timer > boost_delay:
            self.controls.boost = 1
        else:
            self.controls.boost = 0

        self.timer += dt

        self.finished = (self.timer > timeout) or \
                        (self.car.on_ground and self.timer > 0.5)



class Drive:

    __slots__ = ['car', 'target_pos', 'target_speed', 'controls', 'finished']

    def __init__(self, car, target_pos=vec3(0, 0, 0), target_speed=0):

        self.car = car
        self.target_pos = target_pos
        self.target_speed = target_speed
        self.controls = Input()

        self.finished = False

    def step(self, dt):

        max_throttle_speed = 1410
        max_boost_speed    = 2300

        # get the local coordinates of where the ball is, relative to the car
        # delta_local[0]: how far in front
        # delta_local[1]: how far to the left
        # delta_local[2]: how far above
        delta_local = dot(self.target_pos - self.car.pos, self.car.theta)

        # angle between car's forward direction and target position
        phi = math.atan2(delta_local[1], delta_local[0])

        self.controls.steer = clip(2.5 * phi, -1.0, 1.0)

        if abs(phi) > 1.7:
            #self.controls.handbrake = 1
            self.controls.handbrake = 0

        if abs(phi) < 1.5:
            self.controls.handbrake = 0

        if self.controls.handbrake == 1:

            self.controls.boost = 0

        else:

            # forward velocity
            vf = dot(self.car.vel, self.car.forward())

            if vf < self.target_speed:
                self.controls.throttle = 1.0
                if self.target_speed > max_throttle_speed:
                    self.controls.boost = 1
                else:
                    self.controls.boost = 0
            else:
                if (vf - self.target_speed) > 75:
                    self.controls.throttle = -1.0
                else:
                    if self.car.up()[2] > 0.85:
                        self.controls.throttle = 0.0
                    else:
                        self.controls.throttle = 0.01
                self.controls.boost = 0

        if norm(self.car.pos - self.target_pos) < 100:
            self.finished = True


class Wavedash:

    def __init__(self, car, target):

        self.car = car
        self.target = target

        self.controls = Input()
        self.controls.handbrake = True

#        direction = normalize(xy(target - car.pos))
#        R = axis_rotation(-0.6 * cross(vec3(0, 0, 1), direction))
#        self.turn = AerialTurn(car, dot(R, car.theta))

        direction = normalize(xy(target - (car.pos + car.vel)))

        phi = math.atan2(dot(direction, car.left()),
                         dot(direction, car.forward()))

        direction = dot(direction, axis_rotation(vec3(0, 0, phi)))

        R1 = axis_rotation(-0.5 * cross(vec3(0, 0, 1), direction))
        R2 = axis_rotation(vec3(0, 0, phi))

        self.turn = AerialTurn(car, dot(R2, dot(R1, car.theta)))

        self.counter = 0
        self.timer = 0.0

        self.finished = False

    def step(self, dt):

        dodge_time = 0.9

        if self.counter < 1:

            self.turn.step(dt)
            self.controls.roll = self.turn.controls.roll
            self.controls.pitch = self.turn.controls.pitch
            self.controls.yaw = self.turn.controls.yaw
            self.controls.jump = 1

        elif self.timer < dodge_time:

            self.turn.step(dt)
            self.controls.roll = self.turn.controls.roll
            self.controls.pitch = self.turn.controls.pitch
            self.controls.yaw = self.turn.controls.yaw
            self.controls.jump = 0

        else:

            target_local = dot(self.target - self.car.pos, self.car.theta)
            target_local[2] = 0

            direction = normalize(target_local)

            self.controls.roll = 0
            self.controls.pitch = -direction[0]
            self.controls.yaw = sgn(self.car.theta[2,2]) * direction[1]
            self.controls.jump = 1

        self.timer += dt
        self.counter += 1

        if (self.timer > 0.25 and
            self.car.on_ground and
            norm(xy(self.car.omega)) < .5):
            self.finished = True

            print(self.timer)
            print(norm(self.car.vel))
