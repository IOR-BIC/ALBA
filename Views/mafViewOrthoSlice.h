/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewOrthoSlice.h,v $
  Language:  C++
  Date:      $Date: 2005-11-03 09:00:02 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
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
class mmdMouse;
class mafSceneGraph;
class mmgGui;
class mafRWIBase;
class mmgLutSlider;
class mafViewVTK;

//----------------------------------------------------------------------------
// mafViewOrthoSlice :
//----------------------------------------------------------------------------
/** */
class mafViewOrthoSlice: public mafViewCompound
{
public:
  mafViewOrthoSlice(wxString label = "View OrthoSlice", int num_row = 2, int num_col = 2, bool external = false);
  virtual ~mafViewOrthoSlice(); 

  mafTypeMacro(mafViewOrthoSlice, mafViewCompound);

  virtual mafView *Copy(mafObserver *Listener);
  
  /** 
  Create the GUI on the bottom of the compounded view. */
  virtual void     CreateGuiView();

  /** 
  Create visual pipe and initialize them to build an OrthoSlice visualization */
  void PackageView();
  
  virtual void     OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
  enum VIEW_ORTHO_SLICE_WIDGET_ID
  {
    ID_ORTHO_CHILD_VIEW = Superclass::ID_LAST,
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

  mmgLutSlider *m_Luts;
  mafViewVTK   *m_Views[4];
};
#endif

