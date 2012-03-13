
#
# Add all the executables that require rendering
#
ADD_EXECUTABLE(ExampleTest Main.cpp ExampleTestCase.cpp ExampleTestCase.h)

#
# Add tests
#
ADD_TEST(ExampleTest ${EXECUTABLE_OUTPUT_PATH}/ExampleTest)

