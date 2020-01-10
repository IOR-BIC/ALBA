/*=========================================================================

Program: ALBA
Module: albaViewArbitrarySlice
Authors: Eleonora Mambrini , Stefano Perticoni

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaViewArbitrarySlice_H__
#define __albaViewArbitrarySlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewCompoundWindowing.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaViewVTK;
class albaGizmoTranslate;
class albaGizmoRotate;
class albaVMEVolumeGray;
class albaVMESlicer;
class albaMatrix;
class albaAttachCamera;
class albaGUI;
class albaGUILutSlider;
class albaGUILutSwatch;
class albaVMEPolylineEditor;

/** 
Class Name: albaViewArbitrarySlice.
This compound view is made of two child views used to analyze arbitrary slices of the volume.
The left view features a rotation and a translation gizmos which can be used to adjust the slice position and orientation.
The right view camera is always facing the slice.
*/

class ALBA_EXPORT albaViewArbitrarySlice: public albaViewCompoundWindowing
{
public:
	/** constructor*/
	albaViewArbitrarySlice(wxString label = "View Arbitrary Slice with Windowing", bool show_ruler = false);

	/** destructor*/
	virtual ~albaViewArbitrarySlice(); 

	/** RTTI macro.*/
	albaTypeMacro(albaViewArbitrarySlice, albaViewCompoundWindowing);

	enum ID_GUI
	{
		ID_COMBO_GIZMOS = Superclass::ID_LAST,
		ID_RESET,
		ID_TRILINEAR_INTERPOLATION_ON,
		ID_LAST,
	};

	/** Create visual pipe and initialize them to build an OrthoSlice visualization */
	virtual void PackageView();

	/** Show/Hide VMEs into plugged sub-views*/
	virtual void VmeShow(albaVME *vme, bool show);

	/** Remove VME into plugged sub-views*/
	virtual void VmeRemove(albaVME *vme);

	/** Create the GUI on the bottom of the compounded view. */
	virtual void CreateGuiView();

	/** Function that handles events sent from other objects. */
	virtual void OnEvent(albaEventBase *alba_event);

	/** Function that clones instance of the object. */
	virtual albaView* Copy(albaObserver *Listener, bool lightCopyEnabled = false);

	/** Force the updating of the camera. */
	virtual void CameraUpdate();

	/* Update slicer settings according to m_CurrentVolume*/
	void UpdateSlicerBehavior();

	/** return an xpm-icon that can be used to represent this view */
	char ** GetIcon();

	/** Remove Gizmos, Observers from the volume */
	void DestroyGizmos();

	/** Generate Gizmos and pose them in the right position*/
	void CreateGizmos();
protected:

	/** Internally used to create a new instance of the GUI. 
	This function should be overridden by subclasses to create specialized GUIs. 
	Each subclass should append its own widgets and define the enum of IDs for the widgets as 
	an extension of the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
	subclass to continue the ID enumeration from it. For appending the widgets in the
	same panel GUI, each CreateGUI() function should first call the superclass' one.*/
	virtual albaGUI* CreateGui();

	/** Enable/disable view widgets.*/
	void EnableWidgets(bool enable = true);

	/** Handling events sent from other objects. Called by public method OnEvent().*/
	void OnEventThis(albaEventBase *alba_event);  

	/** This function is called when a rotate gizmo is moved*/
	void OnEventGizmoRotate(albaEventBase *alba_event);

	/**	This function is called when a translate gizmo is moved*/
	void OnEventGizmoTranslate(albaEventBase *alba_event);

	/** Post multiply matrix for incoming transform events */
	void PostMultiplyEventMatrix(albaEventBase *alba_event);

	/** Windowing for volumes data. This function overrides superclass method.*/
	void VolumeWindowing(albaVME *volume);

	albaViewVTK *m_ViewSlice;
	albaViewVTK *m_ViewArbitrary;

	albaGizmoTranslate *m_GizmoTranslate;
	albaGizmoRotate		*m_GizmoRotate;
	albaVME          	*m_CurrentVolume;
	albaVMESlicer			*m_Slicer;
	albaMatrix					*m_MatrixReset;
	albaAttachCamera		*m_AttachCamera;

	double	m_SliceCenterSurface[4];
	double	m_SliceCenterSurfaceReset[4];
	double	m_SliceAngleReset[4];
	int			m_TypeGizmo;

	albaGUI				*m_GuiGizmos;
		
	int m_TrilinearInterpolationOn;
};
#endif
