from RLUtilities.linear_algebra import vec3, dot, norm, cross
import math


class Goal:

    WIDTH = 1784.0
    HEIGHT = 640.0
    DISTANCE = 5120.0

    def __init__(self, team, fieldInfo=None):

        self.team = team
        self.sign = -1 if team == 0 else 1
        if fieldInfo is None:
            self.center = vec3(0, self.sign * Goal.DISTANCE, Goal.HEIGHT / 2.0)
            self.corners = [
                vec3(-Goal.WIDTH / 2.0, self.sign * Goal.DISTANCE, 0),
                vec3(Goal.WIDTH / 2.0, self.sign * Goal.DISTANCE, 0),
                vec3(Goal.WIDTH / 2.0, self.sign * Goal.DISTANCE, Goal.HEIGHT),
                vec3(-Goal.WIDTH / 2.0, self.sign * Goal.DISTANCE, Goal.HEIGHT)
            ]
        else:
            for i in range(len(fieldInfo.goals)):
                current = fieldInfo.goals[i]
                current_pos = current.location
                if current.team_num == team:
                    self.center = vec3(current_pos.x, current_pos.y, current_pos.z)
                    if abs(self.center[1]) > 5000:
                        self.corners = [
                            vec3(current_pos.x - Goal.WIDTH / 2.0, current_pos.y, current_pos.z - Goal.HEIGHT / 2.0),
                            vec3(current_pos.x + Goal.WIDTH / 2.0, current_pos.y, current_pos.z - Goal.HEIGHT / 2.0),
                            vec3(current_pos.x + Goal.WIDTH / 2.0, current_pos.y, current_pos.z + Goal.HEIGHT / 2.0),
                            vec3(current_pos.x - Goal.WIDTH / 2.0, current_pos.y, current_pos.z + Goal.HEIGHT / 2.0)
                        ]
                    else:
                        radius = 3564 - abs(current_pos.y)
                        self.corners = [
                            vec3(-radius, current_pos.y - radius, current_pos.z * 2.0),
                            vec3(radius, current_pos.y - radius, current_pos.z * 2.0),
                            vec3(radius, current_pos.y + radius, current_pos.z * 2.0),
                            vec3(-radius, current_pos.y + radius, current_pos.z * 2.0)
                        ]
                    break

    def solid_angle(self, p):

        Omega = 0.0

        a = self.corners[0] - p
        b = self.corners[1] - p
        c = self.corners[2] - p

        numerator = abs(dot(a, cross(b, c)))
        denominator = norm(a) * norm(b) * norm(c) + \
            dot(a, b) * norm(c) +                   \
            dot(b, c) * norm(a) +                   \
            dot(c, a) * norm(b)

        angle = 2 * math.atan(numerator / denominator)

        if angle < 0:
            angle += 2 * math.pi

        Omega += angle

        a = self.corners[2] - p
        b = self.corners[3] - p
        c = self.corners[0] - p

        numerator = abs(dot(a, cross(b, c)))
        denominator = norm(a) * norm(b) * norm(c) + \
            dot(a, b) * norm(c) + \
            dot(b, c) * norm(a) + \
            dot(c, a) * norm(b)

        angle = 2 * math.atan(numerator / denominator)

        if angle < 0:
            angle += 2 * math.pi

        Omega += angle

        return Omega

    def inside(self, p):
        if self.team == 0:
            return p[1] < -Goal.DISTANCE
        else:
            return p[1] > Goal.DISTANCE
