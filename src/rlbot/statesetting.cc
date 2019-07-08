#include "rlbot/statesetting.h"

#include "rlbot/interface.h"

#define CREATEVECTOR3PARTIAL(name, vector3)                           \
  flatbuffers::Offset<rlbot::flat::Vector3Partial> name;              \
  if (vector3.has_value()) {                                          \
    auto _x = rlbot::flat::Float(vector3.value().x);                  \
    auto _y = rlbot::flat::Float(vector3.value().y);                  \
    auto _z = rlbot::flat::Float(vector3.value().z);                  \
    name = rlbot::flat::CreateVector3Partial(builder, &_x, &_y, &_z); \
  } else {                                                            \
    name = 0;                                                         \
  }
#define CREATEROTATORPARTIAL(name, rotator)                           \
  flatbuffers::Offset<rlbot::flat::RotatorPartial> name;              \
  if (rotator.has_value()) {                                          \
    auto _p = rlbot::flat::Float(rotator.value().pitch);              \
    auto _y = rlbot::flat::Float(rotator.value().yaw);                \
    auto _r = rlbot::flat::Float(rotator.value().roll);               \
    name = rlbot::flat::CreateRotatorPartial(builder, &_p, &_y, &_r); \
  } else {                                                            \
    name = 0;                                                         \
  }

PhysicsState::PhysicsState() {}

BallState::BallState() {}

GameState::GameState() {}

void GameState::BuildAndSend() {
  flatbuffers::FlatBufferBuilder builder(1000);

  CREATEVECTOR3PARTIAL(ballLocationOffset, ballState.physicsState.location);
  CREATEROTATORPARTIAL(ballRotationOffset, ballState.physicsState.rotation);
  CREATEVECTOR3PARTIAL(ballVelocityOffset, ballState.physicsState.velocity);
  CREATEVECTOR3PARTIAL(ballAngularVelocityOffset,
                       ballState.physicsState.angularVelocity);

  auto ballPhysicsOffset = rlbot::flat::CreateDesiredPhysics(
      builder, ballLocationOffset, ballRotationOffset, ballVelocityOffset,
      ballAngularVelocityOffset);
  auto ballStateOffset =
      rlbot::flat::CreateDesiredBallState(builder, ballPhysicsOffset);

  std::vector<flatbuffers::Offset<rlbot::flat::DesiredCarState>> cars;

  for (int i = 0; i < carStates.size(); i++) {
    if (carStates[i].has_value()) {
      auto car = carStates[i].value();

      CREATEVECTOR3PARTIAL(carLocationOffset, car.physicsState.location);
      CREATEROTATORPARTIAL(carRotationOffset, car.physicsState.rotation);
      CREATEVECTOR3PARTIAL(carVelocityOffset, car.physicsState.velocity);
      CREATEVECTOR3PARTIAL(carAngularVelocityOffset,
                           car.physicsState.angularVelocity);

      rlbot::flat::Float boost = float(car.boostAmount.value_or(0));

      auto carPhysicsOffset = rlbot::flat::CreateDesiredPhysics(
          builder, carLocationOffset, carRotationOffset, carVelocityOffset,
          carAngularVelocityOffset);
      cars.push_back(rlbot::flat::CreateDesiredCarState(
          builder, carPhysicsOffset, &boost));
    } else {
      // Create an empty car state if this car doesn't have a desired state.
      cars.push_back(rlbot::flat::CreateDesiredCarState(builder));
    }
  }

  auto carsOffset = builder.CreateVector(cars);

  auto gameStateOffset = rlbot::flat::CreateDesiredGameState(
      builder, ballStateOffset, carsOffset, 0, 0);

  builder.Finish(gameStateOffset);

  Interface::SetGameState(builder.GetBufferPointer(), builder.GetSize());
}
