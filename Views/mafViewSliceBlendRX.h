/*=========================================================================
Program:   Alba
Module:    mafViewSliceBlendRX.h
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

#ifndef __mafViewSliceBlendRX_H__
#define __mafViewSliceBlendRX_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafViewCompound.h"
#include "mafSceneNode.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUILutSlider;
class mafGUILutSwatch;
class mafGizmoSlice;
class mafVMEVolume;
class mafViewRX;
class mafViewSliceBlend;
class vtkLookupTable;

//----------------------------------------------------------------------------
// mafViewSliceBlendRX :
//----------------------------------------------------------------------------
/** 
This view features one Rx views and one Slice Blend view.*/
class MAF_EXPORT mafViewSliceBlendRX : public mafViewCompound
{
public:

	/** constructor */
	mafViewSliceBlendRX(wxString label = "View Blend RX");
	/** destructor */
	virtual ~mafViewSliceBlendRX();
	/** RTTI macro */
	mafTypeMacro(mafViewSliceBlendRX, mafViewCompound);

	/** clone the object*/
	/*virtual*/ mafView *Copy(mafObserver *Listener, bool lightCopyEnabled = false);

	/** listen to other object events*/
	/*virtual*/ void OnEvent(mafEventBase *maf_event);

	void SetLutRange(double low, double hi);

	/** Show/Hide VMEs into plugged sub-views*/
	/*virtual*/ void VmeShow(mafVME *vme, bool show);

	/** Remove VME into plugged sub-views*/
	/*virtual*/ void VmeRemove(mafVME *vme);

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
	/*virtual*/ mafGUI  *CreateGui();

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
	void OnEventMouseMove(mafEvent *e);

	/** check and correct gizmos positions using volume bounds  */
	void BoundsValidate(double *pos);

	mafVME *m_CurrentVolume; ///< Current visualized volume

	mafViewRX *m_ViewsRX;
	mafViewSliceBlend *m_ViewSliceBlend;

	mafGizmoSlice *m_GizmoSlice[2];
	
	// this member variables are used by side panel gui view 
	std::vector<mafSceneNode*> m_CurrentSurface;

	mafGUI *m_BlendGui;
	mafGUI *m_GuiViews;
	mafGUILutSlider *m_LutSliders;
	vtkLookupTable *m_VtkLUT;
	mafGUILutSwatch *m_LutWidget;


	double m_BorderColor[2][3];

	double m_SliceTop_Position[3];
	double m_SliceBottom_Position[3];

	double m_GizmoSliceHeight[2];

	double m_LutBlendMinMax[2];
	double m_LutRxMinMax[2];
};
#endif
