cmake_minimum_required(VERSION 3.8)
project(Utilities)

add_definitions(-DASSET_PATH="${PROJECT_SOURCE_DIR}/assets/")

add_subdirectory(pybind11)

add_library(bot_utils STATIC
  ${PROJECT_SOURCE_DIR}/src/car.cpp
  ${PROJECT_SOURCE_DIR}/src/ball.cpp
  ${PROJECT_SOURCE_DIR}/src/bvh.cpp
  ${PROJECT_SOURCE_DIR}/src/pitch.cpp
  ${PROJECT_SOURCE_DIR}/src/mesh.cpp
  ${PROJECT_SOURCE_DIR}/src/primitives.cpp
)

target_include_directories(bot_utils PUBLIC inc/)
target_compile_features(bot_utils PUBLIC cxx_std_11)

pybind11_add_module(LinearAlgebra
  ${PROJECT_SOURCE_DIR}/src/LinearAlgebra.cpp
)
target_include_directories(LinearAlgebra PRIVATE inc/)
target_compile_features(LinearAlgebra PRIVATE cxx_std_11)

pybind11_add_module(Simulation
  ${PROJECT_SOURCE_DIR}/src/car.cpp
  ${PROJECT_SOURCE_DIR}/src/ball.cpp
  ${PROJECT_SOURCE_DIR}/src/bvh.cpp
  ${PROJECT_SOURCE_DIR}/src/pitch.cpp
  ${PROJECT_SOURCE_DIR}/src/mesh.cpp
  ${PROJECT_SOURCE_DIR}/src/primitives.cpp
  ${PROJECT_SOURCE_DIR}/src/Simulation.cpp
)
target_include_directories(Simulation PRIVATE inc/)
target_compile_features(Simulation PRIVATE cxx_std_11)

pybind11_add_module(DetectCollisions
  ${PROJECT_SOURCE_DIR}/src/car.cpp
  ${PROJECT_SOURCE_DIR}/src/bvh.cpp
  ${PROJECT_SOURCE_DIR}/src/mesh.cpp
  ${PROJECT_SOURCE_DIR}/src/pitch.cpp
  ${PROJECT_SOURCE_DIR}/src/primitives.cpp
  ${PROJECT_SOURCE_DIR}/src/detect_collisions_between_cars.cpp
)
target_include_directories(DetectCollisions PRIVATE inc/)
target_compile_features(DetectCollisions PRIVATE cxx_std_11)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(DetectCollisions PUBLIC OpenMP::OpenMP_CXX)
endif()
