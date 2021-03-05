import sys
from pathlib import Path
from .rlutilities import mechanics, simulation, linear_algebra, initialize

sys.modules["rlutilities.mechanics"] = mechanics
sys.modules["rlutilities.simulation"] = simulation
sys.modules["rlutilities.linear_algebra"] = linear_algebra

asset_dir = Path(__file__).parent / "assets"
initialize(asset_dir.as_posix() + "/")
