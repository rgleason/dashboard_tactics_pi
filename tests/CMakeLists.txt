set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/tests)
set(CTEST_BINARY_DIRECTORY ${PROJECT_BINARY_DIR}/tests)

enable_testing()

add_library(test_main OBJECT testmain.cpp)

# add_executable(test_one testmain.cpp test_one.cpp)
# add_test(NAME test_one COMMAND test_one)

file(GLOB files "test_*.cpp")

foreach(file ${files})
  string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${file})
  add_executable(${file_without_ext} testmain.cpp ${file})
  # target_link_libraries(${file_without_ext} ${PROJECT_LIBS})
  add_test(${file_without_ext} ${file_without_ext})
  set_tests_properties(${file_without_ext}
	PROPERTIES
	TIMEOUT 60)
 endforeach()
