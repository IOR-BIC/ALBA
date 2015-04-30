/*=========================================================================

 Program: MAF2
 Module: mafViewArbitraryOrthoSlice
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 


#ifndef __mafViewArbitraryOrthoSlice_H__
#define __mafViewArbitraryOrthoSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewCompoundWindowing.h"
#include "mafVMESurface.h"
#include "mafGizmoInterface.h"
#include "vtkActor2D.h"
#include "vtkTextMapper.h"
#include "vtkCubeSource.h"
#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkTubeFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLineSource.h"
#include "mafPipeSurface.h"
#include "mafInteractorPicker.h"
#include "medInteractorPicker.h"
#include "vtkCaptionActor2D.h"

using namespace std;

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafViewVTK;
class mafVMEVolumeGray;
class mafVMESlicer;
class mafMatrix;
class mafAttachCamera;
class mafGUI;
class mafGUILutSlider;
class mafGUILutSwatch;
class mafVMEPolylineEditor;
class mafGizmoCrossRotateTranslate;

/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!WORK IN PROGRESS: DO NOT USE!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Class Name: mafViewArbitraryOrthoSlice.

OrthoSlice-ArbitrarySlice mix early prototype.

This compound view features three volume slice planes that can be controlled
by using three roto translation gizmos.

An RX projection can be performed on each axis (currently available for structured points only)

It is possible to export arbitrary volume slices as image files.

@sa mafGizmoCrossRotateTranslate , mafVMESlicer


*/

class MAF_EXPORT mafViewArbitraryOrthoSlice: public mafViewCompoundWindowing
{

	/*
	GC : Gizmo Cross

	P      |   Zn
	|   GC_Zn
	--------------------
	Xn     |   Yn
	GC_Xn  |   GC_Yn


	Y
	V
	|      <X
	----------------------
	Z    |   Z 
	V        V
	Y>              <X



	P        ->x
	|	
	Vy
	|
	-------------
	|   
	/\Z        /\Z 
	|	       |
	-->y       --->X



	*/
public:
	/** constructor*/
	mafViewArbitraryOrthoSlice(wxString label = "View Arbitrary OrthoSlice with Windowing", bool show_ruler = false);

	/** destructor*/
	virtual ~mafViewArbitraryOrthoSlice(); 

	/** RTTI macro.*/
	mafTypeMacro(mafViewArbitraryOrthoSlice, mafViewCompoundWindowing);

	enum ID_GUI
	{
		ID_COMBO_CHOOSE_EXPORT_AXIS = Superclass::ID_LAST,
		ID_RESET,
		ID_SHOW_GIZMO,
		ID_CHOOSE_DIR,
		ID_EXPORT,

		ID_UPDATE_LUT, 

		ID_ENABLE_THICKNESS_RED,
		ID_ENABLE_THICKNESS_ACTORS_RED,
		ID_THICKNESS_VALUE_CHANGED_RED,
		ID_NUMBER_OF_AXIAL_SECTIONS_RED,
		ID_EXPORT_PLANES_HEIGHT_RED,
		ID_ENABLE_EXPORT_IMAGES_RED,

		ID_ENABLE_THICKNESS_GREEN,
		ID_ENABLE_THICKNESS_ACTORS_GREEN,
		ID_THICKNESS_VALUE_CHANGED_GREEN,
		ID_NUMBER_OF_AXIAL_SECTIONS_GREEN,
		ID_EXPORT_PLANES_HEIGHT_GREEN,
		ID_ENABLE_EXPORT_IMAGES_GREEN,

		ID_ENABLE_THICKNESS_BLUE,
		ID_ENABLE_THICKNESS_ACTORS_BLUE,
		ID_THICKNESS_VALUE_CHANGED_BLUE,
		ID_NUMBER_OF_AXIAL_SECTIONS_BLUE,
		ID_EXPORT_PLANES_HEIGHT_BLUE,
		ID_ENABLE_EXPORT_IMAGES_BLUE,

		ID_SHOW_RULER,
		ID_HIDE_RULER,
		ID_LAST,
	};

	/** Create visual pipe and initialize them to build an OrthoSlice visualization */
	virtual void PackageView();

	/** Show/Hide VMEs into plugged sub-views*/
	virtual void VmeShow(mafNode *node, bool show);


	/** Remove VME into plugged sub-views*/
	virtual void VmeRemove(mafNode *node);

	/** Create the GUI at the bottom of the compounded view. */
	virtual void CreateGuiView();

	/** Camera update */
	void CameraUpdate();

	/** Function that handles events sent from other objects. */
	virtual void OnEvent(mafEventBase *maf_event);

	void MyMethod( medInteractorPicker * picker, double * pickedPointCoordinates );
	void OnEventGizmoCrossRTZNormalView( mafEventBase * maf_event );
	void OnEventGizmoCrossRTYNormalView( mafEventBase * maf_event );

	/** Function that clones instance of the object. */
	virtual mafView* Copy(mafObserver *Listener, bool lightCopyEnabled = false);

	/** Force the updating of the camera. */
	virtual void UpdateSubviewsCamerasToFaceSlices();

	virtual void OnLayout();

	void OnLayoutInternal( wxSize &windowSize );

	void VmeSelect(mafNode *node, bool select);

protected:

	vtkActor2D *m_XnThicknessTextActor;
	vtkTextMapper *m_XnThicknessTextMapper;

	vtkActor2D *m_YnThicknessTextActor;
	vtkTextMapper *m_YnThicknessTextMapper;

	vtkActor2D *m_ZnThicknessTextActor;
	vtkTextMapper *m_ZnThicknessTextMapper;

	vtkActor2D *m_TextActorLeftXView;
	vtkTextMapper *m_TextMapperLeftXView;

	vtkActor2D *m_TextActorRightXView;
	vtkTextMapper *m_TextMapperRightXView;

	vtkActor2D *m_TextActorLeftYView;
	vtkTextMapper *m_TextMapperLeftYView;

	vtkActor2D *m_TextActorRightYView;
	vtkTextMapper *m_TextMapperRightYView;

	vtkActor2D *m_TextActorLeftZView;
	vtkTextMapper *m_TextMapperLeftZView;

	vtkActor2D *m_TextActorRightZView;
	vtkTextMapper *m_TextMapperRightZView;

	vtkActor2D *m_XnSliceHeightTextActor;
	vtkTextMapper *m_XnSliceHeightTextMapper;

	vtkActor2D *m_YnSliceHeightTextActor;
	vtkTextMapper *m_YnSliceHeightTextMapper;

	vtkActor2D *m_ZnSliceHeightTextActor;
	vtkTextMapper *m_ZnSliceHeightTextMapper;

	bool BelongsToXNormalGizmo( mafVME * vme );
	bool BelongsToYNormalGizmo( mafVME * vme );
	bool BelongsToZNormalGizmo( mafVME * vme );

	void ShowVMESurfacesAndLandmarks( mafNode * node );

	void ShowMafVMEImage( mafVME * vme );
	void HideMafVmeImage();

	void ShowMafVMEVolume( mafVME * vme, bool show );

	void ShowThickness2DTextActors( bool show , int color);

	void StoreCameraParametersForAllSubviews();

	void Update2DActors();

	void ShowSlicers( mafVME * vmeVolume, bool show );

	void BuildSliceHeightFeedbackLinesVMEs();
	void AddVMEToMSFTree(mafVMESurface *vme);

	enum VIEW_DIRECTION {FROM_X = 0, FROM_Y = 1, FROM_Z = 2};
	void ShowPlaneFeedbackLine(int viewDirection, vtkMatrix4x4 *outputMatrix);

	void BuildSlicingPlane( mafVMESurface *inVME, int fromDirection, int guestView,  
		double sliceHeight, mafVMESlicer* targetSlicer = NULL,
    vtkMatrix4x4 * outputMatrix = NULL , bool showHeightText = false, vtkCaptionActor2D *captionActor = NULL);

  void ShowCaptionActorVector( vector<vtkCaptionActor2D *> textActorsVector , int view );
  void ShowCaptionActor(vtkCaptionActor2D *actor, int guestView, wxString text, double x, double y ,double z);

  void ShowVTKDataAsVMESurface( vtkPolyData *vmeVTKData, 
		mafVMESurface *vmeSurface, vtkMatrix4x4 *inputABSMatrix);

	void ResetCameraToSlices();

	/** 
	Create the helper cone giving feedback for camera direction*/
	void BuildXCameraConeVME();
	void BuildYCameraConeVME();
	void BuildZCameraConeVME();

	void HideMafVMEVolume();

	void ShowMafVMEMesh( mafNode * node );

	void ShowmafVMEPolylineEditor( mafNode * node );

	/* Update slicer settings according to m_CurrentVolume*/
	void UpdateSlicerZBehavior();

	/** Enable picking on slicers */
	void EnableSlicersPicking(bool enable);

	/** Internally used to create a new instance of the GUI. 
	This function should be overridden by subclasses to create specialized GUIs. 
	Each subclass should append its own widgets and define the enum of IDs for the widgets as 
	an extension of the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
	subclass to continue the ID enumeration from it. For appending the widgets in the
	same panel GUI, each CreateGUI() function should first call the superclass' one.*/
	virtual mafGUI* CreateGui();

	/** Enable/disable view widgets.*/
	void EnableWidgets(bool enable = true);

	void EnableExportImages( bool enable , int color);
	void OnEventThis(mafEventBase *maf_event);  

	void ExportBLUEView();

	void ExportGREENView();

	void ExportREDView();

	enum {
		X_RULER=0,
		Y_RULER=1,
		Z_RULER=2,
	};

	void ShowRuler( int ruler , bool show);

	void OnLUTChooser();
	void OnLUTRangeModified();

	void UpdateSlicersLUT();

	void OnReset();

	void RestoreCameraParametersForAllSubviews();

	void OnResetmafVMEPolylineEditor();
	void OnResetMafVMEMesh( mafNode * node );
	void OnResetSurfaceAndLandmark( mafNode * node );

	/** This function is called when a rotate gizmo is moved*/
	void OnEventGizmoCrossRotateZNormalView(mafEventBase *maf_event);

	/** Update export images bounds lines actors only if needed*/
	void UpdateExportImagesBoundsLineActors();

	void UpdateXView2DActors();
	void UpdateYView2DActors();

	void UpdateYnViewZPlanes();

	void ShowRulerVMEVector(vector<mafVMESurface *> &rulerVector, 
  vector<vtkCaptionActor2D *> &captionActorVector,
  int fromDirection, int guestView);

	void HideRulerVMEVector(vector<mafVMESurface *> &rulerVector, 
  vector<vtkCaptionActor2D *> &captionActorVector, 
  int guestView);

	void UpdateZView2DActors();

	/**	This function is called when a translate gizmo is moved*/
	void OnEventGizmoCrossTranslateZNormalView(mafEventBase *maf_event);

	void PostMultiplyEventMatrixToGizmoCross( mafEventBase * inputEvent , mafGizmoCrossRotateTranslate *targetGizmo);

	/** Recalculate the RX projection for the three slicers and display it */
	void UpdateAllViewsThickness();

	/** structured points only: create rx projection for the given slicer normal and set it as slicer texture.	
	If rxTexture is provided result of the accumulation will be deepcopied to it */
	void AccumulateTextures(mafVMESlicer *inSlicer, double inRXThickness , vtkImageData *outRXTexture = NULL , bool showProgressBar = false);

	/** Post multiply maf_event matrix to given slicer */
	void PostMultiplyEventMatrixToSlicer(mafEventBase *maf_event, int slicerAxis);

	/** Post multiply maf_event matrix to the 3 slicers */
	void PostMultiplyEventMatrixToSlicers(mafEventBase *maf_event);

	/** Windowing for volumes data. This function overrides superclass method.*/
	void VolumeWindowing(mafVME *volume);
	void OnEventGizmoCrossRotateYNormalView(mafEventBase *maf_event);

	void GetLeftRightLettersFromCamera( double viewUp[3], double viewPlaneNormal[3], wxString &leftLetter, wxString &rightLetter);

	void OnEventGizmoCrossRTXNormalView( mafEventBase * maf_event );

	void OnEventGizmoCrossRotateXNormalView(mafEventBase *maf_event);

	void ChildViewsCameraUpdate();

	void OnEventGizmoCrossTranslateXNormalView(mafEventBase *maf_event);
	void OnEventGizmoCrossTranslateYNormalView(mafEventBase *maf_event);

	void UpdateXnViewZPlanes();
	void UpdateYnViewXPlanes();
	void UpdateXnViewYPlanes();
	void UpdateZnViewYPlanes();
	void UpdateZnViewXPlanes();

	void UpdateXCutPlanes();
	void ShowXCutPlanes( bool show );

	void ShowVMESurfacesVector( vector<mafVMESurface *> &inVector, int view, bool show );

	void UpdateZCutPlanes();
	void ShowZCutPlanes(bool show);

	void UpdateYCutPlanes();
	void ShowYCutPlanes( bool show );

	mafPipeSurface * GetPipe(int inView, mafVMESurface *inSurface);

	void HideAllCutPlanes();
	void OnID_CHOOSE_DIR();
	void OnEventID_ENABLE_EXPORT_IMAGES(int axis);

	void ShowCutPlanes( int axis , bool show);

	/** Write on file the texture slice from the selected view */
	void SaveSlicesTextureToFile(int choosedExportAxis);

	/** Write on file the full render window from the selected view */
	void SaveSlicesFromRenderWindowToFile(int chooseExportAxis);

	void OnEventID_ENABLE_THICKNESS(int color);

	void EnableThickness(bool enable, int color);

	void EnableThicknessGUI( bool enable , int color);

	/** Recompute all slicers output */
	void UpdateSlicers(int axis);

	void OnEventID_THICKNESS_VALUE_CHANGED(int color);
	void OnEventID_EXPORT_PLANES_HEIGHT(int color);

	void CreateViewCameraNormalFeedbackActor(double col[3], int view);
	void DestroyViewCameraNormalFeedbackActor(int view);
	void ThicknessComboAssignment(int axis);
	void UpdateWindowing(bool enable,mafNode *node);
	void UpdateThickness( int axis);
	void ShowSliceHeight2DTextActors( bool show , int color);
  
	mafViewVTK *m_ViewSliceX;
	mafViewVTK *m_ViewSliceY;
	mafViewVTK *m_ViewSliceZ;

	mafViewVTK *m_ViewArbitrary;

	mafVME          	*m_CurrentVolume;
	mafVME				    *m_CurrentImage;

	mafVMESlicer			*m_SlicerX;
	mafVMESlicer			*m_SlicerY;
	mafVMESlicer			*m_SlicerZ;

	mafMatrix					*m_SlicerXResetMatrix;
	mafMatrix					*m_SlicerYResetMatrix;
	mafMatrix					*m_SlicerZResetMatrix;

	mafAttachCamera		*m_AttachCameraToSlicerXInXView;
	mafAttachCamera		*m_AttachCameraToSlicerYInYView;
	mafAttachCamera		*m_AttachCameraToSlicerZInZView;

	double	m_VolumeVTKDataCenterABSCoords[3];
	double	m_VolumeVTKDataCenterABSCoordinatesReset[3];
	double	m_VolumeVTKDataABSOrientation[3];
	int			m_ComboChooseActiveGizmo;

	int m_ShowGizmo;
	mafGUI				*m_GuiGizmos;

	mafVMEPolylineEditor *m_CurrentPolylineGraphEditor;
	mafVMESurface *m_XCameraConeVME;
	mafVMESurface *m_YCameraConeVME;
	mafVMESurface *m_ZCameraConeVME;

	double m_XCameraPositionForReset[3];
	double m_YCameraPositionForReset[3];
	double m_ZCameraPositionForReset[3];

	double m_XCameraFocalPointForReset[3];
	double m_YCameraFocalPointForReset[3];
	double m_ZCameraFocalPointForReset[3];

	double m_XCameraViewUpForReset[3];
	double m_YCameraViewUpForReset[3];
	double m_ZCameraViewUpForReset[3];

	mafGizmoCrossRotateTranslate *m_GizmoZView;
	mafGizmoCrossRotateTranslate *m_GizmoYView;
	mafGizmoCrossRotateTranslate *m_GizmoXView;

	enum AXIS {X = 0, Y = 1, Z = 2};

	mafVMEVolumeGray *m_InputVolume;

	// Xn view cut feedback gizmos
	vector<mafVMESurface *> m_ViewXnSliceYBoundsVMEVector;
  vector<mafVMESurface *> m_ViewXnSliceYRulerVMEVector;
  vector<vtkCaptionActor2D *> m_ViewXnSliceYRulerTextActorsVector;
  
	vector<mafVMESurface *> m_ViewXnSliceZBoundsVMEVector;
	vector<mafVMESurface *> m_ViewXnSliceZRulerVMEVector;
  vector<vtkCaptionActor2D *> m_ViewXnSliceZRulerTextActorsVector;

	// Yn view cut feedback gizmos
	vector<mafVMESurface *> m_ViewYnSliceZBoundsVMEVector;
  vector<mafVMESurface *> m_ViewYnSliceZRulerVMEVector;
  vector<vtkCaptionActor2D *> m_ViewYnSliceZRulerTextActorsVector;

	vector<mafVMESurface *> m_ViewYnSliceXBoundsVMEVector;
	vector<mafVMESurface *> m_ViewYnSliceXRulerVMEVector;
  vector<vtkCaptionActor2D *> m_ViewYnSliceXRulerTextActorsVector;

	// Zn view cut feedback gizmos
	vector<mafVMESurface *> m_ViewZnSliceXBoundsVMEVector;
	vector<mafVMESurface *> m_ViewZnSliceXRulerVMEVector;
  vector<vtkCaptionActor2D *> m_ViewZnSliceXRulerTextActorsVector;

	vector<mafVMESurface *> m_ViewZnSliceYBoundsVMEVector;
	vector<mafVMESurface *> m_ViewZnSliceYRulerVMEVector;
  vector<vtkCaptionActor2D *> m_ViewZnSliceYRulerTextActorsVector;

	int m_FeedbackLineHeight[3];
	int m_NumberOfAxialSections[3];
	int m_ExportPlanesHeight[3];

	int m_EnableExportImages[3];
	
	// Enable RX like accumulation: corresponding point scalar values are added and the resulting number is divided
	// by the number of slices 
	int m_EnableThickness[3]; 

	double m_ThicknessValue[3];
	int m_ThicknessComboAssignment[3];

	wxString m_PathFromDialog;

	/** Enable debug facilities */
	bool m_DebugMode;

	medInteractorPicker *m_XSlicerPicker;
	medInteractorPicker *m_YSlicerPicker;
	medInteractorPicker *m_ZSlicerPicker;

	mafString m_ThicknessText[3];

	enum {RED=0, GREEN, BLUE, NUMBER_OF_COLORS =3};
};

#endif
