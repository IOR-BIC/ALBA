/*=========================================================================

 Program: MAF2
 Module: mafViewOrthoSlice
 Authors: Stefano Perticoni, Gianluigi Crimi, Grazia Di Cosmo
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafViewOrthoSlice_H__
#define __mafViewOrthoSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafViewCompound.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGizmoSlice;
class mafGUILutSlider;
class mafGUIFloatSlider;
class mafViewSlice;
class mafVMEVolume;
class vtkLookupTable;
class mafGUILutSwatch;
class vtkPoints;

//----------------------------------------------------------------------------
// mafViewOrthoSlice :
//----------------------------------------------------------------------------
/** 
This compound view is made of four child views used to analyze different orthogonal 
slices of the volume. This is an experimental component with rotated volumes interaction
and visualization enabled.
*/
class MAF_EXPORT mafViewOrthoSlice: public mafViewCompound
{
public:
  /** constructor */
  mafViewOrthoSlice(wxString label = "View OrthoSlice");

  /** destructor */
  virtual ~mafViewOrthoSlice(); 

  /** RTTI macro*/
  mafTypeMacro(mafViewOrthoSlice, mafViewCompound);

  /** clone an instance of the object*/
  virtual mafView *Copy(mafObserver *Listener, bool lightCopyEnabled = false);

  /** Precess events coming from other objects */
  virtual void OnEvent(mafEventBase *maf_event);
  
  /** 
  Create the GUI on the bottom of the compounded view. */
  virtual void CreateGuiView();

  /** 
  Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(mafVME *vme, bool show);

  /** Remove Gizmos, Observers from the volume */
  void DestroyOrthoSlicesAndGizmos();

  /** generate gizmos and pose them in the right position*/
  void CreateOrthoslicesAndGizmos(mafVME *vme);

  /** Remove VME into plugged sub-views*/
  virtual void VmeRemove(mafVME *vme);

  /** 
  Create visual pipe and initialize them to build an OrthoSlice visualization */
  void PackageView();
  
  /** IDs for the GUI */
  enum VIEW_ORTHO_SLICE_WIDGET_ID
  {
    ID_LUT_CHOOSER = Superclass::ID_LAST,
    ID_SIDE_ORTHO,
		ID_SNAP,
    ID_RESET_SLICES,
		ID_ALL_SURFACE,
		ID_BORDER_CHANGE,
    ID_TRILINEAR_INTERPOLATION,
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
  void PlugVisualPipeInPerspective(mafString vme_type, mafString pipe_type, long visibility = VISIBLE);

  /** Plug a visual pipe for a particular vme in each perspective sub-view
  Note: This method will be called immediately after the call of PackageView */ 
  void PlugVisualPipeInSliceViews(mafString vme_type, mafString pipe_type, long visibility = VISIBLE);

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
  virtual mafGUI  *CreateGui();

  /** Enable/disable view widgets.*/
  void EnableWidgets(bool enable = true);

  /** Create the gizmos to move the slices. */
  void GizmoCreate();

  /** Delete the gizmos. */
  void GizmoDelete();

  /** Update the slices according to the new position. */
  void SetSlicePosition(long gizmoId, vtkPoints *p);

  /** Reset slice positions  */
  void ResetSlicesPosition(mafVME *vme);

  /** process events regarding border thickness in the pipes*/
	void OnEventSetThickness();

  /** set thickness value for all the pipes*/
	void SetThicknessForAllSurfaceSlices(mafVME *root);

  /** Apply View Setting in ChildViews*/
  void ApplyViewSettings(mafVME *vme);
	
  int m_AllSurface;
	double m_Border;

  mafGizmoSlice   *m_Gizmo[3];

  double m_GizmoHandlePosition[3];

  mafVME *m_CurrentVolume; ///< Current visualized volume
  
  mafGUILutSlider *m_LutSlider; ///< Double slider used to change brightness and contrast of the image
  mafGUILutSwatch *m_LutWidget; ///< LUT widget in view side panel 
  vtkLookupTable *m_ColorLUT;
  mafViewSlice *m_Views[4]; ///< Child views
  int	m_Side; ///< change Azimuth of the camera
	int m_Snap; ///<change modality of gizmo to snap on grid
  double m_PolylineRadiusSize;

  bool m_CanPlugVisualPipes; ///< true if u can plug visual pipes

  
  int m_TrilinearInterpolationOn;

  std::vector<mafVME*> m_VMElist; ///< VME List

};
#endif
