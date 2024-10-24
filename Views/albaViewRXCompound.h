/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewRXCompound
 Authors: Stefano Perticoni , Paolo Quadrani, Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaViewRXCompound_H__
#define __albaViewRXCompound_H__

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
class albaGUILutSwatch;
class albaGUILutSlider;

//----------------------------------------------------------------------------
// albaViewRXCompound :
//----------------------------------------------------------------------------
/** 
This view features two Rx views and one compound view made of six CT slices.*/
class ALBA_EXPORT albaViewRXCompound: public albaViewCompound
{
public:
  albaViewRXCompound(wxString label = "View RXCT");
  virtual ~albaViewRXCompound(); 

  albaTypeMacro(albaViewRXCompound, albaViewCompound);

  virtual albaView *Copy(albaObserver *Listener, bool lightCopyEnabled = false);
  virtual void OnEvent(albaEventBase *alba_event);
  
  /** 
  Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(albaVME *vme, bool show);

  /** Remove VME into plugged sub-views*/
  virtual void VmeRemove(albaVME *vme);

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
  virtual albaGUI  *CreateGui();

  /** 
  Redefine to arrange views to generate RXCT visualization.*/
  virtual void LayoutSubView(int width, int height);
	
  /** 
  Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

  albaVME    *m_CurrentVolume; ///< Current visualized volume
  
  albaViewRX       *m_ViewsRX[2];
  

  // this member variables are used by side panel gui view 
  int m_RightOrLeft;
  std::vector<albaSceneNode*> m_CurrentSurface;

  albaGUI  *m_GuiViews[2];
  albaGUILutSlider *m_LutSliders[2];
  vtkLookupTable  *m_VtkLUT[2];  
  albaGUILutSwatch    *m_LutWidget;
};
#endif
