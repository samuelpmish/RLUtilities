import math

from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket

from RLUtilities.GameInfo import GameInfo
from RLUtilities.Simulation import Ball, Field
from RLUtilities.LinearAlgebra import vec3

from RLUtilities.controller_input import controller

class Agent(BaseAgent):

    def __init__(self, name, team, index):
        Field.initialize_soccar()
        self.info = GameInfo(index, team)

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.info.read_packet(packet)

        # make a copy of the ball's info that we can change
        b = Ball(self.info.ball)

        ball_predictions = []
        for i in range(360):

            # simulate the forces acting on the ball for 1 frame
            b.step(1.0 / 120.0)

            # and add a copy of new ball position to the list of predictions
            ball_predictions.append(vec3(b.pos))

        self.renderer.begin_rendering()
        red = self.renderer.create_color(255, 255, 30, 30)
        self.renderer.draw_polyline_3d(ball_predictions, red)
        self.renderer.end_rendering()

        return controller.get_output()
