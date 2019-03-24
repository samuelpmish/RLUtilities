import math
import random

from rlbot.agents.human.controller_input import controller
from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket
from rlbot.utils.game_state_util import GameState, BallState, CarState, Physics, Vector3, Rotator

from rlutilities.mechanics import DrivePath
from rlutilities.linear_algebra import vec3, dot, normalize, norm
from rlutilities.simulation import Car, Game, Curve, ControlPoint, Ball


class Agent(BaseAgent):

    def __init__(self, name, team, index):
        self.game = Game(index, team)
        self.controls = SimpleControllerState()
        self.action = None

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.game.read_game_information(packet,
                                        self.get_rigid_body_tick(),
                                        self.get_field_info())

        if not self.action:
            self.action = DrivePath(self.game.my_car)
            self.action.target = vec3(0, 0, 0)
            self.action.arrival_tangent = vec3(1, 0, 0)
            self.action.arrival_speed = 1600
            self.action.recalculate_path()

        if controller.L1:
            time_estimate = self.action.recalculate_path()
            self.action.arrival_time = self.game.my_car.time + 1.1 * time_estimate

            self.controls = controller.get_output()

        else:
            self.action.step(self.game.time_delta)

            if self.action.finished:
                self.controls = SimpleControllerState()
            else:
                self.controls = self.action.controls

        if self.action:

            vertices = self.action.path.points

            self.renderer.begin_rendering("path")
            red = self.renderer.create_color(255, 230, 30, 30)
            for i in range(0, len(vertices)-1):
                self.renderer.draw_line_3d(vertices[i], vertices[i+1], red)

            self.renderer.draw_string_2d(50, 50, 3, 3, str(self.action.arrival_time - self.game.my_car.time), red)
            self.renderer.end_rendering()

        return self.controls
