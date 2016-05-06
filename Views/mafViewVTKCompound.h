/*=========================================================================

 Program: MAF2
 Module: mafViewVTKCompound
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafViewVTKCompound_H__
#define __mafViewVTKCompound_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafViewCompound.h"
#include "mafViewCompoundWindowing.h"

#include "mafViewVTK.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafViewVTK;
class mafSceneGraph;
class mafGUI;
class mafRWIBase;
class mafViewImage;
class mafGUILutSlider;
class mafGUILutSwatch;
class mafGUIFloatSlider;
class vtkLookupTable;
class vtkWindowLevelLookupTable;

/**
  Class Name: mafViewVTKCompound.
  Class that contain mafViewVTK. Derived by mafViewCompoundWindowing in order to add gui view widget like windowing buttons.

*/
class MAF_EXPORT mafViewVTKCompound: public mafViewCompoundWindowing
{
public:
  /** constructor. */
  mafViewVTKCompound(wxString label = "View Compound with Windowing", int num_row = 1, int num_col = 1);
  /** destructor. */
  virtual ~mafViewVTKCompound(); 

  /** RTTI macro.*/
  mafTypeMacro(mafViewVTKCompound, mafViewCompoundWindowing);


  /**  Redefine this method to package the compounded view */
  virtual void PackageView();

  /** Function to set customized mafViewVTK as child view. To be called before PackageView. */
  virtual void SetExternalView(mafViewVTK *childView);

  /** Function that clones instance of the object. */
  virtual mafView *Copy(mafObserver *Listener, bool lightCopyEnabled = false);

  /** Update camera */
  virtual void CameraUpdate();

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mafGUI  *CreateGui();

   /** A function with windowing activate/deactivate conditions*/
  virtual bool ActivateWindowing(mafVME *vme);

	mafViewVTK *m_ViewVTK;

};
#endif


