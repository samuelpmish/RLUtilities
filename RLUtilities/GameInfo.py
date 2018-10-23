from Simulation import Ball, Pitch, Car
from LinearAlgebra import vec3, euler_rotation

from Goal import Goal

class BoostPad:

    def __init__(self, index, pos, is_active, timer):
        self.index = index
        self.pos = pos
        self.is_active = is_active
        self.timer = timer

class GameInfo:

    DT = 0.01666

    def __init__(self, index, team, fieldInfo = None):

        self.time = 0
        self.ball = Ball()
        self.pitch = Pitch()

        self.my_goal = Goal(team)
        self.their_goal = Goal(1 - team)

        self.team = team
        self.index = index

        self.cars = []
        self.teammates = []
        self.opponents = []

        self.my_car = Car()

        if fieldInfo == None:
            self.boost_pads = [
                BoostPad( 3, vec3(-3072.0, -4096.0, 73.0), True, 0.0),
                BoostPad( 4, vec3( 3072.0, -4096.0, 73.0), True, 0.0),
                BoostPad(15, vec3(-3584.0,     0.0, 73.0), True, 0.0),
                BoostPad(18, vec3( 3584.0,     0.0, 73.0), True, 0.0),
                BoostPad(29, vec3(-3072.0,  4096.0, 73.0), True, 0.0),
                BoostPad(30, vec3( 3072.0,  4096.0, 73.0), True, 0.0)
            ]
        else:
            self.boost_pads = []
            for i in range(fieldInfo.num_boosts):
                current = fieldInfo.boost_pads[i]
                if current.is_full_boost:
                    self.boost_pads.append(BoostPad(i, vec3(current.location.x, current.location.y, current.location.z), True, 0.0))

        self.ball_predictions = []

        self.about_to_score = None
        self.about_to_be_scored_on = None
        self.time_of_goal = -1

    def read_packet(self, packet):

        self.time = packet.game_info.seconds_elapsed

        dyn = packet.game_ball.physics

        self.ball.pos = vec3(dyn.location.x,
                             dyn.location.y,
                             dyn.location.z)
        self.ball.vel = vec3(dyn.velocity.x,
                             dyn.velocity.y,
                             dyn.velocity.z)
        self.ball.omega = vec3(dyn.angular_velocity.x,
                               dyn.angular_velocity.y,
                               dyn.angular_velocity.z)
        self.ball.t = self.time

        self.ball.step(GameInfo.DT)

        for i in range(0, packet.num_cars):

            game_car = packet.game_cars[i]

            dyn = game_car.physics

            car = Car()

            if i < len(self.cars):
                car = self.cars[i]

            car.pos = vec3(dyn.location.x,
                           dyn.location.y,
                           dyn.location.z)
            car.vel = vec3(dyn.velocity.x,
                           dyn.velocity.y,
                           dyn.velocity.z)
            car.omega = vec3(dyn.angular_velocity.x,
                             dyn.angular_velocity.y,
                             dyn.angular_velocity.z)
            car.theta = euler_rotation(vec3(dyn.rotation.pitch,
                                            dyn.rotation.yaw,
                                            dyn.rotation.roll))
            car.on_ground = game_car.has_wheel_contact
            car.supersonic = game_car.is_super_sonic
            car.jumped = game_car.jumped
            car.double_jumped = game_car.double_jumped
            car.boost = game_car.boost
            car.time = self.time

            car.extrapolate(GameInfo.DT)

            if len(self.cars) <= i:

                car.id = i
                car.team = game_car.team
                self.cars.append(car)

                if game_car.team == self.team:
                    if i == self.index:
                        self.my_car = car
                    else:
                        self.teammates.append(car)
                else:
                    self.opponents.append(car)

        for i in range(0, len(self.boost_pads)):

            boost_pad = packet.game_boosts[self.boost_pads[i].index]

            self.boost_pads[i].is_active = boost_pad.is_active

            self.boost_pads[i].timer = boost_pad.timer

        self.time += GameInfo.DT

    def predict_ball(self, num_steps, dt):

        self.about_to_score = False
        self.about_to_be_scored_on = False
        self.time_of_goal = -1

        self.ball_predictions = [Ball(self.ball)]
        prediction = Ball(self.ball)
        for i in range(0, num_steps):
            prediction.step(dt)
            self.ball_predictions.append(Ball(prediction))

            if self.time_of_goal == -1:
                if self.my_goal.inside(prediction.pos):
                    self.about_to_be_scored_on = True
                    self.time_of_goal = prediction.t
                if self.their_goal.inside(prediction.pos):
                    self.about_to_score = True
                    self.time_of_goal = prediction.t
