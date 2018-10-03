import numpy as np
import time

from DetectCollisions import *

num_cars = 8
num_frames = 36000
fields_per_car = 7 # 3 positions, 3 rotations, 1 for specifying hitbox preset
dimensions = (fields_per_car * num_cars, num_frames)

angle_scale = -3
position_scale = 2000

game_info = np.zeros(dimensions, dtype=np.float32, order='F')

for i in range(num_cars):

    offset = fields_per_car * i

    # hitbox preset
    game_info[offset + 0, :] = 0

    # car positions
    game_info[offset + 1, :] = np.random.rand(1, num_frames) * position_scale
    game_info[offset + 2, :] = np.random.rand(1, num_frames) * position_scale
    game_info[offset + 3, :] = np.random.rand(1, num_frames) * position_scale

    # car rotations
    game_info[offset + 4, :] = np.random.rand(1, num_frames) * angle_scale
    game_info[offset + 5, :] = np.random.rand(1, num_frames) * angle_scale
    game_info[offset + 6, :] = np.random.rand(1, num_frames) * angle_scale


start = time.time()
tmp1 = detect_collisions_between_cars(game_info, 1)
end = time.time()
print(f"Total time elapsed (single threaded): {(end - start) * 1000} ms")
print(len(tmp1), "collisions")
start = time.time()
tmp2 = detect_collisions_between_cars(game_info, 10)
end = time.time()
print(f"Total time elapsed (10 threads): {(end - start) * 1000} ms")
print(len(tmp1), "collisions")

same = True
if len(tmp1) == len(tmp2):
    print("got the same number of collision frames")
    for i in range(len(tmp1)):
        if (tmp1[i].mask != tmp2[i].mask) or (tmp1[i].frame != tmp2[i].frame):
            same = False
            break

if same:
    print("all values matched!")
else:
    print("NOT all values matched!")
