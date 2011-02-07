/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medViewArbitraryOrthoSlice.h,v $
Language:  C++
Date:      $Date: 2011-02-07 18:09:58 $
Version:   $Revision: 1.1.2.20 $
Authors:   Stefano Perticoni	
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
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


#ifndef __medViewArbitraryOrthoSlice_H__
#define __medViewArbitraryOrthoSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medViewCompoundWindowing.h"
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
class medVMEPolylineEditor;
class medGizmoCrossRotateTranslate;

/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!WORK IN PROGRESS: DO NOT USE!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Class Name: medViewArbitraryOrthoSlice.

OrthoSlice-ArbitrarySlice mix early prototype.
*/

class medViewArbitraryOrthoSlice: public medViewCompoundWindowing
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
	medViewArbitraryOrthoSlice(wxString label = "View Arbitrary Slice with Windowing", bool show_ruler = false);

	/** destructor*/
	virtual ~medViewArbitraryOrthoSlice(); 

	/** RTTI macro.*/
	mafTypeMacro(medViewArbitraryOrthoSlice, medViewCompoundWindowing);

	enum ID_GUI
	{
		ID_COMBO_CHOOSE_EXPORT_AXIS = Superclass::ID_LAST,
		ID_RESET,
		ID_SHOW_GIZMO,
    ID_ENABLE_EXPORT_IMAGES,
    ID_CHOOSE_DIR,
    ID_EXPORT,

	ID_ENABLE_THICKNESS,
	ID_ENABLE_THICKNESS_ACTORS,
	ID_THICKNESS_VALUE_CHANGED,
	ID_NUMBER_OF_AXIAL_SECTIONS,
	ID_EXPORT_PLANES_HEIGHT,
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

	void OnEventGizmoCrossRTZNormalView( mafEventBase * maf_event );
	void OnEventGizmoCrossRTYNormalView( mafEventBase * maf_event );

	/** Function that clones instance of the object. */
	virtual mafView* Copy(mafObserver *Listener, bool lightCopyEnabled = false);

	/** Force the updating of the camera. */
	virtual void UpdateSubviewsCamerasToFaceSlices();

	virtual void OnLayout();

	void OnLayoutInternal( wxSize &windowSize );

protected:

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

	bool BelongsToXNormalGizmo( mafVME * vme );
	bool BelongsToYNormalGizmo( mafVME * vme );
	bool BelongsToZNormalGizmo( mafVME * vme );

	void ShowVMESurfacesAndLandmarks( mafNode * node );

	void ShowMafVMEImage( mafVME * vme );
	void HideMafVmeImage();

	void ShowMafVMEVolume( mafVME * vme, bool show );

	void StoreCameraParametersForAllSubviews();

	void Update2DActors();

	void ShowSlicers( mafVME * vmeVolume, bool show );

  void BuildSliceHeightFeedbackLinesVMEs();
  void AddVMEToMSFTree(mafVMESurface *vme);

	enum VIEW_DIRECTION {FROM_X = 0, FROM_Y = 1, FROM_Z = 2};
	void ShowPlaneFeedbackLine(int viewDirection, vtkMatrix4x4 *outputMatrix);

  void BuildSlicingPlane( mafVMESurface *inVME, int fromDirection, int guestView,  
    double sliceHeight, mafVMESlicer* targetSlicer = NULL, vtkMatrix4x4 * outputMatrix = NULL );
  
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

	void ShowMedVMEPolylineEditor( mafNode * node );


	/* Update slicer settings according to m_CurrentVolume*/
	void UpdateSlicerZBehavior();

	/** Internally used to create a new instance of the GUI. 
	This function should be overridden by subclasses to create specialized GUIs. 
	Each subclass should append its own widgets and define the enum of IDs for the widgets as 
	an extension of the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
	subclass to continue the ID enumeration from it. For appending the widgets in the
	same panel GUI, each CreateGUI() function should first call the superclass' one.*/
	virtual mafGUI* CreateGui();

	/** Enable/disable view widgets.*/
	void EnableWidgets(bool enable = true);

  void EnableExportImages( bool enable );
	void OnEventThis(mafEventBase *maf_event);  
	void OnLUTChooser();
	void OnRangeModified();

	void OnReset();

	void RestoreCameraParametersForAllSubviews();

	void OnResetMedVMEPolylineEditor();
	void OnResetMafVMEMesh( mafNode * node );
	void OnResetSurfaceAndLandmark( mafNode * node );

	/** This function is called when a rotate gizmo is moved*/
	void OnEventGizmoCrossRotateZNormalView(mafEventBase *maf_event);

	/** Update cut planes only if needed*/
    void UpdateCutPlanes();

    void UpdateXView2DActors();
	void UpdateYView2DActors();

  void UpdateYnViewZPlanes();
	void UpdateZView2DActors();

	void UpdateCameraXViewOnEventGizmoCrossRotateZNormal( mafEvent * event );
	void UpdateCameraYViewOnEventGizmoCrossRotateZNormal( mafEvent * event );
	void UpdateCameraZViewOnEventGizmoCrossRotateZNormal( mafEvent * event );

	/**	This function is called when a translate gizmo is moved*/
	void OnEventGizmoCrossTranslateZNormalView(mafEventBase *maf_event);

	void PostMultiplyEventMatrixToGizmoCross( mafEventBase * inputEvent , medGizmoCrossRotateTranslate *targetGizmo);

	/** Post multiply matrix for incoming transform events */
	void PostMultiplyEventMatrixToSlicers(mafEventBase *maf_event);

	/** Recalculate the RX projection for the thee slicers and display it */
	void UpdateThicknessStuff();

	/** structured points only */
	void Accumulate3TexturePlayGround();

	/** structured points only */
	void AccumulateNTextureFromThickness(mafVMESlicer *inputSlicer);

	void PostMultiplyEventMatrixToSlicer(mafEventBase *maf_event, int slicerAxis);

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

  void UpdateZCutPlanes();
  void ShowZCutPlanes(bool show);

  void UpdateYCutPlanes();
  void ShowYCutPlanes( bool show );

  mafPipeSurface * GetPipe(int inView, mafVMESurface *inSurface);

  void OnEventID_COMBO_CHOOSE_EXPORT_AXIS();

  void HideCutPlanes();
  void OnID_CHOOSE_DIR();
  void OnEventID_ENABLE_EXPORT_IMAGES();
  void OnEventID_EXPORT();
  void OnEventID_ENABLE_THICKNESS();

  void EnableThickness(bool enable);

  void EnableThicknessGUI( bool enable );
  
  /** Recompute all slicers output */
  void UpdateSlicers();

  void OnEventID_THICKNESS_VALUE_CHANGED();
  void OnEventID_EXPORT_PLANES_HEIGHT();
  
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

	medVMEPolylineEditor *m_CurrentPolylineGraphEditor;
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

	medGizmoCrossRotateTranslate *m_GizmoZView;
	medGizmoCrossRotateTranslate *m_GizmoYView;
	medGizmoCrossRotateTranslate *m_GizmoXView;

	enum AXIS {X = 0, Y = 1, Z = 2};

	mafVMEVolumeGray *m_InputVolume;


  // Xn view cut feedback gizmos
  mafVMESurface *m_ViewXnSliceYpVME;
  mafVMESurface *m_ViewXnSliceYmVME;
  
  mafVMESurface *m_ViewXnSliceZpVME;
  mafVMESurface *m_ViewXnSliceZmVME;

  // Yn view cut feedback gizmos
  mafVMESurface *m_ViewYnSliceZpVME;
  mafVMESurface *m_ViewYnSliceZmVME;

  mafVMESurface *m_ViewYnSliceXpVME;
  mafVMESurface *m_ViewYnSliceXmVME;

  // Zn view cut feedback gizmos
  mafVMESurface *m_ViewZnSliceXpVME;
  mafVMESurface *m_ViewZnSliceXmVME;

  mafVMESurface *m_ViewZnSliceYpVME;
  mafVMESurface *m_ViewZnSliceYmVME;


  int m_FeedbackLineHeight;
  int m_NumberOfAxialSections;
  int m_ExportPlanesHeight;

  int m_EnableExportImages;
  int m_ComboChooseExportAxis;
  int m_EnableThickness;
  
  double m_ThicknessValueChanged;
	
  wxString m_PathFromDialog;

  /** Enable debug facilities */
  bool m_DebugMode;
};

#endif
