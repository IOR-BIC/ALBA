/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewGlobalSliceCompound
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaViewGlobalSliceCompound_H__
#define __albaViewGlobalSliceCompound_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewCompoundWindowing.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaSceneGraph;
class albaGUI;
class albaRWIBase;
class albaViewGlobalSlice;
class albaGUIFloatSlider;


/**
  Class Name: albaViewGlobalSliceCompound.
  Class that contain albaViewGlobalSlice in order to add gui view widget like windowing buttons.
*/
class ALBA_EXPORT albaViewGlobalSliceCompound: public albaViewCompoundWindowing
{
public:
  /** constructor .*/
  albaViewGlobalSliceCompound(wxString label = "View Global Slice Compound", int num_row = 1, int num_col = 1);
  /** destructor .*/
  virtual ~albaViewGlobalSliceCompound(); 

  /** RTTI macro. */
  albaTypeMacro(albaViewGlobalSliceCompound, albaViewCompoundWindowing);

  /** Redefine this method to package the compounded view */
  virtual void PackageView();

  /** Function that clones instance of the object. */
  virtual albaView *Copy(albaObserver *Listener, bool lightCopyEnabled = false);

  /** Function that handles events sent from other objects. */
  virtual void     OnEvent(albaEventBase *alba_event);

  /** Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(albaVME *vme, bool show);

  virtual void VmeSelect(albaVME *node, bool select);

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual albaGUI  *CreateGui();

  /** Function for enable/disable all gui widgets. */
	//void EnableWidgets(bool enable);

  albaViewGlobalSlice	*m_ViewGlobalSlice;

	std::vector<albaVME *>	m_VisibleVMEs;
	std::vector<vtkLookupTable *> m_LUTs;

private:
  void UpdateLutSlider();
};
#endif
