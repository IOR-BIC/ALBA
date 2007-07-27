/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewRXCT.h,v $
  Language:  C++
  Date:      $Date: 2007-07-27 09:33:00 $
  Version:   $Revision: 1.12 $
  Authors:   Stefano Perticoni , Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafViewRXCT_H__
#define __mafViewRXCT_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewCompound.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafViewRX;
class mafVMEVolume;
class vtkLookupTable;
class mafGizmoSlice;
class mmgLutSwatch;
class mmgLutSlider;

//----------------------------------------------------------------------------
// mafViewRXCT :
//----------------------------------------------------------------------------
/** 
This view features two Rx views and one compound view made of six CT slices.*/
class mafViewRXCT : public mafViewCompound
{
public:
  mafViewRXCT(wxString label = "View RXCT");
  virtual ~mafViewRXCT(); 

  mafTypeMacro(mafViewRXCT, mafViewCompound);

  virtual mafView *Copy(mafObserver *Listener);
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
    ID_LAST
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
  virtual mmgGui  *CreateGui();

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

  void SetThicknessForAllSurfaceSlices(mafNode *root);

  /**
  Sort the slice */
  void SortSlices();

  /** Reset slice positions  */
  void ResetSlicesPosition(mafNode *node);

	/**
	Check if the gizmo posiztion is inside the bounding box*/
	void BoundsValidate(double *pos);

  void OnEventSnapModality();
	void OnEventRangeModified(mafEventBase *maf_event);
	void OnEventSortSlices();
	void OnEventSetThickness();
  void OnEventMouseMove(mafEvent *e);
  int			m_Sort[6];
  double	m_Pos[6];


  mafGizmoSlice   *m_GizmoSlice[6];
  mafVMEVolume    *m_CurrentVolume; ///< Current visualized volume
  
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

  mmgGui  *m_GuiViews[3];
  mmgLutSlider *m_LutSliders[3];
  vtkLookupTable  *m_vtkLUT[3];  
  mmgLutSwatch    *m_LutWidget;
};
#endif
