/*=========================================================================

 Program: MAF2
 Module: mafViewRXCompound
 Authors: Stefano Perticoni , Paolo Quadrani, Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafViewRXCompound_H__
#define __mafViewRXCompound_H__

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
class mafGUILutSwatch;
class mafGUILutSlider;

//----------------------------------------------------------------------------
// mafViewRXCompound :
//----------------------------------------------------------------------------
/** 
This view features two Rx views and one compound view made of six CT slices.*/
class MAF_EXPORT mafViewRXCompound: public mafViewCompound
{
public:
  mafViewRXCompound(wxString label = "View RXCT");
  virtual ~mafViewRXCompound(); 

  mafTypeMacro(mafViewRXCompound, mafViewCompound);

  virtual mafView *Copy(mafObserver *Listener, bool lightCopyEnabled = false);
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
  virtual mafGUI  *CreateGui();

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

  mafGUI  *m_GuiViews[2];
  mafGUILutSlider *m_LutSliders[2];
  vtkLookupTable  *m_VtkLUT[2];  
  mafGUILutSwatch    *m_LutWidget;
};
#endif
