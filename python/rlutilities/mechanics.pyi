from typing import *
from typing import Iterable as iterable
from typing import Iterator as iterator
from numpy import float64
_Shape = Tuple[int, ...]
import rlutilities.simulation
__all__  = [
"Aerial",
"AerialTurn",
"Boostdash",
"Dodge",
"Drive",
"FollowPath",
"Jump",
"Wavedash"
]
class Aerial():
    boost_accel = 1060.0
    boost_per_second = 30.0
    max_speed = 2300.0
    throttle_accel = 66.66667175292969

    def __init__(self, arg0: rlutilities.simulation.Car) -> None: ...
    def is_viable(self) -> bool: ...
    def simulate(self) -> rlutilities.simulation.Car: ...
    def step(self, arg0: float) -> None: ...

    @property
    def angle_threshold(self) -> float:
        pass
    
    @property
    def arrival_time(self) -> float:
        pass
    
    @property
    def boost_estimate(self) -> float:
        pass
    
    @property
    def controls(self) -> rlutilities.simulation.Input:
        pass
    
    @property
    def finished(self) -> bool:
        pass
    
    @property
    def reorient_distance(self) -> float:
        pass
    
    @property
    def target(self) -> vec3:
        pass
    
    @property
    def target_orientation(self) -> Optional[mat3]:
        pass
    
    @property
    def throttle_distance(self) -> float:
        pass
    
    @property
    def up(self) -> vec3:
        pass
    
    @property
    def velocity_estimate(self) -> vec3:
        pass
    
    pass
class AerialTurn():

    def __init__(self, arg0: rlutilities.simulation.Car) -> None: ...
    def simulate(self) -> rlutilities.simulation.Car: ...
    def step(self, arg0: float) -> None: ...

    @property
    def alpha(self) -> vec3:
        pass
    
    @property
    def controls(self) -> rlutilities.simulation.Input:
        pass
    
    @property
    def eps_omega(self) -> float:
        pass
    
    @property
    def eps_phi(self) -> float:
        pass
    
    @property
    def finished(self) -> bool:
        pass
    
    @property
    def horizon_time(self) -> float:
        pass
    
    @property
    def target(self) -> mat3:
        pass
    
    pass
class Boostdash():

    def __init__(self, arg0: rlutilities.simulation.Car) -> None: ...
    def step(self, arg0: float) -> None: ...

    @property
    def controls(self) -> rlutilities.simulation.Input:
        pass
    
    @property
    def finished(self) -> bool:
        pass
    
    pass
class Dodge():
    forward_torque = 224.0
    input_threshold = 0.5
    side_torque = 260.0
    timeout = 1.5
    torque_time = 0.6499999761581421
    z_damping = 0.3499999940395355
    z_damping_end = 0.20999999344348907
    z_damping_start = 0.15000000596046448

    def __init__(self, arg0: rlutilities.simulation.Car) -> None: ...
    def simulate(self) -> rlutilities.simulation.Car: ...
    def step(self, arg0: float) -> None: ...

    @property
    def controls(self) -> rlutilities.simulation.Input:
        pass
    
    @property
    def delay(self) -> Optional[float]:
        pass
    
    @property
    def direction(self) -> Optional[vec2]:
        pass
    
    @property
    def duration(self) -> Optional[float]:
        pass
    
    @property
    def finished(self) -> bool:
        pass
    
    @property
    def preorientation(self) -> Optional[mat3]:
        pass
    
    @property
    def target(self) -> Optional[vec3]:
        pass
    
    @property
    def timer(self) -> float:
        pass
    
    pass
class Drive():
    boost_accel = 991.6669921875
    brake_accel = 3500.0
    coasting_accel = 525.0
    max_speed = 2300.0
    max_throttle_speed = 1410.0

    def __init__(self, arg0: rlutilities.simulation.Car) -> None: ...
    @staticmethod
    def max_turning_curvature(arg0: float) -> float: ...
    @staticmethod
    def max_turning_speed(arg0: float) -> float: ...
    def step(self, arg0: float) -> None: ...
    @staticmethod
    def throttle_accel(arg0: float) -> float: ...

    @property
    def controls(self) -> rlutilities.simulation.Input:
        pass
    
    @property
    def finished(self) -> bool:
        pass
    
    @property
    def reaction_time(self) -> float:
        pass
    
    @property
    def speed(self) -> float:
        pass
    
    @property
    def target(self) -> vec3:
        pass
    
    pass
class FollowPath():

    def __init__(self, arg0: rlutilities.simulation.Car) -> None: ...
    def step(self, arg0: float) -> None: ...

    @property
    def arrival_speed(self) -> float:
        pass
    
    @property
    def arrival_time(self) -> float:
        pass
    
    @property
    def controls(self) -> rlutilities.simulation.Input:
        pass
    
    @property
    def expected_error(self) -> float:
        pass
    
    @property
    def expected_speed(self) -> float:
        pass
    
    @property
    def finished(self) -> bool:
        pass
    
    @property
    def path(self) -> rlutilities.simulation.Curve:
        pass
    
    pass
class Jump():
    acceleration = 1458.333251953125
    max_duration = 0.20000000298023224
    min_duration = 0.02500000037252903
    speed = 291.6669921875

    def __init__(self, arg0: rlutilities.simulation.Car) -> None: ...
    def simulate(self) -> rlutilities.simulation.Car: ...
    def step(self, arg0: float) -> None: ...

    @property
    def controls(self) -> rlutilities.simulation.Input:
        pass
    
    @property
    def duration(self) -> float:
        pass
    
    @property
    def finished(self) -> bool:
        pass
    
    pass
class Wavedash():

    def __init__(self, arg0: rlutilities.simulation.Car) -> None: ...
    def step(self, arg0: float) -> None: ...

    @property
    def controls(self) -> rlutilities.simulation.Input:
        pass
    
    @property
    def direction(self) -> Optional[vec2]:
        pass
    
    @property
    def finished(self) -> bool:
        pass
    
    pass