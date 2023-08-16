/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewOrthoSlice
 Authors: Stefano Perticoni, Gianluigi Crimi, Grazia Di Cosmo
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaViewOrthoSlice_H__
#define __albaViewOrthoSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewCompound.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGizmoSlice;
class albaGUILutSlider;
class albaGUIFloatSlider;
class albaViewSlice;
class albaVMEVolume;
class vtkLookupTable;
class albaGUILutSwatch;
class vtkPoints;

//----------------------------------------------------------------------------
// albaViewOrthoSlice :
//----------------------------------------------------------------------------
/** 
This compound view is made of four child views used to analyze different orthogonal 
slices of the volume. This is an experimental component with rotated volumes interaction
and visualization enabled.
*/
class ALBA_EXPORT albaViewOrthoSlice: public albaViewCompound
{
public:
  /** constructor */
  albaViewOrthoSlice(wxString label = "View OrthoSlice");

  /** destructor */
  virtual ~albaViewOrthoSlice(); 

  /** RTTI macro*/
  albaTypeMacro(albaViewOrthoSlice, albaViewCompound);

  /** clone an instance of the object*/
  virtual albaView *Copy(albaObserver *Listener, bool lightCopyEnabled = false);

  /** Precess events coming from other objects */
  virtual void OnEvent(albaEventBase *alba_event);
  
  /** Create the GUI on the bottom of the compounded view. */
  virtual void CreateGuiView();

	/**
	Create the plugged sub-view and call virtual method CreateGuiView() */
	virtual void Create();

  /** Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(albaVME *vme, bool show);

	/** Apply mesh and surface slice settings to the pipe */
	void ApplySliceSetting(int view, albaVME * vme);

  /** Remove Gizmos, Observers from the volume */
  void RemoveVolumeFromGizmo();

  /** generate gizmos and pose them in the right position*/
  void CreateOrthoslicesAndGizmos(albaVME *vme);

  /** Remove VME into plugged sub-views*/
  virtual void VmeRemove(albaVME *vme);

  /** 
  Create visual pipe and initialize them to build an OrthoSlice visualization */
  void PackageView();
  
  /** IDs for the GUI */
  enum VIEW_ORTHO_SLICE_WIDGET_ID
  {
    ID_LUT_CHOOSER = Superclass::ID_LAST,
    ID_SIDE_ORTHO,
		ID_SNAP,
		ID_X_SLIDER,
		ID_Y_SLIDER,
		ID_Z_SLIDER,
    ID_RESET_SLICES,
		ID_ALL_SURFACE,
		ID_BORDER_CHANGE,
    ID_TRILINEAR_INTERPOLATION,
		ID_DECIMALS,
    ID_LAST
  };

  enum ORTHOSLICE_SUBVIEW_ID
  {
    PERSPECTIVE_VIEW = 0,
    XN_VIEW,
    YN_VIEW,
    ZN_VIEW,
    VIEWS_NUMBER,
  };


  enum CHILD_VIEW_ID
  {
    CHILD_PERSPECTIVE_VIEW = 0,
    CHILD_ZN_VIEW,
    CHILD_XN_VIEW,
    CHILD_YN_VIEW,
    CHILD_VIEWS_NUMBER,
  };

  enum GIZMO_ID
  {
    GIZMO_XN = 0,
    GIZMO_YN,
    GIZMO_ZN,
    GIZMOS_NUMBER,
  };

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /** Method for determine if the pick is over a Slice view or not*/
  bool IsPickedSliceView();

  /** Plug a visual pipe for a particular vme in the perspective sub-view
  Note: This method will be called immediately after the call of PackageView */ 
  void PlugVisualPipeInPerspective(albaString vme_type, albaString pipe_type, long visibility = VISIBLE);

  /** Plug a visual pipe for a particular vme in each perspective sub-view
  Note: This method will be called immediately after the call of PackageView */ 
  void PlugVisualPipeInSliceViews(albaString vme_type, albaString pipe_type, long visibility = VISIBLE);

	/** return an xpm-icon that can be used to represent this view */
	virtual char ** GetIcon();
	
	/** Returns AllSurface */
	int GetAllSurface() const { return m_AllSurface; }

	/** Sets AllSurface */
	void SetAllSurface(int allSurface) { m_AllSurface = allSurface; }

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same panel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual albaGUI  *CreateGui();

  /** Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

  /** Create the gizmos to move the slices. */
  void CreateGizmo();

	void GizmoShow();

  /** Delete the gizmos. */
  void DestroyGizmo();

  /** Update the slices according to the new position. */
  void SetSlicePosition(long gizmoId, vtkPoints *p);

  /** Reset slice positions  */
  void ResetSlicesPosition(albaVME *vme);

  /** process events regarding border thickness in the pipes*/
	void OnEventSetThickness();

  /** set thickness value for all the pipes*/
	void SetThicknessForAllSurfaceSlices(albaVME *root);

  /** Apply View Setting in ChildViews*/
  void ApplyViewSettings(albaVME *vme);


	void UpdateGizmoBounds(bool show);

	
  int m_AllSurface;
	double m_Border;

  albaGizmoSlice   *m_Gizmo[3];

  double m_GizmoHandlePosition[3];

	albaGUIFloatSlider *m_XSliceSlider;
	albaGUIFloatSlider *m_YSliceSlider;
	albaGUIFloatSlider *m_ZSliceSlider;

  albaVME *m_CurrentVolume; ///< Current visualized volume
  
  albaGUILutSlider *m_LutSlider; ///< Double slider used to change brightness and contrast of the image
  albaGUILutSwatch *m_LutWidget; ///< LUT widget in view side panel 
  vtkLookupTable *m_ColorLUT;
  albaViewSlice *m_Views[4]; ///< Child views
  int	m_Side; ///< change Azimuth of the camera
	int m_Snap; ///<change modality of gizmo to snap on grid
	int m_Decimals;
  double m_PolylineRadiusSize;

  bool m_CanPlugVisualPipes; ///< true if u can plug visual pipes

  
  int m_TrilinearInterpolationOn;

	albaVME *m_Root;

  std::vector<albaVME*> m_VMElist; ///< VME List

};
#endif
