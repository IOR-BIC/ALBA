#
#  CMakeLists.txt
#  ALBA
#
#  Created by Daniele Giunchi - Paolo Quadrani on 10/09/10.
#  Copyright 2009 BIC
#
#  See Licence at: http://tiny.cc/QXJ4D
#
#

## #################################################################
## find doxygen
## #################################################################

set(DOXYGEN_FIND_QUIETLY TRUE)

find_package(Doxygen)

## #################################################################
## Apply doxygen
## #################################################################

if (DOXYGEN_FOUND)
  configure_file(ALBADoxyfile.in     ${CMAKE_BINARY_DIR}/qa/doc_output/ALBADoxyfile IMMEDIATE @ONLY)
  configure_file(ALBADoxyfileWithoutvtkMaf.in     ${CMAKE_BINARY_DIR}/qa/doc_output/ALBADoxyfileWithoutvtkMaf IMMEDIATE @ONLY)
  configure_file(ALBADoxyfileWithTests.in     ${CMAKE_BINARY_DIR}/qa/doc_output/ALBADoxyfileWithTests IMMEDIATE @ONLY)
  configure_file(ALBAPythonDoxyfile.in     ${CMAKE_BINARY_DIR}/qa/doc_output/ALBAPythonDoxyfile IMMEDIATE @ONLY)
  configure_file(vtkALBADoxyfile.in     ${CMAKE_BINARY_DIR}/qa/doc_output/vtkALBADoxyfile IMMEDIATE @ONLY)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${CMAKE_COMMAND} -E remove_directory DocOutput
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${CMAKE_COMMAND} -E remove_directory DocWithTestsOutput
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${CMAKE_COMMAND} -E remove_directory vtkALBADoxyfile
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${CMAKE_COMMAND} -E remove_directory ALBADoxyfileWithoutvtkMaf
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${DOXYGEN_EXECUTABLE} ALBADoxyfile
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${DOXYGEN_EXECUTABLE} ALBADoxyfileWithTests
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${DOXYGEN_EXECUTABLE} vtkALBADoxyfile
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${DOXYGEN_EXECUTABLE} ALBADoxyfileWithoutvtkMaf
)

else (DOXYGEN_FOUND)
  message(INFO "Install doxygen for creating input for QA.")
endif (DOXYGEN_FOUND)

