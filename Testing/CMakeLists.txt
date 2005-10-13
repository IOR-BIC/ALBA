PROJECT (Medical_TEST)

LINK_LIBRARIES(medTesting)

# build tests
IF(BUILD_TESTING)
  OPTION(BUILD_MEDICAL_TESTING "Enable building medical library testing classes" ON)
  IF (BUILD_MEDICAL_TESTING)
    
  ENDIF (BUILD_MEDICAL_TESTING)
ENDIF(BUILD_TESTING)

# wxWidgets specific classes
IF (MAF_USE_WX)
ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
IF (MAF_USE_WX AND MAF_USE_VTK)
ENDIF (MAF_USE_WX AND MAF_USE_VTK)