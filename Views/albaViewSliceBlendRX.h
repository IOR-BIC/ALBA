/*=========================================================================
Program:   Alba
Module:    albaViewSliceBlendRX.h
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaViewSliceBlendRX_H__
#define __albaViewSliceBlendRX_H__

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
class albaViewSliceBlend;
class vtkLookupTable;

//----------------------------------------------------------------------------
// albaViewSliceBlendRX :
//----------------------------------------------------------------------------
/** 
This view features one Rx views and one Slice Blend view.*/
class ALBA_EXPORT albaViewSliceBlendRX : public albaViewCompound
{
public:

	/** constructor */
	albaViewSliceBlendRX(wxString label = "View Blend RX");
	/** destructor */
	virtual ~albaViewSliceBlendRX();
	/** RTTI macro */
	albaTypeMacro(albaViewSliceBlendRX, albaViewCompound);

	/** clone the object*/
	/*virtual*/ albaView *Copy(albaObserver *Listener, bool lightCopyEnabled = false);

	/** listen to other object events*/
	/*virtual*/ void OnEvent(albaEventBase *alba_event);

	void SetLutRange(double low, double hi);

	/** Show/Hide VMEs into plugged sub-views*/
	/*virtual*/ void VmeShow(albaVME *vme, bool show);

	/** Remove VME into plugged sub-views*/
	/*virtual*/ void VmeRemove(albaVME *vme);

	/** Create visual pipe and initialize them to build an RXCT visualization */
	/*virtual*/ void PackageView();

	/** IDs for the GUI */
	enum VIEW_RXCT_WIDGET_ID
	{
		ID_LUT_WIDGET = Superclass::ID_LAST,
		ID_SLICE_POSITION,
		ID_LAST
	};

	/**
	Create the GUI on the bottom of the compounded view. */
	virtual void CreateGuiView();

protected:
	/**
	Internally used to create a new instance of the GUI. This function should be
	overridden by subclasses to create specialized GUIs. Each subclass should append
	its own widgets and define the enum of IDs for the widgets as an extension of
	the superclass enum. The last id value must be defined as "LAST_ID" to allow the
	subclass to continue the ID enumeration from it. For appending the widgets in the
	same panel GUI, each CreateGUI() function should first call the superclass' one.*/
	/*virtual*/ albaGUI  *CreateGui();

	/**
	Redefine to arrange views to generate RXCT visualization.*/
	/*virtual*/ void LayoutSubView(int width, int height);

	/**
	Enable/disable view widgets.*/
	void EnableWidgets(bool enable = true);

	/** Create the gizmo to move the slices. */
	void GizmoCreate();

	/** Delete the gizmo. */
	void GizmoDelete();

	/** listen mouse events*/
	void OnEventMouseMove(albaEvent *e);

	/** check and correct gizmos positions using volume bounds  */
	void BoundsValidate(double *pos);

	albaVME *m_CurrentVolume; ///< Current visualized volume

	albaViewRX *m_ViewsRX;
	albaViewSliceBlend *m_ViewSliceBlend;

	albaGizmoSlice *m_GizmoSlice[2];
	
	// this member variables are used by side panel gui view 
	std::vector<albaSceneNode*> m_CurrentSurface;

	albaGUI *m_BlendGui;
	albaGUI *m_GuiViews;
	albaGUILutSlider *m_LutSliders;
	vtkLookupTable *m_VtkLUT;
	albaGUILutSwatch *m_LutWidget;


	double m_BorderColor[2][3];

	double m_SliceTop_Position[3];
	double m_SliceBottom_Position[3];

	double m_GizmoSliceHeight[2];

	double m_LutBlendMinMax[2];
	double m_LutRxMinMax[2];
};
#endif
