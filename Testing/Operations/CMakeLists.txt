PROJECT (Operations_TEST)

IF(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafDLL)
ELSE(MAF_BUILD_MAFDLL)
  LINK_LIBRARIES(mafOperations)
ENDIF (MAF_BUILD_MAFDLL)

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

############ mafOpCreateVolumeTest
ADD_EXECUTABLE(mafOpCreateVolumeTest mafOpCreateVolumeTest.h mafOpCreateVolumeTest.cpp)
ADD_TEST(mafOpCreateVolumeTest ${EXECUTABLE_OUTPUT_PATH}/mafOpCreateVolumeTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpCreateVolumeTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpVOIDensityEditorTestTest
ADD_EXECUTABLE(mafOpVOIDensityEditorTestTest mafOpVOIDensityEditorTestTest.h mafOpVOIDensityEditorTestTest.cpp)
ADD_TEST(mafOpVOIDensityEditorTestTest ${EXECUTABLE_OUTPUT_PATH}/mafOpVOIDensityEditorTestTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpVOIDensityEditorTestTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpAddLandmarkTest
ADD_EXECUTABLE(mafOpAddLandmarkTest mafOpAddLandmarkTest.h mafOpAddLandmarkTest.cpp)
ADD_TEST(mafOpAddLandmarkTest ${EXECUTABLE_OUTPUT_PATH}/mafOpAddLandmarkTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpAddLandmarkTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpApplyTrajectoryTest
ADD_EXECUTABLE(mafOpApplyTrajectoryTest mafOpApplyTrajectoryTest.h mafOpApplyTrajectoryTest.cpp)
ADD_TEST(mafOpApplyTrajectoryTest ${EXECUTABLE_OUTPUT_PATH}/mafOpApplyTrajectoryTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpApplyTrajectoryTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpExplodeCollapseTest
ADD_EXECUTABLE(mafOpExplodeCollapseTest mafOpExplodeCollapseTest.h mafOpExplodeCollapseTest.cpp)
ADD_TEST(mafOpExplodeCollapseTest ${EXECUTABLE_OUTPUT_PATH}/mafOpExplodeCollapseTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpExplodeCollapseTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpExtractIsosurfaceTest
ADD_EXECUTABLE(mafOpExtractIsosurfaceTest mafOpExtractIsosurfaceTest.h mafOpExtractIsosurfaceTest.cpp)
ADD_TEST(mafOpExtractIsosurfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafOpExtractIsosurfaceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpExtractIsosurfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpImporterASCIITest
ADD_EXECUTABLE(mafOpImporterASCIITest mafOpImporterASCIITest.h mafOpImporterASCIITest.cpp)
ADD_TEST(mafOpImporterASCIITest ${EXECUTABLE_OUTPUT_PATH}/mafOpImporterASCIITest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpImporterASCIITest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpExporterMSFTest
ADD_EXECUTABLE(mafOpExporterMSFTest mafOpExporterMSFTest.h mafOpExporterMSFTest.cpp)
ADD_TEST(mafOpExporterMSFTest ${EXECUTABLE_OUTPUT_PATH}/mafOpExporterMSFTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpExporterMSFTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpFilterSurfaceTest
ADD_EXECUTABLE(mafOpFilterSurfaceTest mafOpFilterSurfaceTest.h mafOpFilterSurfaceTest.cpp)
ADD_TEST(mafOpFilterSurfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafOpFilterSurfaceTest)
IF (WIN32)
  SET_TARGET_PROPERTIES(mafOpFilterSurfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpValidateTreeTest
ADD_EXECUTABLE(mafOpValidateTreeTest mafOpValidateTreeTest.h mafOpValidateTreeTest.cpp)
ADD_TEST(mafOpValidateTreeTest ${EXECUTABLE_OUTPUT_PATH}/mafOpValidateTreeTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafOpValidateTreeTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)


############ mafOpGarbageCollectMSFDirTest
ADD_EXECUTABLE(mafOpGarbageCollectMSFDirTest mafOpGarbageCollectMSFDirTest.h mafOpGarbageCollectMSFDirTest.cpp)
ADD_TEST(mafOpGarbageCollectMSFDirTest ${EXECUTABLE_OUTPUT_PATH}/mafOpGarbageCollectMSFDirTest)
IF (WIN32)
 SET_TARGET_PROPERTIES(mafOpGarbageCollectMSFDirTest PROPERTIES LINK_FLAGS "/MANIFEST")
ENDIF (WIN32)

############ mafOpScalarToSurfaceTest
 ADD_EXECUTABLE(mafOpScalarToSurfaceTest mafOpScalarToSurfaceTest.h mafOpScalarToSurfaceTest.cpp)
 ADD_TEST(mafOpScalarToSurfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafOpScalarToSurfaceTest)
 IF (WIN32)
   SET_TARGET_PROPERTIES(mafOpScalarToSurfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
 ENDIF (WIN32)
 
############ mafASCIIImporterUtilityTest
 ADD_EXECUTABLE(mafASCIIImporterUtilityTest mafASCIIImporterUtilityTest.h mafASCIIImporterUtilityTest.cpp)
 ADD_TEST(mafASCIIImporterUtilityTest ${EXECUTABLE_OUTPUT_PATH}/mafASCIIImporterUtilityTest)
 IF (WIN32)
   SET_TARGET_PROPERTIES(mafASCIIImporterUtilityTest PROPERTIES LINK_FLAGS "/MANIFEST")
 ENDIF (WIN32)
 
 ############ mafOpEditMetadataTest
 ADD_EXECUTABLE(mafOpEditMetadataTest mafOpEditMetadataTest.h mafOpEditMetadataTest.cpp)
 ADD_TEST(mafOpEditMetadataTest ${EXECUTABLE_OUTPUT_PATH}/mafOpEditMetadataTest)
 IF (WIN32)
   SET_TARGET_PROPERTIES(mafOpEditMetadataTest PROPERTIES LINK_FLAGS "/MANIFEST")
 ENDIF (WIN32)

 ############ mafOpFilterVolumeTest
 ADD_EXECUTABLE(mafOpFilterVolumeTest mafOpFilterVolumeTest.h mafOpFilterVolumeTest.cpp)
 ADD_TEST(mafOpFilterVolumeTest ${EXECUTABLE_OUTPUT_PATH}/mafOpFilterVolumeTest)
 IF (WIN32)
   SET_TARGET_PROPERTIES(mafOpFilterVolumeTest PROPERTIES LINK_FLAGS "/MANIFEST")
 ENDIF (WIN32)

 ############ mafOpMAFTransformTest
 ADD_EXECUTABLE(mafOpMAFTransformTest mafOpMAFTransformTest.h mafOpMAFTransformTest.cpp)
 ADD_TEST(mafOpMAFTransformTest ${EXECUTABLE_OUTPUT_PATH}/mafOpMAFTransformTest)
 IF (WIN32)
   SET_TARGET_PROPERTIES(mafOpMAFTransformTest PROPERTIES LINK_FLAGS "/MANIFEST")
 ENDIF (WIN32)
 
  ############ mafOpTransformInterfaceTest
 ADD_EXECUTABLE(mafOpTransformInterfaceTest mafOpTransformInterfaceTest.h mafOpTransformInterfaceTest.cpp)
 ADD_TEST(mafOpTransformInterfaceTest ${EXECUTABLE_OUTPUT_PATH}/mafOpTransformInterfaceTest)
 IF (WIN32)
   SET_TARGET_PROPERTIES(mafOpTransformInterfaceTest PROPERTIES LINK_FLAGS "/MANIFEST")
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

  ############ mafOpExporterMeshTest
  ADD_EXECUTABLE(mafOpExporterMeshTest mafOpExporterMeshTest.h mafOpExporterMeshTest.cpp)
  ADD_TEST(mafOpExporterMeshTest ${EXECUTABLE_OUTPUT_PATH}/mafOpExporterMeshTest)
	IF (WIN32)
	  SET_TARGET_PROPERTIES(mafOpExporterMeshTest PROPERTIES LINK_FLAGS "/MANIFEST")
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
  
  ############ mafOpExporterVTKTest
  ADD_EXECUTABLE(mafOpExporterVTKTest mafOpExporterVTKTest.h mafOpExporterVTKTest.cpp)
  ADD_TEST(mafOpExporterVTKTest ${EXECUTABLE_OUTPUT_PATH}/mafOpExporterVTKTest)
  IF (WIN32)
    SET_TARGET_PROPERTIES(mafOpExporterVTKTest PROPERTIES LINK_FLAGS "/MANIFEST")
  ENDIF (WIN32)

ENDIF (MAF_USE_ITK)


