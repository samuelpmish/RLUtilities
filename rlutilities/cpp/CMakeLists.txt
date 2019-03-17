cmake_minimum_required(VERSION 3.8)
project(RLUtilities LANGUAGES CXX)

add_subdirectory(pybind11)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(rlutilities_src

  # simulation
  ${PROJECT_SOURCE_DIR}/src/simulation/car.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/ball.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/pad.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/goal.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/geometry.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/mesh.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/field.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/curve.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/bvh.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/game.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/ogh.cc

  # mechanics
  ${PROJECT_SOURCE_DIR}/src/mechanics/jump.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/drive.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/dodge.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/aerial.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/wavedash.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/boostdash.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/drive_path.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/aerial_turn.cc

  # linear algebra
  ${PROJECT_SOURCE_DIR}/src/linear_algebra/math.cc

  # misc
  ${PROJECT_SOURCE_DIR}/src/misc/dll.cc

)		

add_library(rlutilities_cpp ${rlutilities_src})
target_include_directories(rlutilities_cpp PUBLIC inc/)
target_compile_features(rlutilities_cpp PUBLIC cxx_std_17)

if (WITH_PYTHON)

  pybind11_add_module(rlutilities
    ${PROJECT_SOURCE_DIR}/src/python_extensions.cc
    ${rlutilities_src}
  )
  target_include_directories(rlutilities PUBLIC inc/)
  target_compile_definitions(rlutilities PUBLIC -DGENERATE_PYTHON_BINDINGS)
  target_compile_features(rlutilities PUBLIC cxx_std_17)

endif()

if (WITH_TESTS)

  enable_testing()

  file(GLOB cc_tests "${PROJECT_SOURCE_DIR}/src/test/*.cc")
  
  foreach(filename ${cc_tests})		
  		
    # remove the filetype suffix		
    string(REPLACE ".cc" "" testname ${filename})		
    		
    # remove the prefix		
    string(REPLACE ${PROJECT_SOURCE_DIR}/src/test/ "" testname ${testname})		
    		
    # use the remaining string to name the binary file		
    add_executable(${testname} ${filename})		
    add_test(${testname} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${testname})		
  		
    target_include_directories(${testname} PUBLIC inc/)
    target_compile_features(${testname} PUBLIC cxx_std_17)		
    target_link_libraries(${testname} PUBLIC rlutilities_cpp)
   		
  endforeach(filename ${cc_tests})

endif()
