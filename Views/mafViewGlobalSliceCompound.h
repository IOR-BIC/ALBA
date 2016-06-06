/*=========================================================================

 Program: MAF2
 Module: mafViewGlobalSliceCompound
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafViewGlobalSliceCompound_H__
#define __mafViewGlobalSliceCompound_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafViewCompoundWindowing.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafSceneGraph;
class mafGUI;
class mafRWIBase;
class mafViewGlobalSlice;
class mafGUIFloatSlider;


/**
  Class Name: mafViewGlobalSliceCompound.
  Class that contain mafViewGlobalSlice in order to add gui view widget like windowing buttons.
*/
class MAF_EXPORT mafViewGlobalSliceCompound: public mafViewCompoundWindowing
{
public:
  /** constructor .*/
  mafViewGlobalSliceCompound(wxString label = "View Global Slice Compound", int num_row = 1, int num_col = 1);
  /** destructor .*/
  virtual ~mafViewGlobalSliceCompound(); 

  /** RTTI macro. */
  mafTypeMacro(mafViewGlobalSliceCompound, mafViewCompoundWindowing);

  /** Redefine this method to package the compounded view */
  virtual void PackageView();

  /** Function that clones instance of the object. */
  virtual mafView *Copy(mafObserver *Listener, bool lightCopyEnabled = false);

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mafGUI  *CreateGui();

  /** Function for enable/disable all gui widgets. */
	//void EnableWidgets(bool enable);

  mafViewGlobalSlice	*m_ViewGlobalSlice;

};
#endif
