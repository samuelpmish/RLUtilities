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

    def initialize_agent(self):
        Game.set_mode("soccar")
        self.game = Game()
        self.game.read_field_info(self.get_field_info())

        self.action = None
        self.navigator = None

        self.timer = 0.0

        self.controls = SimpleControllerState()

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.game.read_packet(packet)

        my_car = self.game.cars[self.index]

        if self.timer > 0.2:

            if not self.action:

                self.action = FollowPath(my_car)
                self.navigator = Navigator(my_car)

                self.navigator.analyze_surroundings(3.0)

                x = 2000 * math.sin(self.game.time / 2.7)
                y = 4900
                z = 1250 + 200 * math.sin(self.game.time / 4.3 + 1.9)
                path = self.navigator.path_to(vec3(x, y, z), vec3(1, 0, 0), 1600)

                self.action.calculate_plan(path, my_car.time + 6.0, 1600)

                self.controls = SimpleControllerState()

            else:

                self.action.step(self.game.time_delta)
                self.controls = self.action.controls

                if self.game.time > self.action.arrival_time:
                    self.action = None
                    self.reset_state()
                    self.timer = 0.0

        if self.action:

            vertices = self.action.path.points

            self.renderer.begin_rendering("path")
            self.renderer.draw_polyline_3d(vertices, self.renderer.red())

            self.renderer.draw_string_2d(50, 50, 3, 3, f"{self.action.arrival_time - my_car.time:.2f}", self.renderer.red())
            self.renderer.end_rendering()

        self.timer += self.game.time_delta

        return self.controls


    def reset_state(self):

        # put the ball somewhere out of the way
        ball_state = BallState(physics=Physics(
            location=Vector3(0, 4500, 100),
            velocity=Vector3(0, 0, 0),
            rotation=Rotator(0, 0, 0),
            angular_velocity=Vector3(0, 0, 0)
        ))

        # put the car in the center
        car_state = CarState(physics=Physics(
            location=Vector3(0, 0, 30),
            velocity=Vector3(0, 0, 0),
            rotation=Rotator(0, math.pi/2, 0),
            angular_velocity=Vector3(0, 0, 0)
        ), boost_amount=100)

        self.set_game_state(GameState(
            ball=ball_state,
            cars={self.index: car_state}
        ))
