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
  configure_file(MAF3Doxyfile.in     ${CMAKE_BINARY_DIR}/qa/doc_output/MAF3Doxyfile IMMEDIATE @ONLY)
  configure_file(MAF3DoxyfileWithTests.in     ${CMAKE_BINARY_DIR}/qa/doc_output/MAF3DoxyfileWithTests IMMEDIATE @ONLY)
  configure_file(MAF3PythonDoxyfile.in     ${CMAKE_BINARY_DIR}/qa/doc_output/MAF3PythonDoxyfile IMMEDIATE @ONLY)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${CMAKE_COMMAND} -E remove_directory DocOutput
  COMMAND ${DOXYGEN_EXECUTABLE} MAF3Doxyfile
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${CMAKE_COMMAND} -E remove_directory DocWithTestsOutput
  COMMAND ${DOXYGEN_EXECUTABLE} MAF3DoxyfileWithTests
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${CMAKE_COMMAND} -E remove_directory PythonDocOutput
  COMMAND ${DOXYGEN_EXECUTABLE} MAF3PythonDoxyfile
)


else (DOXYGEN_FOUND)
  message(INFO "Install doxygen for creating input for QA.")
endif (DOXYGEN_FOUND)
