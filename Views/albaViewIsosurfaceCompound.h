/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module:  albaViewIsosurfaceCompound
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaViewIsosurfaceCompound_H__
#define __albaViewIsosurfaceCompound_H__

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
class albaViewVTK;
class vtkLookupTable;
class albaVMEVolumeGray;
class albaGUIFloatSlider;

/**
  Class Name: albaViewIsosurfaceCompound.
*/
class ALBA_EXPORT albaViewIsosurfaceCompound : public albaViewCompound
{
public:
  /** constructor. */
	albaViewIsosurfaceCompound(wxString label = "View Isosurface Compound", int num_row = 1, int num_col = 1);
  /** destructor. */
  virtual ~albaViewIsosurfaceCompound();

  /** RTTI macro.*/
  albaTypeMacro(albaViewIsosurfaceCompound, albaViewCompound);

	/** IDs for the view GUI */
	enum VIEW_SLICE_WIDGET_ID
	{
		ID_CONTOUR_VALUE_ISO = Superclass::ID_LAST,
		ID_TEXT_CONTOUR_VALUE_ISO,
		ID_LAST
	};

  /**  Redefine this method to package the compounded view */
  virtual void PackageView();

  /** Function that clones instance of the object. */
  virtual albaView *Copy(albaObserver *Listener, bool lightCopyEnabled = false);
  
  /**  Create the GUI on the bottom of the compounded view. */
  virtual void CreateGuiView();
  
  /** Function that handles events sent from other objects. */
  virtual void OnEvent(albaEventBase *alba_event);
	
	/** Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(albaVME *vme, bool show);

	/** Function called on VME remove */
	virtual void VmeRemove(albaVME *vme);

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual albaGUI *CreateGui();

  /** Update Slider with correct values */
	void UpdateISO();

	albaViewVTK				*m_ViewVolume;
	albaVMEVolumeGray	*m_CurrentVolume;

	albaGUIFloatSlider *m_SliderContourIso;

	double m_ContourValueIso;
};
#endif
