PROJECT (MedicalViews_TEST)

LINK_LIBRARIES(medViews)
LINK_LIBRARIES(medCommon)
LINK_LIBRARIES(medVME)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

ADD_EXECUTABLE(mafViewArbitrarySliceTest mafViewArbitrarySliceTest.h mafViewArbitrarySliceTest.cpp)
ADD_TEST(mafViewArbitrarySliceTest ${EXECUTABLE_OUTPUT_PATH}/mafViewArbitrarySliceTest)

ADD_EXECUTABLE(mafViewGlobalSliceCompoundTest mafViewGlobalSliceCompoundTest.h mafViewGlobalSliceCompoundTest.cpp)
ADD_TEST(mafViewGlobalSliceCompoundTest ${EXECUTABLE_OUTPUT_PATH}/mafViewGlobalSliceCompoundTest)

ADD_EXECUTABLE(mafViewImageTest mafViewImageTest.h mafViewImageTest.cpp)
ADD_TEST(mafViewImageTest ${EXECUTABLE_OUTPUT_PATH}/mafViewImageTest)

ADD_EXECUTABLE(mafViewRXTest mafViewRXTest.h mafViewRXTest.cpp)
ADD_TEST(mafViewRXTest ${EXECUTABLE_OUTPUT_PATH}/mafViewRXTest)
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