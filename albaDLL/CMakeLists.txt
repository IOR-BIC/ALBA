#
# Copyright and description to be placed here
#

PROJECT(albaDLL)

DBG_MESSAGE("Configuring project albaDLL")

ADD_DEFINITIONS(-DALBA_EXPORTS -UPROJ_SAX2 -UPROJ_XMLPARSER -UPROJ_XMLUTIL -UPROJ_PARSERS -UPROJ_SAX4C -UPROJ_DOM -UPROJ_VALIDATORS -UXML_SINGLEDLL -UXML_USE_WIN32_TRANSCODER -UXML_USE_WIN32_MSGLOADER -UXML_USE_NETACCESSOR_WINSOCK)

############################## 
# LINKED LIBRARIES 
SET(PROJECT_LIBS glu32 opengl32 ${VTK_LIBRARIES} curl Crypto vtkALBA GPUAPI)

IF (ALBA_USE_XercesC)
  SET(PROJECT_LIBS ${PROJECT_LIBS} XercesC)
ENDIF (ALBA_USE_XercesC)

IF (ALBA_USE_GDCM)
 SET(PROJECT_LIBS ${PROJECT_LIBS} ${GDCM_LIBRARIES})
ENDIF (ALBA_USE_GDCM)

LINK_LIBRARIES(${PROJECT_LIBS})


###############################################
#BASE
###############################################
SET(BASE_SRCS
../Base/albaBase.h
../Base/albaDecl.cpp				../Base/albaDecl.h
../Base/albaDbg.h
../Base/albaDataChecksum.h
../Base/albaDefines.cpp			../Base/albaDefines.h
../Base/albaDirectory.cpp		../Base/albaDirectory.h
../Base/albaDynamicLoader.cpp	../Base/albaDynamicLoader.h
../Base/albaEventBase.cpp		../Base/albaEventBase.h
../Base/albaEventSender.h
../Base/albaEventBroadcaster.cpp		../Base/albaEventBroadcaster.h
../Base/albaIncludeWIN32.h
../Base/albaIncludeWX.h
../Base/albaIndent.cpp			../Base/albaIndent.h
../Base/albaLUTLibrary.cpp		../Base/albaLUTLibrary.h
../Base/albaMTime.cpp			../Base/albaMTime.h
../Base/albaMatrix.cpp			../Base/albaMatrix.h
../Base/albaMatrix3x3.cpp		../Base/albaMatrix3x3.h
../Base/albaVect3d.cpp			../Base/albaVect3d.h
../Base/albaMultiThreader.cpp	../Base/albaMultiThreader.h
../Base/albaMutexLock.cpp		../Base/albaMutexLock.h
../Base/albaOBB.cpp				../Base/albaOBB.h
../Base/albaObject.cpp			../Base/albaObject.h
../Base/albaObjectFactory.cpp	../Base/albaObjectFactory.h
../Base/albaObserver.cpp			../Base/albaObserver.h
../Base/albaObserverCallback.cpp	../Base/albaObserverCallback.h
../Base/albaReferenceCounted.cpp	../Base/albaReferenceCounted.h
../Base/albaSmartPointer.h
../Base/albaString.cpp			../Base/albaString.h
../Base/albaTimeStamped.cpp		../Base/albaTimeStamped.h
../Base/albaTransform.cpp		../Base/albaTransform.h
../Base/albaTransformBase.cpp	../Base/albaTransformBase.h
../Base/albaTransformFrame.cpp	../Base/albaTransformFrame.h
../Base/albaVector.h
../Base/albaVersion.h
../Base/mmuIdFactory.cpp		../Base/mmuIdFactory.h
../Base/albaUtility.h
../Base/vtkALBASmartPointer.h
../Base/vtkALBAToLinearTransform.h
../Base/albaZipUtility.cpp		../Base/albaZipUtility.h
)

IF (ALBA_USE_VTK)
  SET(BASE_SRCS
    ${BASE_SRCS}
    ../Base/vtkALBAToLinearTransform.cxx		../Base/vtkALBAToLinearTransform.h
    ../Base/vtkALBASmartPointer.h
  )
ENDIF (ALBA_USE_VTK)


###############################################
#CORE
###############################################
SET(CORE_SRCS
  ../Core/albaAbsMatrixPipe.cpp  		../Core/albaAbsMatrixPipe.h
  ../Core/albaAttribute.cpp  			../Core/albaAttribute.h
  ../Core/albaAttributeTraceability.cpp  ../Core/albaAttributeTraceability.h
  ../Core/albaCoreFactory.cpp  			../Core/albaCoreFactory.h
  ../Core/albaDataPipe.cpp  				../Core/albaDataPipe.h
  ../Core/albaEventIO.cpp  				../Core/albaEventIO.h
  ../Core/albaMatrixPipe.cpp  			../Core/albaMatrixPipe.h
  ../Core/albaVMEIterator.cpp  			../Core/albaVMEIterator.h
  ../Core/albaOp.cpp  ../Core/albaOp.h
  ../Core/albaOpContextStack.cpp  		../Core/albaOpContextStack.h
  ../Core/albaOpManager.cpp  			../Core/albaOpManager.h
  ../Core/albaOpSelect.cpp  				../Core/albaOpSelect.h
  ../Core/albaOpStack.cpp  				../Core/albaOpStack.h
  ../Core/albaPrintManager.cpp  			../Core/albaPrintManager.h
  ../Core/albaTagArray.cpp  				../Core/albaTagArray.h
  ../Core/albaTagItem.cpp  				../Core/albaTagItem.h
  ../Core/albaTimeMap.h
  ../Core/albaTimeMapScalar.h
  ../Core/albaUser.cpp  					../Core/albaUser.h
  ../Core/albaVME.cpp  					../Core/albaVME.h
  ../Core/albaVMEManager.cpp  			../Core/albaVMEManager.h
  ../Core/albaVMEOutput.cpp  			../Core/albaVMEOutput.h
  ../Core/albaVMEOutputNULL.cpp  		../Core/albaVMEOutputNULL.h
  ../Core/albaVMERoot.cpp  				../Core/albaVMERoot.h
  ../Core/albaVMEStorage.cpp  			../Core/albaVMEStorage.h
  ../Core/mmaApplicationLayout.cpp  	../Core/mmaApplicationLayout.h
  ../Core/albaGUIListCtrlBitmap.cpp  	../Core/albaGUIListCtrlBitmap.h
  ../Core/albaGUIVMEChooser.cpp  		../Core/albaGUIVMEChooser.h
  ../Core/albaGUIVMEChooserTree.cpp  	../Core/albaGUIVMEChooserTree.h
  ../Core/mmuTimeSet.cpp  				../Core/mmuTimeSet.h
  ../Core/albaProgressBarHelper.cpp 		../Core/albaProgressBarHelper.h
)

IF (ALBA_USE_VTK)
  SET(CORE_SRCS
  ${CORE_SRCS}
  ../Core/albaAxes.cpp  					../Core/albaAxes.h
  ../Core/albaLODActor.cpp  				../Core/albaLODActor.h
  ../Core/albaLogicWithManagers.cpp  	../Core/albaLogicWithManagers.h
  ../Core/albaPipe.cpp  					../Core/albaPipe.h
  ../Core/albaPipeBox.cpp  				../Core/albaPipeBox.h
  ../Core/albaPipeFactory.cpp  			../Core/albaPipeFactory.h
  ../Core/albaRWI.cpp  					../Core/albaRWI.h
  ../Core/albaRWIBase.cpp  				../Core/albaRWIBase.h
  ../Core/albaSceneGraph.cpp  			../Core/albaSceneGraph.h
  ../Core/albaSceneNode.cpp  			../Core/albaSceneNode.h
  ../Core/albaViewCompound.cpp  			../Core/albaViewCompound.h
  ../Core/albaViewHTML.cpp  				../Core/albaViewHTML.h
  ../Core/albaViewManager.cpp  			../Core/albaViewManager.h
  ../Core/albaViewPlot.cpp  				../Core/albaViewPlot.h
  ../Core/albaViewVTK.cpp  				../Core/albaViewVTK.h
  ../Core/mmaMaterial.cpp  				../Core/mmaMaterial.h
  ../Core/mmaVolumeMaterial.cpp  		../Core/mmaVolumeMaterial.h
  ../Core/albaGUIContextualMenu.cpp  	../Core/albaGUIContextualMenu.h
  ../Core/albaGUIMaterialButton.cpp  	../Core/albaGUIMaterialButton.h
  ../Core/albaGUIMaterialChooser.cpp  	../Core/albaGUIMaterialChooser.h
  ../Core/albaGUITreeContextualMenu.cpp  ../Core/albaGUITreeContextualMenu.h
  ../Core/vtkALBAAssembly.cpp  ../Core/vtkALBAAssembly.h
  )
ENDIF (ALBA_USE_VTK)

###############################################
#COMMON
###############################################
SET(COMMON_SRCS

  ../Common/albaItkRawMotionImporterUtility.cxx  					../Common/albaItkRawMotionImporterUtility.h  
  ../Common/albaPolylineGraph.cpp  												../Common/albaPolylineGraph.h  
  ../Common/albaCurvilinearAbscissaOnSkeletonHelper.cpp  	../Common/albaCurvilinearAbscissaOnSkeletonHelper.h  
  ../Common/albaRelationalDatabaseAbstractConnector.cpp  	../Common/albaRelationalDatabaseAbstractConnector.h  
  ../Common/albaResultQueryAbstractHandler.cpp							../Common/albaResultQueryAbstractHandler.h  
  ../Common/albaQueryObject.h  
  ../Common/albaHTMLTemplateParser.cpp  										../Common/albaHTMLTemplateParser.h  
  ../Common/albaHTMLTemplateParserBlock.cpp  							../Common/albaHTMLTemplateParserBlock.h  
  ../Common/albaWizardBlock.cpp  													../Common/albaWizardBlock.h  
  ../Common/albaWizard.cpp  																../Common/albaWizard.h  
  ../Common/albaWizardManager.cpp  												../Common/albaWizardManager.h  
  ../Common/albaWizardBlockOperation.cpp  									../Common/albaWizardBlockOperation.h  
  ../Common/albaWizardBlockSelection.cpp  									../Common/albaWizardBlockSelection.h  
  ../Common/albaWizardBlockTypeCheck.cpp  									../Common/albaWizardBlockTypeCheck.h
  ../Common/albaWizardBlockFileExistCheck.cpp  						../Common/albaWizardBlockFileExistCheck.h  
  ../Common/albaWizardBlockVMESelection.cpp  							../Common/albaWizardBlockVMESelection.h
  ../Common/albaWizardBlockVMEFindOrSelection.cpp  				../Common/albaWizardBlockVMEFindOrSelection.h  
  ../Common/albaWizardBlockInformation.cpp  								../Common/albaWizardBlockInformation.h
  ../Common/albaWizardBlockYesNoSelection.cpp  						../Common/albaWizardBlockYesNoSelection.h  
  ../Common/albaWizardWaitOp.cpp  													../Common/albaWizardWaitOp.h  
  ../Common/albaWizardSettings.cpp  												../Common/albaWizardSettings.h  
  ../Common/albaHelpManager.cpp  													../Common/albaHelpManager.h  
	../Common/albaSnapshotManager.cpp  											../Common/albaSnapshotManager.h
	../Common/albaLicenceManager.cpp 												../Common/albaLicenceManager.h  
	../Common/albaTextFileReaderHelper.cpp										../Common/albaTextFileReaderHelper.h
)


###############################################
#SERVICES
###############################################
SET(SERVICE_SRCS
	../Service/albaServiceLocator.cpp 	../Service/albaServiceLocator.h
	../Service/albaServiceClient.cpp 	../Service/albaServiceClient.h
	../Service/albaAbsLogicManager.h
)


###############################################
#GUI
###############################################
SET(GUI_SRCS
  ../Gui/albaEvent.cpp  ../Gui/albaEvent.h
  ../Gui/albaGUIDialogFindVme.cpp  ../Gui/albaGUIDialogFindVme.h
  ../Gui/albaGUIDialogLogin.cpp  ../Gui/albaGUIDialogLogin.h
  
  ../Gui/albaGUIDialogTransferFunction2D.cpp  ../Gui/albaGUIDialogTransferFunction2D.h
  ../Gui/albaTransferFunction2DInteractors.cpp
  
  ../Gui/albaGUISettings.cpp  ../Gui/albaGUISettings.h
  ../Gui/albaGUISettingsTimeBar.cpp  ../Gui/albaGUISettingsTimeBar.h
  
  ../Gui/albaPics.cpp  						../Gui/albaPics.h
  ../Gui/albaWXLog.cpp  					../Gui/albaWXLog.h
  ../Gui/albaGUIApplicationSettings.cpp  	../Gui/albaGUIApplicationSettings.h
  ../Gui/albaGUIButton.cpp  				../Gui/albaGUIButton.h
  ../Gui/albaGUICheckListBox.cpp  			../Gui/albaGUICheckListBox.h
  ../Gui/albaGUICheckTree.cpp  				../Gui/albaGUICheckTree.h
  ../Gui/albaGUIColorSwatch.cpp  			../Gui/albaGUIColorSwatch.h
  ../Gui/albaGUIColorWidget.cpp  			../Gui/albaGUIColorWidget.h
  ../Gui/albaGUICrossSplitter.cpp  			../Gui/albaGUICrossSplitter.h
  ../Gui/albaGUIDialog.cpp  				../Gui/albaGUIDialog.h
  ../Gui/albaGUIDockManager.cpp  			../Gui/albaGUIDockManager.h
  ../Gui/albaGUIFileHistory.cpp  			../Gui/albaGUIFileHistory.h
  ../Gui/albaGUIFlagSplitter.cpp  			../Gui/albaGUIFlagSplitter.h
  ../Gui/albaGUIFloatSlider.cpp  			../Gui/albaGUIFloatSlider.h
  ../Gui/albaGUIFrame.cpp  					../Gui/albaGUIFrame.h
  ../Gui/albaGUI.cpp  						../Gui/albaGUI.h
  ../Gui/albaGUIHolder.cpp  				../Gui/albaGUIHolder.h
  ../Gui/albaGUILab.cpp  					../Gui/albaGUILab.h
  ../Gui/albaGUIListBox.cpp  				../Gui/albaGUIListBox.h
  ../Gui/albaGUIListCtrl.cpp  				../Gui/albaGUIListCtrl.h
  ../Gui/albaGUIMovieCtrl.cpp  				../Gui/albaGUIMovieCtrl.h
  ../Gui/albaGUIMDIChild.cpp  				../Gui/albaGUIMDIChild.h
  ../Gui/albaGUIMDIFrame.cpp  				../Gui/albaGUIMDIFrame.h
  ../Gui/albaGUIMutexPanel.cpp  			../Gui/albaGUIMutexPanel.h
  ../Gui/albaGUINamedPanel.cpp  			../Gui/albaGUINamedPanel.h
  ../Gui/albaGUIPanel.cpp  					../Gui/albaGUIPanel.h
  ../Gui/albaGUIPanelStack.cpp  			../Gui/albaGUIPanelStack.h
  ../Gui/albaGUIPicButton.cpp  				../Gui/albaGUIPicButton.h
  ../Gui/albaGUIRangeSlider.cpp  			../Gui/albaGUIRangeSlider.h
  ../Gui/albaGUIRollOut.cpp  				../Gui/albaGUIRollOut.h
  ../Gui/albaGUISashPanel.h  				../Gui/albaGUISashPanel.cpp
  ../Gui/albaGUIScrolledPanel.cpp  			../Gui/albaGUIScrolledPanel.h
  ../Gui/albaGUISettingsDialog.cpp  		../Gui/albaGUISettingsDialog.h
  ../Gui/albaGUISplittedPanel.cpp  			../Gui/albaGUISplittedPanel.h
  ../Gui/albaGUITimeBar.cpp  				../Gui/albaGUITimeBar.h
  ../Gui/albaGUITree.cpp  					../Gui/albaGUITree.h
  ../Gui/albaGUIValidator.cpp  				../Gui/albaGUIValidator.h
  ../Gui/albaGUIViewFrame.cpp  				../Gui/albaGUIViewFrame.h
  ../Gui/albaGUIViewWin.cpp  				../Gui/albaGUIViewWin.h
  ../Gui/albaGUICrossIncremental.cpp  		../Gui/albaGUICrossIncremental.h
  ../Gui/albaGUIComboBox.cpp  				../Gui/albaGUIComboBox.h
  ../Gui/albaGUILandmark.cpp  				../Gui/albaGUILandmark.h
  ../Gui/albaGUIWizard.cpp  				../Gui/albaGUIWizard.h
  ../Gui/albaGUIWizardPage.cpp  			../Gui/albaGUIWizardPage.h
  ../Gui/albaGUIWizardPageNew.cpp  			../Gui/albaGUIWizardPageNew.h
  ../Gui/albaGUIDicomSettings.cpp  			../Gui/albaGUIDicomSettings.h
  ../Gui/albaGUISettingsAdvanced.cpp  		../Gui/albaGUISettingsAdvanced.h
  ../Gui/albaGUIDynamicVP.cpp  				../Gui/albaGUIDynamicVP.h
  ../Gui/albaGUILutHistogramSwatch.cpp  	../Gui/albaGUILutHistogramSwatch.h
  ../Gui/albaGUILutHistogramEditor.cpp  	../Gui/albaGUILutHistogramEditor.h
	../Gui/albaGUIImageViewer.cpp							../Gui/albaGUIImageViewer.h
	../Gui/albaGUIAboutDialog.cpp				../Gui/albaGUIAboutDialog.h
)

IF (ALBA_USE_VTK)
  SET(GUI_SRCS
  ${GUI_SRCS}
  ../Gui/albaLightKit.cpp					  ../Gui/albaLightKit.h
  ../Gui/albaGUIDialogPreview.cpp			  ../Gui/albaGUIDialogPreview.h
  ../Gui/albaGUIDictionaryWidget.cpp		  ../Gui/albaGUIDictionaryWidget.h
  ../Gui/albaAnimate.cpp					  ../Gui/albaAnimate.h
  ../Gui/albaAttachCamera.cpp				  ../Gui/albaAttachCamera.h
	../Gui/albaAbsSideBar.h
  ../Gui/albaSideBar.cpp					  ../Gui/albaSideBar.h
  ../Gui/albaTextKit.cpp					  ../Gui/albaTextKit.h
  ../Gui/albaVTKLog.cpp						  ../Gui/albaVTKLog.h

  # depends on albaAssembly -> albaRWIBase
  ../Gui/albaView.cpp						  ../Gui/albaView.h

  # these depends on vtkLookupTable
  ../Gui/albaColor.cpp						  ../Gui/albaColor.h
  ../Gui/albaGUILutEditor.cpp				  ../Gui/albaGUILutEditor.h
  ../Gui/albaGUILutPreset.cpp				  ../Gui/albaGUILutPreset.h
  ../Gui/albaGUILutSlider.cpp				  ../Gui/albaGUILutSlider.h
  ../Gui/albaGUILutSwatch.cpp				  ../Gui/albaGUILutSwatch.h
  ../Gui/albaGUILutWidget.cpp				  ../Gui/albaGUILutWidget.h
  ../Gui/albaGUIHistogramWidget.cpp			  ../Gui/albaGUIHistogramWidget.h
  )
ENDIF (ALBA_USE_VTK)


###############################################
#INTERACTION
###############################################
SET(INTERACTION_SRCS
  ../Interaction/albaAction.cpp  							../Interaction/albaAction.h
  ../Interaction/albaAgent.cpp  							../Interaction/albaAgent.h
  ../Interaction/albaAgentEventHandler.cpp  				../Interaction/albaAgentEventHandler.h
  ../Interaction/albaAgentEventQueue.cpp  					../Interaction/albaAgentEventQueue.h
  ../Interaction/albaAgentThreaded.cpp  					../Interaction/albaAgentThreaded.h
  ../Interaction/albaAvatar.cpp  							../Interaction/albaAvatar.h
  ../Interaction/albaAvatar2D.cpp  							../Interaction/albaAvatar2D.h
  ../Interaction/albaAvatar3D.cpp  							../Interaction/albaAvatar3D.h
  ../Interaction/albaAvatar3DCone.cpp  						../Interaction/albaAvatar3DCone.h
  ../Interaction/albaCameraTransform.cpp  					../Interaction/albaCameraTransform.h
  ../Interaction/albaGizmoAutoscaleHelper.cpp  				../Interaction/albaGizmoAutoscaleHelper.h
  ../Interaction/albaDevice.cpp  							../Interaction/albaDevice.h
  ../Interaction/albaDeviceManager.cpp  					../Interaction/albaDeviceManager.h
  ../Interaction/albaDeviceSet.cpp  						../Interaction/albaDeviceSet.h
  ../Interaction/albaEventInteraction.cpp  					../Interaction/albaEventInteraction.h
  ../Interaction/albaGizmoBoundingBox.cpp  					../Interaction/albaGizmoBoundingBox.h
  ../Interaction/albaGizmoHandle.cpp  						../Interaction/albaGizmoHandle.h
  ../Interaction/albaGizmoInterface.cpp  					../Interaction/albaGizmoInterface.h
  ../Interaction/albaGizmoPath.cpp  						../Interaction/albaGizmoPath.h
  ../Interaction/albaGizmoPathRuler.cpp  					../Interaction/albaGizmoPathRuler.h
  ../Interaction/albaGizmoROI.cpp  							../Interaction/albaGizmoROI.h
  ../Interaction/albaGizmoRotate.cpp  						../Interaction/albaGizmoRotate.h
  ../Interaction/albaGizmoRotateCircle.cpp  				../Interaction/albaGizmoRotateCircle.h
  ../Interaction/albaGizmoRotateFan.cpp  					../Interaction/albaGizmoRotateFan.h
  ../Interaction/albaGizmoScale.cpp  						../Interaction/albaGizmoScale.h
  ../Interaction/albaGizmoScaleAxis.cpp  					../Interaction/albaGizmoScaleAxis.h
  ../Interaction/albaGizmoScaleIsotropic.cpp  				../Interaction/albaGizmoScaleIsotropic.h
  ../Interaction/albaGizmoTranslate.cpp  					../Interaction/albaGizmoTranslate.h
  ../Interaction/albaGizmoTranslateAxis.cpp  				../Interaction/albaGizmoTranslateAxis.h
  ../Interaction/albaGizmoTranslatePlane.cpp  				../Interaction/albaGizmoTranslatePlane.h
  ../Interaction/albaGizmoSlice.cpp  						../Interaction/albaGizmoSlice.h
  ../Interaction/albaGizmoInteractionDebugger.cpp  			../Interaction/albaGizmoInteractionDebugger.h
  ../Interaction/albaGUIGizmoInterface.cpp  				../Interaction/albaGUIGizmoInterface.h
  ../Interaction/albaGUIGizmoRotate.cpp  					../Interaction/albaGUIGizmoRotate.h
  ../Interaction/albaGUIGizmoScale.cpp  					../Interaction/albaGUIGizmoScale.h
  ../Interaction/albaGUIGizmoTranslate.cpp  				../Interaction/albaGUIGizmoTranslate.h
  ../Interaction/albaGUISaveRestorePose.cpp  				../Interaction/albaGUISaveRestorePose.h
  ../Interaction/albaGUITransformInterface.cpp  			../Interaction/albaGUITransformInterface.h
  ../Interaction/albaGUITransformMouse.cpp  				../Interaction/albaGUITransformMouse.h
  ../Interaction/albaGUITransformTextEntries.cpp  			../Interaction/albaGUITransformTextEntries.h
  ../Interaction/albaInteractionFactory.cpp  				../Interaction/albaInteractionFactory.h
  ../Interaction/albaInteractionManager.cpp  				../Interaction/albaInteractionManager.h
  ../Interaction/albaInteractor.cpp  						../Interaction/albaInteractor.h
  ../Interaction/albaRefSys.cpp  							../Interaction/albaRefSys.h  
  ../Interaction/albaDeviceButtonsPad.cpp  					../Interaction/albaDeviceButtonsPad.h
  ../Interaction/albaDeviceButtonsPadMouse.cpp  			../Interaction/albaDeviceButtonsPadMouse.h
  ../Interaction/albaDeviceButtonsPadTracker.cpp  			../Interaction/albaDeviceButtonsPadTracker.h  
  ../Interaction/albaInteractor2DAngle.cpp  				../Interaction/albaInteractor2DAngle.h
  ../Interaction/albaInteractor2DDistance.cpp  				../Interaction/albaInteractor2DDistance.h
  ../Interaction/albaInteractor2DIndicator.cpp  			../Interaction/albaInteractor2DIndicator.h
  ../Interaction/albaInteractor6DOF.cpp  					../Interaction/albaInteractor6DOF.h
  ../Interaction/albaInteractor6DOFCameraMove.cpp  			../Interaction/albaInteractor6DOFCameraMove.h
  ../Interaction/albaInteractorCameraMove.cpp  				../Interaction/albaInteractorCameraMove.h
  ../Interaction/albaInteractorCompositorMouse.cpp  		../Interaction/albaInteractorCompositorMouse.h
  ../Interaction/albaInteractorConstraint.cpp  				../Interaction/albaInteractorConstraint.h
  ../Interaction/albaInteractorExtractIsosurface.cpp  		../Interaction/albaInteractorExtractIsosurface.h
  ../Interaction/albaInteractorGeneric6DOF.cpp  			../Interaction/albaInteractorGeneric6DOF.h
  ../Interaction/albaInteractorGenericInterface.cpp  		../Interaction/albaInteractorGenericInterface.h
  ../Interaction/albaInteractorGenericMouse.cpp  			../Interaction/albaInteractorGenericMouse.h
  ../Interaction/albaInteractorPER.cpp  					../Interaction/albaInteractorPER.h
  ../Interaction/albaInteractorPicker.cpp  					../Interaction/albaInteractorPicker.h
  ../Interaction/albaInteractorSelectCell.cpp  				../Interaction/albaInteractorSelectCell.h
  ../Interaction/albaInteractorSER.cpp  					../Interaction/albaInteractorSER.h  ######################
  ../Interaction/albaInteractorCameraPicker.cpp  					../Interaction/albaInteractorCameraPicker.h
  ../Interaction/albaInteractorPERPicker.cpp  				../Interaction/albaInteractorPERPicker.h
  ../Interaction/albaInteractorDICOMImporter.h  			../Interaction/albaInteractorDICOMImporter.cpp  
  ../Interaction/albaInteractorSegmentationPicker.cpp  		../Interaction/albaInteractorSegmentationPicker.h 
  ../Interaction/albaInteractorPERScalarInformation.cpp  	../Interaction/albaInteractorPERScalarInformation.h
  ../Interaction/albaInteractorPERBrushFeedback.cpp  		../Interaction/albaInteractorPERBrushFeedback.h
  ../Interaction/mmiVTKPicker.cpp  						../Interaction/mmiVTKPicker.h
  ../Interaction/mmiInfoImage.cpp  						../Interaction/mmiInfoImage.h
  ../Interaction/mmiSelectPoint.cpp  						../Interaction/mmiSelectPoint.h
  ../Interaction/albaGizmoPolylineGraph.cpp  				../Interaction/albaGizmoPolylineGraph.h
  ../Interaction/albaDeviceButtonsPadMouseDialog.cpp	  	../Interaction/albaDeviceButtonsPadMouseDialog.h
  ../Interaction/albaGUITransformSliders.cpp  				../Interaction/albaGUITransformSliders.h    
  ../Interaction/albaGizmoCrossRotateTranslate.cpp  		../Interaction/albaGizmoCrossRotateTranslate.h	  
  ../Interaction/albaGizmoCrossRotate.cpp  					../Interaction/albaGizmoCrossRotate.h
  ../Interaction/albaGizmoCrossRotateAxis.cpp  				../Interaction/albaGizmoCrossRotateAxis.h
  ../Interaction/albaGizmoCrossRotateFan.cpp  				../Interaction/albaGizmoCrossRotateFan.h
  ../Interaction/albaGizmoCrossTranslate.cpp  				../Interaction/albaGizmoCrossTranslate.h
  ../Interaction/albaGizmoCrossTranslateAxis.cpp  			../Interaction/albaGizmoCrossTranslateAxis.h
  ../Interaction/albaGizmoCrossTranslatePlane.cpp  			../Interaction/albaGizmoCrossTranslatePlane.h
	../Interaction/albaInteractorSlider.cpp								../Interaction/albaInteractorSlider.h
)

IF (ALBA_USE_P5_GLOVE)
  SET(INTERACTION_SRCS
  ${INTERACTION_SRCS}
  ../Interaction/albaDeviceButtonsPadTrackerP5Glove.cpp		../Interaction/albaDeviceButtonsPadTrackerP5Glove.h
  )
ENDIF (ALBA_USE_P5_GLOVE)

IF (ALBA_USE_WIIMOTE)
  SET(INTERACTION_SRCS
  ${INTERACTION_SRCS}
  ../Interaction/albaDeviceTrackerWIIMote.cpp				../Interaction/albaDeviceTrackerWIIMote.h
  )
ENDIF (ALBA_USE_WIIMOTE)


###############################################
#IO
###############################################
SET(IO_SRCS
  ../IO/albaCurlUtility.cpp  			../IO/albaCurlUtility.h
  ../IO/albaStorable.cpp  				../IO/albaStorable.h
  ../IO/albaStorage.cpp  				../IO/albaStorage.h
  ../IO/albaStorageElement.cpp  		../IO/albaStorageElement.h
  ../IO/mmuDOMTreeErrorReporter.h  	../IO/mmuDOMTreeErrorReporter.cpp
)

IF (ALBA_USE_XercesC)
  SET(IO_SRCS
    ${IO_SRCS}
    ../IO/albaXMLElement.cpp    		../IO/albaXMLElement.h
    ../IO/albaXMLStorage.cpp    		../IO/albaXMLStorage.h
    ../IO/albaXMLString.cpp    		../IO/albaXMLString.h
    ../IO/mmuXMLDOM.h
    ../IO/mmuXMLDOMElement.h
  )
ENDIF (ALBA_USE_XercesC)


###############################################
#VME
###############################################
SET(VME_SRCS
  ../VME/albaDataVector.cpp
  ../VME/albaDataVector.h
  ../VME/albaMatrixInterpolator.cpp
  ../VME/albaMatrixInterpolator.h
  ../VME/albaMatrixVector.cpp
  ../VME/albaMatrixVector.h
  ../VME/albaPipeFactoryVME.cpp
  ../VME/albaPipeFactoryVME.h
  ../VME/albaScalarVector.cpp
  ../VME/albaScalarVector.h
  ../VME/albaVMEExternalData.cpp
  ../VME/albaVMEExternalData.h
  ../VME/albaVMEFactory.cpp
  ../VME/albaVMEFactory.h
  ../VME/albaVMEGenericAbstract.cpp
  ../VME/albaVMEGenericAbstract.h
  ../VME/albaVMEGroup.cpp
  ../VME/albaVMEGroup.h
  ../VME/albaVMEItem.cpp
  ../VME/albaVMEItem.h
  ../VME/albaVMEInfoText.cpp
  ../VME/albaVMEInfoText.h
  ../VME/albaMSFImporter.cpp
  ../VME/albaMSFImporter.h
  ../VME/albaPipePolylineSlice.cpp
  ../VME/albaPipePolylineSlice.h
  ../VME/albaPipeMesh.cpp
  ../VME/albaPipeMesh.h
  ../VME/albaPipeMeshSlice.cpp
  ../VME/albaPipeMeshSlice.h
  ../VME/albaPipeScalar.cpp
  ../VME/albaPipeScalar.h
  ../VME/albaVisualPipeVolumeRayCasting.cpp
  ../VME/albaVisualPipeVolumeRayCasting.h
  ../VME/albaVMEOutputScalar.cpp
  ../VME/albaVMEOutputScalar.h
  ../VME/albaVMEScalar.cpp
  ../VME/albaVMEScalar.h
)

IF (ALBA_USE_CRYPTO)
  SET(VME_SRCS
    ${VME_SRCS}
    ../VME/albaCrypt.cpp
    ../VME/albaCrypt.h
  )
ENDIF (ALBA_USE_CRYPTO)

IF (ALBA_USE_ITK)
  SET(VME_SRCS
    ${VME_SRCS}
    ../VME/albaPipeScalarMatrix.cpp
    ../VME/albaPipeScalarMatrix.h
    ../VME/albaVMEOutputScalarMatrix.cpp
    ../VME/albaVMEOutputScalarMatrix.h
    ../VME/albaVMEScalarMatrix.cpp
    ../VME/albaVMEScalarMatrix.h
    ../VME/albaVMEItemScalarMatrix.cpp
    ../VME/albaVMEItemScalarMatrix.h
  )
ENDIF (ALBA_USE_ITK)

IF (ALBA_USE_VTK)
  SET(VME_SRCS
    ${VME_SRCS}
    ../VME/albaPipeGizmo.cpp
    ../VME/albaPipeGizmo.h
    ../VME/albaPipeImage3D.cpp
    ../VME/albaPipeImage3D.h
    ../VME/albaPipeIsosurface.cpp
    ../VME/albaPipeIsosurface.h
    ../VME/albaPipeLandmarkCloud.cpp
    ../VME/albaPipeLandmarkCloud.h
    ../VME/albaPipeMeter.cpp
    ../VME/albaPipeMeter.h
    ../VME/albaPipePointSet.cpp
    ../VME/albaPipePointSet.h
    ../VME/albaPipePolyline.cpp
    ../VME/albaPipePolyline.h
    ../VME/albaPipeSurface.cpp
    ../VME/albaPipeSurface.h
    ../VME/albaPipeSurfaceSlice.cpp
    ../VME/albaPipeSurfaceSlice.h
    ../VME/albaPipeSurfaceTextured.cpp
    ../VME/albaPipeSurfaceTextured.h
    ../VME/albaPipeVector.cpp
    ../VME/albaPipeVector.h
    ../VME/albaPipeVolumeProjected.cpp
    ../VME/albaPipeVolumeProjected.h
    ../VME/albaPipeVolumeSlice.cpp
    ../VME/albaPipeVolumeSlice.h
    ../VME/albaVMEGeneric.cpp
    ../VME/albaVMEGeneric.h
    ../VME/albaVMEGizmo.cpp
    ../VME/albaVMEGizmo.h
    ../VME/albaVMEImage.cpp
    ../VME/albaVMEImage.h
    ../VME/albaVMEItemVTK.cpp
    ../VME/albaVMEItemVTK.h
    ../VME/albaVMELandmark.cpp
    ../VME/albaVMELandmark.h
    ../VME/albaVMELandmarkCloud.cpp
    ../VME/albaVMELandmarkCloud.h
    ../VME/albaVMEMesh.cpp
    ../VME/albaVMEMesh.h
    ../VME/albaVMEMeter.cpp
    ../VME/albaVMEMeter.h
    ../VME/albaVMEOutputImage.cpp
    ../VME/albaVMEOutputImage.h
    ../VME/albaVMEOutputLandmarkCloud.cpp
    ../VME/albaVMEOutputLandmarkCloud.h
    ../VME/albaVMEOutputMesh.cpp
    ../VME/albaVMEOutputMesh.h
    ../VME/albaVMEOutputMeter.cpp
    ../VME/albaVMEOutputMeter.h
    ../VME/albaVMEOutputPointSet.cpp
    ../VME/albaVMEOutputPointSet.h
    ../VME/albaVMEOutputPolyline.cpp
    ../VME/albaVMEOutputPolyline.h
    ../VME/albaVMEOutputSurface.cpp
    ../VME/albaVMEOutputSurface.h
    ../VME/albaVMEOutputVolume.cpp
    ../VME/albaVMEOutputVolume.h
    ../VME/albaVMEOutputVTK.cpp
    ../VME/albaVMEOutputVTK.h
    ../VME/albaVMEPointSet.cpp
    ../VME/albaVMEPointSet.h
    ../VME/albaVMEPolyline.cpp
    ../VME/albaVMEPolyline.h
    ../VME/albaVMEPolylineSpline.cpp
    ../VME/albaVMEPolylineSpline.h
    ../VME/albaVMEProber.cpp
    ../VME/albaVMEProber.h
    ../VME/albaVMERefSys.cpp
    ../VME/albaVMERefSys.h
    ../VME/albaVMESlicer.cpp
    ../VME/albaVMESlicer.h
    ../VME/albaVMESurface.cpp
    ../VME/albaVMESurface.h
    ../VME/albaVMESurfaceParametric.cpp
    ../VME/albaVMESurfaceParametric.h
    ../VME/albaVMEVector.cpp
    ../VME/albaVMEVector.h
    ../VME/albaVMEVolume.cpp
    ../VME/albaVMEVolume.h
    ../VME/albaVMEVolumeGray.cpp
    ../VME/albaVMEVolumeGray.h
    ../VME/albaVMEVolumeRGB.cpp
    ../VME/albaVMEVolumeRGB.h
    ../VME/mmaMeter.cpp
    ../VME/mmaMeter.h
  )
ENDIF (ALBA_USE_VTK)

IF(WIN32)
  SET(VME_SRCS
    ${VME_SRCS}
    ../VME/albaPipeIsosurfaceGPU.cpp
    ../VME/albaPipeIsosurfaceGPU.h
  )
ENDIF(WIN32)

SET(VME_SRCS
  ../VME/albaDataVector.cpp  ../VME/albaDataVector.h
  ../VME/albaMatrixInterpolator.cpp  				../VME/albaMatrixInterpolator.h
  ../VME/albaMatrixVector.cpp  						../VME/albaMatrixVector.h
  ../VME/albaPipeFactoryVME.cpp  					../VME/albaPipeFactoryVME.h
  ../VME/albaScalarVector.cpp  						../VME/albaScalarVector.h
  ../VME/albaVMEExternalData.cpp  					../VME/albaVMEExternalData.h
  ../VME/albaVMEFactory.cpp  						../VME/albaVMEFactory.h
  ../VME/albaVMEGenericAbstract.cpp  				../VME/albaVMEGenericAbstract.h
  ../VME/albaVMEGroup.cpp  							../VME/albaVMEGroup.h
  ../VME/albaVMEItem.cpp  							../VME/albaVMEItem.h
  ../VME/albaVMEInfoText.cpp  						../VME/albaVMEInfoText.h
  ../VME/albaMSFImporter.cpp  						../VME/albaMSFImporter.h
  ../VME/albaVMEOutputScalar.cpp  					../VME/albaVMEOutputScalar.h
  ../VME/albaVMEScalar.cpp  						../VME/albaVMEScalar.h
  ../VME/albaVMELabeledVolume.cpp  					../VME/albaVMELabeledVolume.h
  ../VME/albaVMEPolylineGraph.cpp  					../VME/albaVMEPolylineGraph.h
  ../VME/albaVMEPolylineEditor.cpp  				../VME/albaVMEPolylineEditor.h
  ../VME/albaVMEOutputPolylineEditor.cpp  			../VME/albaVMEOutputPolylineEditor.h
  ../VME/albaVMEMaps.cpp  							../VME/albaVMEMaps.h  
  ../VME/albaVMEWrappedMeter.cpp  					../VME/albaVMEWrappedMeter.h
  ../VME/albaVMEOutputWrappedMeter.cpp  			../VME/albaVMEOutputWrappedMeter.h
  ../VME/albaVMESurfaceEditor.cpp  					../VME/albaVMESurfaceEditor.h
  ../VME/albaVMEOutputSurfaceEditor.cpp  			../VME/albaVMEOutputSurfaceEditor.h  
  ../VME/albaVMEComputeWrapping.cpp  				../VME/albaVMEComputeWrapping.h
  ../VME/albaVMEOutputComputeWrapping.cpp  			../VME/albaVMEOutputComputeWrapping.h   
  ../VME/albaVMESegmentationVolume.cpp  			../VME/albaVMESegmentationVolume.h
  ../VME/albaAttributeSegmentationVolume.cpp  		../VME/albaAttributeSegmentationVolume.h
	../VME/albaDataPipeCustomSegmentationVolume.cpp  	../VME/albaDataPipeCustomSegmentationVolume.h
	../VME/albaVMEPointCloud.cpp 					../VME/albaVMEPointCloud.h
)

IF (ALBA_USE_CRYPTO)
SET(VME_SRCS  ${VME_SRCS}
  ../VME/albaCrypt.cpp  							../VME/albaCrypt.h
  )

	ADD_DEFINITIONS(-DCRYPTOPP_DISABLE_ASM)
ENDIF (ALBA_USE_CRYPTO)

IF (ALBA_USE_ITK)
SET(VME_SRCS  ${VME_SRCS}
  ../VME/albaVMEOutputScalarMatrix.cpp  			../VME/albaVMEOutputScalarMatrix.h
  ../VME/albaVMEScalarMatrix.cpp  					../VME/albaVMEScalarMatrix.h
  ../VME/albaVMEItemScalarMatrix.cpp  				../VME/albaVMEItemScalarMatrix.h
  )
ENDIF (ALBA_USE_ITK)

IF (ALBA_USE_VTK)
  SET(VME_SRCS
  ${VME_SRCS}
  ../VME/albaVMEGeneric.cpp  						../VME/albaVMEGeneric.h
  ../VME/albaVMEGizmo.cpp  							../VME/albaVMEGizmo.h
  ../VME/albaVMEImage.cpp  							../VME/albaVMEImage.h
  ../VME/albaVMEItemVTK.cpp  						../VME/albaVMEItemVTK.h
  ../VME/albaVMELandmark.cpp  						../VME/albaVMELandmark.h
  ../VME/albaVMELandmarkCloud.cpp  					../VME/albaVMELandmarkCloud.h
  ../VME/albaVMEMesh.cpp  							../VME/albaVMEMesh.h
  ../VME/albaVMEMeter.cpp  							../VME/albaVMEMeter.h
  ../VME/albaVMEOutputImage.cpp  					../VME/albaVMEOutputImage.h
  ../VME/albaVMEOutputLandmarkCloud.cpp  			../VME/albaVMEOutputLandmarkCloud.h
  ../VME/albaVMEOutputMesh.cpp  					../VME/albaVMEOutputMesh.h
  ../VME/albaVMEOutputMeter.cpp  					../VME/albaVMEOutputMeter.h
  ../VME/albaVMEOutputPointSet.cpp  				../VME/albaVMEOutputPointSet.h
  ../VME/albaVMEOutputPolyline.cpp  				../VME/albaVMEOutputPolyline.h
  ../VME/albaVMEOutputSurface.cpp  					../VME/albaVMEOutputSurface.h
  ../VME/albaVMEOutputVolume.cpp  					../VME/albaVMEOutputVolume.h
  ../VME/albaVMEOutputVTK.cpp  						../VME/albaVMEOutputVTK.h
  ../VME/albaVMEPointSet.cpp  						../VME/albaVMEPointSet.h
  ../VME/albaVMEPolyline.cpp  						../VME/albaVMEPolyline.h
  ../VME/albaVMEPolylineSpline.cpp  				../VME/albaVMEPolylineSpline.h
  ../VME/albaVMEProber.cpp  						../VME/albaVMEProber.h
  ../VME/albaVMERefSys.cpp  						../VME/albaVMERefSys.h
  ../VME/albaVMESlicer.cpp  						../VME/albaVMESlicer.h
  ../VME/albaVMESurface.cpp  						../VME/albaVMESurface.h
  ../VME/albaVMESurfaceParametric.cpp  				../VME/albaVMESurfaceParametric.h
  ../VME/albaVMEVector.cpp  						../VME/albaVMEVector.h
  ../VME/albaVMEVolume.cpp  						../VME/albaVMEVolume.h
  ../VME/albaVMEVolumeGray.cpp  					../VME/albaVMEVolumeGray.h
  ../VME/albaVMEVolumeRGB.cpp  						../VME/albaVMEVolumeRGB.h
  ../VME/mmaMeter.cpp  							../VME/mmaMeter.h
  ../VME/albaVMERawMotionData.cpp  					../VME/albaVMERawMotionData.h
  ../VME/albaVMEAnalog.cpp  						../VME/albaVMEAnalog.h
  ../VME/albaVMEStent.cpp  							../VME/albaVMEStent.h
  #stent used tool, to find nearest point
  ../VME/kdtree.c  								../VME/kdtree.h
  )
ENDIF (ALBA_USE_VTK)

###############################################
#DATAPIPES
###############################################
SET(DATAPIPES_SRCS
  ../VME/DataPipes/albaDataPipeInterpolator.cpp
  ../VME/DataPipes/albaDataPipeInterpolator.h
  ../VME/DataPipes/albaDataPipeCustom.cpp
  ../VME/DataPipes/albaDataPipeCustom.h
  ../VME/DataPipes/albaDataPipeCustomProber.cpp
  ../VME/DataPipes/albaDataPipeCustomProber.h
  ../VME/DataPipes/albaDataPipeInterpolatorVTK.cpp
  ../VME/DataPipes/albaDataPipeInterpolatorVTK.h
  ../VME/DataPipes/vtkALBADataPipe.cpp
  ../VME/DataPipes/vtkALBADataPipe.h
)

IF (ALBA_USE_ITK)
  SET(DATAPIPES_SRCS
    ${DATAPIPES_SRCS}
    ../VME/DataPipes/albaDataPipeInterpolatorScalarMatrix.cpp
    ../VME/DataPipes/albaDataPipeInterpolatorScalarMatrix.h
  )
ENDIF (ALBA_USE_ITK)

###############################################
#VISUALPIPES
###############################################
# Set your list of sources here.
SET(VISUAL_PIPES_SRCS
  ../VisualPipes/albaPipeGenericPolydata.cpp 				../VisualPipes/albaPipeGenericPolydata.h
  ../VisualPipes/albaPipeMesh.cpp 							../VisualPipes/albaPipeMesh.h
  ../VisualPipes/albaPipeSurface.cpp 						../VisualPipes/albaPipeSurface.h
  ../VisualPipes/albaPipeRefSys.cpp 						../VisualPipes/albaPipeRefSys.h
  ../VisualPipes/albaPipeScalar.cpp  						../VisualPipes/albaPipeScalar.h
  ../VisualPipes/albaVisualPipeVolumeRayCasting.cpp  		../VisualPipes/albaVisualPipeVolumeRayCasting.h
  ../VisualPipes/albaPipeVolumeMIP.cpp  					../VisualPipes/albaPipeVolumeMIP.h
  ../VisualPipes/albaPipeDensityDistance.cpp  				../VisualPipes/albaPipeDensityDistance.h
  ../VisualPipes/albaPipeTrajectories.cpp  					../VisualPipes/albaPipeTrajectories.h
  ../VisualPipes/albaPipeVolumeDRR.cpp  					../VisualPipes/albaPipeVolumeDRR.h
  ../VisualPipes/albaPipeVolumeVR.cpp  						../VisualPipes/albaPipeVolumeVR.h
  ../VisualPipes/albaPipePolylineGraphEditor.cpp  			../VisualPipes/albaPipePolylineGraphEditor.h
  ../VisualPipes/albaPipeWrappedMeter.cpp  					../VisualPipes/albaPipeWrappedMeter.h
  ../VisualPipes/albaPipeSurfaceEditor.cpp  				../VisualPipes/albaPipeSurfaceEditor.h
  ../VisualPipes/albaVisualPipeSlicerSlice.cpp  			../VisualPipes/albaVisualPipeSlicerSlice.h  
  ../VisualPipes/albaVisualPipePolylineGraph.cpp  			../VisualPipes/albaVisualPipePolylineGraph.h
  ../VisualPipes/albaPipeComputeWrapping.cpp  				../VisualPipes/albaPipeComputeWrapping.h
  ../VisualPipes/albaPipeVectorField.cpp  					../VisualPipes/albaPipeVectorField.h
  ../VisualPipes/albaPipeVectorFieldMapWithArrows.cpp  		../VisualPipes/albaPipeVectorFieldMapWithArrows.h
  ../VisualPipes/albaPipeVectorFieldGlyphs.cpp  			../VisualPipes/albaPipeVectorFieldGlyphs.h
  ../VisualPipes/albaPipeVectorFieldSurface.cpp  			../VisualPipes/albaPipeVectorFieldSurface.h
  ../VisualPipes/albaPipeVectorFieldSlice.cpp  				../VisualPipes/albaPipeVectorFieldSlice.h
  ../VisualPipes/albaPipeTensorField.cpp  					../VisualPipes/albaPipeTensorField.h
  ../VisualPipes/albaPipeTensorFieldGlyphs.cpp  			../VisualPipes/albaPipeTensorFieldGlyphs.h
  ../VisualPipes/albaPipeTensorFieldSurface.cpp  			../VisualPipes/albaPipeTensorFieldSurface.h
  ../VisualPipes/albaPipeTensorFieldSlice.cpp  				../VisualPipes/albaPipeTensorFieldSlice.h
  ../VisualPipes/albaPipeCompound.cpp  						../VisualPipes/albaPipeCompound.h
  ../VisualPipes/albaPipeCompoundVolume.cpp  				../VisualPipes/albaPipeCompoundVolume.h
  ../VisualPipes/albaPipeCompoundVolumeFixedScalars.cpp  	../VisualPipes/albaPipeCompoundVolumeFixedScalars.h
  ../VisualPipes/albaPipeSlice.cpp  						../VisualPipes/albaPipeSlice.h
  ../VisualPipes/albaPipeMeshSlice.cpp  					../VisualPipes/albaPipeMeshSlice.h
  ../VisualPipes/albaPipePolylineSlice.cpp  				../VisualPipes/albaPipePolylineSlice.h
  ../VisualPipes/albaPipeSurfaceSlice.cpp  					../VisualPipes/albaPipeSurfaceSlice.h
	../VisualPipes/albaPipeVolumeOrthoSlice.cpp				../VisualPipes/albaPipeVolumeOrthoSlice.h
	../VisualPipes/albaPipeVolumeArbSlice.cpp				../VisualPipes/albaPipeVolumeArbSlice.h
  ../VisualPipes/albaPipeVolumeSliceBlend.cpp  				../VisualPipes/albaPipeVolumeSliceBlend.h
  ../VisualPipes/albaVisualPipeCollisionDetection.cpp  		../VisualPipes/albaVisualPipeCollisionDetection.h
  ../VisualPipes/albaPipeRayCast.cpp  						../VisualPipes/albaPipeRayCast.h	
  ../VisualPipes/albaPipeWithScalar.cpp 					../VisualPipes/albaPipeWithScalar.h
  ../VisualPipes/albaPipeVolumeArbOrthoSlice.cpp			../VisualPipes/albaPipeVolumeArbOrthoSlice.h
  ../VisualPipes/albaPipePointCloud.cpp					../VisualPipes/albaPipePointCloud.h
)


IF (ALBA_USE_ITK)
SET(VISUAL_PIPES_SRCS  ${VISUAL_PIPES_SRCS}
  ../VisualPipes/albaPipeScalarMatrix.cpp  					../VisualPipes/albaPipeScalarMatrix.h
  )
ENDIF (ALBA_USE_ITK)

IF (ALBA_USE_VTK)
  SET(VISUAL_PIPES_SRCS
  ${VISUAL_PIPES_SRCS}
  ../VisualPipes/albaPipeGizmo.cpp  						../VisualPipes/albaPipeGizmo.h
  ../VisualPipes/albaPipeImage3D.cpp  						../VisualPipes/albaPipeImage3D.h
  ../VisualPipes/albaPipeIsosurface.cpp  					../VisualPipes/albaPipeIsosurface.h
  ../VisualPipes/albaPipeLandmarkCloud.cpp  				../VisualPipes/albaPipeLandmarkCloud.h
  ../VisualPipes/albaPipeMeter.cpp  						../VisualPipes/albaPipeMeter.h
  ../VisualPipes/albaPipePointSet.cpp  						../VisualPipes/albaPipePointSet.h
  ../VisualPipes/albaPipePolyline.cpp  						../VisualPipes/albaPipePolyline.h
  ../VisualPipes/albaPipeSurfaceTextured.cpp  				../VisualPipes/albaPipeSurfaceTextured.h
  ../VisualPipes/albaPipeVector.cpp  						../VisualPipes/albaPipeVector.h
  ../VisualPipes/albaPipeVolumeProjected.cpp  				../VisualPipes/albaPipeVolumeProjected.h
  ../VisualPipes/albaPipeGraph.cpp  						../VisualPipes/albaPipeGraph.h
  )
ENDIF (ALBA_USE_VTK)

IF(WIN32)
  SET(VISUAL_PIPES_SRCS
  ${VISUAL_PIPES_SRCS}
  ../VisualPipes/albaPipeIsosurfaceGPU.cpp  				../VisualPipes/albaPipeIsosurfaceGPU.h
  )
ENDIF(WIN32)


###############################################
#OPERATIONS
###############################################
IF (ALBA_USE_VTK)
  SET(OPERATIONS_SRCS
	${OPERATIONS_SRCS}
	../Operations/albaOpDecomposeTimeVarVME.cpp	    ../Operations/albaOpDecomposeTimeVarVME.h
	../Operations/albaOpImporterMSF.cpp							../Operations/albaOpImporterMSF.h
	../Operations/albaOpImporterExternalFile.cpp	    ../Operations/albaOpImporterExternalFile.h
	../Operations/albaOpLabelExtractor.cpp	    			../Operations/albaOpLabelExtractor.h
	../Operations/albaOpOpenExternalFile.cpp	    		../Operations/albaOpOpenExternalFile.h
	../Operations/albaOp2DMeasure.cpp	    					../Operations/albaOp2DMeasure.h
	../Operations/albaOpAddLandmark.cpp	    				../Operations/albaOpAddLandmark.h
	../Operations/albaOpExporterBmp.cpp	    				../Operations/albaOpExporterBmp.h
	../Operations/albaOpClipSurface.cpp	    				../Operations/albaOpClipSurface.h
	../Operations/albaOpCreateGroup.cpp	    				../Operations/albaOpCreateGroup.h
	../Operations/albaOpCreateMeter.cpp	    				../Operations/albaOpCreateMeter.h
	../Operations/albaOpCreateSurfaceParametric.cpp	../Operations/albaOpCreateSurfaceParametric.h
	../Operations/albaOpCreateProber.cpp	    				../Operations/albaOpCreateProber.h
	../Operations/albaOpCreateRefSys.cpp	    				../Operations/albaOpCreateRefSys.h
	../Operations/albaOpCreateRefSysFromViewSlice.cpp	../Operations/albaOpCreateRefSysFromViewSlice.h
	../Operations/albaOpCreateSlicer.cpp	    				../Operations/albaOpCreateSlicer.h
	../Operations/albaOpCreateSpline.cpp	    				../Operations/albaOpCreateSpline.h
	../Operations/albaOpCreateVolume.cpp	    				../Operations/albaOpCreateVolume.h
	../Operations/albaOpCreateAverageLandmark.cpp		../Operations/albaOpCreateAverageLandmark.h
	../Operations/albaOpVolumeUnion.cpp	    				../Operations/albaOpVolumeUnion.h
	../Operations/albaOpRemoveCells.cpp	    				../Operations/albaOpRemoveCells.h
	../Operations/albaOpEditMetadata.cpp	    				../Operations/albaOpEditMetadata.h
	../Operations/albaOpExtractIsosurface.cpp	    	../Operations/albaOpExtractIsosurface.h
	../Operations/albaOpFilterSurface.cpp	    			../Operations/albaOpFilterSurface.h
	../Operations/albaOpDecimateSurface.cpp	    		../Operations/albaOpDecimateSurface.h
	../Operations/albaOpConnectivitySurface.cpp	    ../Operations/albaOpConnectivitySurface.h
	../Operations/albaOpFilterVolume.cpp	    				../Operations/albaOpFilterVolume.h
	../Operations/albaOpImporterImage.cpp	 		   		../Operations/albaOpImporterImage.h
	../Operations/albaOpALBATransform.cpp	   			 		../Operations/albaOpALBATransform.h
	../Operations/albaOpExporterMSF.cpp	   			 		../Operations/albaOpExporterMSF.h
	../Operations/albaOpImporterMSF1x.cpp	    			../Operations/albaOpImporterMSF1x.h
	../Operations/albaOpExporterRaw.cpp	    				../Operations/albaOpExporterRaw.h
	../Operations/albaOpImporterRAWVolume.cpp	    	../Operations/albaOpImporterRAWVolume.h
	../Operations/albaOpReparentTo.cpp	    					../Operations/albaOpReparentTo.h
	../Operations/albaOpExporterSTL.cpp	  		  		../Operations/albaOpExporterSTL.h
	../Operations/albaOpExporterPLY.cpp	  		  		../Operations/albaOpExporterPLY.h
	../Operations/albaOpImporterSTL.cpp	  		  		../Operations/albaOpImporterSTL.h
	../Operations/albaOpImporterPLY.cpp	  		  		../Operations/albaOpImporterPLY.h
	../Operations/albaOpTransformInterface.cpp	    	../Operations/albaOpTransformInterface.h
	../Operations/albaOpValidateTree.cpp	    				../Operations/albaOpValidateTree.h
	../Operations/albaOpVOIDensity.cpp	    					../Operations/albaOpVOIDensity.h
	../Operations/albaOpVOIDensityEditor.cpp		    	../Operations/albaOpVOIDensityEditor.h
	../Operations/albaOpImporterVRML.cpp	  		  		../Operations/albaOpImporterVRML.h
	../Operations/albaOpExporterVTK.cpp	  		  		../Operations/albaOpExporterVTK.h
	../Operations/albaOpImporterVTK.cpp	  		  		../Operations/albaOpImporterVTK.h
	../Operations/albaOpExporterMetaImage.cpp	  		../Operations/albaOpExporterMetaImage.h
	../Operations/albaOpImporterMetaImage.cpp	  		../Operations/albaOpImporterMetaImage.h

	../Operations/albaOpCrop.cpp	    								../Operations/albaOpCrop.h
	../Operations/albaOpBooleanSurface.cpp	    			../Operations/albaOpBooleanSurface.h
	../Operations/albaOpEditNormals.cpp	    				../Operations/albaOpEditNormals.h
	../Operations/albaOpApplyTrajectory.cpp	   		 	../Operations/albaOpApplyTrajectory.h
	../Operations/albaOpGarbageCollectMSFDir.cpp			../Operations/albaOpGarbageCollectMSFDir.h
	../Operations/albaOpVolumeMirror.cpp							../Operations/albaOpVolumeMirror.h
	###############################
	
	../Operations/albaOpImporterAnsysCommon.cpp	../Operations/albaOpImporterAnsysCommon.h	
	../Operations/albaOpImporterAnsysInputFile.cpp	../Operations/albaOpImporterAnsysInputFile.h
	../Operations/albaOpImporterAnsysCDBFile.cpp	../Operations/albaOpImporterAnsysCDBFile.h
	
	../Operations/albaOpExporterAnsysCommon.cpp	../Operations/albaOpExporterAnsysCommon.h
	../Operations/albaOpExporterAnsysInputFile.cpp	../Operations/albaOpExporterAnsysInputFile.h
	../Operations/albaOpExporterAnsysCDBFile.cpp	../Operations/albaOpExporterAnsysCDBFile.h
	
	../Operations/albaOpImporterAbaqusFile.cpp	../Operations/albaOpImporterAbaqusFile.h
	../Operations/albaOpExporterAbaqusFile.cpp	../Operations/albaOpExporterAbaqusFile.h
		
	../Operations/albaGeometryEditorPolylineGraph.cpp	../Operations/albaGeometryEditorPolylineGraph.h
	../Operations/albaOpCreateLabeledVolume.cpp	../Operations/albaOpCreateLabeledVolume.h
	../Operations/albaOpImporterLandmark.cpp		../Operations/albaOpImporterLandmark.h
	../Operations/albaOpImporterLandmarkTXT.cpp	../Operations/albaOpImporterLandmarkTXT.h
	../Operations/albaOpImporterLandmarkWS.cpp		../Operations/albaOpImporterLandmarkWS.h

	../Operations/albaOpMatrixVectorMath.cpp		../Operations/albaOpMatrixVectorMath.h

	../Operations/albaOpMML.cpp  ../Operations/albaOpMML.h
	../Operations/albaOpMMLContourWidget.cpp	../Operations/albaOpMMLContourWidget.h
	../Operations/albaOpMMLModelView.cpp		../Operations/albaOpMMLModelView.h
	../Operations/albaOpMMLParameterView.cpp	../Operations/albaOpMMLParameterView.h

	../Operations/albaOpMML3.cpp  ../Operations/albaOpMML3.h
	../Operations/albaOpMML3ContourWidget.cpp	../Operations/albaOpMML3ContourWidget.h
	../Operations/albaOpMML3GlobalRegistration.cpp	../Operations/albaOpMML3GlobalRegistration.h
	../Operations/albaOpMML3ModelView.cpp	../Operations/albaOpMML3ModelView.h
	../Operations/albaOpMML3ModelView2DPipe.cpp	../Operations/albaOpMML3ModelView2DPipe.h
	../Operations/albaOpMML3ModelView3DPipe.cpp	../Operations/albaOpMML3ModelView3DPipe.h
	../Operations/albaOpMML3NonUniformSlicePipe.cpp	../Operations/albaOpMML3NonUniformSlicePipe.h
	../Operations/albaOpMML3ParameterView.cpp	../Operations/albaOpMML3ParameterView.h

	../Operations/albaOpRegisterClusters.cpp	../Operations/albaOpRegisterClusters.h
	../Operations/albaOpVolumeMeasure.cpp		../Operations/albaOpVolumeMeasure.h
	../Operations/albaOpImporterRAWImages.cpp	../Operations/albaOpImporterRAWImages.h
	../Operations/albaOpMeshQuality.cpp		../Operations/albaOpMeshQuality.h
	../Operations/albaOpCropDeformableROI.cpp	../Operations/albaOpCropDeformableROI.h
	../Operations/albaOpFlipNormals.cpp		../Operations/albaOpFlipNormals.h
	../Operations/albaOpExtrusionHoles.cpp		../Operations/albaOpExtrusionHoles.h
	../Operations/albaOpVolumeResample.cpp		../Operations/albaOpVolumeResample.h
	../Operations/albaOpSplitSurface.cpp		../Operations/albaOpSplitSurface.h
	../Operations/albaOpInteractiveClipSurface.cpp	../Operations/albaOpInteractiveClipSurface.h
	../Operations/albaOpCreateEditSkeleton.cpp	../Operations/albaOpCreateEditSkeleton.h
	../Operations/albaOpCreateSurface.cpp		../Operations/albaOpCreateSurface.h
	../Operations/albaOpTransform.cpp			../Operations/albaOpTransform.h
	../Operations/albaOpMove.cpp				../Operations/albaOpMove.h
	../Operations/albaOpScaleDataset.cpp		../Operations/albaOpScaleDataset.h
	../Operations/albaOpCreateWrappedMeter.cpp

	../Operations/albaOpSegmentation.cpp  ../Operations/albaOpSegmentation.h
	../Operations/albaOpSegmentationHelper.cpp  ../Operations/albaOpSegmentationHelper.h


	../Operations/albaOpExtractGeometry.cpp  ../Operations/albaOpExtractGeometry.h  
	../Operations/albaOpCreateWrappedMeter.cpp	../Operations/albaOpCreateWrappedMeter.h  
	../Operations/albaOpComputeWrapping.cpp	../Operations/albaOpComputeWrapping.h    
	../Operations/albaOpComputeInertialTensor.cpp	../Operations/albaOpComputeInertialTensor.h    
	../Operations/albaOpExporterWrappedMeter.cpp	../Operations/albaOpExporterWrappedMeter.h
	../Operations/albaOpLabelizeSurface.cpp  ../Operations/albaOpLabelizeSurface.h
	../Operations/albaOpFreezeVME.cpp  ../Operations/albaOpFreezeVME.h
  
	../Operations/albaOpIterativeRegistration.cpp  ../Operations/albaOpIterativeRegistration.h
  
	../Operations/albaOpSmoothSurface.cpp  ../Operations/albaOpSmoothSurface.h  
	../Operations/albaOpCleanSurface.cpp  ../Operations/albaOpCleanSurface.h  
	../Operations/albaOpTriangulateSurface.cpp  ../Operations/albaOpTriangulateSurface.h

	../Operations/albaOpSurfaceMirror.cpp  ../Operations/albaOpSurfaceMirror.h  
	../Operations/albaOpImporterVTKXML.cpp  ../Operations/albaOpImporterVTKXML.h  
	../Operations/albaOpExporterVTKXML.cpp  ../Operations/albaOpExporterVTKXML.h
	../Operations/albaOpSubdivide.cpp  ../Operations/albaOpSubdivide.h
    
	../Operations/albaOpFillHoles.cpp  ../Operations/albaOpFillHoles.h
	../Operations/albaOpMeshDeformation.cpp  ../Operations/albaOpMeshDeformation.h
  
	../Operations/albaOpMakeVMETimevarying.cpp  ../Operations/albaOpMakeVMETimevarying.h
	../Operations/albaOpSegmentationRegionGrowingConnectedThreshold.cpp  ../Operations/albaOpSegmentationRegionGrowingConnectedThreshold.h
	../Operations/albaOpSegmentationRegionGrowingLocalAndGlobalThreshold.cpp	../Operations/albaOpSegmentationRegionGrowingLocalAndGlobalThreshold.h
	  
	../Operations/albaOpEqualizeHistogram.cpp  ../Operations/albaOpEqualizeHistogram.h
	  
	../Operations/albaOpExporterAnalogWS.cpp		../Operations/albaOpExporterAnalogWS.h
	../Operations/albaOpExporterGRFWS.cpp		../Operations/albaOpExporterGRFWS.h
	../Operations/albaOpExporterLandmarkWS.cpp	../Operations/albaOpExporterLandmarkWS.h
	  
	../Operations/albaOpExporterFEMCommon.cpp	../Operations/albaOpExporterFEMCommon.h
	
	../Operations/albaOpESPCalibration.cpp		../Operations/albaOpESPCalibration.h
	
	../Operations/albaOpExtractImageFromArbitraryView.cpp		../Operations/albaOpExtractImageFromArbitraryView.h
	../Operations/albaOpImporterDicFile.cpp		../Operations/albaOpImporterDicFile.h
	../Operations/albaOpImporterPointCloud.cpp	../Operations/albaOpImporterPointCloud.h
  )
ENDIF (ALBA_USE_VTK)

IF (ALBA_USE_GDCM)
SET(OPERATIONS_SRCS ${OPERATIONS_SRCS}
	../Operations/albaOpExporterDicom.cpp ../Operations/albaOpExporterDicom.h
    ../Operations/albaOpImporterDicom.cpp	../Operations/albaOpImporterDicom.h
		../Operations/albaOpImporterDicomSliceAccHelper.cpp ../Operations/albaOpImporterDicomSliceAccHelper.h
		../Operations/albaDicomTagDefines.h
    )
ENDIF (ALBA_USE_GDCM)

IF(ALBA_USE_BTK)
	 SET(OPERATIONS_SRCS ${OPERATIONS_SRCS} ../Operations/albaOpImporterC3D.cpp			../Operations/albaOpImporterC3D.h)
ENDIF(ALBA_USE_BTK)

IF (ALBA_USE_ITK)
  SET(OPERATIONS_SRCS
    ${OPERATIONS_SRCS}
    ../Operations/albaASCIIImporterUtility.cpp			../Operations/albaASCIIImporterUtility.h
    ../Operations/albaOpImporterASCII.cpp				../Operations/albaOpImporterASCII.h
    ../Operations/albaVMEDataSetAttributesImporter.cpp	../Operations/albaVMEDataSetAttributesImporter.h
    ../Operations/albaVMEMeshAnsysTextImporter.cpp		../Operations/albaVMEMeshAnsysTextImporter.h
    ../Operations/albaVMEMeshAnsysTextExporter.cpp		../Operations/albaVMEMeshAnsysTextExporter.h
    ../Operations/albaParabolicMeshToLinearMeshFilter.cxx	../Operations/albaParabolicMeshToLinearMeshFilter.h
    ../Operations/albaOpImporterMesh.cpp				../Operations/albaOpImporterMesh.h
    ../Operations/albaOpExporterMesh.cpp				../Operations/albaOpExporterMesh.h
    ../Operations/albaOpScalarToSurface.cpp			../Operations/albaOpScalarToSurface.h
    ../Operations/albaOpImporterVMEDataSetAttributes.cpp	../Operations/albaOpImporterVMEDataSetAttributes.h
	
	##########################################
	# itkRawMotionImporterUtility.cxx
    # itkRawMotionImporterUtility.h
    ../Operations/albaOpImporterMotionData.h
    ../Operations/albaOpExporterLandmark.cpp		../Operations/albaOpExporterLandmark.h
    ../Operations/albaOpClassicICPRegistration.cpp	../Operations/albaOpClassicICPRegistration.h
    ../Operations/albaOpImporterGRFWS.cpp			../Operations/albaOpImporterGRFWS.h
    ../Operations/albaOpImporterAnalogWS.cpp		../Operations/albaOpImporterAnalogWS.h
    ../Operations/albaOpSmoothSurfaceCells.cpp		../Operations/albaOpSmoothSurfaceCells.h
    ../Operations/albaOpExporterMeters.cpp			../Operations/albaOpExporterMeters.h
    ../Operations/albaOpInteractionDebugger.cpp		../Operations/albaOpInteractionDebugger.h
  )
ENDIF (ALBA_USE_ITK)

###############################################
#VIEWS
###############################################
SET(VIEWS_SRCS
  
  ../Views/albaViewArbitrarySlice.cpp				../Views/albaViewArbitrarySlice.h
  ../Views/albaViewOrthoSlice.cpp						../Views/albaViewOrthoSlice.h
  ../Views/albaViewRX.cpp										../Views/albaViewRX.h
  ../Views/albaViewRXCT.cpp									../Views/albaViewRXCT.h
  ../Views/albaViewSlice.cpp								../Views/albaViewSlice.h
  ../Views/albaViewGlobalSlice.cpp					../Views/albaViewGlobalSlice.h
  ../Views/albaViewGlobalSliceCompound.cpp	../Views/albaViewGlobalSliceCompound.h
  ../Views/albaViewImage.cpp								../Views/albaViewImage.h
  ../Views/albaViewRXCompound.cpp						../Views/albaViewRXCompound.h
  ../Views/albaView3D.cpp										../Views/albaView3D.h
  ../Views/albaViewImageCompound.cpp				../Views/albaViewImageCompound.h
  ../Views/albaViewCT.cpp										../Views/albaViewCT.h
  ../Views/albaViewCompoundWindowing.cpp		../Views/albaViewCompoundWindowing.h
  ../Views/albaViewVTKCompound.cpp					../Views/albaViewVTKCompound.h
  ../Views/albaViewSlicer.cpp								../Views/albaViewSlicer.h
  ../Views/albaViewSliceOnCurve.cpp					../Views/albaViewSliceOnCurve.h
  ../Views/albaViewSliceOnCurveCompound.cpp	../Views/albaViewSliceOnCurveCompound.h
  ../Views/albaViewSliceGlobal.cpp					../Views/albaViewSliceGlobal.h
  ../Views/albaViewArbitraryOrthoSlice.cpp	../Views/albaViewArbitraryOrthoSlice.h
  ../Views/albaViewSliceBlendRX.cpp					../Views/albaViewSliceBlendRX.h
  ../Views/albaViewSliceBlend.cpp						../Views/albaViewSliceBlend.h
  ../Views/albaViewSliceSegmentation.cpp		../Views/albaViewSliceSegmentation.h
  ../Views/albaViewIsosurfaceCompound.cpp		../Views/albaViewIsosurfaceCompound.h
)



###############################################
#DRIVERS
###############################################
IF (WIN32)
SET (DRIVERS_SRCS
# Intersense driver
  ../Interaction/Drivers/isdriver.c
  ../Interaction/Drivers/isense.c
  ../Interaction/Drivers/itcom.c
  ../Interaction/Drivers/serial.c
  ../Interaction/Drivers/timer.c
  ../Interaction/Drivers/isdriver.h
  ../Interaction/Drivers/isense.h
  ../Interaction/Drivers/itcom.h
  ../Interaction/Drivers/serial.h
  ../Interaction/Drivers/timer.h
)
ENDIF (WIN32)

IF (WIN32)
SET (DRIVERS_SRCS
  ${DRIVERS_SRCS}
  # Vicon driver
  ../Interaction/Drivers/vicon.cpp
  ../Interaction/Drivers/vicon.h
  )
ENDIF (WIN32)

# P5 driver
IF (ALBA_USE_P5_GLOVE)
  SET (DRIVERS_SRCS 
  ${DRIVERS_SRCS}
  ../Interaction/Drivers/P5Bend.cpp
  ../Interaction/Drivers/P5Bend.h
  ../Interaction/Drivers/P5Motion.cpp
  ../Interaction/Drivers/P5Motion.h
  ../Interaction/Drivers/P5dll.h
  )
ENDIF (ALBA_USE_P5_GLOVE)

IF (ALBA_USE_WIIMOTE)
  SET (DRIVERS_SRCS 
  ${DRIVERS_SRCS}
  ../Interaction/Drivers/defines.h
  ../Interaction/Drivers/Console.h
  ../Interaction/Drivers/Console.cpp
  ../Interaction/Drivers/HIDDevice.h
  ../Interaction/Drivers/HIDDevice.cpp
  ../Interaction/Drivers/hidpi.h
  ../Interaction/Drivers/hidsdi.h
  ../Interaction/Drivers/hidusage.h
  ../Interaction/Drivers/setupapi.h
  ../Interaction/Drivers/Utils.h
  ../Interaction/Drivers/Wiimote.h
  ../Interaction/Drivers/Wiimote.cpp
  )
ENDIF (ALBA_USE_WIIMOTE)

##############################################################################################
##############################################################################################
##############################################################################################
SET(PROJECT_SRCS
  ${BASE_SRCS}
  ${CORE_SRCS}
  ${GUI_SRCS}
  ${COMMON_SRCS}
  ${SERVICE_SRCS}
  ${INTERACTION_SRCS}
  ${IO_SRCS}
  ${VME_SRCS}
  ${VIEWS_SRCS}
  ${VISUAL_PIPES_SRCS}
  ${DATAPIPES_SRCS}
  ${OPERATIONS_SRCS}
  #${DRIVERS_SRCS}
)

#################################
# Add the source file to the project
# FILE(GLOB h_to_add "${PROJECT_SOURCE_DIR}/*.h")
# FILE(GLOB cpp_to_add "${PROJECT_SOURCE_DIR}/*.cpp")
# FILE(GLOB cxx_to_add "${PROJECT_SOURCE_DIR}/*.cxx")
# FILE(GLOB txx_to_add "${PROJECT_SOURCE_DIR}/*.txx")
# FILE(GLOB c_to_add "${PROJECT_SOURCE_DIR}/*.cpp")

# FOREACH(f ${h_to_add})
  # FILE(RELATIVE_PATH file_name "${PROJECT_SOURCE_DIR}" "${f}")
  # SET(PROJECT_SRCS
  # ${PROJECT_SRCS}
  # ${file_name}
  # )
# ENDFOREACH(f ${h_to_add})

# FOREACH(f ${cpp_to_add})
  # FILE(RELATIVE_PATH file_name "${PROJECT_SOURCE_DIR}" "${f}")
  # SET(PROJECT_SRCS
  # ${PROJECT_SRCS}
  # ${file_name}
  # )
# ENDFOREACH(f ${cpp_to_add})

# FOREACH(f ${cxx_to_add})
  # FILE(RELATIVE_PATH file_name "${PROJECT_SOURCE_DIR}" "${f}")
  # SET(PROJECT_SRCS
  # ${PROJECT_SRCS}
  # ${file_name}
  # )
# ENDFOREACH(f ${cxx_to_add})

# FOREACH(f ${txx_to_add})
  # FILE(RELATIVE_PATH file_name "${PROJECT_SOURCE_DIR}" "${f}")
  # SET(PROJECT_SRCS
  # ${PROJECT_SRCS}
  # ${file_name}
  # )
# ENDFOREACH(f ${txx_to_add})

# FOREACH(f ${c_to_add})
  # FILE(RELATIVE_PATH file_name "${PROJECT_SOURCE_DIR}" "${f}")
  # SET(PROJECT_SRCS
  # ${PROJECT_SRCS}
  # ${file_name}
  # )
# ENDFOREACH(f ${c_to_add})
#################################

SET(BUILD_SHARED_LIBS 1)

# List libraries that are needed by this project.
# SET(PROJECT_LIBS curl)

IF (NOT WIN32)
  INCLUDE (${ALBA_SOURCE_DIR}/modules/FindGTK2.cmake)
  INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIRECTORIES})
ENDIF (NOT WIN32)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

IF (ALBA_USE_VTK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
ENDIF (ALBA_USE_VTK)

IF (ALBA_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (ALBA_USE_WX)

IF (ALBA_USE_CRYPTO)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_CRYPTO_LIBRARY)
ENDIF (ALBA_USE_CRYPTO)

IF (ALBA_USE_CURL)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_CURL_LIBRARY)
ENDIF (ALBA_USE_CURL)

IF (ALBA_USE_ITK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_ITK_LIBRARY)
ENDIF (ALBA_USE_ITK)

IF (ALBA_USE_XercesC)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_XercesC_LIBRARY)
ENDIF (ALBA_USE_XercesC)

IF (ALBA_USE_BTK)
 ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_BTK_LIBRARY)
ENDIF (ALBA_USE_BTK)

IF (ALBA_USE_GDCM)
 ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_GDCM_LIBRARY)
ENDIF (ALBA_USE_GDCM)
