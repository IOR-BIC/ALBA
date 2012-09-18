#
# Copyright and description to be placed here
#

PROJECT(medCommon)

# Set your list of sources here.
SET(PROJECT_SRCS

  itkRawMotionImporterUtility.cxx
  itkRawMotionImporterUtility.h
  medGUIContextualMenu.cpp
  medGUIContextualMenu.h
  mafPolylineGraph.cpp
  mafPolylineGraph.h
  mafGizmoSlice.cpp
  mafGizmoSlice.h
  mmiInfoImage.cpp
  mmiInfoImage.h
  mmiSelectPoint.cpp
  mmiSelectPoint.h
  medLogicWithManagers.cpp
  medLogicWithManagers.h
  

  # medGeometryEditorPolylineGraph.cpp
  # medGeometryEditorPolylineGraph.h
  medDecl.h
  medCurvilinearAbscissaOnSkeletonHelper.cpp
  medCurvilinearAbscissaOnSkeletonHelper.h
  medGizmoInteractionDebugger.cpp
  medGizmoInteractionDebugger.h

  medRelationalDatabaseAbstractConnector.cpp
  medRelationalDatabaseAbstractConnector.h

  medResultQueryAbstractHandler.cpp
  medResultQueryAbstractHandler.h
  
  medQueryObject.h
  
  medHTMLTemplateParser.cpp
  medHTMLTemplateParser.h
  medHTMLTemplateParserBlock.cpp
  medHTMLTemplateParserBlock.h
  
  medVect3d.cpp
  medVect3d.h
  
  medWizardBlock.cpp
  medWizardBlock.h
  
  medWizard.cpp
  medWizard.h
  
  medWizardManager.cpp
  medWizardManager.h
  
  medWizardOperationBlock.cpp
  medWizardOperationBlock.h
  
  medWizardSelectionBlock.cpp
  medWizardSelectionBlock.h
  
  medWizardTypeCheckBlock.cpp
  medWizardTypeCheckBlock.h
  
  
	#### SHOULD BE COMMITTED DOWN ######
	mafDbg.h
	mafDataChecksum.h
	##################################
	
	#Removed from vtkMED because it need of MAF Utilities
	../BES_Beta/vtkMAF/vtkMAFLargeDataSetCallback.cxx
  ../BES_Beta/vtkMAF/vtkMAFLargeDataSetCallback.h
)

# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafVME" "mafInteraction")

SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMED)

IF(MED_BUILD_MEDDLL)
  SET(BUILD_SHARED_LIBS 1)
  ADD_DEFINITIONS(-DMED_COMMON_EXPORTS)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} mafDLL)
ELSE(MED_BUILD_MEDDLL)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ENDIF (MED_BUILD_MEDDLL)

ADD_DEPENDENCIES(${PROJECT_NAME} vtkMED)