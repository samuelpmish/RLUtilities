
""" Values from the game """

# JumpComponent
MinJumpTime = 0.025
JumpImpulse = 52500
JumpForce = 262500
JumpForceTime = 0.2
JumpImpulseSpeed = 291.667
JumpAccel = 1458.333374
MaxJumpHeight = 232.58545
MaxJumpHeightTime = 0.897436

# BoostComponent
BoostConsumptionRate = 0.333
BoostForce = 178500

# AirControlComponent
AirTorque = [130, 95, 400]  # Rotator (pitch, yaw, roll)
AirDamping = [30, 20, 50]  # Rotator
ThrottleForce = 12000

# DodgeComponent
DodgeInputThreshold = 0.5
SideDodgeImpulse = 90000
SideDodgeImpulseMaxSpeedScale = 1.9
ForwardDodgeImpulse = 90000
ForwardDodgeImpulseMaxSpeedScale = 1.0
BackwardDodgeImpulse = 96000
BackwardDodgeImpulseMaxSpeedScale = 2.5
SideDodgeTorque = 260
ForwardDodgeTorque = 224
DodgeTorqueTime = 0.65
MinDodgeTorqueTime = 0.041
DodgeZDamping = 0.35
DodgeZDampingDelay = 0.15
DodgeZDampingUpTime = 0.06
DodgeImpulseScale = 1.0
DodgeTorqueScale = 1.0

# FlipComponent
FlipCarImpulse = 200
FlipCarTorque = 1000
FlipCarTime = 0.4

# VehicleSim
DriveTorque = 288000
BrakeTorque = 10500
HandbrakeRate_RiseRate = 5
HandbrakeRate_FallRate = 2
StopThreshold = 25
IdleBrakeFactor = 0.15
OppositeBrakeFactor = 1
bUseAckermannSteering = False

# VehicleSim.Wheels (Octane)
SteerFactor = (1, 1, 0, 0)  # one for each wheel
WheelRadius = (12, 12, 15, 15)  # one for each wheel
SuspensionStiffness = 500
SuspensionDampingCompression = 25
SuspensionDampingRelaxation = 40
SuspensionTravel = 12
SuspensionMaxRaise = 2.5
ContactForceDistance = 0
SpinSpeedDecayRate = 0.7
BoneOffset = [0, 0, 0]  # Vector3
PresetRestPosition = ([51.250, -25.9, -6.0], [51.250, 25.9, -6.0],
                      [-33.75, -29.5, -4.3], [-33.75, 29.5, -4.3])  # Vector3, one for each wheel.
LocalSuspensionRayStart = ([51.250, -25.9, 20.755], [51.250, 25.9, 20.755],
                           [-33.75, -29.5, 20.755], [-33.75, 29.5, 20.755])  # Vector3, one for each wheel.
LocalRestPosition = PresetRestPosition

# StickyForce
Ground = 1.5
Wall = 0

# BallInteraction
PushZScale = 0.35
PushForwardScale = 0.65
MaxRelativeSpeed = 4600
Restitution = 0
Friction = 2

# AutoFlip
Torque = 80
Force = 100

# ThrottleShake
ThrottleRiseSpeed = 3
ThrottleFallSpeed = 1.5
