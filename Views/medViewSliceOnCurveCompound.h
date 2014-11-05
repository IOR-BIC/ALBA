/*========================================================================= 
Program: Multimod Application Framework RELOADED 
Module: $RCSfile: medViewSliceOnCurveCompound.h,v $ 
Language: C++ 
Date: $Date: 2012-04-06 09:37:38 $ 
Version: $Revision: 1.1.2.5 $ 
Authors: Eleonora Mambrini
========================================================================== 
Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
See the COPYINGS file for license details 
=========================================================================
*/


#ifndef __medViewSliceOnCurveCompound_H__
#define __medViewSliceOnCurveCompound_H__

//#define GIZMO_PATH

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medViewsDefines.h"
#include "medViewCompoundWindowing.h"
#include "mafSceneNode.h"
#include "vtkPoints.h"
#ifdef GIZMO_PATH
#include "mafGizmoPath.h"
#else
#include "../Interaction/medGizmoPolylineGraph.h"
#endif

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUIFloatSlider;
class mafViewSlice_BES;
class mafVMEVolume;
class vtkActor2D;
class vtkTextMapper;

//----------------------------------------------------------------------------
// medViewSliceOnCurveCompound :
//----------------------------------------------------------------------------
/** 
This compound view is made of four child views used to analyze different orthogonal slices of the volume*/
class MED_VIEWS_EXPORT medViewSliceOnCurveCompound: public medViewCompoundWindowing
{
public:

  medViewSliceOnCurveCompound(wxString label = "View Slice on Curve");
  virtual ~medViewSliceOnCurveCompound(); 

  mafTypeMacro(medViewSliceOnCurveCompound, medViewCompoundWindowing);

  /** IDs for the GUI */
  enum VIEW_SLICE_ON_CURVE_WIDGET_ID
  {
    ID_VOL_PIPE = Superclass::ID_LAST,
    ID_SHOW_POLYLINE_IN_MAINVIEW,
    ID_SHOW_GIZMOCOORDS, 

#ifdef GIZMO_PATH
    ID_TRACKER,
#endif // GIZMO_PATH

    ID_SLICECAMERA_AUTOFOCUS,
    ID_SLICECAMERA_AUTOROTATE,
    ID_SLICECAMERA_NAVIGATE_3D,
    ID_ENABLE_GPU,
    ID_LAST
  };

  enum VIEWS_ID
  {
    MAIN_VIEW = 0,
    POLYLINE_VIEW = 1,
    SLICE_VIEW = 2,
  };

  enum LAYOUTS_ID
  {
    LAYOUT_MPS_ONEROW = 0,
    LAYOUT_MPS_HORZ = 1,
    LAYOUT_MPS_VERT = 2,
    LAYOUT_SMP_HORZ = 3,
    LAYOUT_SMP_VERT = 4,
    MAX_LAYOUTS = 5,
  };


  //structure to store information about geometry visual pipe
  typedef struct VPIPE_ENTRY
  {
    const char* szClassName;            ///<visual pipe class name
    const char* szUserFriendlyName;     ///<user friendly name to be displayed in GUI
  } VPIPE_ENTRY;

protected:
//   ///layout names to be displayed in GUI
//   static const char* m_LayoutNames[];
// 
//   ///3D volume pipes to be plugged
//   static const VPIPE_ENTRY m_VolumePipes[];

  ///current volume pipe configuration
  int m_VolumePipeConfiguration;	

  int m_ShowPolylineInMainView;       //<non-zero, if the polyline is to be displayed in the main view
  int m_ShowGizmoCoords;              //<non-zero, if the coordinates of gizmo (i.e., the slice origin) should be displayed

  mafNode* m_CurrentVolume;           //<currently selected volume	
  mafNode* m_CurrentPolyLine;         //<currently selected polyline
  mafNode* m_CurrentPolyLineGizmo;    //<VME accepted by gizmo

  double   m_OldPos[3];				//<Old Pose for 3D navigation

  //gizmo that follows curve
#ifdef GIZMO_PATH
  mafGizmoPath* m_Gizmo; 	
  double m_GizmoPos;		//< Position on the polyline (0 - 1)
  double m_GizmoLength;	//< Length of the polyline
#else
  medGizmoPolylineGraph* m_Gizmo;
#endif

  int m_SliceCameraAutoFocus;      //< non-zero, if the camera in the slice view should be focused on the current position on the curve
  int m_SliceCameraAutoRotate;     //< non-zero, if the viewing direction in the slice view should be perpendicular to the cutting plane
  int m_SliceCameraNavigate3D;	   //< non-zero, if the viewing direction in the 3D View view should be perpendicular to the cutting plane

  // text stuff
  vtkActor2D    *m_TextActor;
  vtkTextMapper	*m_TextMapper;  

  // Added by Losi 11.25.2009
  int m_EnableGPU; ///<Non-zero, if the GPU support for slicing is used (default)

public:
  /*virtual*/ mafView *Copy(mafObserver *Listener, bool lightCopyEnabled = false);
  /*virtual*/ void OnEvent(mafEventBase *maf_event);	

  /** 
  Show/Hide VMEs into plugged sub-views*/
  /*virtual*/ void VmeShow(mafNode *node, bool show);

  /** return the current pipe for the specified vme (if any exist at this moment) */
  /*virtual*/ mafPipe* GetNodePipe(mafNode *vme);

  /** 
  return the status of the node within this view. es: NON_VISIBLE,VISIBLE_ON, ... 
  having mafViewCompound::GetNodeStatus allow mafGUICheckTree to not know about mafSceneGraph */
  /*virtual*/ int  GetNodeStatus(mafNode *vme);

  /** 
  Create visual pipe and initialize them to build an SliceOnCurve visualization */
  /*virtual*/ void PackageView();

  /** Returns number of layouts available for the user */
  inline virtual int GetNumberOfLayouts() {
    return medViewSliceOnCurveCompound::MAX_LAYOUTS;
  }

  /** Returns the array of layouts names.
  The list is terminated by NULL item. */
  inline virtual const char** GetLayoutsNames();

  /** Returns number of volumes pipes available for the user */
  virtual int GetNumberOfVolumePipes();

  /** Returns the array of volume pipes descriptor.
  The list is terminated by NULL item. */
  inline virtual const VPIPE_ENTRY* GetVolumePipesDesc(); 

  //change the visualization pipe for volumes
  virtual void ChangeVolumePipe(const char* pipename);

  /** Update the slice according to the new position. */
  void SetSlicePosition(double abscisa, vtkIdType branchId = 0);

  /** Reset slice position to the original */
  inline void ResetSlicePosition() {
    SetSlicePosition(0.0, 0);
  }

  /** Get the curvilinear abscissa */
  inline double GetCurvilinearAbscissa() { 
    return m_Gizmo == NULL ? 0.0 : m_Gizmo->GetCurvilinearAbscissa();
  };

  /** Get the active branch ie the branch to which inputVME is currently constrained*/  
  inline vtkIdType GetActiveBranchId() {
#ifdef GIZMO_PATH
    return 0; //GIZMO_PATH does not support branches
#else
    return m_Gizmo == NULL ? 0 : m_Gizmo->GetActiveBranchId();
#endif    
  };

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

protected:
  /** Hides VMEs of the same type as pNode that are currently displayed in pView
  The display status of pNode is not changed */
  void HideSameVMEs(mafView* pView, mafNode* pNode);

protected:
  /** Creates views and text mappers */
  /*virtual*/ void Create();

  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same panel GUI, each CreateGUI() function should first call the superclass' one.*/
  /*virtual*/ mafGUI  *CreateGui();

  /** Redefine this method to define a custom layout.*/
  /*virtual*/ void LayoutSubViewCustom(int width, int height);

  /** plugs a new volume visualization pipe */
  virtual void PlugVolumePipe();

  /** plugs a new surface visualization pipe */
  virtual void PlugSurfacePipe();

  /** plugs a new mesh visualization pipe */
  virtual void PlugMeshPipe();

  /** plugs a new polyline (graph) visualization pipe */
  virtual void PlugPolylinePipe();

  /** creates the gizmo path for the given node */
  virtual void CreateGizmo(mafNode* node);

  /** destroys the gizmo */
  virtual void DestroyGizmo();	

  /** update the text displayed in the polyline view  */
  virtual void UpdateGizmoStatusText(const char* szText);

  /** this method is called to set slice for the slice child view */
  virtual void SetSlice(double* Origin, double* Normal);

  /** handles the change of gizmo on the "curve" */
  virtual void OnGizmoMoved();

  /** handles the change of the visibility of polyline in the main view  */
  virtual void OnShowPolylineInMainView();

  /** handles the change of the visibility of gizmo status text */
  virtual void OnShowGizmoCoords();

};
#endif
