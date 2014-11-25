/*=========================================================================

 Program: MAF2Medical
 Module: mafViewSingleSliceCompound
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafViewSingleSliceCompound_H__
#define __mafViewSingleSliceCompound_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "medViewCompoundWindowing.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafSceneGraph;
class mafGUI;
class mafRWIBase;
class mafViewSingleSlice;
class mafGUILutSlider;
class mafGUILutSwatch;
class mafGUIFloatSlider;
class vtkLookupTable;

/**
  Class Name: mafViewSingleSliceCompound.
  Class that contain mafViewSingleSlice in order to add gui view widget like windowing buttons.
*/
class MAF_EXPORT mafViewSingleSliceCompound: public medViewCompoundWindowing
{
public:
  /** constructor */
  mafViewSingleSliceCompound(wxString label = "View Single Slice Compound with Windowing", int num_row = 1, int num_col = 1);
  /** destructor. */
  virtual ~mafViewSingleSliceCompound(); 

  /** RTTI macro*/
  mafTypeMacro(mafViewSingleSliceCompound, medViewCompoundWindowing);


  /** Redefine this method to package the compounded view */
  virtual void PackageView();

  /** Function that clones instance of the object. */
  virtual mafView *Copy(mafObserver *Listener, bool lightCopyEnabled = false);
 

  /** Function called when select a vme different from selected one.*/
	//virtual void VmeSelect(mafNode *node, bool select);

  /** Calculate areas for child views in order to create a layout of the compound view.*/
  virtual void OnLayout();

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mafGUI  *CreateGui();

	mafViewSingleSlice	*m_ViewSingleSlice;

};
#endif
