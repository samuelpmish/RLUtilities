from rlutilities.simulation import Car
from rlutilities.mechanics import DrivePath
from rlutilities.linear_algebra import vec3, normalize

c = Car()

c.time = 0.0
c.location = vec3(0, 0, 0)
c.velocity = vec3(1000, 0, 0)
c.angular_velocity = vec3(0.1, -2.0, 1.2)
c.on_ground = False

drive = DrivePath(c)
drive.target = vec3(1600, 400, 0)
drive.arrival_tangent = normalize(vec3(1, 1, 0))
drive.arrival_speed = 1234

drive.recalculate_path()

for p in drive.path.points:
    print(p)
