cmake_minimum_required(VERSION 3.8)
project(RLUtilities)

add_subdirectory(pybind11)
add_definitions(-DGENERATE_PYTHON_BINDINGS)

add_library(cpp_lib

  # simulation
  ${PROJECT_SOURCE_DIR}/src/simulation/car.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/ball.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/pad.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/goal.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/game_data.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/geometry.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/mesh.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/field.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/curve.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/bvh.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/soccar.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/hoops.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/dropshot.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/gametype.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/compositearc.cpp
  ${PROJECT_SOURCE_DIR}/src/simulation/unroller.cpp

  # mechanics
  ${PROJECT_SOURCE_DIR}/src/mechanics/jump.cpp
  ${PROJECT_SOURCE_DIR}/src/mechanics/drive.cpp
  ${PROJECT_SOURCE_DIR}/src/mechanics/dodge.cpp
  ${PROJECT_SOURCE_DIR}/src/mechanics/aerial.cpp
  ${PROJECT_SOURCE_DIR}/src/mechanics/wavedash.cpp
  ${PROJECT_SOURCE_DIR}/src/mechanics/boostdash.cpp
  ${PROJECT_SOURCE_DIR}/src/mechanics/aerialturn.cpp

  # linear algebra
  ${PROJECT_SOURCE_DIR}/src/linear_algebra/math.cpp

  # misc
  ${PROJECT_SOURCE_DIR}/src/misc/convert.cpp
)

target_include_directories(cpp_lib PUBLIC inc/)
target_link_libraries(cpp_lib PUBLIC pybind11)
target_compile_features(cpp_lib PUBLIC cxx_std_17)

pybind11_add_module(rlutilities
  ${PROJECT_SOURCE_DIR}/src/python_extensions.cpp
)
target_link_libraries(rlutilities PRIVATE cpp_lib)
