from timeit import timeit
from rlbot.utils.structures.game_data_struct import FieldInfoPacket, GameTickPacket
from rlutilities import SkeletonAgent


class SkeletonAgentTest(SkeletonAgent):

    """A base class for all UnnamedAgent tests"""

    def get_field_info(self):
        return FieldInfoPacket()


def main():
    """Testing for errors and performance"""

    agent = SkeletonAgentTest("test_agent", 0, 0)
    game_tick_packet = GameTickPacket()

    def test_function():
        return agent.get_output(game_tick_packet)

    fps = 60
    n_times = 1000
    time_taken = timeit(test_function, number=n_times)
    percentage = round(time_taken * fps / n_times * 100, 5)

    print(f"Took {time_taken} seconds to run {n_times} times.")
    print(f"That's {percentage} % of our time budget.")


if __name__ == "__main__":
    main()
