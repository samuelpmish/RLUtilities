import math
import random

from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket
from rlbot.utils.game_state_util import GameState, BallState, CarState, Physics, Vector3, Rotator

from rlutilities.linear_algebra import *
from rlutilities.mechanics import Reorient
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

        # the Reorient mechanic only needs to be instantiated once
        if not self.action:
            self.action = Reorient(self.game.cars[self.index])
        
        forward = self.game.ball.position - self.game.cars[self.index].position # the direction to point the nose of the car
        up = vec3(0, 0, 1) # the direction to orient the roof of the car, as close as possible
        self.action.target_orientation = look_at(forward, up) # look_at combines the forward and up directions and creates an orientation matrix
        
        # execute the mechanic
        self.action.step(self.game.time_delta)

        # change target every 2 seconds, for demonstration purposes
        self.timer += self.game.time_delta
        if self.timer > 2.0:
            self.timer = 0.0
            self.randomly_move_ball()

        self.keep_car_in_air()
        self.visualize_orientations()

        return self.action.controls




    # the following functions are just for demonstration purposes

    def randomly_move_ball(self):
        random_vec = vec3(random.uniform(-2, 2), random.uniform(-2, 2), random.uniform(-2, 2))
        pos_on_sphere = vec3(0, 0, 1000) + normalize(random_vec) * 500

        ball_state = BallState(physics=Physics(
            location=Vector3(pos_on_sphere.x, pos_on_sphere.y, pos_on_sphere.z),
            velocity=Vector3(0, 0, 0),
            angular_velocity=Vector3(0, 0, 0)
        ))
        self.set_game_state(GameState(ball=ball_state))

    def keep_car_in_air(self):
        car_state = CarState(physics=Physics(
            location=Vector3(0, 0, 1000),
            velocity=Vector3(0, 0, 0)
        ))
        self.set_game_state(GameState(cars={self.index: car_state}))

    def visualize_orientations(self):
        car_pos = self.game.cars[self.index].position

        c_o = self.game.cars[self.index].orientation # car orientation
        t_o = self.action.target_orientation # target orientation

        self.renderer.draw_line_3d(car_pos, car_pos + vec3(c_o[0, 0], c_o[1, 0], c_o[2, 0]) * 200, self.renderer.red())
        self.renderer.draw_line_3d(car_pos, car_pos + vec3(c_o[0, 2], c_o[1, 2], c_o[2, 2]) * 200, self.renderer.red())

        self.renderer.draw_line_3d(car_pos, car_pos + vec3(t_o[0, 0], t_o[1, 0], t_o[2, 0]) * 200, self.renderer.lime())
        self.renderer.draw_line_3d(car_pos, car_pos + vec3(t_o[0, 2], t_o[1, 2], t_o[2, 2]) * 200, self.renderer.lime())
