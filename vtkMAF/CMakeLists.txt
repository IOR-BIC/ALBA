CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Set the project/library name here.  Classes should be declared as
# "class vtkMAF_EXPORT vtkFoo", where vtkMAF is the name of the
# project set here.
PROJECT(vtkMAF)

# Set your list of sources here.  Do not change the name of the
# vtkMAF_SRCS variable.
SET(vtkMAF_SRCS

  vtkMAFOrientationMarkerWidget.h
  vtkMAFOrientationMarkerWidget.cpp
  
  vtkMAFAnnotatedCubeActor.h
  vtkMAFAnnotatedCubeActor.cpp
  
  vtkMAFGlobalAxesHeadActor.h
  vtkMAFGlobalAxesHeadActor.cpp
  
  vtkMAFCellsFilter.cxx
  vtkMAFCellsFilter.h  
  vtkMAFRemoveCellsFilter.cxx
  vtkMAFRemoveCellsFilter.h
  vtkMAFClipSurfaceBoundingBox.cxx
  vtkMAFClipSurfaceBoundingBox.h
  vtkMAFContourVolumeMapper.cxx
  vtkMAFContourVolumeMapper.h
  vtkMAFContourVolumeMapperAdv.cxx
  vtkMAFContourVolumeMapperAdv.h
  vtkMAFDistanceFilter.cxx
  vtkMAFDistanceFilter.h
  vtkMAFDOFMatrix.cxx
  vtkMAFDOFMatrix.h
  vtkMAFExtendedGlyph3D.cxx
  vtkMAFExtendedGlyph3D.h
  vtkMAFFixedCutter.cxx
  vtkMAFFixedCutter.h
  vtkMAFGlobalAxisCoordinate.cxx
  vtkMAFGlobalAxisCoordinate.h
  vtkMAFGridActor.cxx
  vtkMAFGridActor.h
  vtkMAFHistogram.cxx
  vtkMAFHistogram.h
  vtkMAFImplicitPolyData.cxx
  vtkMAFImplicitPolyData.h
  vtkMAFLocalAxisCoordinate.cxx
  vtkMAFLocalAxisCoordinate.h
  vtkMAFAdaptiveVolumeMapper.cxx
  vtkMAFAdaptiveVolumeMapper.h
  vtkMAFImageMapToWidgetColors.cxx
  vtkMAFImageMapToWidgetColors.h
  vtkMAFMeshCutter.cxx
  vtkMAFMeshCutter.h
  vtkMAFPolyDataSingleSourceShortestPath.cxx
  vtkMAFPolyDataSingleSourceShortestPath.h
  vtkMAFProjectRG.cxx
  vtkMAFProjectRG.h
  vtkMAFProjectSP.cxx
  vtkMAFProjectSP.h
  vtkMAFRayCast3DPicker.cxx
  vtkMAFRayCast3DPicker.h
  vtkMAFRectilinearGridToRectilinearGridFilter.cxx
  vtkMAFRectilinearGridToRectilinearGridFilter.h
  vtkMAFRGSliceAccumulate.cxx
  vtkMAFRGSliceAccumulate.h
  vtkMAFRulerActor2D.cxx
  vtkMAFRulerActor2D.h
  vtkMAFSimpleRulerActor2D.cxx
  vtkMAFSimpleRulerActor2D.h
  vtkMAFTextActorMeter.cxx
  vtkMAFTextActorMeter.h
  vtkMAFTransferFunction2D.cxx
  vtkMAFTransferFunction2D.h
  vtkMAFVolumeResample.h
  vtkMAFVolumeResample.cxx
  vtkMAFVolumeSlicer.cxx
  vtkMAFVolumeSlicer.h
  vtkMAFPolyDataToSinglePolyLine.cxx
  vtkMAFPolyDataToSinglePolyLine.h
  vtkMAFTextOrientator.cxx
  vtkMAFTextOrientator.h
  vtkMAFProfilingActor.cxx
  vtkMAFProfilingActor.h
  vtkMAFXYPlotActor.cxx
  vtkMAFXYPlotActor.h
  vtkMAFPolyDataNormals.cxx
  vtkMAFPolyDataNormals.h
  ####################### from vtkMAF
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
  vtkMAFCellLocator.cxx
  vtkMAFCellLocator.h
  vtkMAFAddScalarsFilter.cxx
  vtkMAFAddScalarsFilter.h
  vtkMAFExtrudeToCircle.cxx
  vtkMAFExtrudeToCircle.h
  vtkMAFFillingHole.cxx
  vtkMAFFillingHole.h
  vtkMAFFixTopology.cxx
  vtkMAFFixTopology.h
  vtkMAFImageUnsharpFilter.cxx
  vtkMAFImageUnsharpFilter.h
  vtkMAFMatrixVectorMath.h
  vtkMAFMatrixVectorMath.cxx
  vtkMAFPoissonSurfaceReconstruction.cxx
  vtkMAFPoissonSurfaceReconstruction.h
  vtkMAFPolyDataMirror.cxx
  vtkMAFPolyDataMirror.h
  vtkMAFPolyDataNavigator.h
  vtkMAFPolyDataNavigator.cxx
  vtkMAFRegionGrowingLocalGlobalThreshold.cxx
  vtkMAFRegionGrowingLocalGlobalThreshold.h
  vtkMAFHalfTubeRemoval.cxx
  vtkMAFHalfTubeRemoval.h
  vtkMAFInteractorStyleEnableRotation.cxx
  vtkMAFInteractorStyleEnableRotation.h
  vtkMAFSubdividePolylines.cxx
  vtkMAFSubdividePolylines.h
  vtkMAFLandmarkCloudPolydataFilter.cxx
  vtkMAFLandmarkCloudPolydataFilter.h
  vtkBox.cxx
  vtkBox.h
  vtkMaskPolyDataFilter.cxx
  vtkMaskPolyDataFilter.h
  vtkHoleConnectivity.cxx
  vtkHoleConnectivity.h
  vtkPatchLine.h
  vtkPatchLine.cpp
  
  ############# VolumeLarge Files ########
  vtkMAFDataArrayDescriptor.cxx
  vtkMAFDataArrayDescriptor.h
  vtkMAFFileDataProvider.cxx
  vtkMAFFileDataProvider.h
  vtkMAFLargeImageData.cxx
  vtkMAFLargeImageData.h
  vtkMAFLargeImageReader.cxx
  vtkMAFLargeImageReader.h
  vtkMAFLargeImageSource.cxx
  vtkMAFLargeImageSource.h
  vtkMAFFile.cxx
  vtkMAFFile.h	
  vtkMAFDataArrayLayout.cpp
  vtkMAFDataArrayLayout.h
  vtkMAFMultiFileDataProvider.cpp
  vtkMAFMultiFileDataProvider.h
  vtkMAFIdType64.h
  vtkMAFLargeDataProvider.cxx
  vtkMAFLargeDataProvider.h
  vtkMAFLargeDataSet.cxx
  vtkMAFLargeDataSet.h
  vtkMAFLargeDataSetCallback.cxx
  vtkMAFLargeDataSetCallback.h
  ###############################################################
  
  #####################################################
  vtkMAFVolumeRayCastMapper.cxx
  vtkMAFVolumeRayCastMapper.h
  vtkMAFVolumeTextureMapper2D.cxx
  vtkMAFVolumeTextureMapper2D.h
  vtkMAFVolumeSlicer_BES.cxx
  vtkMAFVolumeSlicer_BES.h
  vtkMAFMeshCutter_BES.cxx 
  vtkMAFMeshCutter_BES.h  	
  ###############################################
  
  ###############################################
  vtkMAFPolyDataDeformation.cpp
  vtkMAFPolyDataDeformation.h
  vtkMAFPolyDataDeformation_M1.cpp
  vtkMAFPolyDataDeformation_M1.h
  vtkMAFPolyDataDeformation_M2.cpp
  vtkMAFPolyDataDeformation_M2.h  
  ###############################################
    

  
  vtkMAFErythrocyteSource.cpp
  vtkMAFErythrocyteSource.h
  
  vtkMAFImageFillHolesRemoveIslands.cxx
  vtkMAFImageFillHolesRemoveIslands.h
  
  vtkMAFVolumeSlicerNotInterpolated.cxx
  vtkMAFVolumeSlicerNotInterpolated.h

  vtkMAFBinaryImageFloodFill.cxx
  vtkMAFBinaryImageFloodFill.h
  
  ######vtkbioeng################################
  vtkMAFCollisionDetectionFilter.cxx
  vtkMAFCollisionDetectionFilter.h
  
  vtkMAFRayCastCleaner.cpp
  vtkMAFRayCastCleaner.h
  
  vtkMAFVolumeToClosedSmoothSurface.cxx
  vtkMAFVolumeToClosedSmoothSurface.h
  
  ################################
  vtkBoundingBox.cxx
  vtkBoundingBox.h
  vtkMAFPastValuesList.cpp
  vtkMAFPastValuesList.h
  vtkMAFStentModelSource.cpp
  vtkMAFStentModelSource.h
  vtkMAFDeformableSimplexMeshFilter.h
  vtkMAFDeformableSimplexMeshFilter_Impl.h
	
	vtkMAFTubeFilter.cxx vtkMAFTubeFilter.h
	
	vtkMAFLandmarkCloudOutlineCornerFilter.cxx vtkMAFLandmarkCloudOutlineCornerFilter.h
)

IF(WIN32)
SET(vtkMAF_SRCS
	${vtkMAF_SRCS}
  vtkMAFContourVolumeMapperGPU.cxx
  vtkMAFContourVolumeMapperGPU.h
  )
ENDIF(WIN32)

IF (MAF_USE_ITK)
SET(vtkMAF_SRCS  ${vtkMAF_SRCS}
  mafClassicICPRegistration.cxx
  mafClassicICPRegistration.h
  mafICPUtility.h
)
ENDIF (MAF_USE_ITK)

# List the kits from VTK that are needed by this project.
SET(vtkMAF_LIBS
  vtkCommon
  vtkRendering
  vtkFiltering
  vtkGraphics
  vtkHybrid
)

IF(WIN32)
SET(vtkMAF_LIBS
  ${vtkMAF_LIBS}
  # GPUAPI
  )
ENDIF(WIN32)


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
#OPTION(BUILD_SHARED_LIBS "Build with shared libraries." ${VTK_BUILD_SHARED_LIBS})

# Set VTKLOCAL_BUILD_SHARED_LIBS to 1 or 0 for use in creating the
# configuration header.
IF(MAF_BUILD_MAFDLL)
  IF(WIN32)
  SET(vtkMAF_LIBS
    ${vtkMAF_LIBS}
    GPUAPI
    )
  ENDIF(WIN32)
  SET(BUILD_SHARED_LIBS 1)
  SET(VTKLOCAL_BUILD_SHARED_LIBS_CONFIG 1)
  ADD_DEFINITIONS(-DMAF_EXPORTS)
ELSE(MAF_BUILD_MAFDLL)
  SET(BUILD_SHARED_LIBS 0)
  SET(VTKLOCAL_BUILD_SHARED_LIBS_CONFIG 0)
ENDIF(MAF_BUILD_MAFDLL)

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
  # CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/mafConfigure.h.in
  #                ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Configure.h
  #                @ONLY IMMEDIATE)
				 

  # Create the instantiator for these classes.
  SET(VTK_USE_INSTANTIATOR_NEW 1)
  VTK_MAKE_INSTANTIATOR2("${PROJECT_NAME}Instantiator" vtkMAFInstantiator_SRCS
                         ${vtkMAF_SRCS}
                         EXPORT_MACRO "MAF_EXPORT"
                         HEADER_LOCATION ${PROJECT_BINARY_DIR}
                         INCLUDES "mafConfigure.h")

  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${vtkMAF_SRCS} ${vtkMAFInstantiator_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${vtkMAF_LIBS})
  # this is necessaty to force MFL compilation....

  #2 IF (MFL_BUILD_TARGETS)
  #2   ADD_DEPENDENCIES(${PROJECT_NAME} ${MFL_BUILD_TARGETS})
  #2 ENDIF (MFL_BUILD_TARGETS)

  # Create Tcl wrappers if VTK is wrapped in Tcl.
  IF(VTK_WRAP_TCL)
    VTK_WRAP_TCL2(${PROJECT_NAME}TCL vtkMAFTCL_SRCS ${vtkMAF_SRCS})
    ADD_LIBRARY(${PROJECT_NAME}TCL ${vtkMAFTCL_SRCS})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}TCL vtkCommonTCL ${PROJECT_NAME})

    # Create the list of kits that are required.
    SET(VTKLOCAL_KITS "")
    FOREACH(c ${vtkMAF_LIBS})
      SET(VTKLOCAL_KITS "${VTKLOCAL_KITS} ${c}")
    ENDFOREACH(c)
    FOREACH(c ${vtkMAF_SRCS})
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
    VTK_WRAP_PYTHON2(${PROJECT_NAME}Python vtkMAFPYTHON_SRCS ${vtkMAF_SRCS})
    ADD_LIBRARY(${PROJECT_NAME}Python MODULE ${vtkMAFPYTHON_SRCS})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}Python vtkCommonPython ${PROJECT_NAME})
  ENDIF(VTK_WRAP_PYTHON)

  # Create Java wrappers if VTK is wrapped in Java.
  IF(VTK_WRAP_JAVA)
    SET(VTK_WRAP_HINTS "${PROJECT_SOURCE_DIR}/hints")
    SET(VTK_JAVA_HOME ${PROJECT_BINARY_DIR}/java/${PROJECT_NAME})
    MAKE_DIRECTORY(${VTK_JAVA_HOME})
    VTK_WRAP_JAVA2(${PROJECT_NAME}Java vtkMAFJAVA_SRCS ${vtkMAF_SRCS})
    ADD_LIBRARY(${PROJECT_NAME}Java ${vtkMAFJAVA_SRCS})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}Java vtkCommonJava ${PROJECT_NAME})
  ENDIF(VTK_WRAP_JAVA)
  
  # build testing if necessary
  #OPTION(BUILD_TESTING "Build the testing tree." OFF)
  #IF (BUILD_TESTING)
  #  IF (NOT MAKECOMMAND)
  #    INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
  #    ENABLE_TESTING()
  #  ENDIF (NOT MAKECOMMAND)
    
  #  OPTION(BUILD_vtkMAF_TESTING "Enable building of testing for vtkMAF classes" ON)
  #  IF (BUILD_vtkMAF_TESTING)
  #    SUBDIRS(Testing)
  #  ENDIF (BUILD_vtkMAF_TESTING)
  #ENDIF (BUILD_TESTING)
ENDIF(VTK_FOUND)
