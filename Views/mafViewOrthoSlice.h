/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewOrthoSlice.h,v $
  Language:  C++
  Date:      $Date: 2006-11-15 18:17:32 $
  Version:   $Revision: 1.18 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafViewOrthoSlice_H__
#define __mafViewOrthoSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewCompound.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoSlice;
class mmgLutSlider;
class mmgFloatSlider;
class mafViewSlice;
class mafVMEVolume;
class vtkLookupTable;
class mmgLutSwatch;
class vtkPoints;

//----------------------------------------------------------------------------
// mafViewOrthoSlice :
//----------------------------------------------------------------------------
/** 
  This compound view is made of four child views used to analyze different orthogonal slices of the volume*/
class mafViewOrthoSlice: public mafViewCompound
{
public:
  mafViewOrthoSlice(wxString label = "View OrthoSlice");
  virtual ~mafViewOrthoSlice(); 

  mafTypeMacro(mafViewOrthoSlice, mafViewCompound);

  virtual mafView *Copy(mafObserver *Listener);
  virtual void OnEvent(mafEventBase *maf_event);
  
  /** 
  Create the GUI on the bottom of the compounded view. */
  virtual void CreateGuiView();

  /** 
  Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(mafNode *node, bool show);

  /** Remove VME into plugged sub-views*/
  virtual void VmeRemove(mafNode *node);

  /** 
  Create visual pipe and initialize them to build an OrthoSlice visualization */
  void PackageView();
  
  /** IDs for the GUI */
  enum VIEW_ORTHO_SLICE_WIDGET_ID
  {
    ID_LUT_CHOOSER = Superclass::ID_LAST,
    ID_SIDE_ORTHO,
		ID_SNAP,
    ID_LAST
  };

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same panel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mmgGui  *CreateGui();

  /** Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

  /** Create the gizmos to move the slices. */
  void GizmoCreate();

  /** Delete the gizmos. */
  void GizmoDelete();

  /** Update the slices according to the new position. */
  void SetSlicePosition(long gizmoId, vtkPoints *p);

  mafGizmoSlice   *m_Gizmo[3];

  double m_GizmoHandlePosition[3];

  mafVMEVolume *m_CurrentVolume; ///< Current visualized volume
  mmgLutSlider *m_LutSlider; ///< Double slider used to change brightness and contrast of the image
  mmgLutSwatch *m_LutWidget; ///< LUT widget in view side panel 
  vtkLookupTable *m_ColorLUT;
  mafViewSlice *m_Views[4]; ///< Child views
  int	m_Side; ///< change Azimuth of the camera
	int m_Snap; ///<change modality of gizmo to snap on grid

};
#endif
