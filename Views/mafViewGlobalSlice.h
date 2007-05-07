/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewGlobalSlice.h,v $
  Language:  C++
  Date:      $Date: 2007-05-07 12:13:03 $
  Version:   $Revision: 1.7 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafViewGlobalSlice_H__
#define __mafViewGlobalSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewVTK.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafView;
class mafObserver;
class mafSceneNode;
class mafEventBase;
class mafString;
class vtkActor2D;
class vtkTextMapper;
class vtkPlaneSource;
class vtkOutlineCornerFilter;
class vtkProperty;
class vtkPolyDataMapper;
class vtkActor;

//----------------------------------------------------------------------------
// mafViewGlobalSlice :
//----------------------------------------------------------------------------

class mafViewGlobalSlice: public mafViewVTK
{
public:
  mafViewGlobalSlice(wxString label = "Global Slice", int camera_position = CAMERA_PERSPECTIVE, bool show_axes = false, bool show_grid = false, bool show_ruler = false, int stereo = 0);
  virtual ~mafViewGlobalSlice(); 

  mafTypeMacro(mafViewGlobalSlice, mafViewVTK);

	/** IDs for the view GUI */
  enum VIEW_SLICE_WIDGET_ID
  {
    ID_POS_SLIDER = Superclass::ID_LAST,
		ID_CHANGE_VIEW,
		ID_OPACITY_SLIDER,
    ID_LAST
  };

	virtual mafView*  Copy(mafObserver *Listener);

	virtual void Create();

	/** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(mafNode *vme);

	/** 
  Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(mafNode *vme);

	virtual void OnEvent(mafEventBase *maf_event);

	virtual void VmeSelect(mafNode *node, bool select);

	/** 
	Set slicer parameter to generate the slice. */
  void SetSlice(double origin[3], float xVect[3], float yVect[3]);
	
	/** 
	Set slicer parameter to generate the slice. */
	void SetSlice(double origin[3], double dn = 0.0);

	/** 
  Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(mafNode *node, bool show);

	virtual void CameraUpdate();

protected:

	void UpdateSliceParameters();
	void UpdateSlice();
	void UpdateText();
	void InizializePlane();
	void UpdatePlane();

	virtual mmgGui  *CreateGui();

	double		m_SliceOrigin[3];
	float			m_SliceXVector[3];
	float			m_SliceYVector[3];
	float			m_SliceNormal[3];
	double		m_GlobalBounds[6];
	double		m_SliderOldOrigin;
	double		m_SliderOrigin;
	double		m_Opacity;
	int				m_IDPlane;
	int				m_SliceMode;
	int				m_ViewIndex;
	double		m_Dn;
	bool			m_GlobalBoundsInitialized;

	mafSceneNode		*m_SelectedVolume;
	mmgFloatSlider	*m_GlobalSlider;
	mmgFloatSlider	*m_OpacitySlider;

	vtkActor2D					*m_TextActor;
	vtkTextMapper				*m_TextMapper;
	mafString						 m_Text;
	double							 m_TextColor[3];

	vtkPlaneSource					*m_BoundsPlane;
	vtkOutlineCornerFilter	*m_BoundsOutlineBox;
	vtkProperty             *m_BoundsOutlineProperty;
	vtkPolyDataMapper       *m_BoundsOutlineMapper;
	vtkActor	              *m_BoundsOutlineActor;
};
#endif
