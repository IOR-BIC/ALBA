/*=========================================================================

 Program: MAF2Medical
 Module: medCurvilinearAbscissaOnSkeletonHelper
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medCurvilinearAbscissaOnSkeletonHelper_h
#define __medCurvilinearAbscissaOnSkeletonHelper_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "vtkIdList.h"

//----------------------------------------------------------------------------
// Forward declarations:
//----------------------------------------------------------------------------
class mafEvent;
class mafVME;
class mafPolylineGraph;
class medVMEPolylineGraph;

/** Decorate input vme with curvilinear abscissa interface for medVMEPolylineGraph 
 constrained interaction */
class MED_COMMON_EXPORT medCurvilinearAbscissaOnSkeletonHelper : mafObserver
{
public:

  /** Pass the vme you want to decorate with curvilinear abscissa interface */
  medCurvilinearAbscissaOnSkeletonHelper(mafVME *inputVME, mafObserver *listener = NULL, bool testMode = false);
 
  /** Set the polyline graph constraint */
  void SetConstraintPolylineGraph(medVMEPolylineGraph* constraintPolylineGraph);
  
  /** Get the polyline graph constraint */
  medVMEPolylineGraph *GetSetConstraintPolylineGraph() {return m_ConstraintVMEPolylineGraph;};

  /** Set the curvilinear abscissa value s on branchId branch, return MAF_OK or MAF_ERROR */
  int SetCurvilinearAbscissa( vtkIdType branchId, double s );
  
  /** Get the curvilinear abscissa */
  double GetCurvilinearAbscissa() { return m_CurvilinearAbscissa;};
  
  /** Get the active branch ie the branch to which inputVME is currently constrained*/  
  vtkIdType GetActiveBranchId() { return m_ActiveBranchId;};
  
  /** Move input vme constrained to skeleton in response to mouse events:
  use in your client as:

  Client::OnEvent(e)
  {
    case ID_TRANSFORM:
    {
      m_CurvilinearAbscissaHelper->MoveOnSkeleton(e);
    }
  }

  where e is mouse move event coming from the vme interactor. 
  See medGizmoInteractionDebugger and medOpInteractionDebugger for example usage.
  */
  void MoveOnSkeleton( mafEvent *mouseEvent);

  medCurvilinearAbscissaOnSkeletonHelper::~medCurvilinearAbscissaOnSkeletonHelper();

  /** GUI Stuff */

  virtual void OnEvent(mafEventBase *maf_event);

  /** Return the gui to be plugged*/
  mafGUI *GetGui() {return m_Gui;};

  /** Enable-Disable the GUI's widgets */
  virtual void EnableWidgets(bool enable);

private:
  
  void GetAbsPose( medVMEPolylineGraph *inputConstrainVMEGraph, vtkIdType inBranchId, double s, mafMatrix &moverOutputAbsPose );
  void MoveOnSkeletonInternal( vtkIdType inBranchId, double inS, double inMoveAbsVector[3], vtkIdType &outputBranch, double &outputS, mafMatrix &outputGizmoAbsMatrix );  
  void ComputeLocalPointPositionBetweenVerticesForSkeletonBranch( double distP0s, int idP0, int idP1, double pOut[3] );
  double CheckS( vtkIdType inputBranchId, double inS );
  void FindBoundaryVertices( vtkIdType inputSkeletonBranchId, double inS, int &outIdMin, int &outIdMax, double &outSFromIdMin );
  void FindTargetBranchAfterBifurcation( vtkIdType bifurcationVertexId, double moveAbsVector[3], vtkIdType &outputVertexId, vtkIdType &outputEdgeID, vtkIdType &outputBranchId);
  bool IsBifurcationVertex( mafPolylineGraph *inPG, int inVertexID );
  void FindPerpendicularVersorsToSegment( int idP0, int idP1, double viewUp[3], double normal[3] );
  
  mafVME *m_InputVME;
  medVMEPolylineGraph *m_ConstraintVMEPolylineGraph; 
  mafPolylineGraph *m_ConstraintPolylineGraph;
  vtkIdType m_ActiveBranchId;
  double m_CurvilinearAbscissa;    

  // log helpers
  static void LogPoint( double *point, const char *logMessage = NULL );
  static void LogVector3( double *vector , const char *logMessage = NULL);
  
  // gui stuff
  virtual void CreateGui();

  int m_GUIActiveBranchId;
  double m_GUICurvilinearAbscissa;
  bool m_TestMode;

  mafObserver *m_Listener;
  mafGUI      *m_Gui;    

};
 
#endif
