import time

from RLUtilities.LinearAlgebra import *
from RLUtilities.Mechanics import Dodge
from RLUtilities.Simulation import Car

c = Car()

c.time = 0.0
c.pos = vec3(1509.38, -686.19, 17.01)
c.vel = vec3(-183.501, 1398., 8.321)
c.omega = vec3(0, 0, 0)
c.theta = mat3(-0.130158, -0.991493, -0.00117062, 
                0.991447, -0.130163, 0.00948812 , 
                -0.00955977, 0.0000743404, 0.999954)
c.theta_dodge = mat2(-0.130163, -0.991493, 0.991493, -0.130163)

c.on_ground = True
c.jumped = False
c.double_jumped = False
c.jump_timer = -1.0
c.dodge_timer = -1.0

dodge = Dodge(c)
dodge.direction = vec2(-230.03, 463.42)
dodge.duration = 0.1333
dodge.delay = 0.35

f = open("dodge_simulation.csv", "w")
for i in range(300):
    dodge.step(0.008333)
    print(c.time, dodge.controls.jump, dodge.controls.pitch, dodge.controls.yaw)
    c.step(dodge.controls, 0.008333)
    f.write(f"{c.time}, {c.pos[0]}, {c.pos[1]}, {c.pos[2]}, {c.vel[0]}, {c.vel[1]}, {c.vel[2]}, {c.omega[0]}, {c.omega[1]}, {c.omega[2]}\n")
