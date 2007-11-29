/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medViewSlicer.h,v $
  Language:  C++
  Date:      $Date: 2007-11-29 08:55:09 $
  Version:   $Revision: 1.3 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medViewSlicer_H__
#define __medViewSlicer_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewCompound.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafViewVTK;
class mafGizmoTranslate;
class mafGizmoRotate;
class mafVMEVolumeGray;
class mafVMESlicer;
class mafMatrix;
class mafAttachCamera;
class mmgGui;
class mmgLutSlider;
class mmgLutSwatch;

//----------------------------------------------------------------------------
// mafViewOrthoSlice :
//----------------------------------------------------------------------------
/** 
  This compound view is made of four child views used to analyze different orthogonal slices of the volume*/
class medViewSlicer: public mafViewCompound
{
public:
  medViewSlicer(wxString label = "View Arbitrary Slice", bool show_ruler = false);
  virtual ~medViewSlicer(); 

  mafTypeMacro(medViewSlicer, mafViewCompound);

	enum ID_GUI
	{
		ID_COMBO_GIZMOS = Superclass::ID_LAST,
		ID_RESET,
		ID_LUT_CHOOSER,
		ID_LAST,
	};

  /** 
  Set the visualization status for the node (visible, not visible, mutex, ...) 
  \sa mafSceneGraph mafView*/
  int GetNodeStatus(mafNode *vme);

	/** Create visual pipe and initialize them to build an OrthoSlice visualization */
  virtual void PackageView();

	/** Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(mafNode *node, bool show);

	/** Remove VME into plugged sub-views*/
  virtual void VmeRemove(mafNode *node);

	/** Create the GUI on the bottom of the compounded view. */
  virtual void CreateGuiView();

	virtual void OnEvent(mafEventBase *maf_event);

	virtual mafView* Copy(mafObserver *Listener);

	virtual void CameraUpdate();

protected:

	/** Internally used to create a new instance of the GUI. 
  This function should be overridden by subclasses to create specialized GUIs. 
  Each subclass should append its own widgets and define the enum of IDs for the widgets as 
  an extension of the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same panel GUI, each CreateGUI() function should first call the superclass' one.*/
	virtual mmgGui* CreateGui();

	/** Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

	void OnEventThis(mafEventBase *maf_event);  

	
	mafViewVTK *m_ViewSlice;
	mafViewVTK *m_ViewArbitrary;

	
	mafVME	*m_CurrentVolume;
	mafVMESlicer			*m_CurrentSlicer;

	mafAttachCamera		*m_AttachCamera;

	vtkLookupTable    *m_ColorLUT;

	double	m_SliceCenterSurface[3];
	double	m_SliceCenterSurfaceReset[3];
	double	m_SliceAngleReset[3];
	

	mmgLutSlider	*m_LutSlider; ///< Double slider used to change brightness and contrast of the image
	mmgLutSwatch	*m_LutWidget; ///< LUT widget in view side panel 
	
};
#endif
