PROJECT (MedicalViews_TEST)

LINK_LIBRARIES(medViews)
LINK_LIBRARIES(medCommon)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------
ADD_EXECUTABLE(mafViewRXCTTest mafViewRXCTTest.h mafViewRXCTTest.cpp)
ADD_TEST(mafViewRXCTTest ${EXECUTABLE_OUTPUT_PATH}/mafViewRXCTTest)
ADD_EXECUTABLE(mafViewOrthoSliceTest mafViewOrthoSliceTest.h mafViewOrthoSliceTest.cpp)
ADD_TEST(mafViewOrthoSliceTest ${EXECUTABLE_OUTPUT_PATH}/mafViewOrthoSliceTest)


# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)