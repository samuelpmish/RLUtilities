import math
import random

from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket
from rlbot.utils.game_state_util import GameState, BallState, CarState, Physics, Vector3, Rotator

from rlutilities.mechanics import AerialTurn
from rlutilities.simulation import GameData, gametype, Car
from rlutilities.linear_algebra import vec3, mat3, dot, euler_to_rotation


class Agent(BaseAgent):

    def __init__(self, name, team, index):
        gametype("soccar")
        self.index = index
        self.info = GameData("TurnMeRightRound", index, team)
        self.controls = SimpleControllerState()

        self.timer = 0.0
        self.action = None

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.info.read_game_tick_packet(packet)
        self.controls = SimpleControllerState()

        if self.action == None:
            self.action = AerialTurn(self.info.my_car)
            self.action.target = mat3(1, 0, 0, 0, 1, 0, 0, 0, 1)

        if self.timer < 0.05:

            r = vec3(random.uniform(-1.5, 1.5),
                     random.uniform(-1.5, 1.5),
                     random.uniform(-1.5, 1.5))

            omega = vec3(random.uniform(-3.0, 3.0),
                         random.uniform(-3.0, 3.0),
                         random.uniform(-3.0, 3.0))

            theta = euler_to_rotation(r)

            self.info.my_car.on_ground = False
            self.info.my_car.rotation = theta
            self.info.my_car.angular_velocity = omega

            dt = 1.25 * (self.action.simulate().time - self.info.my_car.time)

            position = Vector3(1000, 1000, 25 + 325 * dt * dt)
            velocity = Vector3(0, 0, 0)
            rotation = Rotator(r[0], r[1], r[2])
            angular_velocity = Vector3(omega[0], omega[1], omega[2])

            car_state = CarState(physics=Physics(
                location=position,
                velocity=velocity,
                rotation=rotation,
                angular_velocity=angular_velocity
            ))

            self.set_game_state(GameState(cars={self.index: car_state}))

        if self.timer > 0.10:

            self.action.step(1.0 / 60.0)
            self.controls = self.action.controls

            self.timer += 1.0 / 60.0
            if self.info.my_car.on_ground:
                print("target:\n", self.action.target)
                print("theta:\n", self.info.my_car.rotation)
                print()
                self.timer = 0.0
                self.action = None

        self.info.my_car.last_input.roll = self.controls.roll
        self.info.my_car.last_input.pitch = self.controls.pitch
        self.info.my_car.last_input.yaw = self.controls.yaw

        self.timer += 1.0 / 60.0
        return self.controls
