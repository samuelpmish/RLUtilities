from rlutilities.simulation import Car, Navigator
from rlutilities.mechanics import FollowPath
from rlutilities.linear_algebra import vec3, normalize

c = Car()

c.time = 0.0
c.position = vec3(0, 0, 0)
c.velocity = vec3(1000, 0, 0)
c.angular_velocity = vec3(0.1, -2.0, 1.2)
c.on_ground = False

navigator = Navigator(c)

drive = FollowPath(c)
drive.arrival_speed = 1234
drive.path = navigator.path_to(vec3(1000, 0, 0), vec3(1, 0, 0), 1000)

for p in drive.path.points:
    print(p)
