import math
import random

from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket
from rlbot.utils.game_state_util import GameState, BallState, CarState, Physics, Vector3, Rotator

from rlutilities.linear_algebra import *
from rlutilities.mechanics import Reorient
from rlutilities.simulation import Game, Ball, Car


class Agent(BaseAgent):

    def __init__(self, name, team, index):
        self.game = Game()
        self.controls = SimpleControllerState()
        self.my_car = self.game.cars[0]

        self.timer = 0.0
        self.action = None
        random.seed(0)

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:

        self.game.read_game_information(packet,
                                        self.get_rigid_body_tick(),
                                        self.get_field_info())

        self.controls = SimpleControllerState()

        if not self.action:
            self.action = Reorient(self.my_car)
            self.action.eps_phi = 0.01
            self.action.eps_omega = 0.02

        if self.timer == 0.0:

            # randomly generate a proper orthogonal matrix
            self.action.target_orientation = axis_to_rotation(vec3(
                random.uniform(-2, 2),
                random.uniform(-2, 2),
                random.uniform(-2, 2)
            ))

        o = self.action.target_orientation
        f = vec3(o[0, 0], o[1, 0], o[2, 0])
        position = Vector3(0, 0, 1000)
        velocity = Vector3(0, 0, 0)

        car_state = CarState(physics=Physics(
            location=position,
            velocity=velocity
        ))

        # spawn the ball in front of the desired orientation to visually indicate where we're trying to go
        ball_state = BallState(physics=Physics(
            location=Vector3(500 * f[0], 500 * f[1], 500 * f[2] + 1000),
            velocity=Vector3(0, 0, 0),
            angular_velocity=Vector3(0, 0, 0)
        ))

        self.set_game_state(GameState(ball=ball_state, cars={0: car_state}))

        self.action.step(self.game.time_delta)
        self.controls = self.action.controls

        self.timer += self.game.time_delta
        if self.timer > 2.0:
            self.timer = 0.0

        error = angle_between(self.my_car.orientation, self.action.target_orientation)

        self.renderer.begin_rendering("path")
        red = self.renderer.create_color(255, 230, 30, 30)
        self.renderer.draw_string_2d(50, 50, 3, 3, f"error: {error:.4f} radians", red)
        self.renderer.draw_string_2d(50, 100, 3, 3, f"Roll:   {self.controls.roll:+.2f}", red)
        self.renderer.draw_string_2d(50, 150, 3, 3, f"Pitch: {self.controls.pitch:+.2f}", red)
        self.renderer.draw_string_2d(50, 200, 3, 3, f"Yaw:  {self.controls.yaw:+.2f}", red)
        self.renderer.end_rendering()

        return self.controls

