import math
import random

from rlbot.agents.human.controller_input import controller
from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket
from rlbot.utils.game_state_util import GameState, BallState, CarState, Physics, Vector3, Rotator

from rlutilities.mechanics import Drive
from rlutilities.linear_algebra import vec3, dot, normalize, norm
from rlutilities.simulation import Car, Game, Curve, ControlPoint, Ball


class Agent(BaseAgent):

    def __init__(self, name, team, index):
        self.game = Game(index, team)
        self.controls = SimpleControllerState()
        self.action = None
        self.counter = 0


        self.timer = 0.0
        self.data = []

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.game.read_game_information(packet,
                                        self.get_rigid_body_tick(),
                                        self.get_field_info())

        if not self.action:
            self.action = Drive(self.game.my_car)
            self.action.target = vec3(1000, 4000, 18)

            car_state = CarState(physics=Physics(
                location=Vector3(1000, -4000, 18),
                velocity=Vector3(0, self.v0, 0),
                rotation=Rotator(0, 0, 0),
                angular_velocity=Vector3(0, 0, 0)
            ), jumped=False, double_jumped=False)

            self.set_game_state(GameState(cars={self.game.id: car_state}))



        self.action.step(self.game.time_delta)

        self.controls = self.action.controls

        self.timer += self.game.time_delta

        return self.controls
