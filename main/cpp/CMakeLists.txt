cmake_minimum_required(VERSION 3.8)
project(RLUtilities)

add_subdirectory(pybind11)
add_definitions(-DGENERATE_PYTHON_BINDINGS)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

add_library(cpp_lib

  # simulation
  ${PROJECT_SOURCE_DIR}/src/simulation/car.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/ball.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/pad.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/goal.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/game_data.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/geometry.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/mesh.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/field.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/curve.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/bvh.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/soccar.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/hoops.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/dropshot.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/gametype.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/composite_arc.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/unroller.cc

  # mechanics
  ${PROJECT_SOURCE_DIR}/src/mechanics/jump.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/drive.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/dodge.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/aerial.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/wavedash.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/boostdash.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/aerial_turn.cc

  # linear algebra
  ${PROJECT_SOURCE_DIR}/src/linear_algebra/math.cc

  # misc
  ${PROJECT_SOURCE_DIR}/src/misc/convert.cc
)

target_include_directories(cpp_lib PUBLIC inc/)
target_link_libraries(cpp_lib PUBLIC pybind11)
target_compile_features(cpp_lib PUBLIC cxx_std_17)

pybind11_add_module(rlutilities
  ${PROJECT_SOURCE_DIR}/src/python_extensions.cc
)
target_link_libraries(rlutilities PRIVATE cpp_lib)
