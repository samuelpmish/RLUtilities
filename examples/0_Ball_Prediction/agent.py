import math

from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.agents.human.controller_input import controller
from rlbot.utils.structures.game_data_struct import GameTickPacket

from rlutilities.simulation import Ball, Field, Game
from rlutilities.linear_algebra import vec3

class Agent(BaseAgent):

    def __init__(self, name, team, index):
        Game.set_mode("soccar")
        self.game = Game(index, team)

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.game.read_game_information(packet, self.get_rigid_body_tick(), self.get_field_info())

        # make a copy of the ball's info that we can change
        b = Ball(self.game.ball)

        ball_predictions = []
        for i in range(360):

            # simulate the forces acting on the ball for 1 frame
            b.step(1.0 / 120.0)

            # and add a copy of new ball position to the list of predictions
            ball_predictions.append(vec3(b.location))

        self.renderer.begin_rendering()
        red = self.renderer.create_color(255, 255, 30, 30)
        self.renderer.draw_polyline_3d(ball_predictions, red)
        self.renderer.end_rendering()

        return controller.get_output()
