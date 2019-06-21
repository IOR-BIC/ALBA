/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewSlicer
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaViewSlicer_H__
#define __albaViewSlicer_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewCompoundWindowing.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaViewVTK;
class albaGizmoTranslate;
class albaGizmoRotate;
class albaVMEVolumeGray;
class albaVMESlicer;
class albaVMEImage;
class albaMatrix;
class albaAttachCamera;
class albaGUI;
class albaGUILutSlider;
class albaGUILutSwatch;

//----------------------------------------------------------------------------
// albaViewOrthoSlice :
//----------------------------------------------------------------------------
/** 
  This compound view is made of four child views used to analyze different orthogonal slices of the volume*/
class ALBA_EXPORT albaViewSlicer: public albaViewCompoundWindowing
{
public:
  albaViewSlicer(wxString label = "View Arbitrary Slice", bool show_ruler = false);
  virtual ~albaViewSlicer(); 

  albaTypeMacro(albaViewSlicer, albaViewCompoundWindowing);

	enum ID_GUI
	{
		ID_COMBO_GIZMOS = Superclass::ID_LAST,
		ID_RESET,
		ID_LAST,
	};

  /** 
  Set the visualization status for the vme (visible, not visible, mutex, ...) 
  \sa albaSceneGraph albaView*/
  int GetNodeStatus(albaVME *vme);

	/** Create visual pipe and initialize them to build an OrthoSlice visualization */
  virtual void PackageView();

	/** Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(albaVME *vme, bool show);

	/** Remove VME into plugged sub-views*/
  virtual void VmeRemove(albaVME *vme);

	/** Create the GUI on the bottom of the compounded view. */
  virtual void CreateGuiView();

  /** Function that handles events sent from other objects. */
	virtual void OnEvent(albaEventBase *alba_event);

   /** Function that clones instance of the object. */
	virtual albaView* Copy(albaObserver *Listener, bool lightCopyEnabled = false);

  /** Force the updating of the camera. */
	virtual void CameraUpdate();
	  
protected:

	/** Internally used to create a new instance of the GUI. 
  This function should be overridden by subclasses to create specialized GUIs. 
  Each subclass should append its own widgets and define the enum of IDs for the widgets as 
  an extension of the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same panel GUI, each CreateGUI() function should first call the superclass' one.*/
	virtual albaGUI* CreateGui();

	/** Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

  /** Handling events sent from other objects. Called by public method OnEvent().*/
	void OnEventThis(albaEventBase *alba_event);  

  /**Protected method returning true if windowing can be enabled.
  It overrides superclass method.*/
  bool ActivateWindowing(albaVME *vme);

  /**Update lutslider with correct values in case of bool variable is true, otherwise disable the widget. */
  void UpdateWindowing(bool enable, albaVME *vme);

  /** Update windowing on slicer visualization. */
  void SlicerWindowing(albaVMESlicer *slicer);

	
	albaViewVTK *m_ViewSlice;
	albaViewVTK *m_ViewArbitrary;

	
	albaVME	      *m_CurrentVolume;
  albaVMEImage   *m_CurrentImage;
	albaVMESlicer  *m_CurrentSlicer;

	albaAttachCamera		*m_AttachCamera;

	double	m_SliceCenterSurface[3];
	double	m_SliceCenterSurfaceReset[3];
	double	m_SliceAngleReset[3];
	
};
#endif
