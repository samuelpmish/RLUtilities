cmake_minimum_required(VERSION 3.8)
project(RLUtilities LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(TARGET_LANGUAGE "cpp" CACHE STRING "Build shared library for \"cpp\", \"python\" or \"both\"")
set_property(CACHE TARGET_LANGUAGE PROPERTY STRINGS "cpp" "python" "both")
message(STATUS "Building libraries for ${TARGET_LANGUAGE}")

add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(rlu_sources

  # simulation
  ${PROJECT_SOURCE_DIR}/src/simulation/car.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/ball.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/geometry.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/mesh.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/field.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/curve.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/bvh.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/game.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/ogh.cc
  ${PROJECT_SOURCE_DIR}/src/simulation/navigator.cc

  # mechanics
  ${PROJECT_SOURCE_DIR}/src/mechanics/jump.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/drive.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/dodge.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/aerial.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/wavedash.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/boostdash.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/aerial_turn.cc
  ${PROJECT_SOURCE_DIR}/src/mechanics/follow_path.cc

  # misc
  ${PROJECT_SOURCE_DIR}/src/misc/dll.cc
  ${PROJECT_SOURCE_DIR}/src/misc/graph.cc

  # resource files for meshes, lookup tables, navigation data
  ${PROJECT_SOURCE_DIR}/assets/assets.rc

)		

if (TARGET_LANGUAGE STREQUAL "cpp" OR TARGET_LANGUAGE STREQUAL "both")

  add_library(rlutilities_cpp SHARED 
    ${rlu_sources}

    #${PROJECT_SOURCE_DIR}/src/rlbot/bot.cc
    ${PROJECT_SOURCE_DIR}/src/rlbot/interface.cc
    ${PROJECT_SOURCE_DIR}/src/rlbot/renderer.cc
    ${PROJECT_SOURCE_DIR}/src/rlbot/statesetting.cc
  )
  target_include_directories(rlutilities_cpp PUBLIC inc)
  target_compile_features(rlutilities_cpp PUBLIC cxx_std_17)
  target_link_libraries(rlutilities_cpp PUBLIC Xinput)

  # ensures a consistent name when used with gcc, clang, msvc
  set_target_properties(rlutilities_cpp PROPERTIES PREFIX "") 
  set_target_properties(rlutilities_cpp PROPERTIES OUTPUT_NAME "rlutilities")
  set_target_properties(rlutilities_cpp PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  )
  
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
      set_target_properties(${testname} PROPERTIES
          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
      )
     		
    endforeach(filename ${cc_tests})
  
  endif()

endif() 


if (TARGET_LANGUAGE STREQUAL "python" OR TARGET_LANGUAGE STREQUAL "both")

  add_subdirectory(python/pybind11)

  add_compile_options($<$<CXX_COMPILER_ID:GNU>:-D_hypot=hypot>)

  pybind11_add_module(rlutilities
    ${PROJECT_SOURCE_DIR}/src/misc/python_extensions.cc

    ${PROJECT_SOURCE_DIR}/src/simulation/car_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/simulation/pad_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/simulation/ball_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/simulation/goal_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/simulation/game_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/simulation/field_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/simulation/curve_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/simulation/geometry_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/simulation/navigator_pybind11.cc

    ${PROJECT_SOURCE_DIR}/src/mechanics/jump_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/mechanics/dodge_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/mechanics/drive_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/mechanics/aerial_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/mechanics/wavedash_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/mechanics/boostdash_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/mechanics/aerial_turn_pybind11.cc
    ${PROJECT_SOURCE_DIR}/src/mechanics/follow_path_pybind11.cc

    ${PROJECT_SOURCE_DIR}/src/linear_algebra/math_pybind11.cc

    ${rlu_sources}
  )
  target_include_directories(rlutilities PUBLIC inc/)
  target_compile_definitions(rlutilities PUBLIC -DGENERATE_PYTHON_BINDINGS)
  target_compile_definitions(rlutilities PUBLIC -DHAVE_SNPRINTF=1)
  target_compile_features(rlutilities PUBLIC cxx_std_17)

endif()
