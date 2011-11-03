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

else (DOXYGEN_FOUND)
  message(INFO "Install doxygen for creating input for QA.")
endif (DOXYGEN_FOUND)

SET(SRC_TMP_DIR ${CMAKE_BINARY_DIR}/qa/doc_output/TmpSrc)
FILE(MAKE_DIRECTORY ${SRC_TMP_DIR})

SET(SRC_DIRS  "${MAF_BINARY_DIR}" "${MAF_SOURCE_DIR}/Base" "${MAF_BINARY_DIR}/Interaction" "${MAF_SOURCE_DIR}/Interaction" "${MAF_SOURCE_DIR}/Interaction/Drivers" 
              "${MAF_SOURCE_DIR}/Core" "${MAF_SOURCE_DIR}/Gui" "${MAF_SOURCE_DIR}/IO" "${MAF_SOURCE_DIR}/Operations" "${MAF_SOURCE_DIR}/VME" 
              "${MAF_SOURCE_DIR}/VME/DataPipes" "${MAF_SOURCE_DIR}/vtkMAF" "${MAF_BINARY_DIR}/vtkMAF" "${MAF_SOURCE_DIR}/GPUAPI" 
              "${MAF_SOURCE_DIR}/GPUAPI/GLEW" "${MAF_SOURCE_DIR}/GPUAPI/FBO")
              
FILE(GLOB files_to_remove "${SRC_TMP_DIR}/*.*")
FOREACH(f ${files_to_remove})
  FILE(REMOVE "${f}")
ENDFOREACH(f ${files_to_remove})

FOREACH(dirs ${SRC_DIRS})
  FILE(GLOB source1_file_list "${dirs}/*.cpp")
  FILE(GLOB template_file_list "${dirs}/*.txx")
  FILE(GLOB source2_file_list "${dirs}/*.cxx")
  
  FOREACH(source1_file ${source1_file_list})
    FILE(RELATIVE_PATH source1_file_name "${dirs}" "${source1_file}")
    CONFIGURE_FILE("${source1_file}" "${CMAKE_BINARY_DIR}/qa/doc_output/TmpSrc/${source1_file_name}")
  ENDFOREACH(source1_file ${source1_file_list})
  
  FOREACH(template_file ${template_file_list})
    FILE(RELATIVE_PATH template_file_name "${dirs}" "${template_file}")
    CONFIGURE_FILE("${template_file}" "${CMAKE_BINARY_DIR}/qa/doc_output/TmpSrc/${template_file_name}")
  ENDFOREACH(template_file ${template_file_list})
  
  FOREACH(source2_file ${source2_file_list})
    FILE(RELATIVE_PATH source2_file_name "${dirs}" "${source2_file}")
    CONFIGURE_FILE("${source2_file}" "${CMAKE_BINARY_DIR}/qa/doc_output/TmpSrc/${source2_file_name}")
  ENDFOREACH(source2_file ${source2_file_list})
ENDFOREACH(dirs ${SRC_DIRS})
