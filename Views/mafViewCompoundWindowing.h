/*=========================================================================

 Program: MAF2
 Module: mafViewCompoundWindowing
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafViewCompoundWindowing_H__
#define __mafViewCompoundWindowing_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafViewCompound.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafSceneGraph;
class mafGUI;
class mafRWIBase;
class mafGUILutSlider;
class mafGUILutSwatch;
class mafGUIFloatSlider;
class mafVMEImage;
class vtkLookupTable;
class vtkWindowLevelLookupTable;

/**
  Class Name: mafViewCompoundWindowing.
  Abstract class managing gui view widget like windowing buttons.
*/
class MAF_EXPORT mafViewCompoundWindowing: public mafViewCompound
{
public:
  /** constructor */
  mafViewCompoundWindowing(wxString label = "View Compound with Windowing", int num_row = 1, int num_col = 1);
  /** destructor. */
  virtual ~mafViewCompoundWindowing(); 

	/** IDs for the view GUI */
	enum VIEW_SLICE_WIDGET_ID
	{
		ID_LUT_CHOOSER = Superclass::ID_LAST,
		ID_LAST
	};

  /** Redefine this method to package the compounded view */
  virtual void PackageView()=0;

  /** Function that clones instance of the object. */
  virtual mafView *Copy(mafObserver *Listener, bool lightCopyEnabled = false)=0;
  
  /** 
  Create the GUI on the bottom of the compounded view. */
  virtual void     CreateGuiView();
  
  /** Function that handles events sent from other objects. */
  virtual void     OnEvent(mafEventBase *maf_event);

	/** 
  Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(mafNode *node, bool show);

  /** Function called when select a vme different from selected one.*/
	virtual void VmeSelect(mafNode *node, bool select);

  /** Calculate areas for child views in order to create a layout of the compound view.*/
  //virtual void OnLayout()=0;

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mafGUI  *CreateGui()=0;

  /** Function for enable/disable all gui widgets. */
  virtual void EnableWidgets(bool enable);

  /** A function with windowing activate/deactivate conditions*/
  virtual bool ActivateWindowing(mafNode *node);

  /** Update lutslider with correct values in case of bool variable is true, otherwise disable the widget. */
	virtual void UpdateWindowing(bool enable,mafNode *node);

  /** Update windowing for image data*/
  virtual void ImageWindowing(mafVMEImage *image);

  /** Update windowing for volume data*/
  virtual void VolumeWindowing(mafVME *volume);

	mafGUILutSwatch		*m_LutWidget; ///< LUT widget in view side panel 
	mafGUILutSlider		*m_LutSlider;
	vtkLookupTable	        *m_ColorLUT;
	
	/** Function that get node pipe of first child     */
	virtual mafPipe* GetNodePipe(mafNode *vme);

};
#endif
