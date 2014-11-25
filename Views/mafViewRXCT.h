/*=========================================================================

 Program: MAF2Medical
 Module: mafViewRXCT
 Authors: Stefano Perticoni , Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafViewRXCT_H__
#define __mafViewRXCT_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafViewCompound.h"
#include "mafSceneNode.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafViewRX;
class mafVMEVolume;
class vtkLookupTable;
class mafGizmoSlice;
class mafGUILutSwatch;
class mafGUILutSlider;

//----------------------------------------------------------------------------
// mafViewRXCT :
//----------------------------------------------------------------------------
/** 
This view features two Rx views and one compound view made of six CT slices.
This is an experimental component with rotated volumes interaction
and visualization enabled.*/

class MAF_EXPORT mafViewRXCT : public mafViewCompound
{
public:
  /** constructor */
  mafViewRXCT(wxString label = "View RXCT Rotated Volumes Debugger");
  /** destructor*/
  virtual ~mafViewRXCT(); 
  
  /** RTTI macro*/
  mafTypeMacro(mafViewRXCT, mafViewCompound);

  /** clone an instance of the object*/
  virtual mafView *Copy(mafObserver *Listener, bool lightCopyEnabled = false);
  /** Precess events coming from other objects */
  virtual void OnEvent(mafEventBase *maf_event);
  
  /** Show/Hide VMEs into plugged sub-views */
  virtual void VmeShow(mafNode *node, bool show);

  /** Remove VME into plugged sub-views */
  virtual void VmeRemove(mafNode *node);

  /** Operations to be performed on vme selection */
	virtual void VmeSelect(mafNode *node, bool select);

  /** Create visual pipe and initialize them to build a RXCT view */
  void PackageView();
  
  /** IDs for the GUI */
  enum VIEW_RXCT_WIDGET_ID
  {
    ID_LUT_WIDGET = Superclass::ID_LAST,
    ID_MOVE_ALL_SLICES,
    ID_RIGHT_OR_LEFT,
    ID_SNAP,
    ID_ADJUST_SLICES,
    ID_BORDER_CHANGE,
    ID_ALL_SURFACE,
    ID_RESET_SLICES,
    ID_ENABLE_GPU,
    ID_TRILINEAR_INTERPOLATION,
	ID_HELP,
    ID_LAST
  };

  /** Create the GUI on the bottom of the compounded view. */
  virtual void CreateGuiView();

  /* Method for determine if the pick is over a Slice view or not*/
  bool IsPickedSliceView();

  /** catch on size event and modify layout*/
  virtual void			OnSize(wxSizeEvent &size_event);

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same panel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mafGUI  *CreateGui();

  /*Let the sub-view to be maximized inside the compounded view. */
  virtual void MaximizeSubView(int subview_id = 0, bool maximize = true);

  /** 
  Redefine to arrange views to generate RXCT visualization.*/
  virtual void LayoutSubViewCustom(int width, int height);

  /** Create the gizmo to move the slices. */
  void GizmoCreate();

  /** Delete the gizmo. */
  void GizmoDelete();

  /** 
  Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

  double m_BorderColor[6][3];

  /** set thickness value for all the pipes*/
  void SetThicknessForAllSurfaceSlices(mafNode *root);

  /**
  Sort the slice */
  void SortSlices();

  /** Reset slice positions  */
  void ResetSlicesPosition(mafNode *node);

	/**
	Check if the gizmo posiztion is inside the bounding box*/
	void BoundsValidate(double *pos);

  /** Precess events coming from other objects */
  void OnEventSnapModality();
  /** Precess events coming from other objects */
	void OnEventRangeModified(mafEventBase *maf_event);
  /** Precess events coming from other objects */
	void OnEventSortSlices();
  /** Precess events coming from other objects */
	void OnEventSetThickness();
  /** Precess events coming from other objects */
  void OnEventMouseMove(mafEvent *e);
  int			m_Sort[6];
  double	m_Pos[6];


  mafGizmoSlice   *m_GizmoSlice[6];
  mafVME    *m_CurrentVolume; ///< Current visualized volume
  
  mafViewRX       *m_ViewsRX[2];
  mafViewCompound *m_ViewCTCompound;

  // this member variables are used by side panel gui view
  int m_MoveAllSlices;
  int m_RightOrLeft;
  int m_Snap;
  int m_AllSurface;
  wxSize m_Size;
  double m_Border;
  std::vector<mafSceneNode*> m_CurrentSurface;

  mafGUI  *m_GuiViews[3];
  mafGUILutSlider *m_LutSliders[3];

  //obsolete variable
  //vtkLookupTable  *m_vtkLUT[3];
  vtkLookupTable  *m_Lut; 

  mafGUILutSwatch    *m_LutWidget;

  // Added by Losi 11.25.2009
  int m_EnableGPU; ///<Non-zero, if the GPU support for slicing is used (default)
  int m_TrilinearInterpolationOn;
};
#endif
