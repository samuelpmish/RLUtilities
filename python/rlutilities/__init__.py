import sys
from .rlutilities import mechanics, simulation, linear_algebra, initialize

sys.modules["rlutilities.mechanics"] = mechanics
sys.modules["rlutilities.simulation"] = simulation
sys.modules["rlutilities.linear_algebra"] = linear_algebra

#sys.modules["mechanics"] = mechanics
#sys.modules["simulation"] = simulation
#sys.modules["linear_algebra"] = linear_algebra

#from .skeleton_agent import SkeletonAgent
