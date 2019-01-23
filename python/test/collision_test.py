from rlutilities.linear_algebra import vec3, mat3
from rlutilities.simulation import gametype, Car, Ball, intersect

gametype("dropshot")

c = Car()

c.location = vec3(-164.13, 0, 88.79)
c.velocity = vec3(1835.87, 0, 372.271)
c.angular_velocity = vec3(0, 3.78721, 0)
c.rotation = mat3(0.9983, -5.23521e-6, 0.0582877, 5.5498e-6, 1.0, -5.23521e-6, -0.0582877, 5.5498e-6, 0.9983)

b = Ball()

b.location = vec3(0, 0, 150)
b.velocity = vec3(0, 0, 0)
b.angular_velocity = vec3(0, 0, 0)

print("before:")
print(b.location)
print(b.velocity)
print(b.angular_velocity)
print("overlapping: ", intersect(c.hitbox(), b.hitbox()))

print()

b.step(0.008333, c)

print("after:")
print(b.location)
print(b.velocity)
print(b.angular_velocity)
print("overlapping: ", intersect(c.hitbox(), b.hitbox()))
