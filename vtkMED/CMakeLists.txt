CMAKE_MINIMUM_REQUIRED(VERSION 1.6)

# Set the project/library name here.  Classes should be declared as
# "class vtkMED_EXPORT vtkFoo", where vtkMED is the name of the
# project set here.
PROJECT(vtkMED)

# Set your list of sources here.  Do not change the name of the
# vtkMED_SRCS variable.
SET(vtkMED_SRCS
  vtkDicomUnPacker.cxx
  vtkDicomUnPacker.h
  vtkImageUnPacker.cxx
  vtkImageUnPacker.h
  vtkPackedImage.cxx
  vtkPackedImage.h
  vtkXRayVolumeMapper.cxx
  vtkXRayVolumeMapper.h
  vtkWeightedLandmarkTransform.cxx
  vtkWeightedLandmarkTransform.h
  vtkTriangleQualityRatio.cxx
  vtkTriangleQualityRatio.h
  vtkMaskPolyDataFilter.cxx
  vtkMaskPolyDataFilter.h
)

IF (MAF_USE_ITK)
SET(vtkMED_SRCS  ${vtkMED_SRCS}
  mafClassicICPRegistration.cxx
  mafClassicICPRegistration.h
  mafICPUtility.h
)
ENDIF (MAF_USE_ITK)

# List the kits from VTK that are needed by this project.
SET(vtkMED_LIBS
  vtkCommon
  vtkRendering
  vtkFiltering
)

#-----------------------------------------------------------------------------
# Most users should not need to change anything below this line.

# Need to include class headers and the configuration header.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

# Find and load VTK settings.
IF (NOT VTK_FOUND)
  INCLUDE(${CMAKE_ROOT}/Modules/FindVTK.cmake)
  IF(VTK_FOUND)
    INCLUDE(${VTK_USE_FILE})
  ENDIF(VTK_FOUND)
ENDIF (NOT VTK_FOUND)

# Give user option of building shared or static libraries.  Default to
# the choice made for VTK.
OPTION(BUILD_SHARED_LIBS "Build with shared libraries." 
       ${VTK_BUILD_SHARED_LIBS})

# Set VTKLOCAL_BUILD_SHARED_LIBS to 1 or 0 for use in creating the
# configuration header.
IF(BUILD_SHARED_LIBS)
  SET(VTKLOCAL_BUILD_SHARED_LIBS_CONFIG 1)
ELSE(BUILD_SHARED_LIBS)
  SET(VTKLOCAL_BUILD_SHARED_LIBS_CONFIG 0)
ENDIF(BUILD_SHARED_LIBS)

IF (NOT LIBRARY_OUTPUT_PATH)
  SET (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH
       "Single output directory for building all libraries.")
  MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH)
ENDIF (NOT LIBRARY_OUTPUT_PATH)

IF (EXECUTABLE_OUTPUT_PATH)
  SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH
       "Single output directory for building all executables.")
  MARK_AS_ADVANCED(EXECUTABLE_OUTPUT_PATH)
ENDIF (EXECUTABLE_OUTPUT_PATH)

IF(VTK_FOUND)
  # If this is a build tree, provide an option for putting this
  # project's executables and libraries in with VTK's.
  IF (EXISTS ${VTK_DIR}/bin)
    OPTION(USE_VTK_OUTPUT_PATHS
     "Use VTK's output directory for this project's executables and libraries."
     OFF)
    MARK_AS_ADVANCED (USE_VTK_OUTPUT_PATHS)
    IF (USE_VTK_OUTPUT_PATHS)
      SET (LIBRARY_OUTPUT_PATH ${VTK_DIR}/bin)
      SET (EXECUTABLE_OUTPUT_PATH ${VTK_DIR}/bin)
    ENDIF (USE_VTK_OUTPUT_PATHS)
  ENDIF (EXISTS ${VTK_DIR}/bin)

  # Create the header to configure the classes.
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/vtkMEDConfigure.h.in
                 ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Configure.h
                 @ONLY IMMEDIATE)

  # Create the instantiator for these classes.
  SET(VTK_USE_INSTANTIATOR_NEW 1)
  VTK_MAKE_INSTANTIATOR2("${PROJECT_NAME}Instantiator" vtkMEDInstantiator_SRCS
                         ${vtkMED_SRCS}
                         EXPORT_MACRO "VTK_${PROJECT_NAME}_EXPORT"
                         HEADER_LOCATION ${PROJECT_BINARY_DIR}
                         INCLUDES "${PROJECT_NAME}Configure.h")

  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${vtkMED_SRCS} ${vtkMEDInstantiator_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${vtkMED_LIBS})
  # this is necessaty to force MFL compilation....
  IF (MFL_BUILD_TARGETS)
    ADD_DEPENDENCIES(${PROJECT_NAME} ${MFL_BUILD_TARGETS})
  ENDIF (MFL_BUILD_TARGETS)

  # Create Tcl wrappers if VTK is wrapped in Tcl.
  IF(VTK_WRAP_TCL)
    VTK_WRAP_TCL2(${PROJECT_NAME}TCL vtkMEDTCL_SRCS ${vtkMED_SRCS})
    ADD_LIBRARY(${PROJECT_NAME}TCL ${vtkMEDTCL_SRCS})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}TCL vtkCommonTCL ${PROJECT_NAME})

    # Create the list of kits that are required.
    SET(VTKLOCAL_KITS "")
    FOREACH(c ${vtkMED_LIBS})
      SET(VTKLOCAL_KITS "${VTKLOCAL_KITS} ${c}")
    ENDFOREACH(c)
    FOREACH(c ${vtkMED_SRCS})
      GET_FILENAME_COMPONENT(VTKLOCAL_LAST_CLASS ${c} NAME_WE)
    ENDFOREACH(c)

    IF(CMAKE_CONFIGURATION_TYPES)
      FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
        SET(VTKLOCAL_TCL_LIBRARY_DIR "${LIBRARY_OUTPUT_PATH}/${config}")
        CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/pkgIndex.tcl.in
                       ${PROJECT_BINARY_DIR}/${config}/pkgIndex.tcl
                       @ONLY IMMEDIATE)
      ENDFOREACH(config)
    ELSE(CMAKE_CONFIGURATION_TYPES)
      SET(VTKLOCAL_TCL_LIBRARY_DIR "${LIBRARY_OUTPUT_PATH}")
      CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/pkgIndex.tcl.in
                     ${PROJECT_BINARY_DIR}/pkgIndex.tcl
                     @ONLY IMMEDIATE)
    ENDIF(CMAKE_CONFIGURATION_TYPES)

  ENDIF(VTK_WRAP_TCL)

  # Create Python wrappers if VTK is wrapped in Python.
  IF(VTK_WRAP_PYTHON)
    VTK_WRAP_PYTHON2(${PROJECT_NAME}Python vtkMEDPYTHON_SRCS ${vtkMED_SRCS})
    ADD_LIBRARY(${PROJECT_NAME}Python MODULE ${vtkMEDPYTHON_SRCS})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}Python vtkCommonPython ${PROJECT_NAME})
  ENDIF(VTK_WRAP_PYTHON)

  # Create Java wrappers if VTK is wrapped in Java.
  IF(VTK_WRAP_JAVA)
    SET(VTK_WRAP_HINTS "${PROJECT_SOURCE_DIR}/hints")
    SET(VTK_JAVA_HOME ${PROJECT_BINARY_DIR}/java/${PROJECT_NAME})
    MAKE_DIRECTORY(${VTK_JAVA_HOME})
    VTK_WRAP_JAVA2(${PROJECT_NAME}Java vtkMEDJAVA_SRCS ${vtkMED_SRCS})
    ADD_LIBRARY(${PROJECT_NAME}Java ${vtkMEDJAVA_SRCS})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}Java vtkCommonJava ${PROJECT_NAME})
  ENDIF(VTK_WRAP_JAVA)
  
  # build testing if necessary
  OPTION(BUILD_TESTING "Build the testing tree." OFF)
  IF (BUILD_TESTING)
    IF (NOT MAKECOMMAND)
      INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
      ENABLE_TESTING()
    ENDIF (NOT MAKECOMMAND)
    
    OPTION(BUILD_vtkMED_TESTING "Enable building of testing for vtkMED classes" ON)
    IF (BUILD_vtkMED_TESTING)
      SUBDIRS(Testing)
    ENDIF (BUILD_vtkMED_TESTING)
  ENDIF (BUILD_TESTING)
ENDIF(VTK_FOUND)
