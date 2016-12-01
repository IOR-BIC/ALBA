/*=========================================================================

 Program: MAF2
 Module:  mafViewIsosurfaceCompound
 Authors: Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafViewIsosurfaceCompound_H__
#define __mafViewIsosurfaceCompound_H__

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
class mafViewVTK;
class vtkLookupTable;
class mafVMEVolumeGray;
class mafGUIFloatSlider;

/**
  Class Name: mafViewIsosurfaceCompound.
*/
class MAF_EXPORT mafViewIsosurfaceCompound : public mafViewCompound
{
public:
  /** constructor. */
	mafViewIsosurfaceCompound(wxString label = "View Isosurface Compound", int num_row = 1, int num_col = 1);
  /** destructor. */
  virtual ~mafViewIsosurfaceCompound();

  /** RTTI macro.*/
  mafTypeMacro(mafViewIsosurfaceCompound, mafViewCompound);

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
  virtual mafView *Copy(mafObserver *Listener, bool lightCopyEnabled = false);
  
  /**  Create the GUI on the bottom of the compounded view. */
  virtual void CreateGuiView();
  
  /** Function that handles events sent from other objects. */
  virtual void OnEvent(mafEventBase *maf_event);
	
	/** Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(mafVME *vme, bool show);

	/** Function called on VME remove */
	virtual void VmeRemove(mafVME *vme);

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mafGUI *CreateGui();

  /** Update Slider with correct values */
	void UpdateISO();

	mafViewVTK				*m_ViewVolume;
	mafVMEVolumeGray	*m_CurrentVolume;

	mafGUIFloatSlider *m_SliderContourIso;

	double m_ContourValueIso;
};
#endif
