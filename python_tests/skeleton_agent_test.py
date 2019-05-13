from timeit import timeit
from rlbot.utils.structures.game_data_struct import FieldInfoPacket, GameTickPacket, \
    MAX_PLAYERS, MAX_BOOSTS, MAX_GOALS
from rlutilities import SkeletonAgent


class SkeletonAgentTest(SkeletonAgent):

    field_info = FieldInfoPacket()
    field_info.num_boosts = MAX_BOOSTS
    field_info.num_goals = MAX_GOALS

    """A base class for all UnnamedAgent tests"""

    def get_field_info(self):
        return self.field_info


def main():
    """Testing for errors and performance"""

    agent = SkeletonAgentTest("test_agent", 0, 0)
    agent.initialize_agent()
    game_tick_packet = GameTickPacket()

    game_tick_packet.num_cars = MAX_PLAYERS
    game_tick_packet.num_boost = MAX_BOOSTS
    game_tick_packet.num_goals = MAX_GOALS

    def test_function():
        return agent.get_output(game_tick_packet)

    fps = 120
    n_times = 1000
    time_taken = timeit(test_function, number=n_times)
    percentage = round(time_taken * fps / n_times * 100, 5)

    print(f"Took {time_taken} seconds to run {n_times} times.")
    print(f"That's {percentage} % of our time budget.")


if __name__ == "__main__":
    main()
