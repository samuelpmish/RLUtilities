from LinearAlgebra import *

a = vec3(1, 0, 0)
b = vec3(0, 1, 0)
c = a + b

R = axis_rotation(a)

print(R[0,0], R[0,1], R[0,2])
print(R[1,0], R[1,1], R[1,2])
print(R[2,0], R[2,1], R[2,2])
