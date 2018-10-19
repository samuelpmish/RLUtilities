import numpy as np
import time

from DetectCollisions import *

#num_cars = 8
#num_frames = 36000
#fields_per_car = 6 # 3 positions, 3 rotations
#dimensions = (fields_per_car * num_cars, num_frames)
#
#angle_scale = -3
#position_scale = 3000
#
#game_info = np.zeros(dimensions, dtype=np.float32, order='F')
#
#for i in range(num_cars):
#
#    offset = fields_per_car * i
#
#    # car positions
#    game_info[offset + 0, :] = np.random.rand(1, num_frames) * position_scale
#    game_info[offset + 1, :] = np.random.rand(1, num_frames) * position_scale
#    game_info[offset + 2, :] = np.random.rand(1, num_frames) * position_scale
#
#    # car rotations
#    game_info[offset + 3, :] = np.random.rand(1, num_frames) * angle_scale
#    game_info[offset + 4, :] = np.random.rand(1, num_frames) * angle_scale
#    game_info[offset + 5, :] = np.random.rand(1, num_frames) * angle_scale

#data = np.load("C:/Users/sam/Dropbox/Utilities/BOTS_JOINING_AND_LEAVING.gzip")
#data = np.load("C:/Users/sam/Dropbox/Utilities/DEFAULT_3_ON_3_AROUND_58_HITS.gzip")
#data = np.load("C:/Users/sam/Dropbox/Utilities/1_DEMO.gzip")
data = np.load("C:/Users/sam/Dropbox/Utilities/3_bumps.gzip")

#discard the ball info for now
game_info = np.copy(data[:, 6:].T, 'F')

print(game_info.shape)
print(game_info.strides[0]//8, game_info.strides[1]//8)

start = time.time()
tmp1 = detect_collisions_between_cars(game_info, 1)
end = time.time()
print(f"Total time elapsed (single threaded): {(end - start) * 1000} ms")
print(len(tmp1), "collision frames")

start = time.time()
tmp2 = detect_collisions_between_cars(game_info, 10)
end = time.time()
print(f"Total time elapsed (10 threads): {(end - start) * 1000} ms")
print(len(tmp2), "collision frames")

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

for report in tmp1:
    c = report.NextCollision()
    print(report.frame, c)
