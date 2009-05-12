/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGizmoPolylineGraph.h,v $
  Language:  C++
  Date:      $Date: 2009-05-12 08:48:38 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Josef Kohout, Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __medGizmoInteractionDebugger_h
#define __medGizmoInteractionDebugger_h

#include "mafGizmoInterface.h"
#include "mafMatrix.h"
#include "mmiGenericMouse.h"
#include "medCurvilinearAbscissaOnSkeletonHelper.h"

//---------------------------------------------------------------------------
// forward refs:
//---------------------------------------------------------------------------
class mmiCompositorMouse;
class mmiGenericMouse;


class mafEvent;
class mafVME;
class mafVMEGizmo;

class vtkSphereSource;
class vtkArrowSource;
class vtkPlaneSource;
class vtkAppendPolyData;
class vtkTransformPolyDataFilter;

class medVMEPolylineGraph;
class medCurvilinearAbscissaOnSkeletonHelper;

/** Gizmo Helper Object for interaction stuff debug: this class should be expanded and
refactored incrementally to ease interaction stuff debug.
See also medOpInteractionDebugger */

class medGizmoPolylineGraph : public mafGizmoInterface
{
public:
  /** The only way to construct correctly the instance */
  inline static medGizmoPolylineGraph* New(mafNode* inputVme, 
    mafObserver *Listener = NULL, const char *name = "GizmoPath")
  {
    medGizmoPolylineGraph* pThis = new medGizmoPolylineGraph(inputVme,Listener, name);
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
  medGizmoPolylineGraph(mafNode* inputVme, mafObserver *Listener = NULL, const char *name = "GizmoPath");
  
  /** Initializes the object. May not be called twice (unless Done method was called).  */
  virtual void InternalInitInstance();
  
  /** Finalizes the object. May not be called twice (unless Init method was called).  */
  virtual void InternalRelease();

public:  
  /** Set the constraint polyline graph */
  inline void SetConstraintPolylineGraph(medVMEPolylineGraph* constraintPolylineGraph);

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

  mmiCompositorMouse *m_GizmoInteractor;
  mmiGenericMouse    *m_LeftMouseInteractor;
    
  vtkSphereSource  *m_SphereSource;   //<sphere denoting the position  
  vtkArrowSource* m_AxisSource[3];          //<source for axis
  vtkTransformPolyDataFilter* m_Axis[3];  //<correctly positioned axis  

  vtkPlaneSource *m_PlaneSource;
  vtkAppendPolyData *m_AppendPolyData;
  
  medCurvilinearAbscissaOnSkeletonHelper *m_CurvilinearAbscissaHelper;

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
inline void medGizmoPolylineGraph::SetConstraintPolylineGraph( medVMEPolylineGraph* constraintPolylineGraph )
//------------------------------------------------------------------------
{
  m_CurvilinearAbscissaHelper->SetConstraintPolylineGraph(constraintPolylineGraph);
}

//------------------------------------------------------------------------
inline vtkIdType medGizmoPolylineGraph::GetActiveBranchId()
//------------------------------------------------------------------------
{
  return m_CurvilinearAbscissaHelper->GetActiveBranchId();
}

//------------------------------------------------------------------------
inline double medGizmoPolylineGraph::GetCurvilinearAbscissa()
//------------------------------------------------------------------------
{
  return m_CurvilinearAbscissaHelper->GetCurvilinearAbscissa();
}

//------------------------------------------------------------------------
inline void medGizmoPolylineGraph::SetCurvilinearAbscissa( vtkIdType branchId, double s )
//------------------------------------------------------------------------
{   
  m_CurvilinearAbscissaHelper->SetCurvilinearAbscissa(branchId, s);
}
#pragma endregion inlines


#endif
