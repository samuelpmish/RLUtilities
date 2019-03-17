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

        self.f = None
        self.action = None
        self.last_frame = 0
        self.counter = 0
        self.duration = 2

    def get_output(self, packet):

        self.game.read_game_information(packet,
                                        self.get_rigid_body_tick(),
                                        self.get_field_info())

        controls = controller.get_output()

        phys_tick = self.get_rigid_body_tick()

        if controller.L1:

            # if len(self.json.frame_info) == 0:
            #     self.action = Aerial(self.info.my_car)
            #     self.action.target = vec3(0, 0, 800)
            #     self.action.arrival_time = self.info.my_car.time + 3
            #     self.action.angle_threshold = 0.3
            #     #self.action.target_orientation = look_at(vec3(1, 0, 0), vec3(0, 0, -1))
            #     self.action.simulate()

            # self.action.step(0.016666)
            # controls = self.action.controls

            if self.counter < self.duration:
                controls.boost = self.counter % 2
            else:
                controls.boost = 0

            self.counter += 1

            if self.counter == 100:
                self.counter = 0
                self.duration += 1

            self.game.my_car.last_input.roll = controls.roll
            self.game.my_car.last_input.pitch = controls.pitch
            self.game.my_car.last_input.yaw = controls.yaw

            self.json.log(phys_tick, packet, controls)

        else:
            if self.json.frame_info:
                self.json.write_to_file("../../Utilities/analysis/boost_minimum_feather")
                self.json.reset()

        return controls

# car ball interaction
#
# if self.count == 0:
# c_position = Vector3(0, 0, 750)
# c_velocity = Vector3(0, 0, 0)
# c_rotator = Vector3(random.uniform(-0.5, 0.5), random.uniform(-0.5, 0.5), random.uniform(-0.5, -0.5))
#
# car_state = CarState(physics=Physics(
#     location=c_position,
#     velocity=c_velocity,
#     rotation=c_rotator,
#     angular_velocity=Vector3(0, 0, 0)
# ))
#
# b_position = Vector3(random.uniform(-40, 40), random.uniform(-40, 40), 950)
# b_velocity = Vector3(0, 0, random.uniform(-1000, -200))
# b_angular_velocity = Vector3(random.uniform(-5, 5), random.uniform(-5, 5), 0)
#
# ball_state = BallState(physics=Physics(
#     location=b_position,
#     velocity=b_velocity,
#     rotation=Rotator(0, 0, 0),
#     angular_velocity=b_angular_velocity
# ))
#
# self.set_game_state(GameState(
#     ball=ball_state,
#     cars={self.index: car_state}
# ))
#
# self.count = 80



