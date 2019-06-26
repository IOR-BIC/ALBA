/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoInteractionDebugger
 Authors: Stefano Perticoni
 
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
#include "albaCurvilinearAbscissaOnSkeletonHelper.h"

//---------------------------------------------------------------------------
// forward refs:
//---------------------------------------------------------------------------
class albaInteractorCompositorMouse;
class albaInteractorGenericMouse;


class albaEvent;
class albaVME;
class albaVMEGizmo;
class albaVME;

class vtkLineSource;
class vtkSphereSource;
class vtkPlaneSource;
class vtkAppendPolyData;

class albaVMEPolylineGraph;
class albaCurvilinearAbscissaOnSkeletonHelper;

/** Gizmo Helper Object for interaction stuff debug: this class should be expanded and
refactored incrementally to ease interaction stuff debug.
See also albaOpInteractionDebugger */

class ALBA_EXPORT albaGizmoInteractionDebugger : public albaGizmoInterface
{
public:

  /** inputVME is used just to put the gizmo on the vme tree: this gizmo is not moving anything but itself */
  albaGizmoInteractionDebugger(albaVME* inputVme, albaObserver *Listener = NULL, const char *name = "GizmoPath", bool testMode = false);

  virtual	~albaGizmoInteractionDebugger();

  /** Set the constraint polyline graph */
  void SetConstraintPolylineGraph(albaVMEPolylineGraph* constraintPolylineGraph);

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
  void OnEvent(albaEventBase *alba_event); 

  /** Not used since this gizmo is not moving anything*/
  void SetInput(albaVME *vme) {return;};

  /** Set the gizmo color */
  void SetColor(double col[3]);

  /** Get the gizmo gui */
  albaGUI *GetGui();

protected:

  

  void Constructor(albaVME *imputVme, albaObserver *listener, const char *name, bool testMode = false);
  void Destructor();

  void CreateVMEGizmo();
  void DestroyVMEGizmo();
  void CreateGizmoVTKData();
  void CreateInteractor();
  
  void LogTransformEvent(albaEvent  *e) ;

  albaString           m_Name;
  albaVMEGizmo        *m_VmeGizmo;
  albaInteractorCompositorMouse *m_GizmoInteractor;
  albaInteractorGenericMouse *m_LeftMouseInteractor;
  
  vtkLineSource *m_LineSource;
  vtkSphereSource  *m_SphereSource;
  vtkPlaneSource *m_PlaneSource;
  vtkAppendPolyData *m_AppendPolyData;
  
  albaCurvilinearAbscissaOnSkeletonHelper *m_CurvilinearAbscissaHelper;

  enum CONSTRAINT_MODALITY_ID
  {
    FREE = 0,
    BOUNDS,
    SNAP_VERTEX,
    CONSTRAINT_MODALITY_NUMBERS,
  };

};

#endif
