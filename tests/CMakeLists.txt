set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/tests)
set(CTEST_BINARY_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
add_definitions( -D__DOCTEST__ )

enable_testing()

add_library(testmain OBJECT testmain.cpp)

#
# test-01_DashboardFunctions
#
add_executable(test-01_DashboardFunctions $<TARGET_OBJECTS:testmain>
  ../src/DashboardFunctions.cpp 
  test-01_DashboardFunctions.cpp)
target_link_libraries(test-01_DashboardFunctions ${wxWidgets_LIBRARIES} )
 
add_test(test-01_DashboardFunctions test-01_DashboardFunctions)

set_tests_properties(test-01_DashboardFunctions
  PROPERTIES
  TIMEOUT 5)

#
# test-02_TacticsFunctions
#
add_executable(test-02_TacticsFunctions $<TARGET_OBJECTS:testmain>
  ../src/TacticsFunctions.cpp 
  test-02_TacticsFunctions.cpp)
target_link_libraries(test-02_TacticsFunctions ${wxWidgets_LIBRARIES} )

add_test(test-02_TacticsFunctions test-02_TacticsFunctions)

set_tests_properties(test-02_TacticsFunctions
  PROPERTIES
  TIMEOUT 5)

#
# test-03_SkData
#
add_executable(test-03_SkData $<TARGET_OBJECTS:testmain>
  ../src/wxJSON/jsonval.cpp
  ../src/SkData.cpp
  test-03_SkData.cpp)
target_link_libraries(test-03_SkData ${wxWidgets_LIBRARIES})

add_test(test-03_SkData test-03_SkData)

set_tests_properties(test-03_SkData
  PROPERTIES
  TIMEOUT 10)

