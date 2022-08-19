/*=========================================================================
Program:   Alba
Module:    albaViewRXCT.h
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2018 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaViewRXCT_H__
#define __albaViewRXCT_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewCompound.h"
#include "albaSceneNode.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaViewRX;
class albaVMEVolume;
class vtkLookupTable;
class albaGizmoSlice;
class albaGUILutSwatch;
class albaGUILutSlider;

//----------------------------------------------------------------------------
// albaViewRXCT :
//----------------------------------------------------------------------------
/** 
This view features two Rx views and one compound view made of six CT slices.
This is an experimental component with rotated volumes interaction
and visualization enabled.*/

class ALBA_EXPORT albaViewRXCT : public albaViewCompound
{
public:
  /** constructor */
  albaViewRXCT(wxString label = "View RXCT Rotated Volumes Debugger");
  /** destructor*/
  virtual ~albaViewRXCT(); 
  
  /** RTTI macro*/
  albaTypeMacro(albaViewRXCT, albaViewCompound);

  /** clone an instance of the object*/
  virtual albaView *Copy(albaObserver *Listener, bool lightCopyEnabled = false);
  /** Precess events coming from other objects */
  virtual void OnEvent(albaEventBase *alba_event);
  
	//Sets the side on volume
	void SetSideOnVolume();

	/** Show/Hide VMEs into plugged sub-views */
  virtual void VmeShow(albaVME *vme, bool show);

  /** Remove VME into plugged sub-views */
  virtual void VmeRemove(albaVME *vme);

  /** Operations to be performed on vme selection */
	virtual void VmeSelect(albaVME *vme, bool select);

  /** Create visual pipe and initialize them to build a RXCT view */
  void PackageView();
  
  /** IDs for the GUI */
  enum VIEW_RXCT_WIDGET_ID
  {
    ID_LUT_WIDGET = Superclass::ID_LAST,
    ID_MOVE_ALL_SLICES,
		ID_SIDE,
		ID_RIGHT_OR_LEFT,
    ID_SNAP,
    ID_ADJUST_SLICES,
    ID_BORDER_CHANGE,
    ID_ALL_SURFACE,
    ID_RESET_SLICES,
    ID_TRILINEAR_INTERPOLATION,
    ID_LAST
  };

  /** Create the GUI on the bottom of the compounded view. */
  virtual void CreateGuiView();

  /* Method for determine if the pick is over a Slice view or not*/
  bool IsPickedSliceView();

  /** catch on size event and modify layout*/
  virtual void			OnSize(wxSizeEvent &size_event);

	/** return an xpm-icon that can be used to represent this view */
	char ** GetIcon();

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same panel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual albaGUI  *CreateGui();

  /*Let the sub-view to be maximized inside the compounded view. */
  virtual void MaximizeSubView(int subview_id = 0, bool maximize = true);

  /** 
  Redefine to arrange views to generate RXCT visualization.*/
  virtual void LayoutSubView(int width, int height);

  /** Create the gizmo to move the slices. */
  void GizmoCreate();

  /** Delete the gizmo. */
  void GizmoDelete();

  /** 
  Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

  double m_BorderColor[6][3];

  /** set thickness value for all the pipes*/
  void SetThicknessForAllSurfaceSlices(albaVME *root);

  /** Sort the slice */
  void SortSlices();

  /** Reset slice positions  */
  void ResetSlicesPosition(albaVME *vme);

	/** Check if the gizmo posiztion is inside the bounding box*/
	void BoundsValidate(double *pos);

  /** Precess events coming from other objects */
  void OnEventSnapModality();
  /** Precess events coming from other objects */
	void OnEventRangeModified(albaEventBase *alba_event);
  /** Precess events coming from other objects */
	void OnEventSortSlices(albaVME *vme = NULL);
  /** Precess events coming from other objects */
	void OnEventSetThickness();

	/** Sets Thickness on Pipe*/
	void SetBorder(albaPipe * p);
	/** Precess events coming from other objects */
  void OnEventMouseMove(albaEvent *e);
  int			m_Sort[6];
  double	m_Pos[6];


  albaGizmoSlice   *m_GizmoSlice[6];
  albaVME    *m_CurrentVolume; ///< Current visualized volume
	albaSceneNode *m_CurrentSceneNode; ///< Current visualized volume

  albaViewRX       *m_ViewsRX[2];
  albaViewCompound *m_ViewCTCompound;

  // this member variables are used by side panel gui view
  int m_MoveAllSlices;
  int m_RightOrLeft;
	int m_Side;
	int m_Snap;
  int m_AllSurface;
  wxSize m_Size;
  double m_Border;

  std::vector<albaSceneNode*> m_CurrentSurface;

  albaGUI  *m_GuiViews[3];
  albaGUILutSlider *m_LutSliders[3];

  //obsolete variable
  //vtkLookupTable  *m_vtkLUT[3];
  vtkLookupTable *m_VtkLUT; 

  albaGUILutSwatch *m_LutWidget;

  // Added by Losi 11.25.2009
  int m_TrilinearInterpolationOn;

	bool m_IsSubViewMaximized;
};
#endif
