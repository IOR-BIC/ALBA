/*=========================================================================

 Program: MAF2Medical
 Module: medGizmoInteractionDebugger
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medGizmoInteractionDebugger_h
#define __medGizmoInteractionDebugger_h


//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafGizmoInterface.h"
#include "mafMatrix.h"
#include "mafCurvilinearAbscissaOnSkeletonHelper.h"

//---------------------------------------------------------------------------
// forward refs:
//---------------------------------------------------------------------------
class mafInteractorCompositorMouse;
class mafInteractorGenericMouse;


class mafEvent;
class mafVME;
class mafVMEGizmo;
class mafVME;

class vtkLineSource;
class vtkSphereSource;
class vtkPlaneSource;
class vtkAppendPolyData;

class medVMEPolylineGraph;
class mafCurvilinearAbscissaOnSkeletonHelper;

/** Gizmo Helper Object for interaction stuff debug: this class should be expanded and
refactored incrementally to ease interaction stuff debug.
See also medOpInteractionDebugger */

class MAF_EXPORT medGizmoInteractionDebugger : public mafGizmoInterface
{
public:

  /** inputVME is used just to put the gizmo on the vme tree: this gizmo is not moving anything but itself */
  medGizmoInteractionDebugger(mafNode* inputVme, mafObserver *Listener = NULL, const char *name = "GizmoPath", bool testMode = false);

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

  

  void Constructor(mafNode *imputVme, mafObserver *listener, const char *name, bool testMode = false);
  void Destructor();

  void CreateVMEGizmo();
  void DestroyVMEGizmo();
  void CreateGizmoVTKData();
  void CreateInteractor();
  
  void LogTransformEvent(mafEvent  *e) ;

  mafString           m_Name;
  mafVMEGizmo        *m_VmeGizmo;
  mafInteractorCompositorMouse *m_GizmoInteractor;
  mafInteractorGenericMouse *m_LeftMouseInteractor;
  
  vtkLineSource *m_LineSource;
  vtkSphereSource  *m_SphereSource;
  vtkPlaneSource *m_PlaneSource;
  vtkAppendPolyData *m_AppendPolyData;
  
  mafCurvilinearAbscissaOnSkeletonHelper *m_CurvilinearAbscissaHelper;

  enum CONSTRAINT_MODALITY_ID
  {
    FREE = 0,
    BOUNDS,
    SNAP_VERTEX,
    CONSTRAINT_MODALITY_NUMBERS,
  };

};

#endif
