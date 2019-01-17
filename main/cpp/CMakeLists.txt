cmake_minimum_required(VERSION 3.8)
project(Utilities)

add_subdirectory(pybind11)
add_definitions(-DGENERATE_PYTHON_BINDINGS)

add_library(cpp_lib

  # simulation
  ${PROJECT_SOURCE_DIR}/src/car.cpp
  ${PROJECT_SOURCE_DIR}/src/ball.cpp
  ${PROJECT_SOURCE_DIR}/src/geometry.cpp
  ${PROJECT_SOURCE_DIR}/src/mesh.cpp
  ${PROJECT_SOURCE_DIR}/src/field.cpp
  ${PROJECT_SOURCE_DIR}/src/curve.cpp
  ${PROJECT_SOURCE_DIR}/src/bvh.cpp
  ${PROJECT_SOURCE_DIR}/src/soccar.cpp
  ${PROJECT_SOURCE_DIR}/src/hoops.cpp
  ${PROJECT_SOURCE_DIR}/src/dropshot.cpp
  ${PROJECT_SOURCE_DIR}/src/gametype.cpp
  ${PROJECT_SOURCE_DIR}/src/compositearc.cpp

  # mechanics
  ${PROJECT_SOURCE_DIR}/src/jump.cpp
  ${PROJECT_SOURCE_DIR}/src/drive.cpp
  ${PROJECT_SOURCE_DIR}/src/dodge.cpp
  ${PROJECT_SOURCE_DIR}/src/aerial.cpp
  ${PROJECT_SOURCE_DIR}/src/unroller.cpp
  ${PROJECT_SOURCE_DIR}/src/wavedash.cpp
  ${PROJECT_SOURCE_DIR}/src/boostdash.cpp
  ${PROJECT_SOURCE_DIR}/src/aerialturn.cpp

)
target_include_directories(cpp_lib PUBLIC inc/)
target_link_libraries(cpp_lib PUBLIC pybind11)
target_compile_features(cpp_lib PUBLIC cxx_std_17)

pybind11_add_module(cpp_extension
  ${PROJECT_SOURCE_DIR}/src/python_extensions.cpp
)
target_link_libraries(cpp_extension PRIVATE cpp_lib)

pybind11_add_module(linear_algebra
  ${PROJECT_SOURCE_DIR}/src/LinearAlgebra.cpp
)
target_include_directories(linear_algebra PUBLIC inc/)
