from rlutilities.linear_algebra import *
from rlutilities.simulation import gametype, Car, Ball, intersect

gametype("dropshot")

c = Car()

c.pos = vec3(-164.13, 0, 88.79)
c.vel = vec3(1835.87, 0, 372.271)
c.omega = vec3(0, 3.78721, 0)
c.theta = mat3(0.9983, -5.23521e-6, 0.0582877, 5.5498e-6, 1.0, -5.23521e-6, -0.0582877, 5.5498e-6, 0.9983)

b = Ball()

b.pos = vec3(0, 0, 150)
b.vel = vec3(0, 0, 0)
b.omega = vec3(0, 0, 0)

print("before:")
print(b.pos)
print(b.vel)
print(b.omega)
print("overlapping: ", intersect(c.hitbox(), b.hitbox()))

print()

b.step(0.008333, c)

print("after:")
print(b.pos)
print(b.vel)
print(b.omega)
print("overlapping: ", intersect(c.hitbox(), b.hitbox()))
