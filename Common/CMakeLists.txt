#
# Copyright and description to be placed here
#

PROJECT(mafCommon)

# Set your list of sources here.
SET(PROJECT_SRCS

  mafItkRawMotionImporterUtility.cxx  					mafItkRawMotionImporterUtility.h  
  mafPolylineGraph.cpp  												mafPolylineGraph.h  
  mafCurvilinearAbscissaOnSkeletonHelper.cpp  	mafCurvilinearAbscissaOnSkeletonHelper.h  
  mafRelationalDatabaseAbstractConnector.cpp  	mafRelationalDatabaseAbstractConnector.h  
  mafResultQueryAbstractHandler.cpp							mafResultQueryAbstractHandler.h  
  mafQueryObject.h  
  mafHTMLTemplateParser.cpp  										mafHTMLTemplateParser.h  
  mafHTMLTemplateParserBlock.cpp  							mafHTMLTemplateParserBlock.h  
  mafWizardBlock.cpp  													mafWizardBlock.h  
  mafWizard.cpp  																mafWizard.h  
  mafWizardManager.cpp  												mafWizardManager.h  
  mafWizardBlockOperation.cpp  									mafWizardBlockOperation.h  
  mafWizardBlockSelection.cpp  									mafWizardBlockSelection.h  
  mafWizardBlockTypeCheck.cpp  									mafWizardBlockTypeCheck.h
  mafWizardBlockFileExistCheck.cpp  						mafWizardBlockFileExistCheck.h  
  mafWizardBlockVMESelection.cpp  							mafWizardBlockVMESelection.h
  mafWizardBlockVMEFindOrSelection.cpp  				mafWizardBlockVMEFindOrSelection.h  
  mafWizardBlockInformation.cpp  								mafWizardBlockInformation.h
  mafWizardBlockYesNoSelection.cpp  						mafWizardBlockYesNoSelection.h  
  mafWizardWaitOp.cpp  													mafWizardWaitOp.h  
  mafWizardSettings.cpp  												mafWizardSettings.h  
  mafHelpManager.cpp  													mafHelpManager.h  
	mafSnapshotManager.cpp  											mafSnapshotManager.h
	mafLicenceManager.cpp 												mafLicenceManager.h  
)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafVME" "mafInteraction")

SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMAF)

IF(MAF_BUILD_MAFDLL)
  SET(BUILD_SHARED_LIBS 1)
  ADD_DEFINITIONS(-DMAF_EXPORTS)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} mafDLL mafGui)
ELSE(MAF_BUILD_MAFDLL)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ENDIF (MAF_BUILD_MAFDLL)

ADD_DEPENDENCIES(${PROJECT_NAME} vtkMAF)
ADD_DEPENDENCIES(${PROJECT_NAME} mafService)