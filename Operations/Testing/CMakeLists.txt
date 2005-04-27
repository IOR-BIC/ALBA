PROJECT (Operations_TEST)

LINK_LIBRARIES(mafGui mafCore mafOperations)

# build tests
IF(BUILD_TESTING)
  OPTION(BUILD_VTKIMPORTER_TESTING "Enable building mmoVTKImporter testing classes" ON)
  IF (BUILD_VTKIMPORTER_TESTING)
    SUBDIRS(VTKImporter)
  ENDIF (BUILD_VTKIMPORTER_TESTING)
  
  OPTION(BUILD_CREATEMETER_TESTING "Enable building mmoCreateMeter testing classes" ON)
  IF (BUILD_CREATEMETER_TESTING)
    SUBDIRS(CreateMeter)
  ENDIF (BUILD_CREATEMETER_TESTING)
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