/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewGlobalSlice.h,v $
  Language:  C++
  Date:      $Date: 2011-12-27 16:49:07 $
  Version:   $Revision: 1.9.2.9 $
  Authors:   Matteo Giacomoni, Simone Brazzale
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafViewGlobalSlice_H__
#define __mafViewGlobalSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medDefines.h"
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
class mafGUIFloatSlider;
class vtkActor2D;
class vtkTextMapper;
class vtkPlaneSource;
class vtkOutlineCornerFilter;
class vtkProperty;
class vtkPolyDataMapper;
class vtkActor;

/**
  Class Name: mafViewGlobalSlice.
  View class that visualize actors sliced from an implicit plane that operates in all
  the 3d world.
*/

class MED_EXPORT mafViewGlobalSlice: public mafViewVTK
{
public:
  /** constructor. */
  mafViewGlobalSlice(wxString label = "Global Slice", int camera_position = CAMERA_PERSPECTIVE, bool show_axes = false, bool show_grid = false, bool show_ruler = false, int stereo = 0);
  /** destructor. */
  virtual ~mafViewGlobalSlice(); 

  /** RTTI macro. */
  mafTypeMacro(mafViewGlobalSlice, mafViewVTK);

	/** IDs for the view GUI */
  enum VIEW_SLICE_WIDGET_ID
  {
    ID_POS_SLIDER = Superclass::ID_LAST,
		ID_CHANGE_VIEW,
		ID_OPACITY_SLIDER,
    ID_TRILINEAR_INTERPOLATION_ON,
    ID_LAST
  };
  
  /** Function that clones instance of the object. */
	virtual mafView *Copy(mafObserver *Listener, bool lightCopyEnabled = false);

  /** Function that creates renderwindow, scenegraph, specific actors and interactors. */
	virtual void Create();

	/** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(mafNode *vme);

	/** Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(mafNode *vme);

  /** Function that handles events sent from other objects. */
	virtual void OnEvent(mafEventBase *maf_event);

  /** Function called when select a vme different from selected one.*/
	virtual void VmeSelect(mafNode *node, bool select);

	/** Set slicer parameter to generate the slice. */
  virtual void SetSlice(double origin[3], float xVect[3], float yVect[3]);
	
	/** Set slicer parameter to generate the slice. */
	virtual void SetSlice(double origin[3], double dn = 0.0);

	/** Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(mafNode *node, bool show);

  /** Force the updating of the camera. */
	virtual void CameraUpdate();

protected:
  /** Force the updating of several slice parameters like origin, vectors and gui components. */
	virtual void UpdateSliceParameters();
  /** Calculate for every actor the correct slice. */
	virtual void UpdateSlice();
  /** Update the correct value corresponding to specific slice; this value is visualized with an actor.*/
	virtual void UpdateText();
  /** Initialize position of the plane and actor properties.*/
	virtual void InizializePlane();
  /** Update origin, point1 and point2 of the vtkPlaneSource. */
	virtual void UpdatePlane();

  /** Create gui widgets that will be attached in view settings tab in side bar.*/
	virtual mafGUI  *CreateGui();

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
  bool      m_GlobalBoundsValid;

	mafSceneNode		*m_SelectedVolume;
	mafGUIFloatSlider	*m_GlobalSlider;
	mafGUIFloatSlider	*m_OpacitySlider;

	vtkActor2D					*m_TextActor;
	vtkTextMapper				*m_TextMapper;
	mafString						 m_Text;
	double							 m_TextColor[3];

  int m_NumberOfVmeInstantiated;
  std::map<mafID,int> m_MapID;

  std::vector<vtkPlaneSource*>					m_BoundsPlane;
  std::vector<vtkOutlineCornerFilter*>	m_BoundsOutlineBox;
  std::vector<vtkProperty*>             m_BoundsOutlineProperty;
  std::vector<vtkPolyDataMapper*>       m_BoundsOutlineMapper;
  std::vector<vtkActor*>	              m_BoundsOutlineActor;

  int m_TrilinearInterpolationOn;
};
#endif
