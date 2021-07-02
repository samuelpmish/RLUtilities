
from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket

from rlutilities.mechanics import Drive
from rlutilities.simulation import Game


class Agent(BaseAgent):

    def initialize_agent(self):
        Game.set_mode("soccar")
        self.game = Game()
        self.game.read_field_info(self.get_field_info())

        # the mechanic cannot be instantiated here, because we need to read a packet to get the Car object first
        self.action = None

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.game.read_packet(packet)

        # the Drive mechanic only needs to be instantiated once
        if self.action is None:
            self.action = Drive(self.game.cars[self.index])
            self.action.speed = 1400 # the speed controller will attempt to drive at this speed

        # if the target position changes, it needs to be updated (in the case of the ball position, every tick)
        self.action.target = self.game.ball.position

        # execute the mechanic
        self.action.step(self.game.time_delta)

        # and use the calculated controls
        return self.action.controls
