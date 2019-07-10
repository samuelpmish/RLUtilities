#include "rlbot/interface.h"

#include <windows.h>

#include "rlbot/MatchSettings.h"
#include "rlbot/rlbot_generated.h"

#include "flatbuffers/flatbuffers.h"

typedef bool (*BoolFunc)(void);
typedef ByteBuffer (*ByteBufferFunc)(void);
typedef void (*VoidFunc)(void*);
typedef int (*SendPacketFunc)(void*, int);

typedef rlbot::flat::Float flat_float;
typedef rlbot::flat::Vector3Partial flat_vector3;
typedef rlbot::flat::RotatorPartial flat_rotator;
typedef flatbuffers::Offset<flat_vector3> flat_vector3_offset;
typedef flatbuffers::Offset<flat_rotator> flat_rotator_offset;

typedef rlbot::flat::DesiredCarState flat_car;
typedef flatbuffers::Offset<flat_car> flat_car_offset;

flat_vector3_offset vec3_to_Vector3Partial(
    flatbuffers::FlatBufferBuilder& builder, vec3 v) {
  flat_float _x(v[0]);
  flat_float _y(v[1]);
  flat_float _z(v[2]);
  return rlbot::flat::CreateVector3Partial(builder, &_x, &_y, &_z);
}

flat_rotator_offset vec3_to_RotatorPartial(
    flatbuffers::FlatBufferBuilder& builder, vec3 v) {
  flat_float _p(v[0]);
  flat_float _y(v[1]);
  flat_float _r(v[2]);
  return rlbot::flat::CreateRotatorPartial(builder, &_p, &_y, &_r);
}

BoolFunc _isInitialized;
VoidFunc _free;

ByteBufferFunc _updateLiveDataPacketFlatbuffer;
ByteBufferFunc _updateFieldInfoFlatbuffer;
ByteBufferFunc _getBallPrediction;

SendPacketFunc _updatePlayerInputFlatbuffer;
SendPacketFunc _renderGroup;
SendPacketFunc _sendQuickChat;
SendPacketFunc _setGameState;
SendPacketFunc _startMatch;

int (* _startMatchStruct)(MatchSettings);

void Interface::LoadInterface(std::string dll) {
  HMODULE handle = LoadLibrary(dll.c_str());

  _isInitialized = (BoolFunc)GetProcAddress(handle, "IsInitialized");
  _free = (VoidFunc)GetProcAddress(handle, "Free");

  _updateLiveDataPacketFlatbuffer =
      (ByteBufferFunc)GetProcAddress(handle, "UpdateLiveDataPacketFlatbuffer");
  _updateFieldInfoFlatbuffer =
      (ByteBufferFunc)GetProcAddress(handle, "UpdateFieldInfoFlatbuffer");
  _getBallPrediction =
      (ByteBufferFunc)GetProcAddress(handle, "GetBallPrediction");

  _updatePlayerInputFlatbuffer =
      (SendPacketFunc)GetProcAddress(handle, "UpdatePlayerInputFlatbuffer");
  _renderGroup = (SendPacketFunc)GetProcAddress(handle, "RenderGroup");
  _sendQuickChat = (SendPacketFunc)GetProcAddress(handle, "SendQuickChat");
  _setGameState = (SendPacketFunc)GetProcAddress(handle, "SetGameState");
  _startMatch = (SendPacketFunc)GetProcAddress(handle, "StartMatchFlatbuffer");
  _startMatchStruct = (int (*)(MatchSettings))GetProcAddress(handle, "StartMatch");

}

bool Interface::IsInitialized() { return _isInitialized(); }

void Interface::Free(void* ptr) { _free(ptr); }

ByteBuffer Interface::UpdateLiveDataPacketFlatbuffer() {
  return _updateLiveDataPacketFlatbuffer();
}

ByteBuffer Interface::UpdateFieldInfoFlatbuffer() {
  return _updateFieldInfoFlatbuffer();
}

ByteBuffer Interface::GetBallPrediction() { return _getBallPrediction(); }

int Interface::SetBotInput(Input controls, int index) {
  flatbuffers::FlatBufferBuilder builder(50);

  auto controllerStateOffset = rlbot::flat::CreateControllerState(
      builder, controls.throttle, controls.steer, controls.pitch, controls.yaw,
      controls.roll, controls.jump, controls.boost, controls.handbrake, controls.useItem);

  auto playerInputOffset =
      rlbot::flat::CreatePlayerInput(builder, index, controllerStateOffset);

  builder.Finish(playerInputOffset);

  return _updatePlayerInputFlatbuffer(builder.GetBufferPointer(),
                                      builder.GetSize());
}

int Interface::RenderGroup(void* data, int size) {
  return _renderGroup(data, size);
}

int Interface::SendQuickChat(rlbot::flat::QuickChatSelection message,
                             int botIndex, bool teamOnly) {
  flatbuffers::FlatBufferBuilder builder(50);

  auto quickChatOffset =
      rlbot::flat::CreateQuickChat(builder, message, botIndex, teamOnly);
  builder.Finish(quickChatOffset);

  return _sendQuickChat(builder.GetBufferPointer(), builder.GetSize());
}

int Interface::SetGameState(void* data, int size) {
  return _setGameState(data, size);
}

int Interface::SetGameState(Game desired) {
  flatbuffers::FlatBufferBuilder builder(1000);

  auto ballPhysicsOffset = rlbot::flat::CreateDesiredPhysics(
      builder, vec3_to_Vector3Partial(builder, desired.ball.position),
      vec3_to_RotatorPartial(builder, vec3{0.0, 0.0, 0.0f}),
      vec3_to_Vector3Partial(builder, desired.ball.velocity),
      vec3_to_Vector3Partial(builder, desired.ball.angular_velocity));
  auto ballStateOffset =
      rlbot::flat::CreateDesiredBallState(builder, ballPhysicsOffset);

  std::vector<flat_car_offset> cars;

  for (int i = 0; i < desired.num_cars; i++) {
    auto carPhysicsOffset = rlbot::flat::CreateDesiredPhysics(
        builder, vec3_to_Vector3Partial(builder, desired.cars[i].position),
        vec3_to_RotatorPartial(builder, rotation_to_euler(desired.cars[i].orientation)),
        vec3_to_Vector3Partial(builder, desired.cars[i].velocity),
        vec3_to_Vector3Partial(builder, desired.cars[i].angular_velocity));

    flat_float boost = desired.cars[i].boost;

    cars.push_back(
        rlbot::flat::CreateDesiredCarState(builder, carPhysicsOffset, &boost));
  }

  auto carsOffset = builder.CreateVector(cars);

  auto gameStateOffset = rlbot::flat::CreateDesiredGameState(
      builder, ballStateOffset, carsOffset, 0, 0);

  builder.Finish(gameStateOffset);

  return _setGameState(builder.GetBufferPointer(), builder.GetSize());
}

int Interface::StartMatch(int num_players) {

  while (!Interface::IsInitialized()) {
    Sleep(100);
  }

#if 0
  std::cout << "start" << std::endl;
  int num_players = 1;
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<rlbot::flat::PlayerConfiguration>>
      playerConfigOffsets(num_players);

  for (int i = 0; i < num_players; i++) {

    std::cout << "player " << i << std::endl;

    int32_t team = 0;
    
    std::cout << "loadout" << std::endl;
    auto playerLoadout = rlbot::flat::CreatePlayerLoadout(builder);

    std::cout << "config" << std::endl;
    playerConfigOffsets[i] = rlbot::flat::CreatePlayerConfiguration(
      builder,
      rlbot::flat::PlayerClass_RLBotPlayer,
      0,
      builder.CreateString("bot"),
      team,
      playerLoadout);
  }

  std::cout << "mode/map" << std::endl;
  rlbot::flat::GameMode gameMode = rlbot::flat::GameMode_Soccer;
  rlbot::flat::GameMap gameMap = rlbot::flat::GameMap_DFHStadium;
  bool skipReplays = true;
  bool instantStart = true;

  std::cout << "mutators" << std::endl;
  auto mutator_settings = rlbot::flat::CreateMutatorSettings(
    builder,
    rlbot::flat::MatchLength_Five_Minutes,
    rlbot::flat::MaxScore_Unlimited,
    rlbot::flat::OvertimeOption_Unlimited,
    rlbot::flat::SeriesLengthOption_Unlimited,
    rlbot::flat::GameSpeedOption_Default,
    rlbot::flat::BallMaxSpeedOption_Default,
    rlbot::flat::BallTypeOption_Default,
    rlbot::flat::BallWeightOption_Default,
    rlbot::flat::BallSizeOption_Default,
    rlbot::flat::BallBouncinessOption_Default,
    rlbot::flat::BoostOption_Normal_Boost,
    rlbot::flat::RumbleOption_None,
    rlbot::flat::BoostStrengthOption_One,
    rlbot::flat::GravityOption_Default,
    rlbot::flat::DemolishOption_Default,
    rlbot::flat::RespawnTimeOption_Three_Seconds);

  std::cout << "matchsettings" << std::endl;
  auto matchSettingsFlat = rlbot::flat::CreateMatchSettings(
      builder, builder.CreateVector(playerConfigOffsets),
      gameMode, gameMap, skipReplays, instantStart,
      mutator_settings);

  std::cout << "finish" << std::endl;
  builder.Finish(matchSettingsFlat);

  std::cout << "calling startMatch" << std::endl;
  auto err = _startMatch(builder.GetBufferPointer(), builder.GetSize());

  std::cout << err << std::endl;
  return err;
#else
  MatchSettings settings;
  memset(&settings, 0, sizeof(MatchSettings));

  settings.NumPlayers = num_players;
  for (int i = 0; i < num_players; i++) {
    PlayerConfiguration& player = settings.PlayerConfiguration[i];
    player.Bot = true;
    player.RLBotControlled = true;
  }

  settings.GameMode = GameMode::Soccer;
  settings.GameMap = GameMap::Mannfield;
  settings.SkipReplays = true;
  settings.InstantStart = true;

  MutatorSettings& mutators = settings.MutatorSettings;
  mutators.MatchLength = MatchLength::Unlimited;
  mutators.MaxScore = MaxScore::Unlimited;
  mutators.OvertimeOptions = OvertimeOption::Unlimited;
  mutators.SeriesLengthOptions = SeriesLengthOption::Unlimited;
  mutators.GameSpeedOptions = GameSpeedOption::Default;
  mutators.BallMaxSpeedOptions = BallMaxSpeedOption::Default;
  mutators.BallTypeOptions = BallTypeOption::Default;
  mutators.BallWeightOptions = BallWeightOption::Default;
  mutators.BallSizeOptions = BallSizeOption::Default;
  mutators.BallBouncinessOptions = BallBouncinessOption::Default;
  mutators.BoostOptions = BoostOption::Unlimited_Boost;
  mutators.RumbleOptions = RumbleOption::None;
  mutators.BoostStrengthOptions = BoostStrengthOption::One;
  mutators.GravityOptions = GravityOption::Default;
  mutators.DemolishOptions = DemolishOption::Default;
  mutators.RespawnTimeOptions = RespawnTimeOption::Three_Seconds;

  return _startMatchStruct(settings);
#endif
}
