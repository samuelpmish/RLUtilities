from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.agents.human.controller_input import controller
from rlbot.utils.structures.game_data_struct import GameTickPacket

from rlutilities.simulation import Ball, Field, Game
from rlutilities.linear_algebra import vec3


class Agent(BaseAgent):

    def initialize_agent(self):
        Game.set_mode("soccar")
        self.game = Game()
        self.game.read_field_info(self.get_field_info())

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.game.read_packet(packet)

        # make a copy of the ball's info that we can change
        ball = Ball(self.game.ball)

        ball_positions = []
        for _ in range(360):

            # simulate the forces acting on the ball for 1 frame
            ball.step(1 / 120) # game physics run at 120 Hz

            # and add a copy of new ball position to the list of predictions
            ball_positions.append(vec3(ball.position))

        # render the predicted path
        self.renderer.draw_polyline_3d(ball_positions, self.renderer.yellow())

        # to play around with the ball prediction, you can control this bot with your controller
        return controller.get_output()
