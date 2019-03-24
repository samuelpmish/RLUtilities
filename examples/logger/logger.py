import math
import json
import random
import time

from rlbot.agents.human.controller_input import controller
from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket
from rlbot.utils.game_state_util import GameState, BallState, CarState, Physics, Vector3, Rotator

from rlutilities.mechanics import Aerial
from rlutilities.linear_algebra import vec2, vec3, mat3, look_at, norm
from rlutilities.simulation import Input, Car, Game


class Logger:

    def __init__(self):
        self.start_frame = 0
        self.frame_info = []

    def reset(self):
        self.frame_info = []

    def write_to_file(self, prefix):
        with open(f"{prefix}.json", "w") as outfile:
            json.dump(self.frame_info, outfile)

    def log(self, phys_tick, gametick, controls):

        if len(self.frame_info) == 0:
            self.start_frame = phys_tick.ball.state.frame

        self.frame_info.append({
            "frame": phys_tick.ball.state.frame,
            "ball": {
                "location": [
                    phys_tick.ball.state.location.x,
                    phys_tick.ball.state.location.y,
                    phys_tick.ball.state.location.z
                ],
                "velocity": [
                    phys_tick.ball.state.velocity.x,
                    phys_tick.ball.state.velocity.y,
                    phys_tick.ball.state.velocity.z
                ],
                "quaternion": [
                    phys_tick.ball.state.rotation.x,
                    phys_tick.ball.state.rotation.y,
                    phys_tick.ball.state.rotation.z,
                    phys_tick.ball.state.rotation.w
                ],
                "angular_velocity": [
                    phys_tick.ball.state.angular_velocity.x,
                    phys_tick.ball.state.angular_velocity.y,
                    phys_tick.ball.state.angular_velocity.z
                ]
            },
            "car": {
                "location": [
                    phys_tick.players[0].state.location.x,
                    phys_tick.players[0].state.location.y,
                    phys_tick.players[0].state.location.z
                ],
                "velocity": [
                    phys_tick.players[0].state.velocity.x,
                    phys_tick.players[0].state.velocity.y,
                    phys_tick.players[0].state.velocity.z
                ],
                "quaternion": [
                    phys_tick.players[0].state.rotation.x,
                    phys_tick.players[0].state.rotation.y,
                    phys_tick.players[0].state.rotation.z,
                    phys_tick.players[0].state.rotation.w
                ],
                "angular_velocity": [
                    phys_tick.players[0].state.angular_velocity.x,
                    phys_tick.players[0].state.angular_velocity.y,
                    phys_tick.players[0].state.angular_velocity.z
                ],
                'is_demolished': gametick.game_cars[0].is_demolished,
                'has_wheel_contact': gametick.game_cars[0].has_wheel_contact,
                'is_super_sonic': gametick.game_cars[0].is_super_sonic,
                'is_bot': gametick.game_cars[0].is_bot,
                'jumped': gametick.game_cars[0].jumped,
                'double_jumped': gametick.game_cars[0].double_jumped
            },
            "input": {
                "throttle": phys_tick.players[0].input.throttle,
                "steer": phys_tick.players[0].input.steer,
                "pitch": phys_tick.players[0].input.pitch,
                "yaw": phys_tick.players[0].input.yaw,
                "roll": phys_tick.players[0].input.roll,
                "jump": phys_tick.players[0].input.jump,
                "boost": phys_tick.players[0].input.boost,
                "handbrake": phys_tick.players[0].input.handbrake
            },
            "botinput": {
                "throttle": controls.throttle,
                "steer": controls.steer,
                "pitch": controls.pitch,
                "yaw": controls.yaw,
                "roll": controls.roll,
                "jump": controls.jump,
                "boost": controls.boost,
                "handbrake": controls.handbrake
            }
        })


class PythonExample(BaseAgent):

    def __init__(self, name, team, index):

        self.index = index
        self.json = Logger()
        self.controls = SimpleControllerState()

        self.game = Game(index, team)

        self.counter = 0
        self.log = None

    def get_output(self, packet):

        then = time.perf_counter()
        self.game.read_game_information(packet,
                                        self.get_rigid_body_tick(),
                                        self.get_field_info())
        now = time.perf_counter()

        print(now - then)

        controls = controller.get_output()

        if controller.L1:

            if not self.log:
                self.log = open(f"log_{self.counter}.ndjson", "w")
                self.counter += 1

            then = time.perf_counter()
            self.log.write(f"{{\"car\":{self.game.my_car.to_json()},"
                           f"\"ball\":{self.game.ball.to_json()}}}\n")
            now = time.perf_counter()

            print(now - then)

        else:
            if self.log:
                self.log.close()
                self.log = None

        return controls
