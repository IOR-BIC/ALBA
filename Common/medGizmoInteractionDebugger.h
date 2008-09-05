/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGizmoInteractionDebugger.h,v $
  Language:  C++
  Date:      $Date: 2008-09-05 11:11:58 $
  Version:   $Revision: 1.3 $
  Authors:   Stefano Perticoni
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
class mafVME;

class vtkLineSource;
class vtkSphereSource;
class vtkPlaneSource;
class vtkAppendPolyData;

class medVMEPolylineGraph;
class medCurvilinearAbscissaOnSkeletonHelper;

/** Gizmo Helper Object for interaction stuff debug: this class should be expanded and
refactored incrementally to ease interaction stuff debug.
See also medOpInteractionDebugger */

class medGizmoInteractionDebugger : public mafGizmoInterface
{
public:

	medGizmoInteractionDebugger(mafNode* inputVme, mafObserver *Listener = NULL, const char *name = "GizmoPath");

  virtual	~medGizmoInteractionDebugger();

  /** Set the constraint polyline graph */
  void SetConstraintPolylineGraph(medVMEPolylineGraph* constraintPolylineGraph);

  /** Set curvilinear abscissa along given constraint skeleton branch*/
  void SetCurvilinearAbscissa(vtkIdType branchId, double s);

  /** Get curvilinear abscissa along given constraint skeleton branch*/
  double GetCurvilinearAbscissa();
  
  /** Get the active branch ie the branch to which gizmo is currently constrained*/  
  vtkIdType GetActiveBranchId();;

  /** Set the length of the gizmo*/
  void SetGizmoLength (double lineLength);

  /** Show/Hide the gizmos using actor visibility instead of pipe creation/destruction: this is used for faster 
  rendering*/
  void Show(bool show) ;

  /** Events handling method */
  void OnEvent(mafEventBase *maf_event); 

  /** Not used since this gizmo is not moving anything*/
  void SetInput(mafVME *vme) {return;};

  /** Set the gizmo color */
  void SetColor(double col[3]);

  /** Get the gizmo gui */
  mafGUI *GetGui();

protected:

  

  void Constructor(mafNode *imputVme, mafObserver *listener, const char *name);
  void Destructor();

  void CreateVMEGizmo();
  void DestroyVMEGizmo();
  void CreateGizmoVTKData();
  void CreateInteractor();
  
  void LogTransformEvent(mafEvent  *e) ;

  mafString           m_Name;
  mafVMEGizmo        *m_VmeGizmo;
  mmiCompositorMouse *m_GizmoInteractor;
  mmiGenericMouse    *m_LeftMouseInteractor;
  
  vtkLineSource *m_LineSource;
  vtkSphereSource  *m_SphereSource;
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

#endif
