/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewArbitraryOrthoSlice
 Authors: Stefano Perticoni, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaViewArbitraryOrthoSlice_H__
#define __albaViewArbitraryOrthoSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaViewCompoundWindowing.h"
#include "albaVMESurface.h"
#include "albaGizmoInterface.h"
#include "vtkActor2D.h"
#include "vtkTextMapper.h"
#include "vtkCubeSource.h"
#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkALBATubeFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLineSource.h"
#include "albaPipeSurface.h"
#include "albaInteractorPicker.h"
#include "albaInteractorCameraPicker.h"
#include "vtkCaptionActor2D.h"

using namespace std;

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaViewVTK;
class albaVMEVolumeGray;
class albaVMESlicer;
class albaMatrix;
class albaAttachCamera;
class albaGUI;
class albaGUILutSlider;
class albaGUILutSwatch;
class albaVMEPolylineEditor;
class albaGizmoCrossRotateTranslate;


/*
OrthoSlice-ArbitrarySlice

This compound view features three volume slice planes that can be controlled
by using three roto translation gizmos.

An RX projection can be performed on each axis (currently available for structured points only)

It is possible to export arbitrary volume slices as image files.

@sa albaGizmoCrossRotateTranslate , albaVMESlicer


*/

class ALBA_EXPORT albaViewArbitraryOrthoSlice: public albaViewCompoundWindowing
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
	albaViewArbitraryOrthoSlice(wxString label = "View Arbitrary OrthoSlice with Windowing");

	/** destructor*/
	virtual ~albaViewArbitraryOrthoSlice(); 

	/** RTTI macro.*/
	albaTypeMacro(albaViewArbitraryOrthoSlice, albaViewCompoundWindowing);

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
	virtual void VmeShow(albaVME *vme, bool show);

	/** Remove VME into plugged sub-views*/
	virtual void VmeRemove(albaVME *vme);

	/** Create the GUI at the bottom of the compounded view. */
	virtual void CreateGuiView();

	/** Function that handles events sent from other objects. */
	virtual void OnEvent(albaEventBase *alba_event);

	/** Function that clones instance of the object. */
	virtual albaView* Copy(albaObserver *Listener, bool lightCopyEnabled = false);

	/** Force the updating of the camera. */
	virtual void UpdateSubviewsCamerasToFaceSlices();

protected:

	enum AXIS { X = 0, Y = 1, Z = 2 };
	
	bool BelongsToNormalGizmo( albaVME * vme, int side );

	void ShowVolume( albaVME * vme, bool show );
		
	void StoreCameraParametersForAllSubviews();

	void ShowSlicers( albaVME * vmeVolume, bool show );
	
	void ResetCameraToSlices();

	/** 
	Create the helper cone giving feedback for camera direction*/
	void BuildCameraConeVME(int axis);

	void HideVolume();

		
	/** Internally used to create a new instance of the GUI. 
	This function should be overridden by subclasses to create specialized GUIs. 
	Each subclass should append its own widgets and define the enum of IDs for the widgets as 
	an extension of the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
	subclass to continue the ID enumeration from it. For appending the widgets in the
	same panel GUI, each CreateGUI() function should first call the superclass' one.*/
	virtual albaGUI* CreateGui();

	/** Enable/disable view widgets.*/
	void EnableWidgets(bool enable = true);

	void OnEventThis(albaEventBase *alba_event);  

	
	void OnLUTChooser();
	void OnLUTRangeModified();

	void UpdateSlicersLUT();

	void OnReset();

	void RestoreCameraParametersForAllSubviews();

	/**	This function is called when a translate gizmo is moved*/
	void OnEventGizmoTranslate(albaEventBase *alba_event, int side);

	void GetOrthoPlanes(int side, int * orthoPlanes);
	void PostMultiplyEventMatrixToGizmoCross(albaEventBase * inputEvent, albaGizmoCrossRotateTranslate *targetGizmo);
	
	/** Post multiply alba_event matrix to given slicer */
	void PostMultiplyEventMatrixToSlicer(albaEventBase *alba_event, int slicerAxis);

	/** Post multiply alba_event matrix to the 3 slicers */
	void PostMultiplyEventMatrixToSlicers(albaEventBase *alba_event);

	/** Windowing for volumes data. This function overrides superclass method.*/
	void VolumeWindowing(albaVME *volume);
	void OnEventGizmoRotate(albaEventBase *alba_event, int side);

	void SetSlices();
			
	albaPipeSurface * GetPipe(int inView, albaVMESurface *inSurface);
			
	/** Recompute all slicers output */
	void UpdateSlicers(int axis);

	void CreateViewCameraNormalFeedbackActors();
	void UpdateWindowing(bool enable,albaVME *vme);
	
	albaViewVTK *m_ViewSlice[3];
	albaViewVTK *m_View3d;

	albaVME          	*m_CurrentVolume;
	albaVMESlicer			*m_Slicer[3];
	albaMatrix					*m_SlicerResetMatrix[3];
	albaAttachCamera		*m_CameraToSlicer[3];

	double	m_VolumeVTKDataCenterABSCoords[3];
	double	m_VolumeVTKDataCenterABSCoordinatesReset[3];
	double	m_VolumeVTKDataABSOrientation[3];
	
	albaVMESurface *m_CameraConeVME[3];
	
	double m_CameraPositionForReset[3][3];
	double m_CameraFocalPointForReset[3][3];
	double m_CameraViewUpForReset[3][3];

	albaGizmoCrossRotateTranslate *m_GizmoRT[3];
		
	albaVMEVolumeGray *m_InputVolume;
};

#endif