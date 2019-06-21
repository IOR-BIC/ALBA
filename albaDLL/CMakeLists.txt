#
# Copyright and description to be placed here
#

PROJECT(albaDLL)

DBG_MESSAGE("Configuring project albaDLL")

ADD_DEFINITIONS(-DALBA_EXPORTS -UPROJ_SAX2 -UPROJ_XMLPARSER -UPROJ_XMLUTIL -UPROJ_PARSERS -UPROJ_SAX4C -UPROJ_DOM -UPROJ_VALIDATORS -UXML_SINGLEDLL -UXML_USE_WIN32_TRANSCODER -UXML_USE_WIN32_MSGLOADER -UXML_USE_NETACCESSOR_WINSOCK)

LINK_LIBRARIES(glu32 opengl32 vtkHybrid vtkIO vtkRendering vtkGraphics vtkFiltering vtkImaging vtkCommon vtkftgl vtkfreetype XercesC curl Crypto vtkALBA GPUAPI)

###############################################
#BASE
###############################################
SET(BASE_SRCS
  ../Base/albaBase.h
  ../Base/albaDecl.cpp
  ../Base/albaDecl.h
  ../Base/albaDefines.cpp
  ../Base/albaDefines.h
  ../Base/albaDirectory.cpp
  ../Base/albaDirectory.h
  ../Base/albaDynamicLoader.cpp
  ../Base/albaDynamicLoader.h
  ../Base/albaEventBase.cpp
  ../Base/albaEventBase.h
  ../Base/albaEventSender.h
  ../Base/albaEventSource.cpp
  ../Base/albaEventSource.h
  ../Base/albaIncludeWIN32.h
  ../Base/albaIncludeWX.h
  ../Base/albaIndent.cpp
  ../Base/albaIndent.h
  ../Base/albaLUTLibrary.cpp
  ../Base/albaLUTLibrary.h
  ../Base/albaMTime.cpp
  ../Base/albaMTime.h
  ../Base/albaMatrix.cpp
  ../Base/albaMatrix.h
  ../Base/albaMatrix3x3.cpp
  ../Base/albaMatrix3x3.h
  ../Base/albaMultiThreader.h
  ../Base/albaMultiThreader.cpp
  ../Base/albaMutexLock.cpp
  ../Base/albaMutexLock.h
  ../Base/albaOBB.cpp
  ../Base/albaOBB.h
  ../Base/albaObject.cpp
  ../Base/albaObject.h
  ../Base/albaObjectFactory.cpp
  ../Base/albaObjectFactory.h
  ../Base/albaObserver.cpp
  ../Base/albaObserver.h
  ../Base/albaObserverCallback.cpp
  ../Base/albaObserverCallback.h
  ../Base/albaReferenceCounted.cpp
  ../Base/albaReferenceCounted.h
  ../Base/albaSmartPointer.h
  ../Base/albaString.cpp
  ../Base/albaString.h
  ../Base/albaTimeStamped.cpp
  ../Base/albaTimeStamped.h
  ../Base/albaTransform.cpp
  ../Base/albaTransform.h
  ../Base/albaTransformBase.cpp
  ../Base/albaTransformBase.h
  ../Base/albaTransformFrame.cpp
  ../Base/albaTransformFrame.h
  ../Base/albaVector.h
  ../Base/albaVector.txx
  ../Base/albaVersion.h
  ../Base/mmuIdFactory.cpp
  ../Base/mmuIdFactory.h
  ../Base/albaUtility.h
  ../Base/vtkALBASmartPointer.h
  ../Base/vtkALBAToLinearTransform.h
  ../Base/albaZipUtility.cpp
  ../Base/albaZipUtility.h
)
IF (ALBA_USE_VTK)
  SET(BASE_SRCS
    ${BASE_SRCS}
      ../Base/vtkALBAToLinearTransform.cxx
      ../Base/vtkALBAToLinearTransform.h
      ../Base/vtkALBASmartPointer.h
  )
ENDIF (ALBA_USE_VTK)
###############################################
#CORE
###############################################
SET(CORE_SRCS
  ../Core/albaAbsMatrixPipe.cpp
  ../Core/albaAbsMatrixPipe.h
  ../Core/albaAttribute.cpp
  ../Core/albaAttribute.h
  ../Core/albaAttributeTraceability.cpp
  ../Core/albaAttributeTraceability.h
  ../Core/albaCoreFactory.cpp
  ../Core/albaCoreFactory.h
  ../Core/albaDataPipe.cpp
  ../Core/albaDataPipe.h
  ../Core/albaExpiration.h
  ../Core/albaExpirationDate.cpp
  ../Core/albaExpirationDate.h
  ../Core/albaEventIO.cpp
  ../Core/albaEventIO.h
  ../Core/albaLogicWithGUI.cpp
  ../Core/albaLogicWithGUI.h
  ../Core/albaMatrixPipe.cpp
  ../Core/albaMatrixPipe.h
  ../Core/albaNode.cpp
  ../Core/albaNode.h
  ../Core/albaNodeFactory.cpp
  ../Core/albaNodeFactory.h
  ../Core/albaNodeGeneric.cpp
  ../Core/albaNodeGeneric.h
  ../Core/albaNodeIterator.cpp
  ../Core/albaNodeIterator.h
  ../Core/albaNodeLayout.cpp
  ../Core/albaNodeLayout.h
  ../Core/albaNodeManager.cpp
  ../Core/albaNodeManager.h
  ../Core/albaNodeRoot.cpp
  ../Core/albaNodeRoot.h
  ../Core/albaOp.cpp
  ../Core/albaOp.h
  ../Core/albaOpContextStack.cpp
  ../Core/albaOpContextStack.h
  ../Core/albaOpManager.cpp
  ../Core/albaOpManager.h
  ../Core/albaOpSelect.cpp
  ../Core/albaOpSelect.h
  ../Core/albaOpStack.cpp
  ../Core/albaOpStack.h
  ../Core/albaPrintSupport.cpp
  ../Core/albaPrintSupport.h
  ../Core/mmdRemoteFileManager.cpp
  ../Core/mmdRemoteFileManager.h
  ../Core/albaRemoteLogic.cpp
  ../Core/albaRemoteLogic.h
  ../Core/albaRemoteStorage.cpp
  ../Core/albaRemoteStorage.h
  ../Core/albaRoot.cpp
  ../Core/albaRoot.h
  ../Core/albaTagArray.cpp
  ../Core/albaTagArray.h
  ../Core/albaTagItem.cpp
  ../Core/albaTagItem.h
  ../Core/albaTimeMap.h
  ../Core/albaTimeMapScalar.h
  ../Core/albaUser.cpp
  ../Core/albaUser.h
  ../Core/albaVME.cpp
  ../Core/albaVME.h
  ../Core/albaVMEManager.cpp
  ../Core/albaVMEManager.h
  ../Core/albaVMEOutput.cpp
  ../Core/albaVMEOutput.h
  ../Core/albaVMEOutputNULL.cpp
  ../Core/albaVMEOutputNULL.h
  ../Core/albaVMERoot.cpp
  ../Core/albaVMERoot.h
  ../Core/albaVMEStorage.cpp
  ../Core/albaVMEStorage.h
  ../Core/mmaApplicationLayout.cpp
  ../Core/mmaApplicationLayout.h
  ../Core/albaGUIListCtrlBitmap.cpp
  ../Core/albaGUIListCtrlBitmap.h
  ../Core/albaGUIVMEChooser.cpp
  ../Core/albaGUIVMEChooser.h
  ../Core/albaGUIVMEChooserTree.cpp
  ../Core/albaGUIVMEChooserTree.h
  ../Core/mmuTimeSet.cpp
  ../Core/mmuTimeSet.h
)

IF (ALBA_USE_VTK)
  SET(CORE_SRCS
    ${CORE_SRCS}
    ../Core/albaAxes.cpp
    ../Core/albaAxes.h
    ../Core/albaLODActor.cpp
    ../Core/albaLODActor.h
    ../Core/albaLogicWithManagers.cpp
    ../Core/albaLogicWithManagers.h
    ../Core/albaPipe.cpp
    ../Core/albaPipe.h
    ../Core/albaPipeBox.cpp
    ../Core/albaPipeBox.h
    ../Core/albaPipeFactory.cpp
    ../Core/albaPipeFactory.h
    ../Core/albaRWI.cpp
    ../Core/albaRWI.h
    ../Core/albaRWIBase.cpp
    ../Core/albaRWIBase.h
    ../Core/albaSceneGraph.cpp
    ../Core/albaSceneGraph.h
    ../Core/albaSceneNode.cpp
    ../Core/albaSceneNode.h
    ../Core/albaViewCompound.cpp
    ../Core/albaViewCompound.h
    ../Core/albaViewHTML.cpp
    ../Core/albaViewHTML.h
    ../Core/albaViewManager.cpp
    ../Core/albaViewManager.h
    ../Core/albaViewPlot.cpp
    ../Core/albaViewPlot.h
    ../Core/albaViewVTK.cpp
    ../Core/albaViewVTK.h
    ../Core/mmaMaterial.cpp
    ../Core/mmaMaterial.h
    ../Core/mmaVolumeMaterial.cpp
    ../Core/mmaVolumeMaterial.h
    ../Core/albaGUIContextualMenu.cpp
    ../Core/albaGUIContextualMenu.h
    ../Core/albaGUIMaterialButton.cpp
    ../Core/albaGUIMaterialButton.h
    ../Core/albaGUIMaterialChooser.cpp
    ../Core/albaGUIMaterialChooser.h
    ../Core/albaGUITreeContextualMenu.cpp
    ../Core/albaGUITreeContextualMenu.h
    ../Core/vtkALBAAssembly.cpp
    ../Core/vtkALBAAssembly.h
  )
ENDIF (ALBA_USE_VTK)
###############################################
#GUI
###############################################
SET(GUI_SRCS
  ../Gui/albaEvent.cpp
  ../Gui/albaEvent.h
  ../Gui/albaGUIDialogFindVme.cpp
  ../Gui/albaGUIDialogFindVme.h
  ../Gui/albaGUIDialogLogin.cpp
  ../Gui/albaGUIDialogLogin.h
  ../Gui/albaGUIDialogTransferFunction2D.cpp
  ../Gui/albaGUIDialogTransferFunction2D.h
  ../Gui/albaTransferFunction2DInteractors.cpp
  ../Gui/albaGUISettings.cpp
  ../Gui/albaGUISettings.h
  ../Gui/albaGUISettingsStorage.cpp
  ../Gui/albaGUISettingsStorage.h
  ../Gui/albaGUISettingsTimeBar.cpp
  ../Gui/albaGUISettingsTimeBar.h
  ../Gui/albaPics.cpp
  ../Gui/albaPics.h
  ../Gui/albaWXLog.cpp
  ../Gui/albaWXLog.h
  ../Gui/albaGUIApplicationLayoutSettings.cpp
  ../Gui/albaGUIApplicationLayoutSettings.h
  ../Gui/albaGUIApplicationSettings.cpp
  ../Gui/albaGUIApplicationSettings.h
  ../Gui/albaGUIButton.cpp
  ../Gui/albaGUIButton.h
  ../Gui/albaGUICheckListBox.cpp
  ../Gui/albaGUICheckListBox.h
  ../Gui/albaGUICheckTree.cpp
  ../Gui/albaGUICheckTree.h
  ../Gui/albaGUIColorSwatch.cpp
  ../Gui/albaGUIColorSwatch.h
  ../Gui/albaGUIColorWidget.cpp
  ../Gui/albaGUIColorWidget.h
  ../Gui/albaGUICrossSplitter.cpp
  ../Gui/albaGUICrossSplitter.h
  ../Gui/albaGUIDialog.cpp
  ../Gui/albaGUIDialog.h
  ../Gui/albaGUIDockManager.cpp
  ../Gui/albaGUIDockManager.h
  ../Gui/albaGUIDockSettings.cpp
  ../Gui/albaGUIDockSettings.h
  ../Gui/albaGUIFileHistory.cpp
  ../Gui/albaGUIFileHistory.h
  ../Gui/albaGUIFlagSplitter.cpp
  ../Gui/albaGUIFlagSplitter.h
  ../Gui/albaGUIFloatSlider.cpp
  ../Gui/albaGUIFloatSlider.h
  ../Gui/albaGUIFrame.cpp
  ../Gui/albaGUIFrame.h
  ../Gui/albaGUI.cpp
  ../Gui/albaGUI.h
  ../Gui/albaGUIHolder.cpp
  ../Gui/albaGUIHolder.h
  ../Gui/albaGUILab.cpp
  ../Gui/albaGUILab.h
  ../Gui/albaGUIListBox.cpp
  ../Gui/albaGUIListBox.h
  ../Gui/albaGUIListCtrl.cpp
  ../Gui/albaGUIListCtrl.h
  ../Gui/albaGUILocaleSettings.cpp
  ../Gui/albaGUILocaleSettings.h
  ../Gui/albaGUIMeasureUnitSettings.cpp
  ../Gui/albaGUIMeasureUnitSettings.h
  ../Gui/albaGUIMovieCtrl.cpp
  ../Gui/albaGUIMovieCtrl.h
  ../Gui/albaGUIMDIChild.cpp
  ../Gui/albaGUIMDIChild.h
  ../Gui/albaGUIMDIFrame.cpp
  ../Gui/albaGUIMDIFrame.h
  ../Gui/albaGUIMutexPanel.cpp
  ../Gui/albaGUIMutexPanel.h
  ../Gui/albaGUINamedPanel.cpp
  ../Gui/albaGUINamedPanel.h
  ../Gui/albaGUIPanel.cpp
  ../Gui/albaGUIPanel.h
  ../Gui/albaGUIPanelStack.cpp
  ../Gui/albaGUIPanelStack.h
  ../Gui/albaGUIPicButton.cpp
  ../Gui/albaGUIPicButton.h
  ../Gui/albaGUIRangeSlider.cpp
  ../Gui/albaGUIRangeSlider.h
  ../Gui/albaGUIRollOut.cpp
  ../Gui/albaGUIRollOut.h
  ../Gui/albaGUISashPanel.h
  ../Gui/albaGUISashPanel.cpp
  ../Gui/albaGUIScrolledPanel.cpp
  ../Gui/albaGUIScrolledPanel.h
  ../Gui/albaGUISettingsDialog.cpp
  ../Gui/albaGUISettingsDialog.h
  ../Gui/albaGUISplittedPanel.cpp
  ../Gui/albaGUISplittedPanel.h
  ../Gui/albaGUITimeBar.cpp
  ../Gui/albaGUITimeBar.h
  ../Gui/albaGUITree.cpp
  ../Gui/albaGUITree.h
  ../Gui/albaGUIValidator.cpp
  ../Gui/albaGUIValidator.h
  ../Gui/albaGUIViewFrame.cpp
  ../Gui/albaGUIViewFrame.h
  ../Gui/albaGUIViewWin.cpp
  ../Gui/albaGUIViewWin.h
  ../Gui/albaGUICrossIncremental.cpp
  ../Gui/albaGUICrossIncremental.h
  ../Gui/albaGUIComboBox.cpp
  ../Gui/albaGUIComboBox.h
)

IF (ALBA_USE_CURL)
	SET(GUI_SRCS
	  ${GUI_SRCS}
	  ../Gui/albaGUIDialogRemoteFile.cpp
	  ../Gui/albaGUIDialogRemoteFile.h
	)
ENDIF (ALBA_USE_CURL)

IF (ALBA_USE_VTK)
  SET(GUI_SRCS
    ${GUI_SRCS}
    ../Gui/albaLightKit.cpp
    ../Gui/albaLightKit.h
    ../Gui/albaGUIDialogPreview.cpp
    ../Gui/albaGUIDialogPreview.h
    ../Gui/albaGUIDictionaryWidget.cpp
    ../Gui/albaGUIDictionaryWidget.h
    ../Gui/albaAnimate.cpp
    ../Gui/albaAnimate.h
    ../Gui/albaAttachCamera.cpp
    ../Gui/albaAttachCamera.h
    ../Gui/albaSideBar.cpp
    ../Gui/albaSideBar.h
    ../Gui/albaTextKit.cpp
    ../Gui/albaTextKit.h
    ../Gui/albaVTKLog.cpp
    ../Gui/albaVTKLog.h
    ../Gui/albaView.cpp
    ../Gui/albaView.h
    ../Gui/albaColor.cpp
    ../Gui/albaColor.h
    ../Gui/albaGUILutEditor.cpp
    ../Gui/albaGUILutEditor.h
    ../Gui/albaGUILutPreset.cpp
    ../Gui/albaGUILutPreset.h
    ../Gui/albaGUILutSlider.cpp
    ../Gui/albaGUILutSlider.h
    ../Gui/albaGUILutSwatch.cpp
    ../Gui/albaGUILutSwatch.h
    ../Gui/albaGUILutWidget.cpp
    ../Gui/albaGUILutWidget.h
    ../Gui/albaGUIHistogramWidget.cpp
    ../Gui/albaGUIHistogramWidget.h
  )
ENDIF (ALBA_USE_VTK)
###############################################
#INTERACTION
###############################################
SET(INTERACTION_SRCS
  ../Interaction/albaAction.cpp
  ../Interaction/albaAction.h
  ../Interaction/albaAgent.cpp
  ../Interaction/albaAgent.h
  ../Interaction/albaAgentEventHandler.cpp
  ../Interaction/albaAgentEventHandler.h
  ../Interaction/albaAgentEventQueue.cpp
  ../Interaction/albaAgentEventQueue.h
  ../Interaction/albaAgentThreaded.cpp
  ../Interaction/albaAgentThreaded.h
  ../Interaction/albaAvatar.cpp
  ../Interaction/albaAvatar.h
  ../Interaction/albaAvatar2D.cpp
  ../Interaction/albaAvatar2D.h
  ../Interaction/albaAvatar3D.cpp
  ../Interaction/albaAvatar3D.h
  ../Interaction/albaAvatar3DCone.cpp
  ../Interaction/albaAvatar3DCone.h
  ../Interaction/albaCameraTransform.cpp
  ../Interaction/albaCameraTransform.h
  ../Interaction/albaGizmoAutoscaleHelper.cpp
  ../Interaction/albaGizmoAutoscaleHelper.h
  ../Interaction/albaDevice.cpp
  ../Interaction/albaDevice.h
  ../Interaction/albaDeviceManager.cpp
  ../Interaction/albaDeviceManager.h
  ../Interaction/albaDeviceSet.cpp
  ../Interaction/albaDeviceSet.h
  ../Interaction/albaEventInteraction.cpp
  ../Interaction/albaEventInteraction.h
  ../Interaction/albaGizmoBoundingBox.cpp
  ../Interaction/albaGizmoBoundingBox.h
  ../Interaction/albaGizmoHandle.cpp
  ../Interaction/albaGizmoHandle.h
  ../Interaction/albaGizmoInterface.cpp
  ../Interaction/albaGizmoInterface.h
  ../Interaction/albaGizmoPath.cpp
  ../Interaction/albaGizmoPath.h
  ../Interaction/albaGizmoPathRuler.cpp
  ../Interaction/albaGizmoPathRuler.h
  ../Interaction/albaGizmoROI.cpp
  ../Interaction/albaGizmoROI.h
  ../Interaction/albaGizmoRotate.cpp
  ../Interaction/albaGizmoRotate.h
  ../Interaction/albaGizmoRotateCircle.cpp
  ../Interaction/albaGizmoRotateCircle.h
  ../Interaction/albaGizmoRotateFan.cpp
  ../Interaction/albaGizmoRotateFan.h
  ../Interaction/albaGizmoScale.cpp
  ../Interaction/albaGizmoScale.h
  ../Interaction/albaGizmoScaleAxis.cpp
  ../Interaction/albaGizmoScaleAxis.h
  ../Interaction/albaGizmoScaleIsotropic.cpp
  ../Interaction/albaGizmoScaleIsotropic.h
  ../Interaction/albaGizmoTranslate.cpp
  ../Interaction/albaGizmoTranslate.h
  ../Interaction/albaGizmoTranslateAxis.cpp
  ../Interaction/albaGizmoTranslateAxis.h
  ../Interaction/albaGizmoTranslatePlane.cpp
  ../Interaction/albaGizmoTranslatePlane.h
  ../Interaction/albaGUIGizmoInterface.cpp
  ../Interaction/albaGUIGizmoInterface.h
  ../Interaction/albaGUIGizmoRotate.cpp
  ../Interaction/albaGUIGizmoRotate.h
  ../Interaction/albaGUIGizmoScale.cpp
  ../Interaction/albaGUIGizmoScale.h
  ../Interaction/albaGUIGizmoTranslate.cpp
  ../Interaction/albaGUIGizmoTranslate.h
  ../Interaction/albaGUISaveRestorePose.cpp
  ../Interaction/albaGUISaveRestorePose.h
  ../Interaction/albaGUITransformInterface.cpp
  ../Interaction/albaGUITransformInterface.h
  ../Interaction/albaGUITransformMouse.cpp
  ../Interaction/albaGUITransformMouse.h
  ../Interaction/albaGUITransformTextEntries.cpp
  ../Interaction/albaGUITransformTextEntries.h
  ../Interaction/albaInteractionFactory.cpp
  ../Interaction/albaInteractionFactory.h
  ../Interaction/albaInteractionManager.cpp
  ../Interaction/albaInteractionManager.h
  ../Interaction/albaInteractor.cpp
  ../Interaction/albaInteractor.h
  ../Interaction/albaRefSys.cpp
  ../Interaction/albaRefSys.h
  ../Interaction/albaDeviceButtonsPad.cpp
  ../Interaction/albaDeviceButtonsPad.h
  ../Interaction/albaDeviceButtonsPadMouse.cpp
  ../Interaction/albaDeviceButtonsPadMouse.h
  ../Interaction/albaDeviceButtonsPadMouseRemote.cpp
  ../Interaction/albaDeviceButtonsPadMouseRemote.h
  ../Interaction/albaDeviceButtonsPadTracker.cpp
  ../Interaction/albaDeviceButtonsPadTracker.h
  ../Interaction/albaInteractor2DAngle.cpp
  ../Interaction/albaInteractor2DAngle.h
  ../Interaction/albaInteractor2DDistance.cpp
  ../Interaction/albaInteractor2DDistance.h
  ../Interaction/albaInteractor2DIndicator.cpp
  ../Interaction/albaInteractor2DIndicator.h
  ../Interaction/albaInteractor6DOF.cpp
  ../Interaction/albaInteractor6DOF.h
  ../Interaction/albaInteractor6DOFCameraMove.cpp
  ../Interaction/albaInteractor6DOFCameraMove.h
  ../Interaction/albaInteractorCameraMove.cpp
  ../Interaction/albaInteractorCameraMove.h
  ../Interaction/albaInteractorCompositorMouse.cpp
  ../Interaction/albaInteractorCompositorMouse.h
  ../Interaction/albaInteractorConstraint.cpp
  ../Interaction/albaInteractorConstraint.h
  ../Interaction/albaInteractorExtractIsosurface.cpp
  ../Interaction/albaInteractorExtractIsosurface.h
  ../Interaction/albaInteractorGeneric6DOF.cpp
  ../Interaction/albaInteractorGeneric6DOF.h
  ../Interaction/albaInteractorGenericInterface.cpp
  ../Interaction/albaInteractorGenericInterface.h
  ../Interaction/albaInteractorGenericMouse.cpp
  ../Interaction/albaInteractorGenericMouse.h
  ../Interaction/albaInteractorPER.cpp
  ../Interaction/albaInteractorPER.h
  ../Interaction/albaInteractorPicker.cpp
  ../Interaction/albaInteractorPicker.h
  ../Interaction/albaInteractorSelectCell.cpp
  ../Interaction/albaInteractorSelectCell.h
  ../Interaction/albaInteractorSER.cpp
  ../Interaction/albaInteractorSER.h
  ../Interaction/albaDeviceClientALBA.cpp
  ../Interaction/albaDeviceClientALBA.h
)

IF (ALBA_USE_P5_GLOVE)
  SET(INTERACTION_SRCS
    ${INTERACTION_SRCS}
    ../Interaction/albaDeviceButtonsPadTrackerP5Glove.cpp
    ../Interaction/albaDeviceButtonsPadTrackerP5Glove.h
  )
ENDIF (ALBA_USE_P5_GLOVE)

IF (ALBA_USE_WIIMOTE)
  SET(INTERACTION_SRCS
    ${INTERACTION_SRCS}
    ../Interaction/albaDeviceTrackerWIIMote.cpp
    ../Interaction/albaDeviceTrackerWIIMote.h
  )
ENDIF (ALBA_USE_WIIMOTE)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/../Interaction/albaInteractionConfigure.h.in ${PROJECT_BINARY_DIR}/../Interaction/albaInteractionConfigure.h IMMEDIATE)

###############################################
#IO
###############################################
SET(IO_SRCS
  ../IO/albaCurlUtility.cpp
  ../IO/albaCurlUtility.h
  ../IO/albaStorable.cpp
  ../IO/albaStorable.h
  ../IO/albaStorage.cpp
  ../IO/albaStorage.h
  ../IO/albaStorageElement.cpp
  ../IO/albaStorageElement.h
  ../IO/mmuDOMTreeErrorReporter.h
  ../IO/mmuDOMTreeErrorReporter.cpp
)

IF (ALBA_USE_XercesC)
  SET(IO_SRCS
    ${IO_SRCS}
    ../IO/albaXMLElement.cpp
    ../IO/albaXMLElement.h
    ../IO/albaXMLStorage.cpp
    ../IO/albaXMLStorage.h
    ../IO/albaXMLString.cpp
    ../IO/albaXMLString.h
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
#OPERATIONS
###############################################
IF (ALBA_USE_VTK)
  SET(OPERATIONS_SRCS
    ${OPERATIONS_SRCS}
    ../Operations/albaOpDecomposeTimeVarVME.cpp
    ../Operations/albaOpDecomposeTimeVarVME.h
    ../Operations/albaOpImporterMSF.cpp
    ../Operations/albaOpImporterMSF.h
    ../Operations/albaOpImporterExternalFile.cpp
    ../Operations/albaOpImporterExternalFile.h
    ../Operations/albaOpLabelExtractor.cpp
    ../Operations/albaOpLabelExtractor.h
    ../Operations/albaOpOpenExternalFile.cpp
    ../Operations/albaOpOpenExternalFile.h
    ../Operations/albaOp2DMeasure.cpp
    ../Operations/albaOp2DMeasure.h
    ../Operations/albaOpAddLandmark.cpp
    ../Operations/albaOpAddLandmark.h
    ../Operations/albaOpExporterBmp.cpp
    ../Operations/albaOpExporterBmp.h
    ../Operations/albaOpClipSurface.cpp
    ../Operations/albaOpClipSurface.h
    ../Operations/albaOpCreateGroup.cpp
    ../Operations/albaOpCreateGroup.h
    ../Operations/albaOpCreateGroup.h
    ../Operations/albaOpCreateMeter.cpp
    ../Operations/albaOpCreateMeter.h
    ../Operations/albaOpCreateSurfaceParametric.cpp
    ../Operations/albaOpCreateSurfaceParametric.h
    ../Operations/albaOpCreateProber.cpp
    ../Operations/albaOpCreateProber.h
    ../Operations/albaOpCreateRefSys.cpp
    ../Operations/albaOpCreateRefSys.h
    ../Operations/albaOpCreateSlicer.cpp
    ../Operations/albaOpCreateSlicer.h
    ../Operations/albaOpCreateSpline.cpp
    ../Operations/albaOpCreateSpline.h
    ../Operations/albaOpCreateVolume.cpp
    ../Operations/albaOpCreateVolume.h
    ../Operations/albaOpRemoveCells.cpp
    ../Operations/albaOpRemoveCells.h
    ../Operations/albaOpEditMetadata.cpp
    ../Operations/albaOpEditMetadata.h
    ../Operations/albaOpExplodeCollapse.cpp
    ../Operations/albaOpExplodeCollapse.h
    ../Operations/albaOpExtractIsosurface.cpp
    ../Operations/albaOpExtractIsosurface.h
    ../Operations/albaOpFilterSurface.cpp
    ../Operations/albaOpFilterSurface.h
    ../Operations/albaOpDecimateSurface.cpp
    ../Operations/albaOpDecimateSurface.h
    ../Operations/albaOpConnectivitySurface.cpp
    ../Operations/albaOpConnectivitySurface.h
    ../Operations/albaOpFilterVolume.cpp
    ../Operations/albaOpFilterVolume.h
    ../Operations/albaOpImporterImage.cpp
    ../Operations/albaOpImporterImage.h
    ../Operations/albaOpALBATransform.cpp
    ../Operations/albaOpALBATransform.h
    ../Operations/albaOpExporterMSF.cpp
    ../Operations/albaOpExporterMSF.h
    ../Operations/albaOpImporterMSF1x.cpp
    ../Operations/albaOpImporterMSF1x.h
    ../Operations/albaOpExporterRaw.cpp
    ../Operations/albaOpExporterRaw.h
    ../Operations/albaOpImporterRAWVolume.cpp
    ../Operations/albaOpImporterRAWVolume.h
    ../Operations/albaOpReparentTo.cpp
    ../Operations/albaOpReparentTo.h
    ../Operations/albaOpExporterSTL.cpp
    ../Operations/albaOpExporterSTL.h
    ../Operations/albaOpImporterSTL.cpp
    ../Operations/albaOpImporterSTL.h
    ../Operations/albaOpTransformInterface.cpp
    ../Operations/albaOpTransformInterface.h
    ../Operations/albaOpValidateTree.cpp
    ../Operations/albaOpValidateTree.h
    ../Operations/albaOpVolumeResample.cpp
    ../Operations/albaOpVolumeResample.h
    ../Operations/albaOpVOIDensity.cpp
    ../Operations/albaOpVOIDensity.h
    ../Operations/albaOpVOIDensityEditor.cpp
    ../Operations/albaOpVOIDensityEditor.h
    ../Operations/albaOpImporterVRML.cpp
    ../Operations/albaOpImporterVRML.h
    ../Operations/albaOpExporterVTK.cpp
    ../Operations/albaOpExporterVTK.h
    ../Operations/albaOpImporterVTK.cpp
    ../Operations/albaOpImporterVTK.h
    ../Operations/albaOpCrop.cpp
    ../Operations/albaOpCrop.h
    ../Operations/albaOpBooleanSurface.cpp
    ../Operations/albaOpBooleanSurface.h
    ../Operations/albaOpEditNormals.cpp
    ../Operations/albaOpEditNormals.h
    ../Operations/albaOpApplyTrajectory.cpp
    ../Operations/albaOpApplyTrajectory.h
    ../Operations/albaOpGarbageCollectMSFDir.cpp
    ../Operations/albaOpGarbageCollectMSFDir.h
  )
ENDIF (ALBA_USE_VTK)

IF (ALBA_USE_ITK)
  SET(OPERATIONS_SRCS
    ${OPERATIONS_SRCS}
    ../Operations/albaASCIIImporterUtility.cpp
    ../Operations/albaASCIIImporterUtility.h
    ../Operations/albaOpImporterASCII.cpp
    ../Operations/albaOpImporterASCII.h
    ../Operations/albaVMEDataSetAttributesImporter.cpp
    ../Operations/albaVMEDataSetAttributesImporter.h
    ../Operations/albaVMEMeshAnsysTextImporter.cpp
    ../Operations/albaVMEMeshAnsysTextImporter.h
    ../Operations/albaVMEMeshAnsysTextExporter.cpp
    ../Operations/albaVMEMeshAnsysTextExporter.h
    ../Operations/albaParabolicMeshToLinearMeshFilter.cxx
    ../Operations/albaParabolicMeshToLinearMeshFilter.h
    ../Operations/albaOpImporterMesh.cpp
    ../Operations/albaOpImporterMesh.h
    ../Operations/albaOpExporterMesh.cpp
    ../Operations/albaOpExporterMesh.h
    ../Operations/albaOpScalarToSurface.cpp
    ../Operations/albaOpScalarToSurface.h
    ../Operations/albaOpImporterVMEDataSetAttributes.cpp
    ../Operations/albaOpImporterVMEDataSetAttributes.h
  )
ENDIF (ALBA_USE_ITK)
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
    #../Interaction/Drivers/LEDPlayer.h
    #../Interaction/Drivers/LEDPlayer.cpp
    #../Interaction/Drivers/Resource.h
    ../Interaction/Drivers/setupapi.h
    ../Interaction/Drivers/Utils.h
    ../Interaction/Drivers/Wiimote.h
    ../Interaction/Drivers/Wiimote.cpp
  )
ENDIF (ALBA_USE_WIIMOTE)
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
  INCLUDE (${ALBA_SOURCE_DIR}/modules/FindGTK2.cmake)
  INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIRECTORIES})
ENDIF (NOT WIN32)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

IF (ALBA_USE_VTK)
  # SET(PROJECT_LIBS ${PROJECT_LIBS} vtkCommon vtkIO vtkRendering vtkGraphics vtkHybrid)
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

