### ACar_TA
#### float MaxTimeForDodge
```1.25```

### AVehicle_TA
```
/** Force to apply down on the vehicle when it is on flat ground */
var() float StickyForceGround;
/** Force to apply down on the vehicle when it is on a wall */
var() float StickyForceWall;
/** Torque to roll the car when holding throttle on its back */
var() float AutoFlipTorque;
/** Force to apply down on car when rolling */
var() float AutoFlipForce;
/** Tweak how we interact with the ball */
var() @NULL BallInteraction;
```

#### FStickyForceData StickyForce
```
float Ground  0.5
float Wall    1.5
```

#### FAutoFlipData AutoFlip
```
float Torque  80
float Force   100
```

```
// ScriptStruct TAGame.Vehicle_TA.BallInteractionSettings
// 0x0028
struct FBallInteractionSettings
{
	unsigned long                                      bSkipScriptForces : 1;                                    // 0x0000(0x0004) (Edit)
	struct FInterpCurveFloat                           PushFactorCurve;                                          // 0x0004(0x0010) (Edit, NeedCtorLink)
	float                                              PushZScale;                                               // 0x0014(0x0004) (Edit)
	float                                              PushForwardScale;                                         // 0x0018(0x0004) (Edit)
	float                                              MaxRelativeSpeed;                                         // 0x001C(0x0004) (Edit)
	float                                              Restitution;                                              // 0x0020(0x0004) (Edit)
	float                                              Friction;                                                 // 0x0024(0x0004) (Edit)
};
```

#### FBallInteractionSettings BallInteraction
```
bool bSkipScriptForces              0
FInterpCurveFloat PushFactorCurve   ->
  Data                *
  Count               5
  Max                 5
  EInterpMethodType   0
  ---
  Data:
  0, 0.6499999762, 0, 0, 0
  500, 0.6499999762, 0, 0, 0
  1400, 0.6000000238, 0, 0, 0
  2300, 0.5500000119, 0, 0, 0
  4600, 0.3000000119, 0, 0, 0

float PushZScale                  0.349999994
float PushForwardScale            0.649999999762
float MaxRelativeSpeed            4600
float Restitution                 0
float Friction                    2
```

### ARBActor_TA
#### float MaxLinearSpeed
```2300```

#### float MaxAngularSpeed
```5.5```

### APawn
#### float Mass
```180```
