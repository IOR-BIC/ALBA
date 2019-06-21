/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewCompoundWindowing
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaViewCompoundWindowing_H__
#define __albaViewCompoundWindowing_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewCompound.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaSceneGraph;
class albaGUI;
class albaRWIBase;
class albaGUILutSlider;
class albaGUILutSwatch;
class albaGUIFloatSlider;
class albaVMEImage;
class vtkLookupTable;
class vtkWindowLevelLookupTable;

/**
  Class Name: albaViewCompoundWindowing.
  Abstract class managing gui view widget like windowing buttons.
*/
class ALBA_EXPORT albaViewCompoundWindowing: public albaViewCompound
{
public:
  /** constructor */
  albaViewCompoundWindowing(wxString label = "View Compound with Windowing", int num_row = 1, int num_col = 1);
  /** destructor. */
  virtual ~albaViewCompoundWindowing(); 

	/** IDs for the view GUI */
	enum VIEW_SLICE_WIDGET_ID
	{
		ID_LUT_CHOOSER = Superclass::ID_LAST,
		ID_LAST
	};

  /** Redefine this method to package the compounded view */
  virtual void PackageView()=0;

  /** Function that clones instance of the object. */
  virtual albaView *Copy(albaObserver *Listener, bool lightCopyEnabled = false)=0;
  
  /** 
  Create the GUI on the bottom of the compounded view. */
  virtual void     CreateGuiView();
  
  /** Function that handles events sent from other objects. */
  virtual void     OnEvent(albaEventBase *alba_event);

	/** 
  Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(albaVME *vme, bool show);

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
  virtual albaGUI  *CreateGui()=0;

  /** Function for enable/disable all gui widgets. */
  virtual void EnableWidgets(bool enable);

  /** A function with windowing activate/deactivate conditions*/
  virtual bool ActivateWindowing(albaVME *vme);

  /** Update lutslider with correct values in case of bool variable is true, otherwise disable the widget. */
	virtual void UpdateWindowing(bool enable,albaVME *vme);

  /** Update windowing for image data*/
  virtual void ImageWindowing(albaVMEImage *image);

  /** Update windowing for volume data*/
  virtual void VolumeWindowing(albaVME *volume);

	albaGUILutSwatch		*m_LutWidget; ///< LUT widget in view side panel 
	albaGUILutSlider		*m_LutSlider;
	vtkLookupTable	        *m_ColorLUT;
	
	/** Function that get vme pipe of first child     */
	virtual albaPipe* GetNodePipe(albaVME *vme);

};
#endif
