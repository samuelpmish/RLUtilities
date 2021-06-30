from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket
from rlbot.utils.game_state_util import GameState, BallState, CarState, Physics, Vector3, Rotator

from rlutilities.linear_algebra import *
from rlutilities.mechanics import Boostdash
from rlutilities.simulation import Game, Ball, Car


class Agent(BaseAgent):

    def initialize_agent(self):
        Game.set_mode("soccar")
        self.game = Game()
        self.game.read_field_info(self.get_field_info())

        # the mechanic cannot be instantiated here, because we need to read a packet to get the Car object first
        self.action = None

        self.timer = 0.0

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.game.read_packet(packet)

        # wait a bit after setting game state for the car to stabilize
        if self.timer > 0.5:

            # the Boostdash object can only be used once
            if self.action is None:
                self.action = Boostdash(self.game.cars[self.index])
                # Boostdash does not have any parameters
                
            self.action.step(self.game.time_delta)

        if self.timer > 3.0:
            self.timer = 0.0
            self.action = None
            self.reset_state()

        self.timer += self.game.time_delta

        return self.action.controls if self.action else SimpleControllerState()

    def reset_state(self):
        # put the car in the middle of the field
        car_state = CarState(physics=Physics(
            location=Vector3(-3000, -500, 18),
            velocity=Vector3(1200, 0, 0),
            rotation=Rotator(0, 0, 0),
            angular_velocity=Vector3(0, 0, 0)
        ), boost_amount=100)

        # put the ball somewhere out of the way
        ball_state = BallState(physics=Physics(
            location=Vector3(0, 0, 100),
            velocity=Vector3(0, 0, 0),
            rotation=Rotator(0, 0, 0),
            angular_velocity=Vector3(0, 0, 0)
        ))

        self.set_game_state(GameState(
            ball=ball_state,
            cars={self.index: car_state})
        )
