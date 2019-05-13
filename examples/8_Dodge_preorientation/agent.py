from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket
from rlbot.utils.game_state_util import GameState, BallState, CarState, Physics, Vector3, Rotator

from rlutilities.linear_algebra import *
from rlutilities.mechanics import Dodge, AerialTurn
from rlutilities.simulation import Game, Ball, Car


class State:
    RESET = 0
    WAIT = 1
    INITIALIZE = 2
    RUNNING = 3


class Agent(BaseAgent):

    def __init__(self, name, team, index):
        self.game = Game(index, team)
        self.controls = SimpleControllerState()

        self.timer = 0.0
        self.timeout = 3.0
        self.counter = 0

        self.turn = None
        self.dodge = None
        self.name = None
        self.state = State.RESET

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.game.read_game_information(packet,
                                        self.get_rigid_body_tick(),
                                        self.get_field_info())
        self.controls = SimpleControllerState()

        next_state = self.state

        if self.state == State.RESET:

            self.timer = 0.0

            # put the car in the middle of the field
            car_state = CarState(physics=Physics(
                location=Vector3(-2000, 0, 18),
                velocity=Vector3(0, 0, 0),
                rotation=Rotator(0, 0, 0),
                angular_velocity=Vector3(0, 0, 0)
            ), jumped=False, double_jumped=False)

            # put the ball somewhere out of the way
            ball_state = BallState(physics=Physics(
                location=Vector3(0, 5000, 0),
                velocity=Vector3(0, 0, 0),
                rotation=Rotator(0, 0, 0),
                angular_velocity=Vector3(0, 0, 0)
            ))

            self.set_game_state(GameState(
                ball=ball_state,
                cars={self.game.id: car_state})
            )

            next_state = State.WAIT

        if self.state == State.WAIT:

            if self.timer > 0.2:
                next_state = State.INITIALIZE

        if self.state == State.INITIALIZE:

            self.dodge = Dodge(self.game.my_car)
            self.turn = AerialTurn(self.game.my_car)

            f = self.game.my_car.forward()
            l = self.game.my_car.left()
            u = self.game.my_car.up()

            # musty flick
            if self.counter % 3 == 0:
                self.name = "Musty Flick"
                self.dodge.duration = 0.2
                self.dodge.delay = 0.8
                self.dodge.direction = vec2(f)
                self.dodge.preorientation = look_at(-0.1 * f - u, -1.0 * u)

            # diagonal forward dodge
            if self.counter % 3 == 1:
                self.name = "Fast Forward Dodge"
                self.dodge.duration = 0.15
                self.dodge.delay = 0.4
                self.dodge.direction = vec2(1.0, 0.0)
                self.dodge.preorientation = dot(axis_to_rotation(vec3(0, 0, 3)), self.game.my_car.rotation)

            # diagonal twist
            if self.counter % 3 == 2:
                self.name = "Air-Roll Dodge hit"
                self.dodge.duration = 0.15
                self.dodge.delay = 0.5
                self.dodge.direction = vec2(f - 0.3 * l)
                self.dodge.preorientation = dot(self.game.my_car.rotation, axis_to_rotation(vec3(-0.8, -0.4, 0)))

            self.counter += 1

            next_state = State.RUNNING

        if self.state == State.RUNNING:

            if self.timer > 1.2:
                self.turn.target = look_at(xy(self.game.my_car.velocity), vec3(0, 0, 1))
                self.turn.step(self.game.time_delta)
                self.controls = self.turn.controls

            else:
                self.dodge.step(self.game.time_delta)
                self.controls = self.dodge.controls

            if self.timer > self.timeout:
                next_state = State.RESET

        self.game.my_car.last_input.roll = self.controls.roll
        self.game.my_car.last_input.pitch = self.controls.pitch
        self.game.my_car.last_input.yaw = self.controls.yaw

        self.timer += self.game.time_delta
        self.state = next_state

        if self.name:
            self.renderer.begin_rendering()
            self.renderer.draw_string_2d(50, 50, 6, 6, self.name, self.renderer.red())
            self.renderer.end_rendering()

        return self.controls






