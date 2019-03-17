from rlutilities.linear_algebra import vec3, axis_to_rotation, look_at
from rlutilities.mechanics import AerialTurn
from rlutilities.simulation import Car

c = Car()

c.time = 0.0
c.location = vec3(0, 0, 500)
c.velocity = vec3(0, 0, 0)
c.angular_velocity = vec3(0.1, -2.0, 1.2)
c.rotation = axis_to_rotation(vec3(1.7, -0.5, 0.3))
c.on_ground = False

turn = AerialTurn(c)
turn.target = look_at(vec3(1, 0, 0), vec3(0, 0, 1))
turn.horizon_time = 0.05

simulation = turn.simulate()
print(simulation.time)
