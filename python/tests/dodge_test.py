from rlutilities.linear_algebra import vec3, vec2, mat3, mat2
from rlutilities.mechanics import Dodge
from rlutilities.simulation import Car

c = Car()

c.time = 0.0
c.position = vec3(1509.38, -686.19, 17.01)
c.velocity = vec3(-183.501, 1398., 8.321)
c.angular_velocity = vec3(0, 0, 0)
c.orientation = mat3(-0.130158, -0.991493, -0.00117062,
                  0.991447, -0.130163, 0.00948812,
                  -0.00955977, 0.0000743404, 0.999954)

c.on_ground = True
c.jumped = False
c.double_jumped = False
c.jump_timer = -1.0
c.dodge_timer = -1.0

dodge = Dodge(c)
dodge.direction = vec2(-230.03, 463.42)
dodge.jump_duration = 0.1333
dodge.delay = 0.35

f = open("dodge_simulation.csv", "w")
for i in range(300):
    dodge.step(0.008333)
    print(c.time, dodge.controls.jump, dodge.controls.pitch, dodge.controls.yaw)
    c.step(dodge.controls, 0.008333)
    f.write(f"{c.time}, {c.position[0]}, {c.position[1]}, {c.position[2]}, "
            f"{c.velocity[0]}, {c.velocity[1]}, {c.velocity[2]}, "
            f"{c.angular_velocity[0]}, {c.angular_velocity[1]}, {c.angular_velocity[2]}\n")
