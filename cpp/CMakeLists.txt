cmake_minimum_required(VERSION 3.8)
project(RLUtilities)

add_subdirectory(pybind11)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

set(rlutilities_src

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
  ${PROJECT_SOURCE_DIR}/src/simulation/path.cc

  # mechanics
  ${PROJECT_SOURCE_DIR}/src/mechanics/jump.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/drive.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/dodge.cc
  #${PROJECT_SOURCE_DIR}/src/mechanics/slide.cc
  #${PROJECT_SOURCE_DIR}/src/mechanics/slide_LUTs.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/aerial.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/wavedash.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/boostdash.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/aerial_turn.cc

  # linear algebra
  ${PROJECT_SOURCE_DIR}/src/linear_algebra/math.cc

  # misc
  ${PROJECT_SOURCE_DIR}/src/misc/convert.cc

)		

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

  file(GLOB unit_tests "${PROJECT_SOURCE_DIR}/src/test/*.cc")
  add_library(rlutilities_without_pybind11 ${rlutilities_src})
  target_include_directories(rlutilities_without_pybind11 PUBLIC inc/)
  target_compile_features(rlutilities_without_pybind11 PUBLIC cxx_std_17)
  
  foreach(filename ${unit_tests})		
  		
    # remove the filetype suffix		
    string(REPLACE ".cc" "" testname ${filename})		
    		
    # remove the prefix		
    string(REPLACE ${PROJECT_SOURCE_DIR}/src/test/ "" testname ${testname})		
    		
    # use the remaining string to name the binary file		
    add_executable(${testname} ${filename})		
    add_test(${testname} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${testname})		
  		
    target_compile_features(${testname} PUBLIC cxx_std_17)		
    target_link_libraries(${testname} PUBLIC rlutilities_without_pybind11)
   		
  endforeach(filename ${CPP_TESTS})		

endif()
