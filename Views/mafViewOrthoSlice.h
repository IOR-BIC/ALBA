/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewOrthoSlice.h,v $
  Language:  C++
  Date:      $Date: 2005-12-21 13:55:24 $
  Version:   $Revision: 1.6 $
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
class mmgLutSlider;
class mmgFloatSlider;
class mafViewSlice;
class mafVMEVolume;
class vtkLookupTable;
//----------------------------------------------------------------------------
// mafViewOrthoSlice :
//----------------------------------------------------------------------------
/** */
class mafViewOrthoSlice: public mafViewCompound
{
public:
  mafViewOrthoSlice(wxString label = "View OrthoSlice", bool external = false);
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

  /** 
  Create visual pipe and initialize them to build an OrthoSlice visualization */
  void PackageView();
  
  /** IDs for the GUI */
  enum VIEW_ORTHO_SLICE_WIDGET_ID
  {
    ID_ORTHO_SLICE_X = Superclass::ID_LAST,
    ID_ORTHO_SLICE_Y,
    ID_ORTHO_SLICE_Z,
    ID_LUT_CHOOSER,
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
  Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

  mmgFloatSlider *m_SliderX;
  mmgFloatSlider *m_SliderY;
  mmgFloatSlider *m_SliderZ;

  double m_Origin[3];

  mafVMEVolume *m_CurrentVolume; ///< Current visualized volume
  mmgLutSlider *m_Luts; ///< Double slider used to change brightness and contrast of the image
  vtkLookupTable *m_ColorLUT;
  mafViewSlice *m_Views[4];
};
#endif
