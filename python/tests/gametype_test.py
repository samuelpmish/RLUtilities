from rlutilities.simulation import Field, Ball, gametype

gametype("soccar")
print("Field loaded: ", Field.mode)
print("Ball's physical radius: ", Ball.radius)
print("Ball's collision radius: ", Ball.collision_radius)
print("Ball's moment of inertia: ", Ball.moment_of_inertia)
print()

gametype("hoops")
print("Field loaded: ", Field.mode)
print("Ball's physical radius: ", Ball.radius)
print("Ball's collision radius: ", Ball.collision_radius)
print("Ball's moment of inertia: ", Ball.moment_of_inertia)
print()

gametype("dropshot")
print("Field loaded: ", Field.mode)
print("Ball's physical radius: ", Ball.radius)
print("Ball's collision radius: ", Ball.collision_radius)
print("Ball's moment of inertia: ", Ball.moment_of_inertia)
