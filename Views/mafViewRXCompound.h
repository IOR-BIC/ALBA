/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewRXCompound.h,v $
  Language:  C++
  Date:      $Date: 2007-11-29 08:53:31 $
  Version:   $Revision: 1.2 $
  Authors:   Stefano Perticoni , Paolo Quadrani, Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafViewRXCompound_H__
#define __mafViewRXCompound_H__

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
class mmgLutSwatch;
class mmgLutSlider;

//----------------------------------------------------------------------------
// mafViewRXCompound :
//----------------------------------------------------------------------------
/** 
This view features two Rx views and one compound view made of six CT slices.*/
class mafViewRXCompound: public mafViewCompound
{
public:
  mafViewRXCompound(wxString label = "View RXCT");
  virtual ~mafViewRXCompound(); 

  mafTypeMacro(mafViewRXCompound, mafViewCompound);

  virtual mafView *Copy(mafObserver *Listener);
  virtual void OnEvent(mafEventBase *maf_event);
  
  /** 
  Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(mafNode *node, bool show);

  /** Remove VME into plugged sub-views*/
  virtual void VmeRemove(mafNode *node);

  /** 
  Create visual pipe and initialize them to build an RXCT visualization */
  void PackageView();
  
  /** IDs for the GUI */
  enum VIEW_RXCT_WIDGET_ID
  {
    ID_LUT_WIDGET = Superclass::ID_LAST,
    ID_RIGHT_OR_LEFT,
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
  virtual mmgGui  *CreateGui();

  /** 
  Redefine to arrange views to generate RXCT visualization.*/
  virtual void LayoutSubViewCustom(int width, int height);

  

  /** 
  Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

  mafVME    *m_CurrentVolume; ///< Current visualized volume
  
  mafViewRX       *m_ViewsRX[2];
  

  // this member variables are used by side panel gui view 
  int m_RightOrLeft;
  std::vector<mafSceneNode*> m_CurrentSurface;

  mmgGui  *m_GuiViews[2];
  mmgLutSlider *m_LutSliders[2];
  vtkLookupTable  *m_vtkLUT[2];  
  mmgLutSwatch    *m_LutWidget;
};
#endif
