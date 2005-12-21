/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewRXCT.h,v $
  Language:  C++
  Date:      $Date: 2005-12-21 13:55:14 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
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

//----------------------------------------------------------------------------
// mafViewRXCT :
//----------------------------------------------------------------------------
/** */
class mafViewRXCT: public mafViewCompound
{
public:
  mafViewRXCT(wxString label = "View RXCT", bool external = false);
  virtual ~mafViewRXCT(); 

  mafTypeMacro(mafViewRXCT, mafViewCompound);

  virtual mafView *Copy(mafObserver *Listener);
  virtual void OnEvent(mafEventBase *maf_event);
  
  /** 
  Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(mafNode *node, bool show);

  /** 
  Create visual pipe and initialize them to build an RXCT visualization */
  void PackageView();
  
  /** IDs for the GUI */
  enum VIEW_RXCT_WIDGET_ID
  {
    ID_LUT_CHOOSER = Superclass::ID_LAST,
    ID_LAST
  };

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
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
  mafGizmoSlice *m_gizmo[6];
  mafVMEVolume *m_CurrentVolume; ///< Current visualized volume
  vtkLookupTable *m_ColorLUT;
  mafViewRX *m_ViewsRX[2];
  mafViewCompound *m_ViewCT;
};
#endif
