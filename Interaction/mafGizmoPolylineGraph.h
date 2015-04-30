/*=========================================================================

 Program: MAF2
 Module: mafGizmoPolylineGraph
 Authors: Josef Kohout, Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGizmoInteractionDebugger_h
#define __mafGizmoInteractionDebugger_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafGizmoInterface.h"
#include "mafMatrix.h"
#include "mafInteractorGenericMouse.h"
#include "mafCurvilinearAbscissaOnSkeletonHelper.h"

//---------------------------------------------------------------------------
// forward refs:
//---------------------------------------------------------------------------
class mafInteractorCompositorMouse;
class mafInteractorGenericMouse;


class mafEvent;
class mafVME;
class mafVMEGizmo;

class vtkSphereSource;
class vtkArrowSource;
class vtkPlaneSource;
class vtkAppendPolyData;
class vtkTransformPolyDataFilter;

class mafVMEPolylineGraph;
class mafCurvilinearAbscissaOnSkeletonHelper;

/** Gizmo Helper Object for interaction stuff debug: this class should be expanded and
refactored incrementally to ease interaction stuff debug.
See also mafOpInteractionDebugger */

class MAF_EXPORT mafGizmoPolylineGraph : public mafGizmoInterface
{
public:
  /** The only way to construct correctly the instance */
  inline static mafGizmoPolylineGraph* New(mafNode* inputVme, 
    mafObserver *Listener = NULL, const char *name = "GizmoPath", bool showOnlyDirectionAxis = false, bool testMode = false)
  {
    mafGizmoPolylineGraph* pThis = new mafGizmoPolylineGraph(inputVme,Listener, name, showOnlyDirectionAxis, testMode);
    pThis->InternalInitInstance();
    return pThis;
  }

  /** The only way to destruct correctly the instance */
  inline virtual void Delete() 
  {
    if (this != NULL)
    {
      InternalRelease();
      delete this;
    }
  }

protected:
  mafGizmoPolylineGraph(mafNode* inputVme, mafObserver *Listener = NULL, const char *name = "GizmoPath", bool showOnlyDirectionAxis = false, bool testMode = false);
  
  /** Initializes the object. May not be called twice (unless Done method was called).  */
  virtual void InternalInitInstance();
  
  /** Finalizes the object. May not be called twice (unless Init method was called).  */
  virtual void InternalRelease();

public:  
  /** Set the constraint polyline graph */
  inline void SetConstraintPolylineGraph(mafVMEPolylineGraph* constraintPolylineGraph);

  /** Set curvilinear abscissa along given constraint skeleton branch*/
  virtual void SetCurvilinearAbscissa(vtkIdType branchId, double s);

  /** Get curvilinear abscissa along given constraint skeleton branch*/
  inline double GetCurvilinearAbscissa();
  
  /** Get the active branch ie the branch to which gizmo is currently constrained*/  
  inline vtkIdType GetActiveBranchId();;

  /** Set the length of the gizmo*/
  virtual void SetGizmoLength (double lineLength);

  /** Show/Hide the gizmos using actor visibility instead of pipe creation/destruction: this is used for faster 
  rendering*/
  virtual void Show(bool show) ;

  /** Events handling method */
  /*virtual*/ void OnEvent(mafEventBase *maf_event); 

  /** Not used since this gizmo is not moving anything*/
  void SetInput(mafVME *vme) {return;};

  /** Set the gizmo color */
  virtual void SetColor(double col[3]);

  inline mafVMEGizmo* GetOutput() {
    return m_VmeGizmo; 
  }

protected:
  /** Creates the gizmo. Called from Init. */
  virtual void CreateVMEGizmo();

  /** Destroys the gizmo. Called from Done. */
  virtual void DestroyVMEGizmo();

  /** Creates VTK pipeline for the gizmo. Called from CreateVMEGizmo. */    
  virtual void CreateGizmoVTKData();

  /** Destroys VTK pipeline for the gizmo. Called from DestroyVMEGizmo. */
  virtual void DestroyGizmoVTKData();
  
  /** Creates the interactor for the gizmo. Called from Init. */
  virtual void CreateInteractor();

  /** Destroys the interactor of the gizmo. Called from Done. */
  virtual void DestroyInteractor();

  /** Handles ID_TRANSFORM event */
  virtual void OnTransform(mafEvent *e);
  
  void LogTransformEvent(mafEvent  *e) ;

  mafString           m_Name;
  mafVMEGizmo        *m_VmeGizmo;     //<VME gizmo (this is used by medCurvilinearAbscisaHelper)

  mafInteractorCompositorMouse *m_GizmoInteractor;
  mafInteractorGenericMouse *m_LeftMouseInteractor;
    
  vtkSphereSource  *m_SphereSource;   //<sphere denoting the position  
  vtkArrowSource* m_AxisSource[3];          //<source for axis
  vtkTransformPolyDataFilter* m_Axis[3];  //<correctly positioned axis  

  vtkPlaneSource *m_PlaneSource;
  vtkAppendPolyData *m_AppendPolyData;

  bool m_ShowOnlyDirectionAxis; //< if true visualize only directional arrow source and hides the perpendiculars
  bool m_TestMode; //< if true don't visualize gui
 
  
  mafCurvilinearAbscissaOnSkeletonHelper *m_CurvilinearAbscissaHelper;

  enum CONSTRAINT_MODALITY_ID
  {
    FREE = 0,
    BOUNDS,
    SNAP_VERTEX,
    CONSTRAINT_MODALITY_NUMBERS,
  };

};

#pragma region inlines
//------------------------------------------------------------------------
inline void mafGizmoPolylineGraph::SetConstraintPolylineGraph( mafVMEPolylineGraph* constraintPolylineGraph )
//------------------------------------------------------------------------
{
  m_CurvilinearAbscissaHelper->SetConstraintPolylineGraph(constraintPolylineGraph);
}

//------------------------------------------------------------------------
inline vtkIdType mafGizmoPolylineGraph::GetActiveBranchId()
//------------------------------------------------------------------------
{
  return m_CurvilinearAbscissaHelper->GetActiveBranchId();
}

//------------------------------------------------------------------------
inline double mafGizmoPolylineGraph::GetCurvilinearAbscissa()
//------------------------------------------------------------------------
{
  return m_CurvilinearAbscissaHelper->GetCurvilinearAbscissa();
}

//------------------------------------------------------------------------
inline void mafGizmoPolylineGraph::SetCurvilinearAbscissa( vtkIdType branchId, double s )
//------------------------------------------------------------------------
{   
  m_CurvilinearAbscissaHelper->SetCurvilinearAbscissa(branchId, s);
}
#pragma endregion inlines


#endif
