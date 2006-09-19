/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewArbitrarySlice.h,v $
  Language:  C++
  Date:      $Date: 2006-09-19 12:23:03 $
  Version:   $Revision: 1.2 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafViewArbitrarySlice_H__
#define __mafViewArbitrarySlice_H__

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

//----------------------------------------------------------------------------
// mafViewOrthoSlice :
//----------------------------------------------------------------------------
/** 
  This compound view is made of four child views used to analyze different orthogonal slices of the volume*/
class mafViewArbitrarySlice: public mafViewCompound
{
public:
  mafViewArbitrarySlice(wxString label = "View Arbitrary Slice", bool show_ruler = false);
  virtual ~mafViewArbitrarySlice(); 

  mafTypeMacro(mafViewArbitrarySlice, mafViewCompound);

	enum ID_GUI
	{
		ID_COMBO_GIZMOS = Superclass::ID_LAST,
		ID_RESET,
		ID_LAST,
	};

	/** 
  Create visual pipe and initialize them to build an OrthoSlice visualization */
  virtual void PackageView();

	/** 
  Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(mafNode *node, bool show);

	/** 
	Remove VME into plugged sub-views*/
  virtual void VmeRemove(mafNode *node);

	virtual void OnEvent(mafEventBase *maf_event);

	virtual mmgGui* CreateGui();

	virtual mafView* Copy(mafObserver *Listener);

	virtual void CameraUpdate();

protected:

	void OnEventThis(mafEventBase *maf_event);  

	/**
	This function is called when a rotate gizmo is moved*/
	void OnEventGizmoRotate(mafEventBase *maf_event);

	/**
	This function is called when a tanslate gizmo is moved*/
	void OnEventGizmoTranslate(mafEventBase *maf_event);

	void PostMultiplyEventMatrix(mafEventBase *maf_event);

	mafViewVTK *m_ViewSlice;
	mafViewVTK *m_ViewArbitrary;

	mafGizmoTranslate *m_GizmoTranslate;
	mafGizmoRotate		*m_GizmoRotate;
	mafVMEVolumeGray	*m_CurrentVolume;
	mafVMESlicer			*m_Slicer;
	mafMatrix					*m_MatrixReset;
	mafAttachCamera		*m_AttachCamera;

	vtkLookupTable    *m_vtkLUT;

	double	m_SliceCenterSurface[3];
	double	m_SliceCenterSurfaceReset[3];
	double	m_SliceAngleReset[3];
	int			m_TypeGizmo;

	mmgGui	*m_GuiGizmos;
};
#endif
