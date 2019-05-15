from typing import *
from typing import Iterable as iterable
from typing import Iterator as iterator
from numpy import float64
_Shape = Tuple[int, ...]
__all__  = [
"Ball",
"Car",
"ControlPoint",
"Curve",
"Field",
"Game",
"Goal",
"Input",
"Navigator",
"Pad",
"obb",
"ray",
"sphere",
"tri",
"intersect"
]
class Ball():
    collision_radius = 93.1500015258789
    drag = -0.030500000342726707
    friction = 2.0
    mass = 30.0
    max_omega = 6.0
    max_speed = 4000.0
    moment_of_inertia = 99918.75
    radius = 91.25
    restitution = 0.6000000238418579

    @overload
    def __init__(self, arg0: Ball) -> None: 
        pass
    @overload
    def __init__(self) -> None: ...
    def hitbox(self) -> sphere: ...
    @overload
    def step(self, arg0: float) -> None: 
        pass
    @overload
    def step(self, arg0: float, arg1: Car) -> None: ...
    def to_json(self) -> str: ...

    @property
    def angular_velocity(self) -> vec3:
        pass
    
    @property
    def location(self) -> vec3:
        pass
    
    @property
    def time(self) -> float:
        pass
    
    @property
    def velocity(self) -> vec3:
        pass
    
    pass
class Car():

    @overload
    def __init__(self, arg0: Car) -> None: 
        pass
    @overload
    def __init__(self) -> None: ...
    def extrapolate(self, arg0: float) -> None: ...
    def forward(self) -> vec3: ...
    def hitbox(self) -> obb: ...
    def left(self) -> vec3: ...
    def step(self, arg0: Input, arg1: float) -> None: ...
    def to_json(self) -> str: ...
    def up(self) -> vec3: ...

    @property
    def angular_velocity(self) -> vec3:
        pass
    
    @property
    def boost(self) -> int:
        pass
    
    @property
    def controls(self) -> Input:
        pass
    
    @property
    def dodge_rotation(self) -> mat<2, 2>:
        pass
    
    @property
    def dodge_timer(self) -> float:
        pass
    
    @property
    def double_jumped(self) -> bool:
        pass
    
    @property
    def id(self) -> int:
        pass
    
    @property
    def jump_timer(self) -> float:
        pass
    
    @property
    def jumped(self) -> bool:
        pass
    
    @property
    def location(self) -> vec3:
        pass
    
    @property
    def on_ground(self) -> bool:
        pass
    
    @property
    def quaternion(self) -> vec4:
        pass
    
    @property
    def rotation(self) -> mat<3, 3>:
        pass
    
    @property
    def rotator(self) -> vec3:
        pass
    
    @property
    def supersonic(self) -> bool:
        pass
    
    @property
    def team(self) -> int:
        pass
    
    @property
    def time(self) -> float:
        pass
    
    @property
    def velocity(self) -> vec3:
        pass
    
    pass
class ControlPoint():

    @overload
    def __init__(self) -> None: 
        pass
    @overload
    def __init__(self, arg0: vec3, arg1: vec3, arg2: vec3) -> None: ...

    @property
    def n(self) -> vec3:
        pass
    
    @property
    def p(self) -> vec3:
        pass
    
    @property
    def t(self) -> vec3:
        pass
    
    pass
class Curve():

    @overload
    def __init__(self, arg0: List[vec3]) -> None: 
        pass
    @overload
    def __init__(self, arg0: List[ControlPoint]) -> None: ...
    def calculate_distances(self) -> None: ...
    def calculate_max_speeds(self, arg0: float, arg1: float) -> float: ...
    def calculate_tangents(self) -> None: ...
    def curvature_at(self, arg0: float) -> float: ...
    def find_nearest(self, arg0: vec3) -> float: ...
    def max_speed_at(self, arg0: float) -> float: ...
    def point_at(self, arg0: float) -> vec3: ...
    def pop_front(self) -> None: ...
    def tangent_at(self, arg0: float) -> vec3: ...
    def write_to_file(self, arg0: str) -> None: ...

    @property
    def length(self) -> float:
        pass
    
    @property
    def points(self) -> List[vec3]:
        pass
    
    pass
class Field():
    mode = 'Uninitialized'
    triangles = []
    walls = []

    @staticmethod
    @overload
    def collide(arg0: sphere) -> ray: 
        pass
    @staticmethod
    @overload
    def collide(arg0: obb) -> ray: ...
    @staticmethod
    def raycast_any(arg0: ray) -> ray: ...
    @staticmethod
    def snap(arg0: vec3) -> ray: ...

    pass
class Game():
    frametime = 0.008333333767950535
    gravity = -650.0
    map = 'map_not_set'

    def __init__(self, arg0: int, arg1: int) -> None: ...
    def read_game_information(self, arg0: object, arg1: object, arg2: object) -> None: ...
    @staticmethod
    def set_mode(arg0: str) -> None: ...

    @property
    def ball(self) -> Ball:
        pass
    
    @property
    def cars(self) -> List[Car[8]]:
        pass
    
    @property
    def frame(self) -> int:
        pass
    
    @property
    def frame_delta(self) -> int:
        pass
    
    @property
    def id(self) -> int:
        pass
    
    @property
    def kickoff_pause(self) -> bool:
        pass
    
    @property
    def match_ended(self) -> bool:
        pass
    
    @property
    def my_car(self) -> Car:
        pass
    
    @property
    def num_cars(self) -> int:
        pass
    
    @property
    def overtime(self) -> bool:
        pass
    
    @property
    def pads(self) -> List[Pad]:
        pass
    
    @property
    def round_active(self) -> bool:
        pass
    
    @property
    def team(self) -> int:
        pass
    
    @property
    def time(self) -> float:
        pass
    
    @property
    def time_delta(self) -> float:
        pass
    
    @property
    def time_remaining(self) -> float:
        pass
    
    pass
class Goal():

    def __init__(self) -> None: ...

    @property
    def direction(self) -> vec3:
        pass
    
    @property
    def location(self) -> vec3:
        pass
    
    @property
    def team(self) -> int:
        pass
    
    pass
class Input():

    def __init__(self) -> None: ...

    @property
    def boost(self) -> bool:
        pass
    
    @property
    def handbrake(self) -> bool:
        pass
    
    @property
    def jump(self) -> bool:
        pass
    
    @property
    def pitch(self) -> float:
        pass
    
    @property
    def roll(self) -> float:
        pass
    
    @property
    def steer(self) -> float:
        pass
    
    @property
    def throttle(self) -> float:
        pass
    
    @property
    def yaw(self) -> float:
        pass
    
    pass
class Navigator():

    def __init__(self, arg0: Car) -> None: ...
    def analyze_surroundings(self, arg0: float) -> None: ...
    def path_to(self, arg0: vec3, arg1: vec3, arg2: float) -> Curve: ...

    pass
class Pad():

    def __init__(self) -> None: ...

    @property
    def is_active(self) -> bool:
        pass
    
    @property
    def is_full_boost(self) -> bool:
        pass
    
    @property
    def location(self) -> vec3:
        pass
    
    @property
    def timer(self) -> float:
        pass
    
    pass
class obb():

    def __init__(self) -> None: ...

    @property
    def center(self) -> vec3:
        pass
    
    @property
    def half_width(self) -> vec3:
        pass
    
    @property
    def orientation(self) -> mat<3, 3>:
        pass
    
    pass
class ray():

    @overload
    def __init__(self, arg0: vec3, arg1: vec3) -> None: 
        pass
    @overload
    def __init__(self) -> None: ...

    @property
    def direction(self) -> vec3:
        pass
    
    @property
    def start(self) -> vec3:
        pass
    
    pass
class sphere():

    @overload
    def __init__(self, arg0: vec3, arg1: float) -> None: 
        pass
    @overload
    def __init__(self) -> None: ...

    @property
    def center(self) -> vec3:
        pass
    
    @property
    def radius(self) -> float:
        pass
    
    pass
class tri():

    def __getitem__(self, arg0: int) -> vec3: ...
    def __init__(self) -> None: ...
    def __setitem__(self, arg0: int, arg1: vec3) -> None: ...

    pass
@overload
def intersect(arg0: obb, arg1: sphere) -> bool:
    pass
@overload
def intersect(arg0: sphere, arg1: obb) -> bool:
    pass