/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewVTKCompound
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaViewVTKCompound_H__
#define __albaViewVTKCompound_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewCompound.h"
#include "albaViewCompoundWindowing.h"

#include "albaViewVTK.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaViewVTK;
class albaSceneGraph;
class albaGUI;
class albaRWIBase;
class albaViewImage;
class albaGUILutSlider;
class albaGUILutSwatch;
class albaGUIFloatSlider;
class vtkLookupTable;
class vtkWindowLevelLookupTable;

/**
  Class Name: albaViewVTKCompound.
  Class that contain albaViewVTK. Derived by albaViewCompoundWindowing in order to add gui view widget like windowing buttons.

*/
class ALBA_EXPORT albaViewVTKCompound: public albaViewCompoundWindowing
{
public:
  /** constructor. */
  albaViewVTKCompound(wxString label = "View Compound with Windowing", int num_row = 1, int num_col = 1);
  /** destructor. */
  virtual ~albaViewVTKCompound(); 

  /** RTTI macro.*/
  albaTypeMacro(albaViewVTKCompound, albaViewCompoundWindowing);


  /**  Redefine this method to package the compounded view */
  virtual void PackageView();

  /** Function to set customized albaViewVTK as child view. To be called before PackageView. */
  virtual void SetExternalView(albaViewVTK *childView);

  /** Function that clones instance of the object. */
  virtual albaView *Copy(albaObserver *Listener, bool lightCopyEnabled = false);

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
  virtual albaGUI  *CreateGui();

   /** A function with windowing activate/deactivate conditions*/
  virtual bool ActivateWindowing(albaVME *vme);

	albaViewVTK *m_ViewVTK;

};
#endif


