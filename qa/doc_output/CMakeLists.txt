#
#  CMakeLists.txt
#  MAF
#
#  Created by Daniele Giunchi - Paolo Quadrani on 10/09/10.
#  Copyright 2009 B3C. All rights reserved.
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
  configure_file(MAF2Doxyfile.in     ${CMAKE_BINARY_DIR}/qa/doc_output/MAF2Doxyfile IMMEDIATE @ONLY)
  configure_file(MAF2DoxyfileWithTests.in     ${CMAKE_BINARY_DIR}/qa/doc_output/MAF2DoxyfileWithTests IMMEDIATE @ONLY)
  configure_file(MAF2PythonDoxyfile.in     ${CMAKE_BINARY_DIR}/qa/doc_output/MAF2PythonDoxyfile IMMEDIATE @ONLY)
  configure_file(vtkMAF2Doxyfile.in     ${CMAKE_BINARY_DIR}/qa/doc_output/vtkMAF2Doxyfile IMMEDIATE @ONLY)

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
  COMMAND ${DOXYGEN_EXECUTABLE} MAF2Doxyfile
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${DOXYGEN_EXECUTABLE} MAF2DoxyfileWithTests
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${DOXYGEN_EXECUTABLE} vtkMAF2Doxyfile
)

else (DOXYGEN_FOUND)
  message(INFO "Install doxygen for creating input for QA.")
endif (DOXYGEN_FOUND)

