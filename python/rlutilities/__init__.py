import sys
from .rlutilities import mechanics, simulation, linear_algebra, initialize

sys.modules["rlutilities.mechanics"] = mechanics
sys.modules["rlutilities.simulation"] = simulation
sys.modules["rlutilities.linear_algebra"] = linear_algebra
