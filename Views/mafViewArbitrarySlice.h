/*=========================================================================

Program: MAF2
Module: mafViewArbitrarySlice
Authors: Eleonora Mambrini , Stefano Perticoni

Copyright (c) B3C
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafViewArbitrarySlice_H__
#define __mafViewArbitrarySlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafViewCompoundWindowing.h"

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
class mafVMEPolylineEditor;

/** 
Class Name: mafViewArbitrarySlice.
This compound view is made of two child views used to analyze arbitrary slices of the volume.
The left view features a rotation and a translation gizmos which can be used to adjust the slice position and orientation.
The right view camera is always facing the slice.
*/

class MAF_EXPORT mafViewArbitrarySlice: public mafViewCompoundWindowing
{
public:
	/** constructor*/
	mafViewArbitrarySlice(wxString label = "View Arbitrary Slice with Windowing", bool show_ruler = false);

	/** destructor*/
	virtual ~mafViewArbitrarySlice(); 

	/** RTTI macro.*/
	mafTypeMacro(mafViewArbitrarySlice, mafViewCompoundWindowing);

	enum ID_GUI
	{
		ID_COMBO_GIZMOS = Superclass::ID_LAST,
		ID_RESET,
		ID_TRILINEAR_INTERPOLATION_ON,
		ID_HELP,
		ID_LAST,
	};

	/** Create visual pipe and initialize them to build an OrthoSlice visualization */
	virtual void PackageView();

	/** Show/Hide VMEs into plugged sub-views*/
	virtual void VmeShow(mafNode *node, bool show);

	/** Remove VME into plugged sub-views*/
	virtual void VmeRemove(mafNode *node);

	/** Create the GUI on the bottom of the compounded view. */
	virtual void CreateGuiView();

	/** Function that handles events sent from other objects. */
	virtual void OnEvent(mafEventBase *maf_event);

	/** Function that clones instance of the object. */
	virtual mafView* Copy(mafObserver *Listener, bool lightCopyEnabled = false);

	/** Force the updating of the camera. */
	virtual void CameraUpdate();

	/* Update slicer settings according to m_CurrentVolume*/
	void UpdateSlicerBehavior();

protected:

	/** Internally used to create a new instance of the GUI. 
	This function should be overridden by subclasses to create specialized GUIs. 
	Each subclass should append its own widgets and define the enum of IDs for the widgets as 
	an extension of the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
	subclass to continue the ID enumeration from it. For appending the widgets in the
	same panel GUI, each CreateGUI() function should first call the superclass' one.*/
	virtual mafGUI* CreateGui();

	/** Enable/disable view widgets.*/
	void EnableWidgets(bool enable = true);

	/** Handling events sent from other objects. Called by public method OnEvent().*/
	void OnEventThis(mafEventBase *maf_event);  

	/** This function is called when a rotate gizmo is moved*/
	void OnEventGizmoRotate(mafEventBase *maf_event);

	/**	This function is called when a translate gizmo is moved*/
	void OnEventGizmoTranslate(mafEventBase *maf_event);

	/** Post multiply matrix for incoming transform events */
	void PostMultiplyEventMatrix(mafEventBase *maf_event);

	/** Windowing for volumes data. This function overrides superclass method.*/
	void VolumeWindowing(mafVME *volume);

	mafViewVTK *m_ViewSlice;
	mafViewVTK *m_ViewArbitrary;

	mafGizmoTranslate *m_GizmoTranslate;
	mafGizmoRotate		*m_GizmoRotate;
	mafVME          	*m_CurrentVolume;
	mafVME				    *m_CurrentImage;
	mafVMESlicer			*m_Slicer;
	mafMatrix					*m_MatrixReset;
	mafAttachCamera		*m_AttachCamera;

	double	m_SliceCenterSurface[3];
	double	m_SliceCenterSurfaceReset[3];
	double	m_SliceAngleReset[3];
	int			m_TypeGizmo;

	mafGUI				*m_GuiGizmos;

	mafVMEPolylineEditor *m_CurrentPolylineGraphEditor;

	int m_TrilinearInterpolationOn;
};
#endif
