/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewArbitrarySlice.h,v $
  Language:  C++
  Date:      $Date: 2009-07-16 09:34:24 $
  Version:   $Revision: 1.8.2.1 $
  Authors:   Eleonora Mambrini
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafViewArbitrarySlice_H__
#define __mafViewArbitrarySlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medViewCompoundWindowing.h"

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
class mafGUI;
class mafGUILutSlider;
class mafGUILutSwatch;
class medVMEPolylineEditor;

//----------------------------------------------------------------------------
// mafViewOrthoSlice :
//----------------------------------------------------------------------------
/** 
  This compound view is made of four child views used to analyze different orthogonal slices of the volume*/
class mafViewArbitrarySlice: public medViewCompoundWindowing
{
public:
  mafViewArbitrarySlice(wxString label = "View Arbitrary Slice with Windowing", bool show_ruler = false);
  virtual ~mafViewArbitrarySlice(); 

  mafTypeMacro(mafViewArbitrarySlice, medViewCompoundWindowing);

	enum ID_GUI
	{
		ID_COMBO_GIZMOS = Superclass::ID_LAST,
		ID_RESET,
		ID_LAST,
	};

	/** Create visual pipe and initialize them to build an OrthoSlice visualization */
  virtual void PackageView();

	/** Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(mafNode *node, bool show);

	/** Remove VME into plugged sub-views*/
  virtual void VmeRemove(mafNode *node);

	/** Create the GUI on the bottom of the compounded view. */
  virtual void CreateGuiView();

  /** Function that handles events sent from other objects. */
	virtual void OnEvent(mafEventBase *maf_event);

  /** Function that clones instance of the object. */
	virtual mafView* Copy(mafObserver *Listener);

	virtual void CameraUpdate();

  void UpdateSlicerBehavior();

protected:

	/** Internally used to create a new instance of the GUI. 
  This function should be overridden by subclasses to create specialized GUIs. 
  Each subclass should append its own widgets and define the enum of IDs for the widgets as 
  an extension of the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same panel GUI, each CreateGUI() function should first call the superclass' one.*/
	virtual mafGUI* CreateGui();

	/** Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

	void OnEventThis(mafEventBase *maf_event);  

	/** This function is called when a rotate gizmo is moved*/
	void OnEventGizmoRotate(mafEventBase *maf_event);

	/**	This function is called when a translate gizmo is moved*/
	void OnEventGizmoTranslate(mafEventBase *maf_event);

	void PostMultiplyEventMatrix(mafEventBase *maf_event);

	mafViewVTK *m_ViewSlice;
	mafViewVTK *m_ViewArbitrary;

	mafGizmoTranslate *m_GizmoTranslate;
	mafGizmoRotate		*m_GizmoRotate;
	mafVME          	*m_CurrentVolume;
  mafVME				    *m_CurrentImage;
	mafVMESlicer			*m_Slicer;
	mafMatrix					*m_MatrixReset;
	mafAttachCamera		*m_AttachCamera;

	double	m_SliceCenterSurface[3];
	double	m_SliceCenterSurfaceReset[3];
	double	m_SliceAngleReset[3];
	int			m_TypeGizmo;

	mafGUI				*m_GuiGizmos;

  medVMEPolylineEditor *m_CurrentPolylineGraphEditor;


};
#endif
