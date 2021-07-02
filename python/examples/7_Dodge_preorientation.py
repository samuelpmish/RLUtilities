from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket
from rlbot.utils.game_state_util import GameState, BallState, CarState, Physics, Vector3, Rotator

from rlutilities.linear_algebra import *
from rlutilities.mechanics import Dodge, Reorient
from rlutilities.simulation import Game, Ball, Car


class State:
    RESET = 0
    WAIT = 1
    INITIALIZE = 2
    RUNNING = 3


class Agent(BaseAgent):

    def initialize_agent(self):
        Game.set_mode("soccar")
        self.game = Game()
        self.game.read_field_info(self.get_field_info())

        self.controls = SimpleControllerState()

        self.timer = 0.0
        self.timeout = 3.0
        self.counter = 0

        self.turn = None
        self.dodge = None
        self.maneuver_name = None
        self.state = State.RESET

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.game.read_packet(packet)
        self.controls = SimpleControllerState()

        next_state = self.state

        my_car = self.game.cars[self.index]

        if self.state == State.RESET:

            self.timer = 0.0

            # put the car in the middle of the field
            car_state = CarState(physics=Physics(
                location=Vector3(-2000, 0, 18),
                velocity=Vector3(0, 0, 0),
                rotation=Rotator(0, 0, 0),
                angular_velocity=Vector3(0, 0, 0)
            ))

            # put the ball somewhere out of the way
            ball_state = BallState(physics=Physics(
                location=Vector3(0, 5000, 0),
                velocity=Vector3(0, 0, 0),
                rotation=Rotator(0, 0, 0),
                angular_velocity=Vector3(0, 0, 0)
            ))

            self.set_game_state(GameState(
                ball=ball_state,
                cars={self.index: car_state})
            )

            next_state = State.WAIT

        if self.state == State.WAIT:

            if self.timer > 0.2:
                next_state = State.INITIALIZE

        if self.state == State.INITIALIZE:

            self.dodge = Dodge(my_car)
            self.turn = Reorient(my_car)

            f = my_car.forward()
            l = my_car.left()
            u = my_car.up()

            # musty flick
            if self.counter % 3 == 0:
                self.maneuver_name = "Musty Flick"
                self.dodge.jump_duration = 0.2
                self.dodge.delay = 0.8
                self.dodge.direction = vec2(f)
                self.dodge.preorientation = look_at(-0.1 * f - u, -1.0 * u)

            # diagonal forward dodge
            if self.counter % 3 == 1:
                self.maneuver_name = "Fast Forward Dodge"
                self.dodge.jump_duration = 0.15
                self.dodge.delay = 0.4
                self.dodge.direction = vec2(1.0, 0.0)
                self.dodge.preorientation = dot(axis_to_rotation(vec3(0, 0, 3)), my_car.orientation)

            # diagonal twist
            if self.counter % 3 == 2:
                self.maneuver_name = "Air-Roll Dodge hit"
                self.dodge.jump_duration = 0.15
                self.dodge.delay = 0.5
                self.dodge.direction = vec2(f - 0.3 * l)
                self.dodge.preorientation = dot(my_car.orientation, axis_to_rotation(vec3(-0.8, -0.4, 0)))

            self.counter += 1

            next_state = State.RUNNING

        if self.state == State.RUNNING:

            if self.timer > 1.2:
                self.turn.target_orientation = look_at(xy(my_car.velocity), vec3(0, 0, 1))
                self.turn.step(self.game.time_delta)
                self.controls = self.turn.controls

            else:
                self.dodge.step(self.game.time_delta)
                self.controls = self.dodge.controls

            if self.timer > self.timeout:
                next_state = State.RESET

        self.timer += self.game.time_delta
        self.state = next_state

        if self.maneuver_name:
            self.renderer.begin_rendering()
            self.renderer.draw_string_2d(50, 50, 6, 6, self.maneuver_name, self.renderer.red())
            self.renderer.end_rendering()

        return self.controls






