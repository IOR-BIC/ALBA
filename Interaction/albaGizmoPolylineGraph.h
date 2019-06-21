/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoPolylineGraph
 Authors: Josef Kohout, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoInteractionDebugger_h
#define __albaGizmoInteractionDebugger_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaGizmoInterface.h"
#include "albaMatrix.h"
#include "albaInteractorGenericMouse.h"
#include "albaCurvilinearAbscissaOnSkeletonHelper.h"

//---------------------------------------------------------------------------
// forward refs:
//---------------------------------------------------------------------------
class albaInteractorCompositorMouse;
class albaInteractorGenericMouse;


class albaEvent;
class albaVME;
class albaVMEGizmo;

class vtkSphereSource;
class vtkArrowSource;
class vtkPlaneSource;
class vtkAppendPolyData;
class vtkTransformPolyDataFilter;

class albaVMEPolylineGraph;
class albaCurvilinearAbscissaOnSkeletonHelper;

/** Gizmo Helper Object for interaction stuff debug: this class should be expanded and
refactored incrementally to ease interaction stuff debug.
See also albaOpInteractionDebugger */

class ALBA_EXPORT albaGizmoPolylineGraph : public albaGizmoInterface
{
public:
  /** The only way to construct correctly the instance */
  inline static albaGizmoPolylineGraph* New(albaVME* inputVme, 
    albaObserver *Listener = NULL, const char *name = "GizmoPath", bool showOnlyDirectionAxis = false, bool testMode = false)
  {
    albaGizmoPolylineGraph* pThis = new albaGizmoPolylineGraph(inputVme,Listener, name, showOnlyDirectionAxis, testMode);
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
  albaGizmoPolylineGraph(albaVME* inputVme, albaObserver *Listener = NULL, const char *name = "GizmoPath", bool showOnlyDirectionAxis = false, bool testMode = false);
  
  /** Initializes the object. May not be called twice (unless Done method was called).  */
  virtual void InternalInitInstance();
  
  /** Finalizes the object. May not be called twice (unless Init method was called).  */
  virtual void InternalRelease();

public:  
  /** Set the constraint polyline graph */
  inline void SetConstraintPolylineGraph(albaVMEPolylineGraph* constraintPolylineGraph);

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
  /*virtual*/ void OnEvent(albaEventBase *alba_event); 

  /** Not used since this gizmo is not moving anything*/
  void SetInput(albaVME *vme) {return;};

  /** Set the gizmo color */
  virtual void SetColor(double col[3]);

  inline albaVMEGizmo* GetOutput() {
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
  virtual void OnTransform(albaEvent *e);
  
  void LogTransformEvent(albaEvent  *e) ;

  albaString           m_Name;
  albaVMEGizmo        *m_VmeGizmo;     //<VME gizmo (this is used by medCurvilinearAbscisaHelper)

  albaInteractorCompositorMouse *m_GizmoInteractor;
  albaInteractorGenericMouse *m_LeftMouseInteractor;
    
  vtkSphereSource  *m_SphereSource;   //<sphere denoting the position  
  vtkArrowSource* m_AxisSource[3];          //<source for axis
  vtkTransformPolyDataFilter* m_Axis[3];  //<correctly positioned axis  

  vtkPlaneSource *m_PlaneSource;
  vtkAppendPolyData *m_AppendPolyData;

  bool m_ShowOnlyDirectionAxis; //< if true visualize only directional arrow source and hides the perpendiculars
  bool m_TestMode; //< if true don't visualize gui
 
  
  albaCurvilinearAbscissaOnSkeletonHelper *m_CurvilinearAbscissaHelper;

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
inline void albaGizmoPolylineGraph::SetConstraintPolylineGraph( albaVMEPolylineGraph* constraintPolylineGraph )
//------------------------------------------------------------------------
{
  m_CurvilinearAbscissaHelper->SetConstraintPolylineGraph(constraintPolylineGraph);
}

//------------------------------------------------------------------------
inline vtkIdType albaGizmoPolylineGraph::GetActiveBranchId()
//------------------------------------------------------------------------
{
  return m_CurvilinearAbscissaHelper->GetActiveBranchId();
}

//------------------------------------------------------------------------
inline double albaGizmoPolylineGraph::GetCurvilinearAbscissa()
//------------------------------------------------------------------------
{
  return m_CurvilinearAbscissaHelper->GetCurvilinearAbscissa();
}

//------------------------------------------------------------------------
inline void albaGizmoPolylineGraph::SetCurvilinearAbscissa( vtkIdType branchId, double s )
//------------------------------------------------------------------------
{   
  m_CurvilinearAbscissaHelper->SetCurvilinearAbscissa(branchId, s);
}
#pragma endregion inlines


#endif
