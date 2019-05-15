import math
import random

from rlbot.agents.human.controller_input import controller
from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket
from rlbot.utils.game_state_util import GameState, BallState, CarState, Physics, Vector3, Rotator

from rlutilities.mechanics import FollowPath
from rlutilities.linear_algebra import vec3, dot, normalize, norm
from rlutilities.simulation import Car, Game, Curve, ControlPoint, Ball, Navigator


class Agent(BaseAgent):

    def __init__(self, name, team, index):
        self.game = Game(index, team)
        self.controls = SimpleControllerState()
        self.action = None
        self.navigator = None

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.game.read_game_information(packet,
                                        self.get_rigid_body_tick(),
                                        self.get_field_info())

        x = 2000 * math.sin(self.game.time / 2.7)
        y = 4900
        z = 1250 + 300 * math.sin(self.game.time / 4.3 + 1.9)

        # put the ball somewhere out of the way
        ball_state = BallState(physics=Physics(
            location=Vector3(x, y, z),
            velocity=Vector3(0, 0, 0),
            rotation=Rotator(0, 0, 0),
            angular_velocity=Vector3(0, 0, 0)
        ))

        self.set_game_state(GameState(ball=ball_state))

        if not self.action:
            self.action = FollowPath(self.game.my_car)
            self.action.arrival_speed = 1600

            self.navigator = Navigator(self.game.my_car)

        if controller.L1:

            self.navigator.analyze_surroundings(3.0)
            self.action.path = self.navigator.path_to(vec3(x, y, z), vec3(1, 0, 0), self.action.arrival_speed)
            self.action.arrival_time = self.game.my_car.time + 3

            self.controls = controller.get_output()

        else:

            # self.controls = SimpleControllerState()

            self.action.step(self.game.time_delta)

            if self.action.finished:
                self.controls = SimpleControllerState()
            else:
                self.controls = self.action.controls

        print(f"step {self.game.time_delta}\n")

        if self.action:

            vertices = self.action.path.points

            self.renderer.begin_rendering("path")
            red = self.renderer.create_color(255, 255, 255, 255)
            for i in range(0, len(vertices)-1):
                self.renderer.draw_line_3d(vertices[i], vertices[i+1], red)

            self.renderer.draw_string_2d(50, 50, 3, 3, str(self.action.arrival_time - self.game.my_car.time), red)
            self.renderer.end_rendering()

        return self.controls
