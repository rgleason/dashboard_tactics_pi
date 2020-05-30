set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/tests)
set(CTEST_BINARY_DIRECTORY ${PROJECT_BINARY_DIR}/tests)

enable_testing()

add_library(testmain OBJECT testmain.cpp)

#
# test-01_tactics_functions
#
add_executable(test-01_tactics_functions $<TARGET_OBJECTS:testmain>
  ../src/TacticsFunctions.cpp 
  test-01_tactics_functions.cpp)

add_test(test-01_tactics_functions test-01_tactics_functions)

set_tests_properties(test-01_tactics_functions
  PROPERTIES
  TIMEOUT 5)

