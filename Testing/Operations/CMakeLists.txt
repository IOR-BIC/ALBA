PROJECT (Operations_TEST)

LINK_LIBRARIES(mafOperations)

####### Tests
############ mafOpImporterMSF1xTest
#ADD_EXECUTABLE(mafOpImporterMSF1xTest mafOpImporterMSF1xTest.h mafOpImporterMSF1xTest.cpp)
#ADD_TEST(mafOpImporterMSF1xTest ${EXECUTABLE_OUTPUT_PATH}/mafOpImporterMSF1xTest)
#IF (WIN32)
#  SET_TARGET_PROPERTIES(mafOpImporterMSF1xTest PROPERTIES LINK_FLAGS "/MANIFEST")
#ENDIF (WIN32)

############ mafOpImporterSTLTest
ADD_EXECUTABLE(mafOpImporterSTLTest mafOpImporterSTLTest.h mafOpImporterSTLTest.cpp)
ADD_TEST(mafOpImporterSTLTest ${EXECUTABLE_OUTPUT_PATH}/mafOpImporterSTLTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpImporterSTLTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpExporterSTLTest
ADD_EXECUTABLE(mafOpExporterSTLTest mafOpExporterSTLTest.h mafOpExporterSTLTest.cpp)
ADD_TEST(mafOpExporterSTLTest ${EXECUTABLE_OUTPUT_PATH}/mafOpExporterSTLTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpExporterSTLTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpCropTest
ADD_EXECUTABLE(mafOpCropTest mafOpCropTest.h mafOpCropTest.cpp)
ADD_TEST(mafOpCropTest ${EXECUTABLE_OUTPUT_PATH}/mafOpCropTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpCropTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpVOIDensityTest
ADD_EXECUTABLE(mafOpVOIDensityTest mafOpVOIDensityTest.h mafOpVOIDensityTest.cpp)
ADD_TEST(mafOpVOIDensityTest ${EXECUTABLE_OUTPUT_PATH}/mafOpVOIDensityTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpVOIDensityTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpImporterImageTest
ADD_EXECUTABLE(mafOpImporterImageTest mafOpImporterImageTest.h mafOpImporterImageTest.cpp)
ADD_TEST(mafOpImporterImageTest ${EXECUTABLE_OUTPUT_PATH}/mafOpImporterImageTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpImporterImageTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpImporterRAWVolumeTest
ADD_EXECUTABLE(mafOpImporterRAWVolumeTest mafOpImporterRAWVolumeTest.h mafOpImporterRAWVolumeTest.cpp)
ADD_TEST(mafOpImporterRAWVolumeTest ${EXECUTABLE_OUTPUT_PATH}/mafOpImporterRAWVolumeTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpImporterRAWVolumeTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpImporterVRMLTest
ADD_EXECUTABLE(mafOpImporterVRMLTest mafOpImporterVRMLTest.h mafOpImporterVRMLTest.cpp)
ADD_TEST(mafOpImporterVRMLTest ${EXECUTABLE_OUTPUT_PATH}/mafOpImporterVRMLTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpImporterVRMLTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpExporterRAWTest
ADD_EXECUTABLE(mafOpExporterRAWTest mafOpExporterRAWTest.h mafOpExporterRAWTest.cpp)
ADD_TEST(mafOpExporterRAWTest ${EXECUTABLE_OUTPUT_PATH}/mafOpExporterRAWTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpExporterRAWTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpCreateGroupTest
ADD_EXECUTABLE(mafOpCreateGroupTest mafOpCreateGroupTest.h mafOpCreateGroupTest.cpp)
ADD_TEST(mafOpCreateGroupTest ${EXECUTABLE_OUTPUT_PATH}/mafOpCreateGroupTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpCreateGroupTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpReparentToTest
ADD_EXECUTABLE(mafOpReparentToTest mafOpReparentToTest.h mafOpReparentToTest.cpp)
ADD_TEST(mafOpReparentToTest ${EXECUTABLE_OUTPUT_PATH}/mafOpReparentToTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpReparentToTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpVolumeResampleTest
ADD_EXECUTABLE(mafOpVolumeResampleTest mafOpVolumeResampleTest.h mafOpVolumeResampleTest.cpp)
ADD_TEST(mafOpVolumeResampleTest ${EXECUTABLE_OUTPUT_PATH}/mafOpVolumeResampleTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpVolumeResampleTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpDecimateSurfaceTest
ADD_EXECUTABLE(mafOpDecimateSurfaceTest mafOpDecimateSurfaceTest.h mafOpDecimateSurfaceTest.cpp)
ADD_TEST(mafOpDecimateSurfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafOpDecimateSurfaceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpDecimateSurfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpConnectivitySurfaceTest
ADD_EXECUTABLE(mafOpConnectivitySurfaceTest mafOpConnectivitySurfaceTest.h mafOpConnectivitySurfaceTest.cpp)
ADD_TEST(mafOpConnectivitySurfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafOpConnectivitySurfaceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpConnectivitySurfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpRemoveCellsTest
ADD_EXECUTABLE(mafOpRemoveCellsTest mafOpRemoveCellsTest.h mafOpRemoveCellsTest.cpp)
ADD_TEST(mafOpRemoveCellsTest ${EXECUTABLE_OUTPUT_PATH}/mafOpRemoveCellsTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpRemoveCellsTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpBooleanSurfaceTest
ADD_EXECUTABLE(mafOpBooleanSurfaceTest mafOpBooleanSurfaceTest.h mafOpBooleanSurfaceTest.cpp)
ADD_TEST(mafOpBooleanSurfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafOpBooleanSurfaceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpBooleanSurfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

# commented since it continues to fail under ctest only... need more investigation
# Dashboard is not corrupted since I created another one from copy and this continues to fail
############ mafOpEditNormalsTest
# ADD_EXECUTABLE(mafOpEditNormalsTest mafOpEditNormalsTest.h mafOpEditNormalsTest.cpp)
# ADD_TEST(mafOpEditNormalsTest ${EXECUTABLE_OUTPUT_PATH}/mafOpEditNormalsTest)
# IF (WIN32)
#   SET_TARGET_PROPERTIES(mafOpEditNormalsTest PROPERTIES LINK_FLAGS "/MANIFEST")
# ENDIF (WIN32)

############ mafOpExporterBMPTest
ADD_EXECUTABLE(mafOpExporterBMPTest mafOpExporterBMPTest.h mafOpExporterBMPTest.cpp)
ADD_TEST(mafOpExporterBMPTest ${EXECUTABLE_OUTPUT_PATH}/mafOpExporterBMPTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpExporterBMPTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpImporterMSFTest
ADD_EXECUTABLE(mafOpImporterMSFTest mafOpImporterMSFTest.h mafOpImporterMSFTest.cpp)
ADD_TEST(mafOpImporterMSFTest ${EXECUTABLE_OUTPUT_PATH}/mafOpImporterMSFTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpImporterMSFTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpImporterExternalFileTest
ADD_EXECUTABLE(mafOpImporterExternalFileTest mafOpImporterExternalFileTest.h mafOpImporterExternalFileTest.cpp)
ADD_TEST(mafOpImporterExternalFileTest ${EXECUTABLE_OUTPUT_PATH}/mafOpImporterExternalFileTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpImporterExternalFileTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpOpenExternalFileTest
ADD_EXECUTABLE(mafOpOpenExternalFileTest mafOpOpenExternalFileTest.h mafOpOpenExternalFileTest.cpp)
ADD_TEST(mafOpOpenExternalFileTest ${EXECUTABLE_OUTPUT_PATH}/mafOpOpenExternalFileTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpOpenExternalFileTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpDecomposeTimeVarVMETest
ADD_EXECUTABLE(mafOpDecomposeTimeVarVMETest mafOpDecomposeTimeVarVMETest.h mafOpDecomposeTimeVarVMETest.cpp)
ADD_TEST(mafOpDecomposeTimeVarVMETest ${EXECUTABLE_OUTPUT_PATH}/mafOpDecomposeTimeVarVMETest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpDecomposeTimeVarVMETest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpLabelExtractorTest
ADD_EXECUTABLE(mafOpLabelExtractorTest mafOpLabelExtractorTest.h mafOpLabelExtractorTest.cpp)
ADD_TEST(mafOpLabelExtractorTest ${EXECUTABLE_OUTPUT_PATH}/mafOpLabelExtractorTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpLabelExtractorTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpImporterVTKTest
ADD_EXECUTABLE(mafOpImporterVTKTest mafOpImporterVTKTest.h mafOpImporterVTKTest.cpp)
ADD_TEST(mafOpImporterVTKTest ${EXECUTABLE_OUTPUT_PATH}/mafOpImporterVTKTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpImporterVTKTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


IF (MAF_USE_ITK)
  ############ mafVMEMeshAnsysTextImporterTest
  ADD_EXECUTABLE(mafVMEMeshAnsysTextImporterTest mafVMEMeshAnsysTextImporterTest.h mafVMEMeshAnsysTextImporterTest.cpp)
  ADD_TEST(mafVMEMeshAnsysTextImporterTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEMeshAnsysTextImporterTest)
	IF (WIN32)
	  SET_TARGET_PROPERTIES(mafVMEMeshAnsysTextImporterTest PROPERTIES LINK_FLAGS "/MANIFEST")
	ENDIF (WIN32)

  ############ mafVMEMeshAnsysTextExporterTest
  ADD_EXECUTABLE(mafVMEMeshAnsysTextExporterTest mafVMEMeshAnsysTextExporterTest.h mafVMEMeshAnsysTextExporterTest.cpp)
  ADD_TEST(mafVMEMeshAnsysTextExporterTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEMeshAnsysTextExporterTest)
	IF (WIN32)
	  SET_TARGET_PROPERTIES(mafVMEMeshAnsysTextExporterTest PROPERTIES LINK_FLAGS "/MANIFEST")
	ENDIF (WIN32)
  
  ############ mafOpImporterMeshTest
  ADD_EXECUTABLE(mafOpImporterMeshTest mafOpImporterMeshTest.h mafOpImporterMeshTest.cpp)
  ADD_TEST(mafOpImporterMeshTest ${EXECUTABLE_OUTPUT_PATH}/mafOpImporterMeshTest)
	IF (WIN32)
	  SET_TARGET_PROPERTIES(mafOpImporterMeshTest PROPERTIES LINK_FLAGS "/MANIFEST")
	ENDIF (WIN32)

  ############ mafVMEDataSetAttributesImporterTest
  ADD_EXECUTABLE(mafVMEDataSetAttributesImporterTest mafVMEDataSetAttributesImporterTest.h mafVMEDataSetAttributesImporterTest.cpp)
  ADD_TEST(mafVMEDataSetAttributesImporterTest ${EXECUTABLE_OUTPUT_PATH}/mafVMEDataSetAttributesImporterTest)
	IF (WIN32)
	  SET_TARGET_PROPERTIES(mafVMEDataSetAttributesImporterTest PROPERTIES LINK_FLAGS "/MANIFEST")
	ENDIF (WIN32)

  ############ mafOpImporterVMEDataSetAttributesTest
  ADD_EXECUTABLE(mafOpImporterVMEDataSetAttributesTest mafOpImporterVMEDataSetAttributesTest.h mafOpImporterVMEDataSetAttributesTest.cpp)
  ADD_TEST(mafOpImporterVMEDataSetAttributesTest ${EXECUTABLE_OUTPUT_PATH}/mafOpImporterVMEDataSetAttributesTest)
	IF (WIN32)
	  SET_TARGET_PROPERTIES(mafOpImporterVMEDataSetAttributesTest PROPERTIES LINK_FLAGS "/MANIFEST")
	ENDIF (WIN32)
  
  ############ mafOpCreateMeterTest
  ADD_EXECUTABLE(mafOpCreateMeterTest mafOpCreateMeterTest.h mafOpCreateMeterTest.cpp)
  ADD_TEST(mafOpCreateMeterTest ${EXECUTABLE_OUTPUT_PATH}/mafOpCreateMeterTest)
  IF (WIN32)
    SET_TARGET_PROPERTIES(mafOpCreateMeterTest PROPERTIES LINK_FLAGS "/MANIFEST")
  ENDIF (WIN32)
  
  ############ mafOpCreateProberTest
  ADD_EXECUTABLE(mafOpCreateProberTest mafOpCreateProberTest.h mafOpCreateProberTest.cpp)
  ADD_TEST(mafOpCreateProberTest ${EXECUTABLE_OUTPUT_PATH}/mafOpCreateProberTest)
  IF (WIN32)
    SET_TARGET_PROPERTIES(mafOpCreateProberTest PROPERTIES LINK_FLAGS "/MANIFEST")
  ENDIF (WIN32)
  
  ############ mafOpCreateRefSysTest
  ADD_EXECUTABLE(mafOpCreateRefSysTest mafOpCreateRefSysTest.h mafOpCreateRefSysTest.cpp)
  ADD_TEST(mafOpCreateRefSysTest ${EXECUTABLE_OUTPUT_PATH}/mafOpCreateRefSysTest)
  IF (WIN32)
    SET_TARGET_PROPERTIES(mafOpCreateRefSysTest PROPERTIES LINK_FLAGS "/MANIFEST")
  ENDIF (WIN32)
  
  ############ mafOpCreateSlicerTest
  ADD_EXECUTABLE(mafOpCreateSlicerTest mafOpCreateSlicerTest.h mafOpCreateSlicerTest.cpp)
  ADD_TEST(mafOpCreateSlicerTest ${EXECUTABLE_OUTPUT_PATH}/mafOpCreateSlicerTest)
  IF (WIN32)
    SET_TARGET_PROPERTIES(mafOpCreateSlicerTest PROPERTIES LINK_FLAGS "/MANIFEST")
  ENDIF (WIN32)
  
  ############ mafOpCreateSplineTest
  ADD_EXECUTABLE(mafOpCreateSplineTest mafOpCreateSplineTest.h mafOpCreateSplineTest.cpp)
  ADD_TEST(mafOpCreateSplineTest ${EXECUTABLE_OUTPUT_PATH}/mafOpCreateSplineTest)
  IF (WIN32)
    SET_TARGET_PROPERTIES(mafOpCreateSplineTest PROPERTIES LINK_FLAGS "/MANIFEST")
  ENDIF (WIN32)
  
  ############ mafOpCreateSurfaceParametricTest
  ADD_EXECUTABLE(mafOpCreateSurfaceParametricTest mafOpCreateSurfaceParametricTest.h mafOpCreateSurfaceParametricTest.cpp)
  ADD_TEST(mafOpCreateSurfaceParametricTest ${EXECUTABLE_OUTPUT_PATH}/mafOpCreateSurfaceParametricTest)
  IF (WIN32)
    SET_TARGET_PROPERTIES(mafOpCreateSurfaceParametricTest PROPERTIES LINK_FLAGS "/MANIFEST")
  ENDIF (WIN32)

ENDIF (MAF_USE_ITK)
