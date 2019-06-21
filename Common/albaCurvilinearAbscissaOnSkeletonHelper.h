/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCurvilinearAbscissaOnSkeletonHelper
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaCurvilinearAbscissaOnSkeletonHelper_h
#define __albaCurvilinearAbscissaOnSkeletonHelper_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "vtkIdList.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

//----------------------------------------------------------------------------
// Forward declarations:
//----------------------------------------------------------------------------
class albaEvent;
class albaVME;
class albaPolylineGraph;
class albaVMEPolylineGraph;

/** Decorate input vme with curvilinear abscissa interface for albaVMEPolylineGraph 
 constrained interaction */
class ALBA_EXPORT albaCurvilinearAbscissaOnSkeletonHelper : albaObserver, public albaServiceClient
{
public:

  /** Pass the vme you want to decorate with curvilinear abscissa interface */
  albaCurvilinearAbscissaOnSkeletonHelper(albaVME *inputVME, albaObserver *listener = NULL, bool testMode = false);
 
  /** Set the polyline graph constraint */
  void SetConstraintPolylineGraph(albaVMEPolylineGraph* constraintPolylineGraph);
  
  /** Get the polyline graph constraint */
  albaVMEPolylineGraph *GetSetConstraintPolylineGraph() {return m_ConstraintVMEPolylineGraph;};

  /** Set the curvilinear abscissa value s on branchId branch, return ALBA_OK or ALBA_ERROR */
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
  See albaGizmoInteractionDebugger and albaOpInteractionDebugger for example usage.
  */
  void MoveOnSkeleton( albaEvent *mouseEvent);

  albaCurvilinearAbscissaOnSkeletonHelper::~albaCurvilinearAbscissaOnSkeletonHelper();

  /** GUI Stuff */

  virtual void OnEvent(albaEventBase *alba_event);

  /** Return the gui to be plugged*/
  albaGUI *GetGui() {return m_Gui;};

  /** Enable-Disable the GUI's widgets */
  virtual void EnableWidgets(bool enable);

private:
  
  void GetAbsPose( albaVMEPolylineGraph *inputConstrainVMEGraph, vtkIdType inBranchId, double s, albaMatrix &moverOutputAbsPose );
  void MoveOnSkeletonInternal( vtkIdType inBranchId, double inS, double inMoveAbsVector[3], vtkIdType &outputBranch, double &outputS, albaMatrix &outputGizmoAbsMatrix );  
  void ComputeLocalPointPositionBetweenVerticesForSkeletonBranch( double distP0s, int idP0, int idP1, double pOut[3] );
  double CheckS( vtkIdType inputBranchId, double inS );
  void FindBoundaryVertices( vtkIdType inputSkeletonBranchId, double inS, int &outIdMin, int &outIdMax, double &outSFromIdMin );
  void FindTargetBranchAfterBifurcation( vtkIdType bifurcationVertexId, double moveAbsVector[3], vtkIdType &outputVertexId, vtkIdType &outputEdgeID, vtkIdType &outputBranchId);
  bool IsBifurcationVertex( albaPolylineGraph *inPG, int inVertexID );
  void FindPerpendicularVersorsToSegment( int idP0, int idP1, double viewUp[3], double normal[3] );
  
  albaVME *m_InputVME;
  albaVMEPolylineGraph *m_ConstraintVMEPolylineGraph; 
  albaPolylineGraph *m_ConstraintPolylineGraph;
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

  albaObserver *m_Listener;
  albaGUI      *m_Gui;    

};
 
#endif
