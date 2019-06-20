/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewGlobalSlice
 Authors: Matteo Giacomoni, Simone Brazzale
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaViewGlobalSlice_H__
#define __albaViewGlobalSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewVTK.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaView;
class albaObserver;
class albaSceneNode;
class albaEventBase;
class albaString;
class albaGUIFloatSlider;
class vtkActor2D;
class vtkTextMapper;
class vtkPlaneSource;
class vtkOutlineCornerFilter;
class vtkProperty;
class vtkPolyDataMapper;
class vtkActor;

/**
  Class Name: albaViewGlobalSlice.
  View class that visualize actors sliced from an implicit plane that operates in all
  the 3d world.
*/

class ALBA_EXPORT albaViewGlobalSlice: public albaViewVTK
{
public:
  /** constructor. */
  albaViewGlobalSlice(wxString label = "Global Slice", int camera_position = CAMERA_PERSPECTIVE, bool show_axes = false, bool show_grid = false, int stereo = 0);
  /** destructor. */
  virtual ~albaViewGlobalSlice(); 

  /** RTTI macro. */
  albaTypeMacro(albaViewGlobalSlice, albaViewVTK);

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
	virtual albaView *Copy(albaObserver *Listener, bool lightCopyEnabled = false);

  /** Function that creates renderwindow, scenegraph, specific actors and interactors. */
	virtual void Create();

	/** 
  Create the visual pipe for the vme passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(albaVME *vme);

	/** Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(albaVME *vme);

  /** Function that handles events sent from other objects. */
	virtual void OnEvent(albaEventBase *alba_event);

  /** Function called when select a vme different from selected one.*/
	virtual void VmeSelect(albaVME *vme, bool select);

	/** Set slicer parameter to generate the slice. */
  virtual void SetSlice(double origin[3], float xVect[3], float yVect[3]);
	
	/** Set slicer parameter to generate the slice. */
	virtual void SetSlice(double origin[3], double dn = 0.0);

	/** Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(albaVME *vme, bool show);

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
	virtual albaGUI  *CreateGui();

	double		m_SliceOrigin[3];
	float			m_SliceXVector[3];
	float			m_SliceYVector[3];
	float			m_SliceNormal[3];
	double		m_GlobalBounds[6];
	double		m_SliderOldOrigin;
	double		m_SliderOrigin;
	double		m_Opacity;
	int				m_IDPlane;
	int				m_ViewIndex;
	double		m_Dn;
	bool			m_GlobalBoundsInitialized;
  bool      m_GlobalBoundsValid;

	albaSceneNode		*m_SelectedVolume;
	albaGUIFloatSlider	*m_GlobalSlider;
	albaGUIFloatSlider	*m_OpacitySlider;

	vtkActor2D					*m_TextActor;
	vtkTextMapper				*m_TextMapper;
	albaString						 m_Text;
	double							 m_TextColor[3];

  int m_NumberOfVmeInstantiated;
  std::map<albaID,int> m_MapID;

  std::vector<vtkPlaneSource*>					m_BoundsPlane;
  std::vector<vtkOutlineCornerFilter*>	m_BoundsOutlineBox;
  std::vector<vtkProperty*>             m_BoundsOutlineProperty;
  std::vector<vtkPolyDataMapper*>       m_BoundsOutlineMapper;
  std::vector<vtkActor*>	              m_BoundsOutlineActor;

  int m_TrilinearInterpolationOn;
};
#endif
