/*=========================================================================
Program: ALBA (Agile Library for Biomedical Applications)
Module: albaViewVirtualRX
Authors: Nicola Vanella

Copyright (c) BIC
All rights reserved. See Copyright.txt or

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaViewVirtualRX_H__
#define __albaViewVirtualRX_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewCompound.h"
#include "albaSceneNode.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUILutSlider;
class albaGUILutSwatch;
class albaGizmoSlice;
class albaVMEVolume;
class albaViewRX;
class vtkLookupTable;

//----------------------------------------------------------------------------
// albaViewVirtualRX :
//----------------------------------------------------------------------------
/** This view features two Rx views with projection options*/
class ALBA_EXPORT albaViewVirtualRX : public albaViewCompound
{
public:
	albaViewVirtualRX(wxString label = "View Virtual RX");
	virtual ~albaViewVirtualRX();

	albaTypeMacro(albaViewVirtualRX, albaViewCompound);

	/** return an xpm-icon that can be used to represent this view */
	char ** GetIcon();

	virtual albaView *Copy(albaObserver *Listener, bool lightCopyEnabled = false);
	virtual void OnEvent(albaEventBase *alba_event);

	/** Show/Hide VMEs into plugged sub-views*/
	virtual void VmeShow(albaVME *vme, bool show);

	/** Remove VME into plugged sub-views*/
	virtual void VmeRemove(albaVME *vme);

	/** Create visual pipe and initialize them to build an RXCT visualization */
	void PackageView();

	/** IDs for the GUI */
	enum VIEW_VRRX_WIDGET_ID
	{
		ID_LUT_WIDGET = Superclass::ID_LAST,
		ID_RIGHT_OR_LEFT,
		ID_PROJECTION_RANGE,
		ID_PROJECTION_MODE,
		ID_PROJECTION_RESET,
		ID_WINDOWING_UPDATE,
		ID_LAST,
	};

	enum GIZMO_ID
	{
		GIZMO_XA = 0,
		GIZMO_XB,
		GIZMO_YA,
		GIZMO_YB,
// 		GIZMO_ZA,
// 		GIZMO_ZB,
		GIZMOS_NUMBER,
	};

	/** Create the GUI on the bottom of the compounded view. */
	virtual void CreateGuiView();

protected:
	/**
	Internally used to create a new instance of the GUI. This function should be
	overridden by subclasses to create specialized GUIs. Each subclass should append
	its own widgets and define the enum of IDs for the widgets as an extension of
	the superclass enum. The last id value must be defined as "LAST_ID" to allow the
	subclass to continue the ID enumeration from it. For appending the widgets in the
	same panel GUI, each CreateGUI() function should first call the superclass' one.*/
	virtual albaGUI  *CreateGui();

	/** Listen mouse events*/
	void OnEventMouseMove(albaEvent *e);

	bool Validate(long gizmoId, double gizmoPoint);

	/** Redefine to arrange views to generate RXCT visualization.*/
	virtual void LayoutSubView(int width, int height);

	/** Enable/disable view widgets.*/
	void EnableWidgets(bool enable = true);

	void CreateGizmo();
	void DestroyGizmo();
	void ShowGizmo();

	void UpdateProjection(double min, double max, int plane, int modality);
	void ResetProjection();
	void UpdateWindowing(int plane);
	void UpdateProjectionGui();
	void UpdateProjectionGizmos();

	albaVME *m_CurrentVolume; ///< Current visualized volume
	double m_VolumeBounds[6];
	double m_VolumeSize[3];
	int m_VolumeDims[2];
	double m_Spacing[3];

	albaViewRX *m_ViewsRX[3];
	
	// This member variables are used by side panel Gui view 
	int m_RightOrLeft;
	int m_ProjectionMode = 0;
	int m_ProjectionPlane = 0;
	double m_GizmoPoints[6];

	albaGUILutSlider *m_ProjectionRangeGuiSliderX;
	albaGUILutSlider *m_ProjectionRangeGuiSliderY;
	albaGUILutSlider *m_ProjectionRangeGuiSliderZ;

	albaGizmoSlice *m_PrjGizmo[6];

	albaGUI				*m_GuiViews[2];
	albaGUILutSlider	*m_LutSliders[2];
	vtkLookupTable		*m_VtkLUT[2];
	albaGUILutSwatch    *m_LutWidget;

	int m_UpdateWindowing;
};
#endif
