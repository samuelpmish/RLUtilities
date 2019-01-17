from RLUtilities.linear_algebra import *
from RLUtilities.mechanics import AerialTurn
from RLUtilities.simulation import Car

c = Car()

c.time = 0.0
c.pos = vec3(0, 0, 500)
c.vel = vec3(0, 0, 0)
c.omega = vec3(0.1, -2.0, 1.2)
c.theta = axis_rotation(vec3(1.7, -0.5, 0.3))
c.on_ground = False

turn = AerialTurn(c)
turn.target = look_at(vec3(1, 0, 0), vec3(0, 0, 1))
turn.horizon_time = 0.05
turn.eps_phi = 0.30
turn.eps_omega = 10.0

simulation = turn.simulate()
print(simulation.time)
