#
# Copyright and description to be placed here
#

PROJECT(mafCore)
MESSAGE("Configuring project mafCore")
# Set your list of sources here.  Do not change the name of the
# vtkLocal_SRCS variable.
SET(mafCore_SRCS
  mafString.cpp
  mafString.h
  mafDefines.h
)

# List the kits from VTK that are needed by this project.
SET(mafCore_LIBS
  vtkCommon
)

#-----------------------------------------------------------------------------
# Do not change anything below this line.

# Need to include class headers and the configuration header.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

# Create the header to configure the classes.
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Configure.h.in
               ${PROJECT_BINARY_DIR}/Configure.h
               @ONLY IMMEDIATE)

# Create the instantiator for these classes.
#SET(VTK_USE_INSTANTIATOR_NEW 1)
#VTK_MAKE_INSTANTIATOR2("${PROJECT_NAME}Instantiator" vtkLocalInstantiator_SRCS
 #                      ${vtkLocal_SRCS}
#                       EXPORT_MACRO "VTK_${PROJECT_NAME}_EXPORT"
#                       HEADER_LOCATION ${PROJECT_BINARY_DIR}
#                       INCLUDES "${PROJECT_NAME}Configure.h")

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${mafCore_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${mafCore_LIBS})

# Create Tcl wrappers if VTK is wrapped in Tcl.
#IF(VTK_WRAP_TCL)
#  VTK_WRAP_TCL2(${PROJECT_NAME}TCL vtkLocalTCL_SRCS ${vtkLocal_SRCS})
#  ADD_LIBRARY(${PROJECT_NAME}TCL ${vtkLocalTCL_SRCS})
#  TARGET_LINK_LIBRARIES(${PROJECT_NAME}TCL vtkCommonTCL ${PROJECT_NAME})

  # Create the list of kits that are required.
#  SET(VTKLOCAL_KITS "")
#  FOREACH(c ${vtkLocal_LIBS})
#    SET(VTKLOCAL_KITS "${VTKLOCAL_KITS} ${c}")
#  ENDFOREACH(c)
#  FOREACH(c ${vtkLocal_SRCS})
#    GET_FILENAME_COMPONENT(VTKLOCAL_LAST_CLASS ${c} NAME_WE)
#  ENDFOREACH(c)

#  IF(CMAKE_CONFIGURATION_TYPES)
#    FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
#      SET(VTKLOCAL_TCL_LIBRARY_DIR "${LIBRARY_OUTPUT_PATH}/${config}")
#      CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/pkgIndex.tcl.in
#                     ${PROJECT_BINARY_DIR}/${config}/pkgIndex.tcl
#                     @ONLY IMMEDIATE)
#    ENDFOREACH(config)
#  ELSE(CMAKE_CONFIGURATION_TYPES)
#    SET(VTKLOCAL_TCL_LIBRARY_DIR "${LIBRARY_OUTPUT_PATH}")
#    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/pkgIndex.tcl.in
#                   ${PROJECT_BINARY_DIR}/pkgIndex.tcl
#                   @ONLY IMMEDIATE)
#  ENDIF(CMAKE_CONFIGURATION_TYPES)

#ENDIF(VTK_WRAP_TCL)

#  # Create Python wrappers if VTK is wrapped in Python.
#  IF(VTK_WRAP_PYTHON)
#    VTK_WRAP_PYTHON2(${PROJECT_NAME}Python vtkLocalPYTHON_SRCS ${vtkLocal_SRCS})
#    ADD_LIBRARY(${PROJECT_NAME}Python MODULE ${vtkLocalPYTHON_SRCS})
#    TARGET_LINK_LIBRARIES(${PROJECT_NAME}Python vtkCommonPython ${PROJECT_NAME})
#  ENDIF(VTK_WRAP_PYTHON)

  # Create Java wrappers if VTK is wrapped in Java.
#  IF(VTK_WRAP_JAVA)
#    SET(VTK_WRAP_HINTS "${PROJECT_SOURCE_DIR}/hints")
#    SET(VTK_JAVA_HOME ${PROJECT_BINARY_DIR}/java/${PROJECT_NAME})
#    MAKE_DIRECTORY(${VTK_JAVA_HOME})
#    VTK_WRAP_JAVA2(${PROJECT_NAME}Java vtkLocalJAVA_SRCS ${vtkLocal_SRCS})
#    ADD_LIBRARY(${PROJECT_NAME}Java ${vtkLocalJAVA_SRCS})
#    TARGET_LINK_LIBRARIES(${PROJECT_NAME}Java vtkCommonJava ${PROJECT_NAME})
#  ENDIF(VTK_WRAP_JAVA)

IF(BUILD_TESTING)
  SUBDIRS(Testing)
ENDIF(BUILD_TESTING)