PROJECT (MedicalVME_TEST)

LINK_LIBRARIES(medVME)
LINK_LIBRARIES(medCommon)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
IF (MAF_USE_ITK)
  ADD_EXECUTABLE(mafVMERawMotionDataTest mafVMERawMotionDataTest.h mafVMERawMotionDataTest.cpp)
  ADD_TEST(mafVMERawMotionDataTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMERawMotionDataTest)
ENDIF (MAF_USE_ITK)

ADD_EXECUTABLE(mafVMEAdvancedProberTest mafVMEAdvancedProberTest.h mafVMEAdvancedProberTest.cpp)
ADD_TEST(mafVMEAdvancedProberTest  ${EXECUTABLE_OUTPUT_PATH}/mafVMEAdvancedProberTest)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)