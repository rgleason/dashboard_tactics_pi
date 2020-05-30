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

#
# test-02_SkData_class
#
add_executable(test-02_SkData_class $<TARGET_OBJECTS:testmain>
  ../src/SkData.cpp
  ../src/wxJSON/jsonval.cpp
  test-02_SkData_class.cpp)
target_link_libraries(test-02_SkData_class ${wxWidgets_LIBRARIES})

add_test(test-02_SkData_class test-02_SkData_class)

set_tests_properties(test-02_SkData_class
  PROPERTIES
  TIMEOUT 10)

