# help python find the module files
import sys
sys.path.append("../../")

from Utilities.Simulation import Input
from Utilities.Maneuvers import HalfFlip
from Utilities.Goal import Goal
from Utilities.GameInfo import GameInfo, BoostPad
from Utilities.LinearAlgebra import mat3

from controller_input import controller

from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket

def convert_input(original):
    converted = SimpleControllerState()
    converted.steer = original.steer
    converted.throttle = original.throttle
    converted.pitch = original.pitch
    converted.yaw = original.yaw
    converted.roll = original.roll
    converted.jump = original.jump
    converted.boost = original.boost
    converted.handbrake = original.slide
    return converted

class PythonExample(BaseAgent):

    def __init__(self, name, team, index):
        self.action = None
        self.controls = Input()
        self.info = GameInfo(index, team)

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:

        self.info.read_packet(packet)

        if self.action is None:
            self.action = HalfFlip(self.info.my_car, True)

        if controller.L1:
            self.controls = controller.get_output()
            self.action = None
        else:
            self.action.step(0.016666)
            self.controls = convert_input(self.action.controls)
            if self.action.timer > 3:
                self.action = None

        return self.controls
