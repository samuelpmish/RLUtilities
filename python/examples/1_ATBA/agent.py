
from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket

from rlutilities.mechanics import Drive
from rlutilities.simulation import Game


class Agent(BaseAgent):

    def __init__(self, name, team, index):
        self.game = Game(index, team)
        self.controls = SimpleControllerState()
        self.action = None

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.game.read_game_information(packet,
                                        self.get_rigid_body_tick(),
                                        self.get_field_info())

        if not self.action:
            self.action = Drive(self.game.my_car)
            self.action.speed = 1400

        self.action.target = self.game.ball.location
        self.action.step(self.game.time_delta)

        self.controls = self.action.controls

        return self.controls
