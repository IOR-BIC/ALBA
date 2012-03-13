PROJECT (mafVME_TEST)

LINK_LIBRARIES(mafVME mafDataPipes vtkRendering vtkGraphics)
 

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

############ mafVMEVolumeGrayTest
ADD_EXECUTABLE(mafVMEVolumeGrayTest mafVMEVolumeGrayTest.h mafVMEVolumeGrayTest.cpp)
ADD_TEST(mafVMEVolumeGrayTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEVolumeGrayTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEVolumeGrayTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMESurfaceParametricTest
ADD_EXECUTABLE(mafVMESurfaceParametricTest mafVMESurfaceParametricTest.h mafVMESurfaceParametricTest.cpp)
ADD_TEST(mafVMESurfaceParametricTest ${EXECUTABLE_OUTPUT_PATH}/mafVMESurfaceParametricTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMESurfaceParametricTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMESurfaceTest
ADD_EXECUTABLE(mafVMESurfaceTest mafVMESurfaceTest.h mafVMESurfaceTest.cpp)
ADD_TEST(mafVMESurfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafVMESurfaceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMESurfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEGroupTest
ADD_EXECUTABLE(mafVMEGroupTest mafVMEGroupTest.h mafVMEGroupTest.cpp)
ADD_TEST(mafVMEGroupTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEGroupTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEGroupTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEGizmoTest
ADD_EXECUTABLE(mafVMEGizmoTest mafVMEGizmoTest.h mafVMEGizmoTest.cpp)
ADD_TEST(mafVMEGizmoTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEGizmoTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEGizmoTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

IF (MAF_USE_ITK)
	############ mafVMEScalarMatrixTest
	ADD_EXECUTABLE(mafVMEScalarMatrixTest mafVMEScalarMatrixTest.h mafVMEScalarMatrixTest.cpp)
	ADD_TEST(mafVMEScalarMatrixTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEScalarMatrixTest)
	IF (WIN32)
	  SET_TARGET_PROPERTIES(mafVMEScalarMatrixTest PROPERTIES LINK_FLAGS "/MANIFEST")
	ENDIF (WIN32)
ENDIF (MAF_USE_ITK)

############ mafVMEScalarTest
ADD_EXECUTABLE(mafVMEScalarTest mafVMEScalarTest.h mafVMEScalarTest.cpp)
ADD_TEST(mafVMEScalarTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEScalarTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEScalarTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEPolylineTest
ADD_EXECUTABLE(mafVMEPolylineTest mafVMEPolylineTest.h mafVMEPolylineTest.cpp)
ADD_TEST(mafVMEPolylineTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEPolylineTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEPolylineTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEPolylineSplineTest
ADD_EXECUTABLE(mafVMEPolylineSplineTest mafVMEPolylineSplineTest.h mafVMEPolylineSplineTest.cpp)
ADD_TEST(mafVMEPolylineSplineTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEPolylineSplineTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEPolylineSplineTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEMeshTest
ADD_EXECUTABLE(mafVMEMeshTest mafVMEMeshTest.h mafVMEMeshTest.cpp)
ADD_TEST(mafVMEMeshTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEMeshTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEMeshTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeMeshTest
ADD_EXECUTABLE(mafPipeMeshTest mafPipeMeshTest.h mafPipeMeshTest.cpp)
ADD_TEST(mafPipeMeshTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeMeshTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeMeshTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeMeshSliceTest
ADD_EXECUTABLE(mafPipeMeshSliceTest mafPipeMeshSliceTest.h mafPipeMeshSliceTest.cpp)
ADD_TEST(mafPipeMeshSliceTest  ${EXECUTABLE_OUTPUT_PATH}/mafPipeMeshSliceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeMeshSliceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMELandmarkTest
ADD_EXECUTABLE(mafVMELandmarkTest mafVMELandmarkTest.h mafVMELandmarkTest.cpp)
ADD_TEST(mafVMELandmarkTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMELandmarkTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMELandmarkTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMELandmarkCloudTest
ADD_EXECUTABLE(mafVMELandmarkCloudTest mafVMELandmarkCloudTest.h mafVMELandmarkCloudTest.cpp)
ADD_TEST(mafVMELandmarkCloudTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMELandmarkCloudTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMELandmarkCloudTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


############ mafMatrixVectorTest
ADD_EXECUTABLE(mafMatrixVectorTest mafMatrixVectorTest.h mafMatrixVectorTest.cpp)
ADD_TEST(mafMatrixVectorTest  ${EXECUTABLE_OUTPUT_PATH}/mafMatrixVectorTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafMatrixVectorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


ADD_EXECUTABLE(mafVMEFactoryTest mafVMEFactoryTest.h mafVMEFactoryTest.cpp)
ADD_TEST(mafVMEFactoryTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEFactoryTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEFactoryTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafDataVectorTest
ADD_EXECUTABLE(mafDataVectorTest mafDataVectorTest.h mafDataVectorTest.cpp)
ADD_TEST(mafDataVectorTest ${EXECUTABLE_OUTPUT_PATH}/mafDataVectorTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafDataVectorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEFactoryTest
ADD_EXECUTABLE(mafVMEFactoryTest mafVMEFactoryTest.h mafVMEFactoryTest.cpp)
ADD_TEST(mafVMEFactoryTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEFactoryTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEFactoryTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafVMEItemVTKTest
ADD_EXECUTABLE(mafVMEItemVTKTest mafVMEItemVTKTest.h mafVMEItemVTKTest.cpp)
ADD_TEST(mafVMEItemVTKTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEItemVTKTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafVMEItemVTKTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeSurfaceTest
ADD_EXECUTABLE(mafPipeSurfaceTest mafPipeSurfaceTest.h mafPipeSurfaceTest.cpp)
ADD_TEST(mafPipeSurfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeSurfaceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeSurfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeGizmoTest
ADD_EXECUTABLE(mafPipeGizmoTest mafPipeGizmoTest.h mafPipeGizmoTest.cpp)
ADD_TEST(mafPipeGizmoTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeGizmoTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeGizmoTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafPipeSurfaceTexturedTest
ADD_EXECUTABLE(mafPipeSurfaceTexturedTest mafPipeSurfaceTexturedTest.h mafPipeSurfaceTexturedTest.cpp)
ADD_TEST(mafPipeSurfaceTexturedTest ${EXECUTABLE_OUTPUT_PATH}/mafPipeSurfaceTexturedTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafPipeSurfaceTexturedTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

#-----------------------------------------------------------------------------
# commented tests: to be renamed to match test names standard
# and to be refactored to use cppunit.  Also to be checked if they use any external data.
#-----------------------------------------------------------------------------
# 
# 
# ADD_EXECUTABLE(testDataVector testDataVector.cpp)
# ADD_TEST(testDataVector ${EXECUTABLE_OUTPUT_PATH}/testDataVector)
# 
# ADD_EXECUTABLE(testMatrixVector testMatrixVector.cpp)
# ADD_TEST(testMatrixVector ${EXECUTABLE_OUTPUT_PATH}/testMatrixVector)
# 
# ADD_EXECUTABLE(testVMEGenericPose testVMEGenericPose.cpp)
# ADD_TEST(testVMEGenericPose ${EXECUTABLE_OUTPUT_PATH}/testVMEGenericPose)
# 
# ADD_EXECUTABLE(testVMEGenericBounds testVMEGenericBounds.cpp)
# ADD_TEST(testVMEGenericBounds ${EXECUTABLE_OUTPUT_PATH}/testVMEGenericBounds)
# 
# ADD_EXECUTABLE(testVMESlicer testVMESlicer.cpp)
# ADD_TEST(testVMESlicer ${EXECUTABLE_OUTPUT_PATH}/testVMESlicer)
# 
# ADD_EXECUTABLE(testVMELandmarkCloud testVMELandmarkCloud.cpp)
# ADD_TEST(testVMELandmarkCloud ${EXECUTABLE_OUTPUT_PATH}/testVMELandmarkCloud)
# 
# ADD_EXECUTABLE(testCustomDataPipe testCustomDataPipe.cpp)
# ADD_TEST(testCustomDataPipe ${EXECUTABLE_OUTPUT_PATH}/testCustomDataPipe)
# 
# wxWidgets specific classes
# IF (MAF_USE_WX)
# ENDIF (MAF_USE_WX)
# 
# # VTK specific classes
IF (MAF_USE_VTK)
	############ testMSF
  # ADD_EXECUTABLE(testMSF testMSF.cpp)
  # ADD_TEST(testMSF ${EXECUTABLE_OUTPUT_PATH}/testMSF)

	############ testVMEGeneric
	ADD_EXECUTABLE(testVMEGeneric testVMEGeneric.cpp)
	ADD_TEST(testVMEGeneric ${EXECUTABLE_OUTPUT_PATH}/testVMEGeneric)
	IF (WIN32)
	  SET_TARGET_PROPERTIES(testVMEGeneric PROPERTIES LINK_FLAGS "/MANIFEST")
	ENDIF (WIN32)
ENDIF (MAF_USE_VTK)  
# 
# classes using VTK+wxWidgets
# IF (MAF_USE_WX AND MAF_USE_VTK)
# ENDIF (MAF_USE_WX AND MAF_USE_VTK)
