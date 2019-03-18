from typing import *
from typing import Iterable as iterable
from typing import Iterator as iterator
from numpy import float64
_Shape = Tuple[int, ...]
import rlutilities.rlutilities.simulation
__all__  = [
"Aerial",
"AerialTurn",
"Boostdash",
"Dodge",
"Drive",
"DrivePath",
"Jump",
"Wavedash"
]
class Aerial():
    def __init__(self, arg0: rlutilities.rlutilities.simulation.Car) -> None: ...
    def is_viable(self) -> bool: ...
    def simulate(self) -> rlutilities.rlutilities.simulation.Car: ...
    def step(self, arg0: float) -> None: ...
    @property
    def angle_threshold(self) -> float:
        """
:type: float"""
    @angle_threshold.setter
    def angle_threshold(self, arg0: float) -> None:
        pass
    @property
    def arrival_time(self) -> float:
        """
:type: float"""
    @arrival_time.setter
    def arrival_time(self, arg0: float) -> None:
        pass
    @property
    def boost_estimate(self) -> float:
        """
:type: float"""
    @property
    def controls(self) -> rlutilities.rlutilities.simulation.Input:
        """
:type: rlutilities.rlutilities.simulation.Input"""
    @property
    def finished(self) -> bool:
        """
:type: bool"""
    @property
    def reorient_distance(self) -> float:
        """
:type: float"""
    @reorient_distance.setter
    def reorient_distance(self, arg0: float) -> None:
        pass
    @property
    def target(self) -> vec<3>:
        """
:type: vec<3>"""
    @target.setter
    def target(self, arg0: vec<3>) -> None:
        pass
    @property
    def target_orientation(self) -> Optional[mat<3,3>]:
        """
:type: Optional[mat<3,3>]"""
    @target_orientation.setter
    def target_orientation(self, arg0: Optional[mat<3,3>]) -> None:
        pass
    @property
    def throttle_distance(self) -> float:
        """
:type: float"""
    @throttle_distance.setter
    def throttle_distance(self, arg0: float) -> None:
        pass
    @property
    def up(self) -> vec<3>:
        """
:type: vec<3>"""
    @up.setter
    def up(self, arg0: vec<3>) -> None:
        pass
    @property
    def velocity_estimate(self) -> vec<3>:
        """
:type: vec<3>"""
    pass
class AerialTurn():
    def __init__(self, arg0: rlutilities.rlutilities.simulation.Car) -> None: ...
    def simulate(self) -> rlutilities.rlutilities.simulation.Car: ...
    def step(self, arg0: float) -> None: ...
    def time_estimate(self) -> float: ...
    @property
    def controls(self) -> rlutilities.rlutilities.simulation.Input:
        """
:type: rlutilities.rlutilities.simulation.Input"""
    @controls.setter
    def controls(self, arg0: rlutilities.rlutilities.simulation.Input) -> None:
        pass
    @property
    def eps_omega(self) -> float:
        """
:type: float"""
    @eps_omega.setter
    def eps_omega(self, arg0: float) -> None:
        pass
    @property
    def eps_phi(self) -> float:
        """
:type: float"""
    @eps_phi.setter
    def eps_phi(self, arg0: float) -> None:
        pass
    @property
    def finished(self) -> bool:
        """
:type: bool"""
    @finished.setter
    def finished(self, arg0: bool) -> None:
        pass
    @property
    def horizon_time(self) -> float:
        """
:type: float"""
    @horizon_time.setter
    def horizon_time(self, arg0: float) -> None:
        pass
    @property
    def target(self) -> mat<3,3>:
        """
:type: mat<3,3>"""
    @target.setter
    def target(self, arg0: mat<3,3>) -> None:
        pass
    pass
class Boostdash():
    def __init__(self, arg0: rlutilities.rlutilities.simulation.Car) -> None: ...
    def step(self, arg0: float) -> None: ...
    @property
    def controls(self) -> rlutilities.rlutilities.simulation.Input:
        """
:type: rlutilities.rlutilities.simulation.Input"""
    @property
    def finished(self) -> bool:
        """
:type: bool"""
    pass
class Dodge():
    def __init__(self, arg0: rlutilities.rlutilities.simulation.Car) -> None: ...
    def simulate(self) -> rlutilities.rlutilities.simulation.Car: ...
    def step(self, arg0: float) -> None: ...
    @property
    def controls(self) -> rlutilities.rlutilities.simulation.Input:
        """
:type: rlutilities.rlutilities.simulation.Input"""
    @property
    def delay(self) -> Optional[float]:
        """
:type: Optional[float]"""
    @delay.setter
    def delay(self, arg0: Optional[float]) -> None:
        pass
    @property
    def direction(self) -> Optional[vec<2>]:
        """
:type: Optional[vec<2>]"""
    @direction.setter
    def direction(self, arg0: Optional[vec<2>]) -> None:
        pass
    @property
    def duration(self) -> Optional[float]:
        """
:type: Optional[float]"""
    @duration.setter
    def duration(self, arg0: Optional[float]) -> None:
        pass
    @property
    def finished(self) -> bool:
        """
:type: bool"""
    @property
    def preorientation(self) -> Optional[mat<3,3>]:
        """
:type: Optional[mat<3,3>]"""
    @preorientation.setter
    def preorientation(self, arg0: Optional[mat<3,3>]) -> None:
        pass
    @property
    def target(self) -> Optional[vec<3>]:
        """
:type: Optional[vec<3>]"""
    @target.setter
    def target(self, arg0: Optional[vec<3>]) -> None:
        pass
    pass
class Drive():
    def __init__(self, arg0: rlutilities.rlutilities.simulation.Car) -> None: ...
    @staticmethod
    def max_turning_curvature(arg0: float) -> float: ...
    @staticmethod
    def max_turning_speed(arg0: float) -> float: ...
    def step(self, arg0: float) -> None: ...
    @staticmethod
    def throttle_accel(arg0: float) -> float: ...
    @property
    def acceleration(self) -> float:
        """
:type: float"""
    @acceleration.setter
    def acceleration(self, arg0: float) -> None:
        pass
    @property
    def controls(self) -> rlutilities.rlutilities.simulation.Input:
        """
:type: rlutilities.rlutilities.simulation.Input"""
    @controls.setter
    def controls(self, arg0: rlutilities.rlutilities.simulation.Input) -> None:
        pass
    @property
    def debug(self) -> List[vec<3>]:
        """
:type: List[vec<3>]"""
    @debug.setter
    def debug(self, arg0: List[vec<3>]) -> None:
        pass
    @property
    def finished(self) -> bool:
        """
:type: bool"""
    @finished.setter
    def finished(self, arg0: bool) -> None:
        pass
    @property
    def speed(self) -> float:
        """
:type: float"""
    @speed.setter
    def speed(self, arg0: float) -> None:
        pass
    @property
    def target(self) -> vec<3>:
        """
:type: vec<3>"""
    @target.setter
    def target(self, arg0: vec<3>) -> None:
        pass
    pass
class DrivePath():
    def __init__(self, arg0: rlutilities.rlutilities.simulation.Car) -> None: ...
    def accel_buffer(self) -> float: ...
    def recalculate_path(self) -> None: ...
    def step(self, arg0: float) -> None: ...
    @property
    def arrival_accel(self) -> float:
        """
:type: float"""
    @arrival_accel.setter
    def arrival_accel(self, arg0: float) -> None:
        pass
    @property
    def arrival_speed(self) -> float:
        """
:type: float"""
    @arrival_speed.setter
    def arrival_speed(self, arg0: float) -> None:
        pass
    @property
    def arrival_tangent(self) -> vec<3>:
        """
:type: vec<3>"""
    @arrival_tangent.setter
    def arrival_tangent(self, arg0: vec<3>) -> None:
        pass
    @property
    def arrival_time(self) -> float:
        """
:type: float"""
    @arrival_time.setter
    def arrival_time(self, arg0: float) -> None:
        pass
    @property
    def controls(self) -> rlutilities.rlutilities.simulation.Input:
        """
:type: rlutilities.rlutilities.simulation.Input"""
    @property
    def expected_error(self) -> float:
        """
:type: float"""
    @expected_error.setter
    def expected_error(self, arg0: float) -> None:
        pass
    @property
    def expected_speed(self) -> float:
        """
:type: float"""
    @expected_speed.setter
    def expected_speed(self, arg0: float) -> None:
        pass
    @property
    def finished(self) -> bool:
        """
:type: bool"""
    @property
    def path(self) -> rlutilities.rlutilities.simulation.Curve:
        """
:type: rlutilities.rlutilities.simulation.Curve"""
    @path.setter
    def path(self, arg0: rlutilities.rlutilities.simulation.Curve) -> None:
        pass
    @property
    def target(self) -> vec<3>:
        """
:type: vec<3>"""
    @target.setter
    def target(self, arg0: vec<3>) -> None:
        pass
    pass
class Jump():
    def __init__(self, arg0: rlutilities.rlutilities.simulation.Car) -> None: ...
    def simulate(self) -> rlutilities.rlutilities.simulation.Car: ...
    def step(self, arg0: float) -> None: ...
    @property
    def controls(self) -> rlutilities.rlutilities.simulation.Input:
        """
:type: rlutilities.rlutilities.simulation.Input"""
    @controls.setter
    def controls(self, arg0: rlutilities.rlutilities.simulation.Input) -> None:
        pass
    @property
    def duration(self) -> float:
        """
:type: float"""
    @duration.setter
    def duration(self, arg0: float) -> None:
        pass
    @property
    def finished(self) -> bool:
        """
:type: bool"""
    @finished.setter
    def finished(self, arg0: bool) -> None:
        pass
    pass
class Wavedash():
    def __init__(self, arg0: rlutilities.rlutilities.simulation.Car) -> None: ...
    def step(self, arg0: float) -> None: ...
    @property
    def controls(self) -> rlutilities.rlutilities.simulation.Input:
        """
:type: rlutilities.rlutilities.simulation.Input"""
    @property
    def direction(self) -> Optional[vec<2>]:
        """
:type: Optional[vec<2>]"""
    @direction.setter
    def direction(self, arg0: Optional[vec<2>]) -> None:
        pass
    @property
    def finished(self) -> bool:
        """
:type: bool"""
    pass