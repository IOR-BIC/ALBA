#
# Copyright and description to be placed here
#

PROJECT(mafDLL)

DBG_MESSAGE("Configuring project mafDLL")

ADD_DEFINITIONS(-DMAF_EXPORTS)

LINK_LIBRARIES(glu32 opengl32 vtkHybrid vtkIO vtkRendering vtkGraphics vtkFiltering vtkImaging vtkCommon vtkftgl vtkfreetype XercesC curl Crypto vtkMAF GPUAPI)

###############################################
#BASE
###############################################
SET(BASE_SRCS
  ../Base/mafBase.h
  ../Base/mafDecl.cpp
  ../Base/mafDecl.h
  ../Base/mafDefines.cpp
  ../Base/mafDefines.h
  ../Base/mafDirectory.cpp
  ../Base/mafDirectory.h
  ../Base/mafDynamicLoader.cpp
  ../Base/mafDynamicLoader.h
  ../Base/mafEventBase.cpp
  ../Base/mafEventBase.h
  ../Base/mafEventSender.h
  ../Base/mafEventSource.cpp
  ../Base/mafEventSource.h
  ../Base/mafIncludeWIN32.h
  ../Base/mafIncludeWX.h
  ../Base/mafIndent.cpp
  ../Base/mafIndent.h
  ../Base/mafLUTLibrary.cpp
  ../Base/mafLUTLibrary.h
  ../Base/mafMTime.cpp
  ../Base/mafMTime.h
  ../Base/mafMatrix.cpp
  ../Base/mafMatrix.h
  ../Base/mafMatrix3x3.cpp
  ../Base/mafMatrix3x3.h
  ../Base/mafMultiThreader.h
  ../Base/mafMultiThreader.cpp
  ../Base/mafMutexLock.cpp
  ../Base/mafMutexLock.h
  ../Base/mafOBB.cpp
  ../Base/mafOBB.h
  ../Base/mafObject.cpp
  ../Base/mafObject.h
  ../Base/mafObjectFactory.cpp
  ../Base/mafObjectFactory.h
  ../Base/mafObserver.cpp
  ../Base/mafObserver.h
  ../Base/mafObserverCallback.cpp
  ../Base/mafObserverCallback.h
  ../Base/mafReferenceCounted.cpp
  ../Base/mafReferenceCounted.h
  ../Base/mafSmartPointer.h
  ../Base/mafString.cpp
  ../Base/mafString.h
  ../Base/mafTimeStamped.cpp
  ../Base/mafTimeStamped.h
  ../Base/mafTransform.cpp
  ../Base/mafTransform.h
  ../Base/mafTransformBase.cpp
  ../Base/mafTransformBase.h
  ../Base/mafTransformFrame.cpp
  ../Base/mafTransformFrame.h
  ../Base/mafVector.h
  ../Base/mafVector.txx
  ../Base/mafVersion.h
  ../Base/mmuIdFactory.cpp
  ../Base/mmuIdFactory.h
  ../Base/mmuUtility.h
  ../Base/vtkMAFSmartPointer.h
  ../Base/vtkMAFToLinearTransform.h
  ../Base/mafZipUtility.cpp
  ../Base/mafZipUtility.h
)
IF (MAF_USE_VTK)
  SET(BASE_SRCS
    ${BASE_SRCS}
      ../Base/vtkMAFToLinearTransform.cxx
      ../Base/vtkMAFToLinearTransform.h
      ../Base/vtkMAFSmartPointer.h
  )
ENDIF (MAF_USE_VTK)
###############################################
#CORE
###############################################
SET(CORE_SRCS
  ../Core/mafAbsMatrixPipe.cpp
  ../Core/mafAbsMatrixPipe.h
  ../Core/mafAttribute.cpp
  ../Core/mafAttribute.h
  ../Core/mafAttributeTraceability.cpp
  ../Core/mafAttributeTraceability.h
  ../Core/mafCoreFactory.cpp
  ../Core/mafCoreFactory.h
  ../Core/mafDataPipe.cpp
  ../Core/mafDataPipe.h
  ../Core/mafExpiration.h
  ../Core/mafExpirationDate.cpp
  ../Core/mafExpirationDate.h
  ../Core/mafEventIO.cpp
  ../Core/mafEventIO.h
  ../Core/mafLogicWithGUI.cpp
  ../Core/mafLogicWithGUI.h
  ../Core/mafMatrixPipe.cpp
  ../Core/mafMatrixPipe.h
  ../Core/mafNode.cpp
  ../Core/mafNode.h
  ../Core/mafNodeFactory.cpp
  ../Core/mafNodeFactory.h
  ../Core/mafNodeGeneric.cpp
  ../Core/mafNodeGeneric.h
  ../Core/mafNodeIterator.cpp
  ../Core/mafNodeIterator.h
  ../Core/mafNodeLayout.cpp
  ../Core/mafNodeLayout.h
  ../Core/mafNodeManager.cpp
  ../Core/mafNodeManager.h
  ../Core/mafNodeRoot.cpp
  ../Core/mafNodeRoot.h
  ../Core/mafOp.cpp
  ../Core/mafOp.h
  ../Core/mafOpContextStack.cpp
  ../Core/mafOpContextStack.h
  ../Core/mafOpManager.cpp
  ../Core/mafOpManager.h
  ../Core/mafOpSelect.cpp
  ../Core/mafOpSelect.h
  ../Core/mafOpStack.cpp
  ../Core/mafOpStack.h
  ../Core/mafPrintSupport.cpp
  ../Core/mafPrintSupport.h
  ../Core/mmdRemoteFileManager.cpp
  ../Core/mmdRemoteFileManager.h
  ../Core/mafRemoteLogic.cpp
  ../Core/mafRemoteLogic.h
  ../Core/mafRemoteStorage.cpp
  ../Core/mafRemoteStorage.h
  ../Core/mafRoot.cpp
  ../Core/mafRoot.h
  ../Core/mafTagArray.cpp
  ../Core/mafTagArray.h
  ../Core/mafTagItem.cpp
  ../Core/mafTagItem.h
  ../Core/mafTimeMap.h
  ../Core/mafTimeMapScalar.h
  ../Core/mafUser.cpp
  ../Core/mafUser.h
  ../Core/mafVME.cpp
  ../Core/mafVME.h
  ../Core/mafVMEManager.cpp
  ../Core/mafVMEManager.h
  ../Core/mafVMEOutput.cpp
  ../Core/mafVMEOutput.h
  ../Core/mafVMEOutputNULL.cpp
  ../Core/mafVMEOutputNULL.h
  ../Core/mafVMERoot.cpp
  ../Core/mafVMERoot.h
  ../Core/mafVMEStorage.cpp
  ../Core/mafVMEStorage.h
  ../Core/mmaApplicationLayout.cpp
  ../Core/mmaApplicationLayout.h
  ../Core/mafGUIListCtrlBitmap.cpp
  ../Core/mafGUIListCtrlBitmap.h
  ../Core/mafGUIVMEChooser.cpp
  ../Core/mafGUIVMEChooser.h
  ../Core/mafGUIVMEChooserTree.cpp
  ../Core/mafGUIVMEChooserTree.h
  ../Core/mmuTimeSet.cpp
  ../Core/mmuTimeSet.h
)

IF (MAF_USE_VTK)
  SET(CORE_SRCS
    ${CORE_SRCS}
    ../Core/mafAxes.cpp
    ../Core/mafAxes.h
    ../Core/mafLODActor.cpp
    ../Core/mafLODActor.h
    ../Core/mafLogicWithManagers.cpp
    ../Core/mafLogicWithManagers.h
    ../Core/mafPipe.cpp
    ../Core/mafPipe.h
    ../Core/mafPipeBox.cpp
    ../Core/mafPipeBox.h
    ../Core/mafPipeFactory.cpp
    ../Core/mafPipeFactory.h
    ../Core/mafRWI.cpp
    ../Core/mafRWI.h
    ../Core/mafRWIBase.cpp
    ../Core/mafRWIBase.h
    ../Core/mafSceneGraph.cpp
    ../Core/mafSceneGraph.h
    ../Core/mafSceneNode.cpp
    ../Core/mafSceneNode.h
    ../Core/mafViewCompound.cpp
    ../Core/mafViewCompound.h
    ../Core/mafViewHTML.cpp
    ../Core/mafViewHTML.h
    ../Core/mafViewManager.cpp
    ../Core/mafViewManager.h
    ../Core/mafViewPlot.cpp
    ../Core/mafViewPlot.h
    ../Core/mafViewVTK.cpp
    ../Core/mafViewVTK.h
    ../Core/mmaMaterial.cpp
    ../Core/mmaMaterial.h
    ../Core/mmaVolumeMaterial.cpp
    ../Core/mmaVolumeMaterial.h
    ../Core/mafGUIContextualMenu.cpp
    ../Core/mafGUIContextualMenu.h
    ../Core/mafGUIMaterialButton.cpp
    ../Core/mafGUIMaterialButton.h
    ../Core/mafGUIMaterialChooser.cpp
    ../Core/mafGUIMaterialChooser.h
    ../Core/mafGUITreeContextualMenu.cpp
    ../Core/mafGUITreeContextualMenu.h
    ../Core/vtkMAFAssembly.cpp
    ../Core/vtkMAFAssembly.h
  )
ENDIF (MAF_USE_VTK)
###############################################
#GUI
###############################################
SET(GUI_SRCS
  ../Gui/mafEvent.cpp
  ../Gui/mafEvent.h
  ../Gui/mafGUIDialogFindVme.cpp
  ../Gui/mafGUIDialogFindVme.h
  ../Gui/mafGUIDialogLogin.cpp
  ../Gui/mafGUIDialogLogin.h
  ../Gui/mafGUIDialogTransferFunction2D.cpp
  ../Gui/mafGUIDialogTransferFunction2D.h
  ../Gui/mafTransferFunction2DInteractors.cpp
  ../Gui/mafGUISettings.cpp
  ../Gui/mafGUISettings.h
  ../Gui/mafGUISettingsStorage.cpp
  ../Gui/mafGUISettingsStorage.h
  ../Gui/mafGUISettingsTimeBar.cpp
  ../Gui/mafGUISettingsTimeBar.h
  ../Gui/mafPics.cpp
  ../Gui/mafPics.h
  ../Gui/mafWXLog.cpp
  ../Gui/mafWXLog.h
  ../Gui/mafGUIApplicationLayoutSettings.cpp
  ../Gui/mafGUIApplicationLayoutSettings.h
  ../Gui/mafGUIApplicationSettings.cpp
  ../Gui/mafGUIApplicationSettings.h
  ../Gui/mafGUIButton.cpp
  ../Gui/mafGUIButton.h
  ../Gui/mafGUICheckListBox.cpp
  ../Gui/mafGUICheckListBox.h
  ../Gui/mafGUICheckTree.cpp
  ../Gui/mafGUICheckTree.h
  ../Gui/mafGUIColorSwatch.cpp
  ../Gui/mafGUIColorSwatch.h
  ../Gui/mafGUIColorWidget.cpp
  ../Gui/mafGUIColorWidget.h
  ../Gui/mafGUICrossSplitter.cpp
  ../Gui/mafGUICrossSplitter.h
  ../Gui/mafGUIDialog.cpp
  ../Gui/mafGUIDialog.h
  ../Gui/mafGUIDockManager.cpp
  ../Gui/mafGUIDockManager.h
  ../Gui/mafGUIDockSettings.cpp
  ../Gui/mafGUIDockSettings.h
  ../Gui/mafGUIFileHistory.cpp
  ../Gui/mafGUIFileHistory.h
  ../Gui/mafGUIFlagSplitter.cpp
  ../Gui/mafGUIFlagSplitter.h
  ../Gui/mafGUIFloatSlider.cpp
  ../Gui/mafGUIFloatSlider.h
  ../Gui/mafGUIFrame.cpp
  ../Gui/mafGUIFrame.h
  ../Gui/mafGUI.cpp
  ../Gui/mafGUI.h
  ../Gui/mafGUIHolder.cpp
  ../Gui/mafGUIHolder.h
  ../Gui/mafGUILab.cpp
  ../Gui/mafGUILab.h
  ../Gui/mafGUIListBox.cpp
  ../Gui/mafGUIListBox.h
  ../Gui/mafGUIListCtrl.cpp
  ../Gui/mafGUIListCtrl.h
  ../Gui/mafGUILocaleSettings.cpp
  ../Gui/mafGUILocaleSettings.h
  ../Gui/mafGUIMeasureUnitSettings.cpp
  ../Gui/mafGUIMeasureUnitSettings.h
  ../Gui/mafGUIMovieCtrl.cpp
  ../Gui/mafGUIMovieCtrl.h
  ../Gui/mafGUIMDIChild.cpp
  ../Gui/mafGUIMDIChild.h
  ../Gui/mafGUIMDIFrame.cpp
  ../Gui/mafGUIMDIFrame.h
  ../Gui/mafGUIMutexPanel.cpp
  ../Gui/mafGUIMutexPanel.h
  ../Gui/mafGUINamedPanel.cpp
  ../Gui/mafGUINamedPanel.h
  ../Gui/mafGUIPanel.cpp
  ../Gui/mafGUIPanel.h
  ../Gui/mafGUIPanelStack.cpp
  ../Gui/mafGUIPanelStack.h
  ../Gui/mafGUIPicButton.cpp
  ../Gui/mafGUIPicButton.h
  ../Gui/mafGUIRangeSlider.cpp
  ../Gui/mafGUIRangeSlider.h
  ../Gui/mafGUIRollOut.cpp
  ../Gui/mafGUIRollOut.h
  ../Gui/mafGUISashPanel.h
  ../Gui/mafGUISashPanel.cpp
  ../Gui/mafGUIScrolledPanel.cpp
  ../Gui/mafGUIScrolledPanel.h
  ../Gui/mafGUISettingsDialog.cpp
  ../Gui/mafGUISettingsDialog.h
  ../Gui/mafGUISplittedPanel.cpp
  ../Gui/mafGUISplittedPanel.h
  ../Gui/mafGUITimeBar.cpp
  ../Gui/mafGUITimeBar.h
  ../Gui/mafGUITree.cpp
  ../Gui/mafGUITree.h
  ../Gui/mafGUIValidator.cpp
  ../Gui/mafGUIValidator.h
  ../Gui/mafGUIViewFrame.cpp
  ../Gui/mafGUIViewFrame.h
  ../Gui/mafGUIViewWin.cpp
  ../Gui/mafGUIViewWin.h
  ../Gui/mafGUICrossIncremental.cpp
  ../Gui/mafGUICrossIncremental.h
  ../Gui/mafGUIComboBox.cpp
  ../Gui/mafGUIComboBox.h
)

IF (MAF_USE_CURL)
	SET(GUI_SRCS
	  ${GUI_SRCS}
	  ../Gui/mafGUIDialogRemoteFile.cpp
	  ../Gui/mafGUIDialogRemoteFile.h
	)
ENDIF (MAF_USE_CURL)

IF (MAF_USE_VTK)
  SET(GUI_SRCS
    ${GUI_SRCS}
    ../Gui/mafLightKit.cpp
    ../Gui/mafLightKit.h
    ../Gui/mafGUIDialogPreview.cpp
    ../Gui/mafGUIDialogPreview.h
    ../Gui/mafGUIDictionaryWidget.cpp
    ../Gui/mafGUIDictionaryWidget.h
    ../Gui/mafAnimate.cpp
    ../Gui/mafAnimate.h
    ../Gui/mafAttachCamera.cpp
    ../Gui/mafAttachCamera.h
    ../Gui/mafSideBar.cpp
    ../Gui/mafSideBar.h
    ../Gui/mafTextKit.cpp
    ../Gui/mafTextKit.h
    ../Gui/mafVTKLog.cpp
    ../Gui/mafVTKLog.h
    ../Gui/mafView.cpp
    ../Gui/mafView.h
    ../Gui/mafColor.cpp
    ../Gui/mafColor.h
    ../Gui/mafGUILutEditor.cpp
    ../Gui/mafGUILutEditor.h
    ../Gui/mafGUILutPreset.cpp
    ../Gui/mafGUILutPreset.h
    ../Gui/mafGUILutSlider.cpp
    ../Gui/mafGUILutSlider.h
    ../Gui/mafGUILutSwatch.cpp
    ../Gui/mafGUILutSwatch.h
    ../Gui/mafGUILutWidget.cpp
    ../Gui/mafGUILutWidget.h
    ../Gui/mafGUIHistogramWidget.cpp
    ../Gui/mafGUIHistogramWidget.h
  )
ENDIF (MAF_USE_VTK)
###############################################
#INTERACTION
###############################################
SET(INTERACTION_SRCS
  ../Interaction/mafAction.cpp
  ../Interaction/mafAction.h
  ../Interaction/mafAgent.cpp
  ../Interaction/mafAgent.h
  ../Interaction/mafAgentEventHandler.cpp
  ../Interaction/mafAgentEventHandler.h
  ../Interaction/mafAgentEventQueue.cpp
  ../Interaction/mafAgentEventQueue.h
  ../Interaction/mafAgentThreaded.cpp
  ../Interaction/mafAgentThreaded.h
  ../Interaction/mafAvatar.cpp
  ../Interaction/mafAvatar.h
  ../Interaction/mafAvatar2D.cpp
  ../Interaction/mafAvatar2D.h
  ../Interaction/mafAvatar3D.cpp
  ../Interaction/mafAvatar3D.h
  ../Interaction/mafAvatar3DCone.cpp
  ../Interaction/mafAvatar3DCone.h
  ../Interaction/mafCameraTransform.cpp
  ../Interaction/mafCameraTransform.h
  ../Interaction/mafGizmoAutoscaleHelper.cpp
  ../Interaction/mafGizmoAutoscaleHelper.h
  ../Interaction/mafDevice.cpp
  ../Interaction/mafDevice.h
  ../Interaction/mafDeviceManager.cpp
  ../Interaction/mafDeviceManager.h
  ../Interaction/mafDeviceSet.cpp
  ../Interaction/mafDeviceSet.h
  ../Interaction/mafEventInteraction.cpp
  ../Interaction/mafEventInteraction.h
  ../Interaction/mafGizmoBoundingBox.cpp
  ../Interaction/mafGizmoBoundingBox.h
  ../Interaction/mafGizmoHandle.cpp
  ../Interaction/mafGizmoHandle.h
  ../Interaction/mafGizmoInterface.cpp
  ../Interaction/mafGizmoInterface.h
  ../Interaction/mafGizmoPath.cpp
  ../Interaction/mafGizmoPath.h
  ../Interaction/mafGizmoPathRuler.cpp
  ../Interaction/mafGizmoPathRuler.h
  ../Interaction/mafGizmoROI.cpp
  ../Interaction/mafGizmoROI.h
  ../Interaction/mafGizmoRotate.cpp
  ../Interaction/mafGizmoRotate.h
  ../Interaction/mafGizmoRotateCircle.cpp
  ../Interaction/mafGizmoRotateCircle.h
  ../Interaction/mafGizmoRotateFan.cpp
  ../Interaction/mafGizmoRotateFan.h
  ../Interaction/mafGizmoScale.cpp
  ../Interaction/mafGizmoScale.h
  ../Interaction/mafGizmoScaleAxis.cpp
  ../Interaction/mafGizmoScaleAxis.h
  ../Interaction/mafGizmoScaleIsotropic.cpp
  ../Interaction/mafGizmoScaleIsotropic.h
  ../Interaction/mafGizmoTranslate.cpp
  ../Interaction/mafGizmoTranslate.h
  ../Interaction/mafGizmoTranslateAxis.cpp
  ../Interaction/mafGizmoTranslateAxis.h
  ../Interaction/mafGizmoTranslatePlane.cpp
  ../Interaction/mafGizmoTranslatePlane.h
  ../Interaction/mafGUIGizmoInterface.cpp
  ../Interaction/mafGUIGizmoInterface.h
  ../Interaction/mafGUIGizmoRotate.cpp
  ../Interaction/mafGUIGizmoRotate.h
  ../Interaction/mafGUIGizmoScale.cpp
  ../Interaction/mafGUIGizmoScale.h
  ../Interaction/mafGUIGizmoTranslate.cpp
  ../Interaction/mafGUIGizmoTranslate.h
  ../Interaction/mafGUISaveRestorePose.cpp
  ../Interaction/mafGUISaveRestorePose.h
  ../Interaction/mafGUITransformInterface.cpp
  ../Interaction/mafGUITransformInterface.h
  ../Interaction/mafGUITransformMouse.cpp
  ../Interaction/mafGUITransformMouse.h
  ../Interaction/mafGUITransformTextEntries.cpp
  ../Interaction/mafGUITransformTextEntries.h
  ../Interaction/mafInteractionFactory.cpp
  ../Interaction/mafInteractionFactory.h
  ../Interaction/mafInteractionManager.cpp
  ../Interaction/mafInteractionManager.h
  ../Interaction/mafInteractor.cpp
  ../Interaction/mafInteractor.h
  ../Interaction/mafRefSys.cpp
  ../Interaction/mafRefSys.h
  ../Interaction/mafDeviceButtonsPad.cpp
  ../Interaction/mafDeviceButtonsPad.h
  ../Interaction/mafDeviceButtonsPadMouse.cpp
  ../Interaction/mafDeviceButtonsPadMouse.h
  ../Interaction/mafDeviceButtonsPadMouseRemote.cpp
  ../Interaction/mafDeviceButtonsPadMouseRemote.h
  ../Interaction/mafDeviceButtonsPadTracker.cpp
  ../Interaction/mafDeviceButtonsPadTracker.h
  ../Interaction/mafInteractor2DAngle.cpp
  ../Interaction/mafInteractor2DAngle.h
  ../Interaction/mafInteractor2DDistance.cpp
  ../Interaction/mafInteractor2DDistance.h
  ../Interaction/mafInteractor2DIndicator.cpp
  ../Interaction/mafInteractor2DIndicator.h
  ../Interaction/mafInteractor6DOF.cpp
  ../Interaction/mafInteractor6DOF.h
  ../Interaction/mafInteractor6DOFCameraMove.cpp
  ../Interaction/mafInteractor6DOFCameraMove.h
  ../Interaction/mafInteractorCameraMove.cpp
  ../Interaction/mafInteractorCameraMove.h
  ../Interaction/mafInteractorCompositorMouse.cpp
  ../Interaction/mafInteractorCompositorMouse.h
  ../Interaction/mafInteractorConstraint.cpp
  ../Interaction/mafInteractorConstraint.h
  ../Interaction/mafInteractorExtractIsosurface.cpp
  ../Interaction/mafInteractorExtractIsosurface.h
  ../Interaction/mafInteractorGeneric6DOF.cpp
  ../Interaction/mafInteractorGeneric6DOF.h
  ../Interaction/mafInteractorGenericInterface.cpp
  ../Interaction/mafInteractorGenericInterface.h
  ../Interaction/mafInteractorGenericMouse.cpp
  ../Interaction/mafInteractorGenericMouse.h
  ../Interaction/mafInteractorPER.cpp
  ../Interaction/mafInteractorPER.h
  ../Interaction/mafInteractorPicker.cpp
  ../Interaction/mafInteractorPicker.h
  ../Interaction/mafInteractorSelectCell.cpp
  ../Interaction/mafInteractorSelectCell.h
  ../Interaction/mafInteractorSER.cpp
  ../Interaction/mafInteractorSER.h
  ../Interaction/mafDeviceClientMAF.cpp
  ../Interaction/mafDeviceClientMAF.h
)

IF (MAF_USE_P5_GLOVE)
  SET(INTERACTION_SRCS
    ${INTERACTION_SRCS}
    ../Interaction/mafDeviceButtonsPadTrackerP5Glove.cpp
    ../Interaction/mafDeviceButtonsPadTrackerP5Glove.h
  )
ENDIF (MAF_USE_P5_GLOVE)

IF (MAF_USE_WIIMOTE)
  SET(INTERACTION_SRCS
    ${INTERACTION_SRCS}
    ../Interaction/mafDeviceTrackerWIIMote.cpp
    ../Interaction/mafDeviceTrackerWIIMote.h
  )
ENDIF (MAF_USE_WIIMOTE)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/../Interaction/mafInteractionConfigure.h.in ${PROJECT_BINARY_DIR}/mafInteractionConfigure.h IMMEDIATE)

###############################################
#IO
###############################################
SET(IO_SRCS
  ../IO/mafCurlUtility.cpp
  ../IO/mafCurlUtility.h
  ../IO/mafStorable.cpp
  ../IO/mafStorable.h
  ../IO/mafStorage.cpp
  ../IO/mafStorage.h
  ../IO/mafStorageElement.cpp
  ../IO/mafStorageElement.h
  ../IO/mmuDOMTreeErrorReporter.h
  ../IO/mmuDOMTreeErrorReporter.cpp
)

IF (MAF_USE_XercesC)
  SET(IO_SRCS
    ${IO_SRCS}
    ../IO/mafXMLElement.cpp
    ../IO/mafXMLElement.h
    ../IO/mafXMLStorage.cpp
    ../IO/mafXMLStorage.h
    ../IO/mafXMLString.cpp
    ../IO/mafXMLString.h
    ../IO/mmuXMLDOM.h
    ../IO/mmuXMLDOMElement.h
  )
ENDIF (MAF_USE_XercesC)
###############################################
#VME
###############################################
SET(VME_SRCS
  ../VME/mafDataVector.cpp
  ../VME/mafDataVector.h
  ../VME/mafMatrixInterpolator.cpp
  ../VME/mafMatrixInterpolator.h
  ../VME/mafMatrixVector.cpp
  ../VME/mafMatrixVector.h
  ../VME/mafPipeFactoryVME.cpp
  ../VME/mafPipeFactoryVME.h
  ../VME/mafScalarVector.cpp
  ../VME/mafScalarVector.h
  ../VME/mafVMEExternalData.cpp
  ../VME/mafVMEExternalData.h
  ../VME/mafVMEFactory.cpp
  ../VME/mafVMEFactory.h
  ../VME/mafVMEGenericAbstract.cpp
  ../VME/mafVMEGenericAbstract.h
  ../VME/mafVMEGroup.cpp
  ../VME/mafVMEGroup.h
  ../VME/mafVMEItem.cpp
  ../VME/mafVMEItem.h
  ../VME/mafVMEInfoText.cpp
  ../VME/mafVMEInfoText.h
  ../VME/mafMSFImporter.cpp
  ../VME/mafMSFImporter.h
  ../VME/mafPipePolylineSlice.cpp
  ../VME/mafPipePolylineSlice.h
  ../VME/mafPipeMesh.cpp
  ../VME/mafPipeMesh.h
  ../VME/mafPipeMeshSlice.cpp
  ../VME/mafPipeMeshSlice.h
  ../VME/mafPipeScalar.cpp
  ../VME/mafPipeScalar.h
  ../VME/mafVisualPipeVolumeRayCasting.cpp
  ../VME/mafVisualPipeVolumeRayCasting.h
  ../VME/mafVMEOutputScalar.cpp
  ../VME/mafVMEOutputScalar.h
  ../VME/mafVMEScalar.cpp
  ../VME/mafVMEScalar.h
)

IF (MAF_USE_CRYPTO)
  SET(VME_SRCS
    ${VME_SRCS}
    ../VME/mafCrypt.cpp
    ../VME/mafCrypt.h
  )
ENDIF (MAF_USE_CRYPTO)

IF (MAF_USE_ITK)
  SET(VME_SRCS
    ${VME_SRCS}
    ../VME/mafPipeScalarMatrix.cpp
    ../VME/mafPipeScalarMatrix.h
    ../VME/mafVMEOutputScalarMatrix.cpp
    ../VME/mafVMEOutputScalarMatrix.h
    ../VME/mafVMEScalarMatrix.cpp
    ../VME/mafVMEScalarMatrix.h
    ../VME/mafVMEItemScalarMatrix.cpp
    ../VME/mafVMEItemScalarMatrix.h
  )
ENDIF (MAF_USE_ITK)

IF (MAF_USE_VTK)
  SET(VME_SRCS
    ${VME_SRCS}
    ../VME/mafPipeGizmo.cpp
    ../VME/mafPipeGizmo.h
    ../VME/mafPipeImage3D.cpp
    ../VME/mafPipeImage3D.h
    ../VME/mafPipeIsosurface.cpp
    ../VME/mafPipeIsosurface.h
    ../VME/mafPipeLandmarkCloud.cpp
    ../VME/mafPipeLandmarkCloud.h
    ../VME/mafPipeMeter.cpp
    ../VME/mafPipeMeter.h
    ../VME/mafPipePointSet.cpp
    ../VME/mafPipePointSet.h
    ../VME/mafPipePolyline.cpp
    ../VME/mafPipePolyline.h
    ../VME/mafPipeSurface.cpp
    ../VME/mafPipeSurface.h
    ../VME/mafPipeSurfaceSlice.cpp
    ../VME/mafPipeSurfaceSlice.h
    ../VME/mafPipeSurfaceTextured.cpp
    ../VME/mafPipeSurfaceTextured.h
    ../VME/mafPipeVector.cpp
    ../VME/mafPipeVector.h
    ../VME/mafPipeVolumeProjected.cpp
    ../VME/mafPipeVolumeProjected.h
    ../VME/mafPipeVolumeSlice.cpp
    ../VME/mafPipeVolumeSlice.h
    ../VME/mafVMEGeneric.cpp
    ../VME/mafVMEGeneric.h
    ../VME/mafVMEGizmo.cpp
    ../VME/mafVMEGizmo.h
    ../VME/mafVMEImage.cpp
    ../VME/mafVMEImage.h
    ../VME/mafVMEItemVTK.cpp
    ../VME/mafVMEItemVTK.h
    ../VME/mafVMELandmark.cpp
    ../VME/mafVMELandmark.h
    ../VME/mafVMELandmarkCloud.cpp
    ../VME/mafVMELandmarkCloud.h
    ../VME/mafVMEMesh.cpp
    ../VME/mafVMEMesh.h
    ../VME/mafVMEMeter.cpp
    ../VME/mafVMEMeter.h
    ../VME/mafVMEOutputImage.cpp
    ../VME/mafVMEOutputImage.h
    ../VME/mafVMEOutputLandmarkCloud.cpp
    ../VME/mafVMEOutputLandmarkCloud.h
    ../VME/mafVMEOutputMesh.cpp
    ../VME/mafVMEOutputMesh.h
    ../VME/mafVMEOutputMeter.cpp
    ../VME/mafVMEOutputMeter.h
    ../VME/mafVMEOutputPointSet.cpp
    ../VME/mafVMEOutputPointSet.h
    ../VME/mafVMEOutputPolyline.cpp
    ../VME/mafVMEOutputPolyline.h
    ../VME/mafVMEOutputSurface.cpp
    ../VME/mafVMEOutputSurface.h
    ../VME/mafVMEOutputVolume.cpp
    ../VME/mafVMEOutputVolume.h
    ../VME/mafVMEOutputVTK.cpp
    ../VME/mafVMEOutputVTK.h
    ../VME/mafVMEPointSet.cpp
    ../VME/mafVMEPointSet.h
    ../VME/mafVMEPolyline.cpp
    ../VME/mafVMEPolyline.h
    ../VME/mafVMEPolylineSpline.cpp
    ../VME/mafVMEPolylineSpline.h
    ../VME/mafVMEProber.cpp
    ../VME/mafVMEProber.h
    ../VME/mafVMERefSys.cpp
    ../VME/mafVMERefSys.h
    ../VME/mafVMESlicer.cpp
    ../VME/mafVMESlicer.h
    ../VME/mafVMESurface.cpp
    ../VME/mafVMESurface.h
    ../VME/mafVMESurfaceParametric.cpp
    ../VME/mafVMESurfaceParametric.h
    ../VME/mafVMEVector.cpp
    ../VME/mafVMEVector.h
    ../VME/mafVMEVolume.cpp
    ../VME/mafVMEVolume.h
    ../VME/mafVMEVolumeGray.cpp
    ../VME/mafVMEVolumeGray.h
    ../VME/mafVMEVolumeRGB.cpp
    ../VME/mafVMEVolumeRGB.h
    ../VME/mmaMeter.cpp
    ../VME/mmaMeter.h
    ../VME/mafDataVectorVTK.cpp
    ../VME/mafDataVectorVTK.h
  )
ENDIF (MAF_USE_VTK)

IF(WIN32)
  SET(VME_SRCS
    ${VME_SRCS}
    ../VME/mafPipeIsosurfaceGPU.cpp
    ../VME/mafPipeIsosurfaceGPU.h
  )
ENDIF(WIN32)
###############################################
#DATAPIPES
###############################################
SET(DATAPIPES_SRCS
  ../VME/DataPipes/mafDataInterpolator.cpp
  ../VME/DataPipes/mafDataInterpolator.h
  ../VME/DataPipes/mafDataPipeCustom.cpp
  ../VME/DataPipes/mafDataPipeCustom.h
  ../VME/DataPipes/mafDataPipeCustomProber.cpp
  ../VME/DataPipes/mafDataPipeCustomProber.h
  ../VME/DataPipes/mafVTKInterpolator.cpp
  ../VME/DataPipes/mafVTKInterpolator.h
  ../VME/DataPipes/vtkMAFDataPipe.cpp
  ../VME/DataPipes/vtkMAFDataPipe.h
)

IF (MAF_USE_ITK)
  SET(DATAPIPES_SRCS
    ${DATAPIPES_SRCS}
    ../VME/DataPipes/mafScalarMatrixInterpolator.cpp
    ../VME/DataPipes/mafScalarMatrixInterpolator.h
  )
ENDIF (MAF_USE_ITK)
###############################################
#OPERATIONS
###############################################
IF (MAF_USE_VTK)
  SET(OPERATIONS_SRCS
    ${OPERATIONS_SRCS}
    ../Operations/mafOpDecomposeTimeVarVME.cpp
    ../Operations/mafOpDecomposeTimeVarVME.h
    ../Operations/mafOpImporterMSF.cpp
    ../Operations/mafOpImporterMSF.h
    ../Operations/mafOpImporterExternalFile.cpp
    ../Operations/mafOpImporterExternalFile.h
    ../Operations/mafOpLabelExtractor.cpp
    ../Operations/mafOpLabelExtractor.h
    ../Operations/mafOpOpenExternalFile.cpp
    ../Operations/mafOpOpenExternalFile.h
    ../Operations/mafOp2DMeasure.cpp
    ../Operations/mafOp2DMeasure.h
    ../Operations/mafOpAddLandmark.cpp
    ../Operations/mafOpAddLandmark.h
    ../Operations/mafOpExporterBmp.cpp
    ../Operations/mafOpExporterBmp.h
    ../Operations/mafOpClipSurface.cpp
    ../Operations/mafOpClipSurface.h
    ../Operations/mafOpCreateGroup.cpp
    ../Operations/mafOpCreateGroup.h
    ../Operations/mafOpCreateGroup.h
    ../Operations/mafOpCreateMeter.cpp
    ../Operations/mafOpCreateMeter.h
    ../Operations/mafOpCreateSurfaceParametric.cpp
    ../Operations/mafOpCreateSurfaceParametric.h
    ../Operations/mafOpCreateProber.cpp
    ../Operations/mafOpCreateProber.h
    ../Operations/mafOpCreateRefSys.cpp
    ../Operations/mafOpCreateRefSys.h
    ../Operations/mafOpCreateSlicer.cpp
    ../Operations/mafOpCreateSlicer.h
    ../Operations/mafOpCreateSpline.cpp
    ../Operations/mafOpCreateSpline.h
    ../Operations/mafOpCreateVolume.cpp
    ../Operations/mafOpCreateVolume.h
    ../Operations/mafOpRemoveCells.cpp
    ../Operations/mafOpRemoveCells.h
    ../Operations/mafOpEditMetadata.cpp
    ../Operations/mafOpEditMetadata.h
    ../Operations/mafOpExplodeCollapse.cpp
    ../Operations/mafOpExplodeCollapse.h
    ../Operations/mafOpExtractIsosurface.cpp
    ../Operations/mafOpExtractIsosurface.h
    ../Operations/mafOpFilterSurface.cpp
    ../Operations/mafOpFilterSurface.h
    ../Operations/mafOpDecimateSurface.cpp
    ../Operations/mafOpDecimateSurface.h
    ../Operations/mafOpConnectivitySurface.cpp
    ../Operations/mafOpConnectivitySurface.h
    ../Operations/mafOpFilterVolume.cpp
    ../Operations/mafOpFilterVolume.h
    ../Operations/mafOpImporterImage.cpp
    ../Operations/mafOpImporterImage.h
    ../Operations/mafOpMAFTransform.cpp
    ../Operations/mafOpMAFTransform.h
    ../Operations/mafOpExporterMSF.cpp
    ../Operations/mafOpExporterMSF.h
    ../Operations/mafOpImporterMSF1x.cpp
    ../Operations/mafOpImporterMSF1x.h
    ../Operations/mafOpExporterRaw.cpp
    ../Operations/mafOpExporterRaw.h
    ../Operations/mafOpImporterRAWVolume.cpp
    ../Operations/mafOpImporterRAWVolume.h
    ../Operations/mafOpReparentTo.cpp
    ../Operations/mafOpReparentTo.h
    ../Operations/mafOpExporterSTL.cpp
    ../Operations/mafOpExporterSTL.h
    ../Operations/mafOpImporterSTL.cpp
    ../Operations/mafOpImporterSTL.h
    ../Operations/mafOpTransformInterface.cpp
    ../Operations/mafOpTransformInterface.h
    ../Operations/mafOpValidateTree.cpp
    ../Operations/mafOpValidateTree.h
    ../Operations/mafOpVolumeResample.cpp
    ../Operations/mafOpVolumeResample.h
    ../Operations/mafOpVOIDensity.cpp
    ../Operations/mafOpVOIDensity.h
    ../Operations/mafOpVOIDensityEditor.cpp
    ../Operations/mafOpVOIDensityEditor.h
    ../Operations/mafOpImporterVRML.cpp
    ../Operations/mafOpImporterVRML.h
    ../Operations/mafOpExporterVTK.cpp
    ../Operations/mafOpExporterVTK.h
    ../Operations/mafOpImporterVTK.cpp
    ../Operations/mafOpImporterVTK.h
    ../Operations/mafOpCrop.cpp
    ../Operations/mafOpCrop.h
    ../Operations/mafOpBooleanSurface.cpp
    ../Operations/mafOpBooleanSurface.h
    ../Operations/mafOpEditNormals.cpp
    ../Operations/mafOpEditNormals.h
    ../Operations/mafOpApplyTrajectory.cpp
    ../Operations/mafOpApplyTrajectory.h
    ../Operations/mafOpGarbageCollectMSFDir.cpp
    ../Operations/mafOpGarbageCollectMSFDir.h
  )
ENDIF (MAF_USE_VTK)

IF (MAF_USE_ITK)
  SET(OPERATIONS_SRCS
    ${OPERATIONS_SRCS}
    ../Operations/mafASCIIImporterUtility.cpp
    ../Operations/mafASCIIImporterUtility.h
    ../Operations/mafOpImporterASCII.cpp
    ../Operations/mafOpImporterASCII.h
    ../Operations/mafVMEDataSetAttributesImporter.cpp
    ../Operations/mafVMEDataSetAttributesImporter.h
    ../Operations/mafVMEMeshAnsysTextImporter.cpp
    ../Operations/mafVMEMeshAnsysTextImporter.h
    ../Operations/mafVMEMeshAnsysTextExporter.cpp
    ../Operations/mafVMEMeshAnsysTextExporter.h
    ../Operations/mafParabolicMeshToLinearMeshFilter.cxx
    ../Operations/mafParabolicMeshToLinearMeshFilter.h
    ../Operations/mafOpImporterMesh.cpp
    ../Operations/mafOpImporterMesh.h
    ../Operations/mafOpExporterMesh.cpp
    ../Operations/mafOpExporterMesh.h
    ../Operations/mafOpScalarToSurface.cpp
    ../Operations/mafOpScalarToSurface.h
    ../Operations/mafOpImporterVMEDataSetAttributes.cpp
    ../Operations/mafOpImporterVMEDataSetAttributes.h
  )
ENDIF (MAF_USE_ITK)
###############################################
#DRIVERS
###############################################
IF (WIN32)
  SET (DRIVERS_SRCS
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
    ../Interaction/Drivers/wsock.c
    ../Interaction/Drivers/wsock.h
  )
ENDIF (WIN32)

SET(DRIVERS_SRCS
  ${DRIVERS_SRCS}
  ../Interaction/Drivers/ClientUnit.cpp
  ../Interaction/Drivers/ClientUnit.h  
)

IF (WIN32)
  SET (DRIVERS_SRCS
    ${DRIVERS_SRCS}
    ../Interaction/Drivers/vicon.cpp
    ../Interaction/Drivers/vicon.h
  )
ENDIF (WIN32)

# P5 driver
IF (MAF_USE_P5_GLOVE)
  SET (DRIVERS_SRCS 
    ${DRIVERS_SRCS}
    ../Interaction/Drivers/P5Bend.cpp
    ../Interaction/Drivers/P5Bend.h
    ../Interaction/Drivers/P5Motion.cpp
    ../Interaction/Drivers/P5Motion.h
    ../Interaction/Drivers/P5dll.h
  )
ENDIF (MAF_USE_P5_GLOVE)

IF (MAF_USE_WIIMOTE)
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
    #../Interaction/Drivers/LEDPlayer.h
    #../Interaction/Drivers/LEDPlayer.cpp
    #../Interaction/Drivers/Resource.h
    ../Interaction/Drivers/setupapi.h
    ../Interaction/Drivers/Utils.h
    ../Interaction/Drivers/Wiimote.h
    ../Interaction/Drivers/Wiimote.cpp
  )
ENDIF (MAF_USE_WIIMOTE)
###############################################
SET(PROJECT_SRCS
  ${BASE_SRCS}
  ${CORE_SRCS}
  ${GUI_SRCS}
  ${INTERACTION_SRCS}
  ${IO_SRCS}
  ${VME_SRCS}
  ${DATAPIPES_SRCS}
  ${OPERATIONS_SRCS}
  ${DRIVERS_SRCS}
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
  INCLUDE (${MAF_SOURCE_DIR}/modules/FindGTK2.cmake)
  INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIRECTORIES})
ENDIF (NOT WIN32)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

IF (MAF_USE_VTK)
  # SET(PROJECT_LIBS ${PROJECT_LIBS} vtkCommon vtkIO vtkRendering vtkGraphics vtkHybrid)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
ENDIF (MAF_USE_VTK)

IF (MAF_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (MAF_USE_WX)

IF (MAF_USE_CRYPTO)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_CRYPTO_LIBRARY)
ENDIF (MAF_USE_CRYPTO)

IF (MAF_USE_CURL)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_CURL_LIBRARY)
ENDIF (MAF_USE_CURL)

IF (MAF_USE_ITK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_ITK_LIBRARY)
ENDIF (MAF_USE_ITK)

IF (MAF_USE_XercesC)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_XercesC_LIBRARY)
ENDIF (MAF_USE_XercesC)

