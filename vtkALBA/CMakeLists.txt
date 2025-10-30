CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Set the project/library name here.  Classes should be declared as
# "class vtkALBA_EXPORT vtkFoo", where vtkALBA is the name of the
# project set here.
PROJECT(vtkALBA)

MESSAGE(STATUS "ALBA: Configuring vtkALBA")

# Set your list of sources here.  Do not change the name of the
# vtkALBA_SRCS variable.
SET(vtkALBA_SRCS

  vtkALBAOrientationMarkerWidget.h  						vtkALBAOrientationMarkerWidget.cpp  
  vtkALBAAnnotatedCubeActor.h  							vtkALBAAnnotatedCubeActor.cpp  
  vtkALBAGlobalAxesPolydataActor.h  			vtkALBAGlobalAxesPolydataActor.cpp
  
  vtkALBACellsFilter.cxx  								vtkALBACellsFilter.h  
  vtkALBARemoveCellsFilter.cxx  							vtkALBARemoveCellsFilter.h
  vtkALBAClipSurfaceBoundingBox.cxx  					vtkALBAClipSurfaceBoundingBox.h
  vtkALBADistanceFilter.cxx  							vtkALBADistanceFilter.h
  vtkALBADOFMatrix.cxx  									vtkALBADOFMatrix.h
  vtkALBAExtendedGlyph3D.cxx  							vtkALBAExtendedGlyph3D.h
  vtkALBAFixedCutter.cxx  								vtkALBAFixedCutter.h
  vtkALBAGlobalAxisCoordinate.cxx  						vtkALBAGlobalAxisCoordinate.h
  vtkALBAGridActor.cxx  									vtkALBAGridActor.h
  vtkALBAHistogram.cxx  									vtkALBAHistogram.h
  vtkALBAImplicitPolyData.cxx  							vtkALBAImplicitPolyData.h
  vtkALBALocalAxisCoordinate.cxx  						vtkALBALocalAxisCoordinate.h
  vtkALBAImageMapToWidgetColors.cxx  					vtkALBAImageMapToWidgetColors.h
  vtkALBAPolyDataSingleSourceShortestPath.cxx  			vtkALBAPolyDataSingleSourceShortestPath.h
  vtkALBAProjectVolume.cxx  									vtkALBAProjectVolume.h
  vtkALBARayCast3DPicker.cxx  							vtkALBARayCast3DPicker.h
  vtkALBARulerActor2D.cxx  								vtkALBARulerActor2D.h
  vtkALBASimpleRulerActor2D.cxx  						vtkALBASimpleRulerActor2D.h
  vtkALBATextActorMeter.cxx  							vtkALBATextActorMeter.h
  vtkALBATransferFunction2D.cxx  						vtkALBATransferFunction2D.h
  vtkALBAVolumeResample.h  								vtkALBAVolumeResample.cxx
  vtkALBAPolyDataToSinglePolyLine.cxx  					vtkALBAPolyDataToSinglePolyLine.h
  vtkALBATextOrientator.cxx  							vtkALBATextOrientator.h
  vtkALBAProfilingActor.cxx  							vtkALBAProfilingActor.h
  vtkALBAXYPlotActor.cxx  								vtkALBAXYPlotActor.h
  vtkALBAPolyDataNormals.cxx  							vtkALBAPolyDataNormals.h
  ##from vtkALBA
  vtkImageUnPacker.cxx  								vtkImageUnPacker.h
  vtkPackedImage.cxx  									vtkPackedImage.h
  vtkXRayVolumeMapper.cxx    							vtkXRayVolumeMapper.h
  vtkWeightedLandmarkTransform.cxx    					vtkWeightedLandmarkTransform.h
  vtkTriangleQualityRatio.cxx  							vtkTriangleQualityRatio.h
  vtkALBAAddScalarsFilter.cxx  							vtkALBAAddScalarsFilter.h
  vtkALBAExtrudeToCircle.cxx  							vtkALBAExtrudeToCircle.h
  vtkALBAFillingHole.cxx  								vtkALBAFillingHole.h
  vtkALBAFixTopology.cxx  								vtkALBAFixTopology.h
  vtkALBAImageUnsharpFilter.cxx  						vtkALBAImageUnsharpFilter.h
  vtkALBAMatrixVectorMath.h  							vtkALBAMatrixVectorMath.cxx
  vtkALBAPoissonSurfaceReconstruction.cxx  				vtkALBAPoissonSurfaceReconstruction.h
  vtkALBAPolyDataMirror.cxx  							vtkALBAPolyDataMirror.h
  vtkALBAPolyDataNavigator.h  							vtkALBAPolyDataNavigator.cxx
  vtkALBARegionGrowingLocalGlobalThreshold.cxx  			vtkALBARegionGrowingLocalGlobalThreshold.h
  vtkALBAHalfTubeRemoval.cxx  							vtkALBAHalfTubeRemoval.h
  vtkALBAInteractorStyleEnableRotation.cxx  				vtkALBAInteractorStyleEnableRotation.h
  vtkALBASubdividePolylines.cxx  						vtkALBASubdividePolylines.h
  vtkALBALandmarkCloudPolydataFilter.cxx  				vtkALBALandmarkCloudPolydataFilter.h
  vtkMaskPolyDataFilter.cxx  							vtkMaskPolyDataFilter.h
  vtkHoleConnectivity.cxx  								vtkHoleConnectivity.h
  vtkPatchLine.h  										vtkPatchLine.cpp
  
  vtkALBALineStripSource.cpp 							vtkALBALineStripSource.h

  vtkALBADataArrayDescriptor.cxx  						vtkALBADataArrayDescriptor.h
  vtkALBAFile.cxx  										vtkALBAFile.h	
  
  ###############################################
  vtkALBAVolumeTextureMapper2D.cxx  						vtkALBAVolumeTextureMapper2D.h
  vtkALBAVolumeSlicer.cxx  								vtkALBAVolumeSlicer.h
	vtkALBAVolumeOrthoSlicer.cxx 			vtkALBAVolumeOrthoSlicer.h
  vtkALBAMeshCutter.cxx   								vtkALBAMeshCutter.h  	
	vtkALBARGtoSPImageFilter.cxx					vtkALBARGtoSPImageFilter.h
  ###############################################
  
  ###############################################
  vtkALBAPolyDataDeformation.cpp  						vtkALBAPolyDataDeformation.h
  vtkALBAPolyDataDeformation_M1.cpp  					vtkALBAPolyDataDeformation_M1.h
  vtkALBAPolyDataDeformation_M2.cpp  					vtkALBAPolyDataDeformation_M2.h  
  ###############################################
      
  vtkALBAErythrocyteSource.cpp  							vtkALBAErythrocyteSource.h  
  vtkALBAImageFillHolesRemoveIslands.cxx  				vtkALBAImageFillHolesRemoveIslands.h  
  vtkALBABinaryImageFloodFill.cxx  						vtkALBABinaryImageFloodFill.h
  
  ##vtkbioeng####################################
  vtkALBACollisionDetectionFilter.cxx  					vtkALBACollisionDetectionFilter.h  
  vtkALBARayCastCleaner.cpp  							vtkALBARayCastCleaner.h  
  vtkALBAVolumeToClosedSmoothSurface.cxx  				vtkALBAVolumeToClosedSmoothSurface.h
  
  ###############################################
  vtkALBATubeFilter.cxx 									vtkALBATubeFilter.h
  vtkALBALandmarkCloudOutlineCornerFilter.cxx 			vtkALBALandmarkCloudOutlineCornerFilter.h
  vtkALBACircleSource.cxx								vtkALBACircleSource.h
  vtkALBATIFFReader.cxx									vtkALBATIFFReader.h

  vtkALBAStructuredPointsAlgorithm.cxx vtkALBAStructuredPointsAlgorithm.h
)


IF (ALBA_USE_ITK)
SET(vtkALBA_SRCS  ${vtkALBA_SRCS}
  albaClassicICPRegistration.cxx  						albaClassicICPRegistration.h
  albaICPUtility.h
)
ENDIF (ALBA_USE_ITK)


#####PUT HERE FILES THAT DOES NOT NEEDS AN INSTANTIATOR
SET(vtkALBA_FULL_SRCS
	${vtkALBA_SRCS}
  vtkALBAAbstractPointsSource.cpp 						vtkALBAAbstractPointsSource.h
  vtkALBAPastValuesList.cpp  							vtkALBAPastValuesList.h
  vtkALBAStentModelSource.cpp  							vtkALBAStentModelSource.h
  vtkALBADeformableSimplexMeshFilter.h
  vtkALBADeformableSimplexMeshFilter_Impl.h
  albaGPU3DTextureProviderHelper.cpp 						albaGPU3DTextureProviderHelper.h
  vtkALBAIdType64.h
  )


# List the kits from VTK that are needed by this project.
SET(vtkALBA_LIBS ${VTK_LIBRARIES})


IF(WIN32)
SET(vtkALBA_LIBS
  ${vtkALBA_LIBS}
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
IF(ALBA_BUILD_ALBADLL)
  IF(WIN32)
	SET(vtkALBA_LIBS ${vtkALBA_LIBS} GPUAPI)
  ENDIF(WIN32)
  SET(BUILD_SHARED_LIBS 1)
  SET(VTKLOCAL_BUILD_SHARED_LIBS_CONFIG 1)
  ADD_DEFINITIONS(-DALBA_EXPORTS)
ELSE(ALBA_BUILD_ALBADLL)
  SET(BUILD_SHARED_LIBS 0)
  SET(VTKLOCAL_BUILD_SHARED_LIBS_CONFIG 0)
ENDIF(ALBA_BUILD_ALBADLL)

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

 
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${vtkALBA_FULL_SRCS} ${vtkALBAInstantiator_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${vtkALBA_LIBS})
  # this is necessaty to force MFL compilation....

  #2 IF (MFL_BUILD_TARGETS)
  #2   ADD_DEPENDENCIES(${PROJECT_NAME} ${MFL_BUILD_TARGETS})
  #2 ENDIF (MFL_BUILD_TARGETS)

  # Create Tcl wrappers if VTK is wrapped in Tcl.
  IF(VTK_WRAP_TCL)
    VTK_WRAP_TCL2(${PROJECT_NAME}TCL vtkALBATCL_SRCS ${vtkALBA_SRCS})
    ADD_LIBRARY(${PROJECT_NAME}TCL ${vtkALBATCL_SRCS})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}TCL vtkCommonTCL ${PROJECT_NAME})

    # Create the list of kits that are required.
    SET(VTKLOCAL_KITS "")
    FOREACH(c ${vtkALBA_LIBS})
      SET(VTKLOCAL_KITS "${VTKLOCAL_KITS} ${c}")
    ENDFOREACH(c)
    FOREACH(c ${vtkALBA_SRCS})
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
    VTK_WRAP_PYTHON2(${PROJECT_NAME}Python vtkALBAPYTHON_SRCS ${vtkALBA_SRCS})
    ADD_LIBRARY(${PROJECT_NAME}Python MODULE ${vtkALBAPYTHON_SRCS})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}Python vtkCommonPython ${PROJECT_NAME})
  ENDIF(VTK_WRAP_PYTHON)

  # Create Java wrappers if VTK is wrapped in Java.
  IF(VTK_WRAP_JAVA)
    SET(VTK_WRAP_HINTS "${PROJECT_SOURCE_DIR}/hints")
    SET(VTK_JAVA_HOME ${PROJECT_BINARY_DIR}/java/${PROJECT_NAME})
    MAKE_DIRECTORY(${VTK_JAVA_HOME})
    VTK_WRAP_JAVA2(${PROJECT_NAME}Java vtkALBAJAVA_SRCS ${vtkALBA_SRCS})
    ADD_LIBRARY(${PROJECT_NAME}Java ${vtkALBAJAVA_SRCS})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}Java vtkCommonJava ${PROJECT_NAME})
  ENDIF(VTK_WRAP_JAVA)
  
  # build testing if necessary
  #OPTION(BUILD_TESTING "Build the testing tree." OFF)
  #IF (BUILD_TESTING)
  #  IF (NOT MAKECOMMAND)
  #    INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
  #    ENABLE_TESTING()
  #  ENDIF (NOT MAKECOMMAND)
    
  #  OPTION(BUILD_vtkALBA_TESTING "Enable building of testing for vtkALBA classes" ON)
  #  IF (BUILD_vtkALBA_TESTING)
  #    SUBDIRS(Testing)
  #  ENDIF (BUILD_vtkALBA_TESTING)
  #ENDIF (BUILD_TESTING)
ENDIF(VTK_FOUND)

ADD_DEPENDENCIES(${PROJECT_NAME} albaService)

IF (ALBA_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (ALBA_USE_WX)
