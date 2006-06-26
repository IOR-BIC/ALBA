PROJECT (Medical_TEST)

# Load CppUnit stuff
INCLUDE(${MED_SOURCE_DIR}/FindCppUnit.cmake)
IF (CPPUNIT_FOUND)
  #MESSAGE("Found Cppunit library")
  INCLUDE_DIRECTORIES(${CPPUNIT_INCLUDE_DIR})
  LINK_LIBRARIES(${CPPUNIT_LIBRARIES})
  
ELSE (CPPUNIT_FOUND)
  MESSAGE("Cppunit library not found! Please fill the corresponding cache entries")
ENDIF (CPPUNIT_FOUND)
  
OPTION(BUILD_VIEWS_TESTING "Enable building of testing for BASE classes" ON)
IF (BUILD_VIEWS_TESTING)
  SUBDIRS(Views)
ENDIF (BUILD_VIEWS_TESTING)


# wxWidgets specific classes
IF (MAF_USE_WX)
ENDIF (MAF_USE_WX)

# VTK specific classes
IF (MAF_USE_VTK)
ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
IF (MAF_USE_WX AND MAF_USE_VTK)
ENDIF (MAF_USE_WX AND MAF_USE_VTK)