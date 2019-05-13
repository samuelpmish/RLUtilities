from math import sin, cos
import random

from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket
from rlbot.utils.game_state_util import GameState, BallState, CarState, Physics, Vector3, Rotator

from RLUtilities.GameInfo import GameInfo
from RLUtilities.Simulation import Car, Ball, Input
from RLUtilities.LinearAlgebra import vec3, dot

from RLUtilities.Maneuvers import HalfFlip

class State:
    RESET = 0
    WAIT = 1
    INITIALIZE = 2
    RUNNING = 3

class Agent(BaseAgent):

    def __init__(self, name, team, index):
        self.index = index
        self.info = GameInfo(index, team)
        self.controls = SimpleControllerState()

        self.timer = 0.0
        self.timeout = 3.0

        self.action = None
        self.state = State.RESET

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.info.read_packet(packet)
        self.controls = SimpleControllerState()

        next_state = self.state

        if self.state == State.RESET:

            self.timer = 0.0

            # put the car in the middle of the field
            car_state = CarState(physics=Physics(
                location=Vector3(0, 0, 20),
                velocity=Vector3(1000, 0, 0),
                rotation=Rotator(0, 0, 0),
                angular_velocity=Vector3(0, 0, 0)
            ))

            # put the ball somewhere out of the way
            ball_state = BallState(physics=Physics(
                location=Vector3(0, -3000, 100),
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

            self.action = HalfFlip(self.info.my_car)

            next_state = State.RUNNING

        if self.state == State.RUNNING:

            self.action.step(0.01666)
            self.controls = self.action.controls

            if self.timer > self.timeout:
                next_state = State.RESET

        self.timer += 0.01666
        self.state = next_state

        return self.controls
