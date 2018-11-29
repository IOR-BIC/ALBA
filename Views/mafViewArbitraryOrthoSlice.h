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
#include "vtkMAFTubeFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLineSource.h"
#include "mafPipeSurface.h"
#include "mafInteractorPicker.h"
#include "mafInteractorCameraPicker.h"
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


/*
OrthoSlice-ArbitrarySlice

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
	mafViewArbitraryOrthoSlice(wxString label = "View Arbitrary OrthoSlice with Windowing");

	/** destructor*/
	virtual ~mafViewArbitraryOrthoSlice(); 

	/** RTTI macro.*/
	mafTypeMacro(mafViewArbitraryOrthoSlice, mafViewCompoundWindowing);

	enum ID_GUI
	{
		ID_COMBO_CHOOSE_EXPORT_AXIS = Superclass::ID_LAST,
		ID_RESET,
		ID_SHOW_GIZMO,
		ID_UPDATE_LUT, 
		ID_LAST,
	};

	/** Create visual pipe and initialize them to build an OrthoSlice visualization */
	virtual void PackageView();

	void CreateAndPlugSliceView(int v);

	/** Show/Hide VMEs into plugged sub-views*/
	virtual void VmeShow(mafVME *vme, bool show);

	/** Remove VME into plugged sub-views*/
	virtual void VmeRemove(mafVME *vme);

	/** Create the GUI at the bottom of the compounded view. */
	virtual void CreateGuiView();

	/** Camera update */
	void CameraUpdate();

	/** Function that handles events sent from other objects. */
	virtual void OnEvent(mafEventBase *maf_event);

	/** Function that clones instance of the object. */
	virtual mafView* Copy(mafObserver *Listener, bool lightCopyEnabled = false);

	/** Force the updating of the camera. */
	virtual void UpdateSubviewsCamerasToFaceSlices();

protected:
	
	bool BelongsToNormalGizmo( mafVME * vme, int side );

	void ShowVolume( mafVME * vme, bool show );
		
	void StoreCameraParametersForAllSubviews();

	void ShowSlicers( mafVME * vmeVolume, bool show );

	void BuildSliceHeightFeedbackLinesVMEs();
	void AddVMEToMSFTree(mafVMESurface *vme);

	enum VIEW_DIRECTION {FROM_X = 0, FROM_Y = 1, FROM_Z = 2};
		
  void ShowVTKDataAsVMESurface( vtkPolyData *vmeVTKData, 
		mafVMESurface *vmeSurface, vtkMatrix4x4 *inputABSMatrix);

	void ResetCameraToSlices();

	/** 
	Create the helper cone giving feedback for camera direction*/
	void BuildXCameraConeVME();
	void BuildYCameraConeVME();
	void BuildZCameraConeVME();

	void HideVolume();

		
	/** Internally used to create a new instance of the GUI. 
	This function should be overridden by subclasses to create specialized GUIs. 
	Each subclass should append its own widgets and define the enum of IDs for the widgets as 
	an extension of the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
	subclass to continue the ID enumeration from it. For appending the widgets in the
	same panel GUI, each CreateGUI() function should first call the superclass' one.*/
	virtual mafGUI* CreateGui();

	/** Enable/disable view widgets.*/
	void EnableWidgets(bool enable = true);

	void OnEventThis(mafEventBase *maf_event);  

	
	void OnLUTChooser();
	void OnLUTRangeModified();

	void UpdateSlicersLUT();

	void OnReset();

	void RestoreCameraParametersForAllSubviews();

	/**	This function is called when a translate gizmo is moved*/
	void OnEventGizmoTranslate(mafEventBase *maf_event, int side);

	void PostMultiplyEventMatrixToGizmoCross( mafEventBase * inputEvent , mafGizmoCrossRotateTranslate *targetGizmo);
	
	/** Post multiply maf_event matrix to given slicer */
	void PostMultiplyEventMatrixToSlicer(mafEventBase *maf_event, int slicerAxis);

	/** Post multiply maf_event matrix to the 3 slicers */
	void PostMultiplyEventMatrixToSlicers(mafEventBase *maf_event);

	/** Windowing for volumes data. This function overrides superclass method.*/
	void VolumeWindowing(mafVME *volume);
	void OnEventGizmoRotate(mafEventBase *maf_event, int side);

	void SetSlices();

	void ChildViewsCameraUpdate();
		
	mafPipeSurface * GetPipe(int inView, mafVMESurface *inSurface);
			
	/** Recompute all slicers output */
	void UpdateSlicers(int axis);

	void CreateViewCameraNormalFeedbackActor(double col[3], int view);
	void UpdateWindowing(bool enable,mafVME *vme);
	
	mafViewVTK *m_ViewSlice[3];
	mafViewVTK *m_View3d;

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
	vector<mafVMESurface *> m_ViewXnSliceZBoundsVMEVector;

	// Yn view cut feedback gizmos
	vector<mafVMESurface *> m_ViewYnSliceZBoundsVMEVector;
	vector<mafVMESurface *> m_ViewYnSliceXBoundsVMEVector;

	// Zn view cut feedback gizmos
	vector<mafVMESurface *> m_ViewZnSliceXBoundsVMEVector;
	vector<mafVMESurface *> m_ViewZnSliceYBoundsVMEVector;

	int m_FeedbackLineHeight[3];
	int m_NumberOfAxialSections[3];
		
	/** Enable debug facilities */
	bool m_DebugMode;
	
	enum {RED=0, GREEN, BLUE, NUMBER_OF_COLORS =3};
};

#endif
