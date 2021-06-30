import random

from rlbot.agents.base_agent import BaseAgent, SimpleControllerState
from rlbot.utils.structures.game_data_struct import GameTickPacket
from rlbot.utils.game_state_util import GameState, BallState, CarState, Physics, Vector3, Rotator

from rlutilities.linear_algebra import *
from rlutilities.mechanics import Aerial, Reorient
from rlutilities.simulation import Game, Ball, Car


class Agent(BaseAgent):

    def initialize_agent(self):
        Game.set_mode("soccar")
        self.game = Game()
        self.game.read_field_info(self.get_field_info())

        self.action = None

        self.timer = 0.0
        self.ball_predictions = None

    def calculate_intercept(self) -> Ball:
        # find the first future ball state that we can reach with an aerial

        prediction = Ball(self.game.ball)
        self.ball_predictions = [vec3(prediction.position)] # store predicted positions for visualization

        for _ in range(300):

            prediction.step(1 / 120)
            prediction.step(1 / 120)
            self.ball_predictions.append(vec3(prediction.position))

            # simulate the aerial for this prediction
            self.action.target_position = prediction.position
            self.action.arrival_time = prediction.time
            final_car = self.action.simulate()

            # check if the simulated aerial was successful (the car ended up near the target)
            if norm(final_car.position - self.action.target_position) < 100:
                break

        return prediction

    def get_output(self, packet: GameTickPacket) -> SimpleControllerState:
        self.game.read_packet(packet)

        # wait a bit after setting game state for the car to stabilize
        if self.timer > 0.2:

            # the Aerial object can only be used once
            if self.action is None:
                self.action = Aerial(self.game.cars[self.index])

                # Aerial calculates the forward direction to accelerate in
                # but the up direction (see the Reorient example) is optional
                # for example: if you change this to vec3(0, 0, -1), the car will aerial upside down
                # and this can (like all the other parameters) be changed throughout the duration of the aerial
                self.action.up = vec3(0, 0, 1)

                # if True, the car essentially performs a "fast" aerial
                # if False, the car only jumps once, and you can use the second jump for dodging into the ball
                self.action.double_jump = True 

                # target_orientation determines how to orient the car in the final moments where the car doesn't need to boost anymore
                # if not set, the car points towards the target position
                # self.action.target_orientation = ...

                target_ball = self.calculate_intercept()

                self.action.target_position = target_ball.position # the position to aerial to
                self.action.arrival_time = target_ball.time # the (absolute) time to arrive at the target position

            self.action.step(self.game.time_delta)

        if self.timer > 4.0:
            self.timer = 0.0
            self.reset_state()
            self.action = None

        self.timer += self.game.time_delta

        self.visualize_intercept()

        return self.action.controls if self.action else SimpleControllerState()


    # the remaining functions are for demonstration purposes

    def visualize_intercept(self):
        if self.action:
            r = 200
            x = vec3(r, 0, 0)
            y = vec3(0, r, 0)
            z = vec3(0, 0, r)
            target = self.action.target_position

            self.renderer.draw_line_3d(target - x, target + x, self.renderer.purple())
            self.renderer.draw_line_3d(target - y, target + y, self.renderer.purple())
            self.renderer.draw_line_3d(target - z, target + z, self.renderer.purple())

        if self.ball_predictions:
            self.renderer.draw_polyline_3d(self.ball_predictions, self.renderer.purple())

    def reset_state(self):
        b_position = Vector3(
            random.uniform(-1500, 1500),
            random.uniform( 2500, 3500),
            random.uniform(  300,  500)
        )

        b_velocity = Vector3(
            random.uniform(-300,  300),
            random.uniform(-100,  100),
            random.uniform(1000, 1500)
        )

        ball_state = BallState(physics=Physics(
            location=b_position,
            velocity=b_velocity,
            rotation=Rotator(0, 0, 0),
            angular_velocity=Vector3(0, 0, 0)
        ))

        # this just initializes the car and ball
        # to different starting points each time
        c_position = Vector3(b_position.x, 0 * random.uniform(-1500, -1000), 25)

        car_state = CarState(physics=Physics(
            location=c_position,
            velocity=Vector3(0, 800, 0),
            rotation=Rotator(0, 1.6, 0),
            angular_velocity=Vector3(0, 0, 0)
        ), boost_amount=100)

        self.set_game_state(GameState(
            ball=ball_state,
            cars={self.index: car_state})
        )




