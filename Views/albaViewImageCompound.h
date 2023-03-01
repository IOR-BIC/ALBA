/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewImageCompound
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaViewImageCompound_H__
#define __albaViewImageCompound_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewCompound.h"

#include "vtkALBASimpleRulerActor2D.h"
#include "vtkRenderWindow.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaSceneGraph;
class albaGUI;
class albaRWIBase;
class albaViewImage;
class albaGUILutSlider;
class albaGUILutSwatch;
class albaGUIFloatSlider;
class albaGUIPicButton;
class vtkLookupTable;
class albaVMEImage;
class vtkALBASimpleRulerActor2D;

/**
  Class Name: albaViewImageCompound.
  Class that contain albaViewImage in order to add gui view widget like windowing buttons.
*/
class ALBA_EXPORT albaViewImageCompound: public albaViewCompound
{
public:
  /** constructor. */
  albaViewImageCompound(wxString label = "View Single Slice Compound", int num_row = 1, int num_col = 1);
  /** destructor. */
  virtual ~albaViewImageCompound(); 

  /** RTTI macro.*/
  albaTypeMacro(albaViewImageCompound, albaViewCompound);

	/** IDs for the view GUI */
	enum VIEW_SLICE_WIDGET_ID
	{
		ID_LUT_CHOOSER = Superclass::ID_LAST,
		ID_REVERSE_LUT,
		ID_VIEW_RULER,
		ID_LAST
	};

  /**  Redefine this method to package the compounded view */
  virtual void PackageView();

  /** Function that clones instance of the object. */
  virtual albaView *Copy(albaObserver *Listener, bool lightCopyEnabled = false);
  
  /**  Create the GUI on the bottom of the compounded view. */
  virtual void     CreateGuiView();
  
  /** Function that handles events sent from other objects. */
  virtual void     OnEvent(albaEventBase *alba_event);

  void LutReverse();

	/** Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(albaVME *vme, bool show);

	/** Function called on VME remove */
	virtual void VmeRemove(albaVME *vme);

	bool RulerIsVisible() { return m_ShowRuler; };
	void ShowRuler(bool show);

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
	void EnableWidgets(bool enable);

  /** Update lutslider with correct values in case of bool variable is true, otherwise disable the widget. */
	void UpdateWindowing(bool enable);

	void SetRendererByView();

	albaViewImage	*m_ViewImage;

	albaVMEImage *m_CurrentImage;

	albaGUILutSwatch		*m_LutWidget; ///< LUT widget in view side panel 
	albaGUILutSlider		*m_LutSlider;

	vtkLookupTable	*m_ColorLUT;

	vtkALBASimpleRulerActor2D *m_Ruler;
	vtkRenderer *m_Renderer;
	albaGUIPicButton *m_RulerButton;
	bool m_ShowRulerButton;
	bool m_ShowRuler;

	albaGUIPicButton *m_ReverseLUTButton;
	bool m_ShowReverseLUTButton;
	bool m_IsLutReversed;
	int m_CurrentLUTPreset;
};
#endif
