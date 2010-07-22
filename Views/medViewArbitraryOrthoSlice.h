/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medViewArbitraryOrthoSlice.h,v $
Language:  C++
Date:      $Date: 2010-07-22 09:41:50 $
Version:   $Revision: 1.1.2.3 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medViewArbitraryOrthoSlice_H__
#define __medViewArbitraryOrthoSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medViewCompoundWindowing.h"
#include "mafVMESurface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafViewVTK;
class mafGizmoTranslate;
class mafGizmoRotate;
class mafVMEVolumeGray;
class mafVMESlicer;
class mafMatrix;
class mafAttachCamera;
class mafGUI;
class mafGUILutSlider;
class mafGUILutSwatch;
class medVMEPolylineEditor;

/**
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!WORK IN PROGRESS: DO NOT USE!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Class Name: medViewArbitraryOrthoSlice.

OrthoSlice-ArbitrarySlice mix early prototype.
*/

class medViewArbitraryOrthoSlice: public medViewCompoundWindowing
{
public:
	/** constructor*/
	medViewArbitraryOrthoSlice(wxString label = "View Arbitrary Slice with Windowing", bool show_ruler = false);

	/** destructor*/
	virtual ~medViewArbitraryOrthoSlice(); 

	/** RTTI macro.*/
	mafTypeMacro(medViewArbitraryOrthoSlice, medViewCompoundWindowing);

	enum ID_GUI
	{
		ID_COMBO_CHOOSE_ACTIVE_GIZMO = Superclass::ID_LAST,
		ID_RESET,
    ID_SHOW_GIZMO,
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

	/** Function that handles events sent from other objects. */
	virtual void OnEvent(mafEventBase *maf_event);

	/** Function that clones instance of the object. */
	virtual mafView* Copy(mafObserver *Listener);

	/** Force the updating of the camera. */
	virtual void CameraUpdate();

protected:

	void ShowVMESurfacesAndLandmarks( mafNode * node );

	void ShowMafVMEImage( mafVME * vme );
	void HideMafVmeImage();

	void ShowMafVMEVolume( mafVME * vme, bool show );

	void ShowSlicers( mafVME * vmeVolume, bool show );

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

	void OnEventThis(mafEventBase *maf_event);  
	void OnChooseActiveGizmo();
	void OnLUTChooser();
	void OnRangeModified();
	
	void OnReset();
	void OnResetMedVMEPolylineEditor();
	void OnResetMafVMEMesh( mafNode * node );
	void OnResetSurfaceAndLandmark( mafNode * node );
	
	/** This function is called when a rotate gizmo is moved*/
	void OnEventGizmoRotate(mafEventBase *maf_event);

	/**	This function is called when a translate gizmo is moved*/
	void OnEventGizmoTranslate(mafEventBase *maf_event);

	/** Post multiply matrix for incoming transform events */
	void PostMultiplyEventMatrix(mafEventBase *maf_event);

	/** Windowing for volumes data. This function overrides superclass method.*/
	void VolumeWindowing(mafVME *volume);

	mafViewVTK *m_ViewSliceX;
	mafViewVTK *m_ViewSliceY;
	mafViewVTK *m_ViewSliceZ;

	mafViewVTK *m_ViewArbitrary;

	mafGizmoTranslate *m_GizmoTranslate;
	mafGizmoRotate		*m_GizmoRotate;
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
};
#endif
