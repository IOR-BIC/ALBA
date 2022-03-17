#
# Copyright and description to be placed here
#

PROJECT(albaCommon)

# Set your list of sources here.
SET(PROJECT_SRCS

  albaItkRawMotionImporterUtility.cxx  					albaItkRawMotionImporterUtility.h  
  albaPolylineGraph.cpp  												albaPolylineGraph.h  
  albaCurvilinearAbscissaOnSkeletonHelper.cpp  	albaCurvilinearAbscissaOnSkeletonHelper.h  
  albaRelationalDatabaseAbstractConnector.cpp  	albaRelationalDatabaseAbstractConnector.h  
  albaResultQueryAbstractHandler.cpp							albaResultQueryAbstractHandler.h  
  albaQueryObject.h  
  albaHTMLTemplateParser.cpp  										albaHTMLTemplateParser.h  
  albaHTMLTemplateParserBlock.cpp  							albaHTMLTemplateParserBlock.h  
  albaWizardBlock.cpp  													albaWizardBlock.h  
  albaWizard.cpp  																albaWizard.h  
  albaWizardManager.cpp  												albaWizardManager.h  
  albaWizardBlockOperation.cpp  									albaWizardBlockOperation.h  
  albaWizardBlockSelection.cpp  									albaWizardBlockSelection.h  
  albaWizardBlockTypeCheck.cpp  									albaWizardBlockTypeCheck.h
  albaWizardBlockFileExistCheck.cpp  						albaWizardBlockFileExistCheck.h  
  albaWizardBlockVMESelection.cpp  							albaWizardBlockVMESelection.h
  albaWizardBlockVMEFindOrSelection.cpp  				albaWizardBlockVMEFindOrSelection.h  
  albaWizardBlockInformation.cpp  								albaWizardBlockInformation.h
  albaWizardBlockYesNoSelection.cpp  						albaWizardBlockYesNoSelection.h  
  albaWizardWaitOp.cpp  													albaWizardWaitOp.h  
  albaWizardSettings.cpp  												albaWizardSettings.h  
  albaHelpManager.cpp  													albaHelpManager.h  
	albaSnapshotManager.cpp  											albaSnapshotManager.h
	albaLicenceManager.cpp 												albaLicenceManager.h  
	albaTextFileReaderHelper.cpp										albaTextFileReaderHelper.h
	albaProsthesesDBManager.cpp albaProsthesesDBManager.h
)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "albaVME" "albaInteraction")

SET(PROJECT_LIBS ${PROJECT_LIBS} vtkALBA)

IF(ALBA_BUILD_ALBADLL)
  SET(BUILD_SHARED_LIBS 1)
  ADD_DEFINITIONS(-DALBA_EXPORTS)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} albaDLL albaGui)
ELSE(ALBA_BUILD_ALBADLL)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ENDIF (ALBA_BUILD_ALBADLL)

ADD_DEPENDENCIES(${PROJECT_NAME} vtkALBA)
ADD_DEPENDENCIES(${PROJECT_NAME} albaService)