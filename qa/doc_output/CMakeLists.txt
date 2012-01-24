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
  configure_file(MEDDoxyfile.in     ${CMAKE_BINARY_DIR}/qa/doc_output/MEDDoxyfile IMMEDIATE @ONLY)
  configure_file(MEDDoxyfileWithTests.in     ${CMAKE_BINARY_DIR}/qa/doc_output/MEDDoxyfileWithTests IMMEDIATE @ONLY)
  configure_file(MEDDoxyfileWithoutvtkMed.in     ${CMAKE_BINARY_DIR}/qa/doc_output/MEDDoxyfileWithoutvtkMed IMMEDIATE @ONLY)
  # configure_file(MAF2PythonDoxyfile.in     ${CMAKE_BINARY_DIR}/qa/doc_output/MAF2PythonDoxyfile IMMEDIATE @ONLY)
  configure_file(vtkMEDDoxyfile.in     ${CMAKE_BINARY_DIR}/qa/doc_output/vtkMEDDoxyfile IMMEDIATE @ONLY)

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
  COMMAND ${CMAKE_COMMAND} -E remove_directory DocOutputWithoutvtkMed
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${CMAKE_COMMAND} -E remove_directory DocOutputvtkMed
)


execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${DOXYGEN_EXECUTABLE} MEDDoxyfile
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${DOXYGEN_EXECUTABLE} MEDDoxyfileWithTests
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${DOXYGEN_EXECUTABLE} vtkMEDDoxyfile
)

execute_process(
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/qa/doc_output
  COMMAND ${DOXYGEN_EXECUTABLE} MEDDoxyfileWithoutvtkMed
)

else (DOXYGEN_FOUND)
  message(INFO "Install doxygen for creating input for QA.")
endif (DOXYGEN_FOUND)

