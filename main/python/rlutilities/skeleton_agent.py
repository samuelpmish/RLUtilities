from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket
from rlutilities.simulation import GameData


class SkeletonAgent(BaseAgent):

    """Base class inheriting from BaseAgent that manages data provided by the rlbot framework,
    and converts it into our internal data structure, and extracts further useful info."""

    def __init__(self, name: str = 'skeleton', team: int = 0, index: int = 0):

        super(SkeletonAgent, self).__init__(name, team, index)
        self.game_data = GameData(name, team, index)
        self.controls = SimpleControllerState()

    def get_output(self, game_tick_packet: GameTickPacket) -> SimpleControllerState:
        """Overriding this function is not advised, use update_controls() instead."""

        self.pre_process(game_tick_packet)

        self.controls = self.update_controls()

        self.feedback()

        return self.controls

    def pre_process(self, game_tick_packet: GameTickPacket):
        """First thing executed in get_output()."""
        self.game_data.read_field_info(self.get_field_info())
        self.game_data.read_game_tick_packet(game_tick_packet)

    def feedback(self):
        """Last thing executed in get_output() before return statement."""

    def update_controls(self) -> SimpleControllerState:
        """Function to override by inheriting classes"""
        return self.controls
