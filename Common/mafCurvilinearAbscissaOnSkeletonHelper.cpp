/*=========================================================================

 Program: MAF2
 Module: mafCurvilinearAbscissaOnSkeletonHelper
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

const bool DEBUG_MODE = true;

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <math.h>

#include "mafGUI.h"
#include "mafGizmoInteractionDebugger.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mmaMaterial.h"
#include "mafGUIMaterialButton.h"
#include "mafInteractorCompositorMouse.h"
#include "mafInteractorGenericMouse.h"
#include "mafSmartPointer.h"
#include "mafVME.h"
#include "mafVMEGizmo.h"
#include "mafVMEPolyline.h"
#include "mafVMEPolylineSpline.h"
#include "mafMatrix.h"
#include "mafAbsMatrixPipe.h" 
#include "mafVMERoot.h"
#include "mafRefSys.h"

#include "vtkMath.h"
#include "vtkLineSource.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkTransform.h"

// new stuff
#include "mafVMEPolylineGraph.h"
#include "mafPolylineGraph.h"
#include "vtkIdList.h"

const double defaultLineLength = 50;


enum GUI_GIZMO_TRANSLATE_ID
{
  ID_BRANCH = MINID,
  ID_S,
  ID_TRANSLATE_Z,
};


void mafCurvilinearAbscissaOnSkeletonHelper::MoveOnSkeleton( mafEvent *mouseEvent )
{
  
  int eventId = mouseEvent->GetId();

  assert(eventId == ID_TRANSFORM);

  vtkMatrix4x4* incomingMatrix = mouseEvent->GetMatrix()->GetVTKMatrix();

  double inputMoveAbsVector[3] = {0,0,0};
  mafTransform::GetPosition(incomingMatrix, inputMoveAbsVector);

  if (DEBUG_MODE)
  {
    mafLogMessage("Move WC Vector:") ;
    mafCurvilinearAbscissaOnSkeletonHelper::LogVector3(inputMoveAbsVector);
  }
  
  double inputS = this->GetCurvilinearAbscissa();
  
  double outputS = -1;
  vtkIdType outputBranchId = -1;
  mafMatrix outputAbsMatrix;

  this->MoveOnSkeletonInternal(this->GetActiveBranchId(), inputS, inputMoveAbsVector
    ,outputBranchId, outputS, outputAbsMatrix);


  double newS;

  if (outputBranchId != this->GetActiveBranchId())
  {
    m_ActiveBranchId = outputBranchId;
    newS = outputS;
  }
  else
  {
    newS = outputS;
  }
  
  this->SetCurvilinearAbscissa(this->GetActiveBranchId(), newS);


}

void mafCurvilinearAbscissaOnSkeletonHelper::LogVector3( double *vector , const char *logMessage /*= NULL*/ )
{
  std::ostringstream stringStream;
  if (logMessage)stringStream << logMessage << std::endl;
  stringStream << "Vector components: [" << vector[0] << " , " << vector[1] << " , " << vector[2] << " ]" << std::endl;
  stringStream << "Vector module: " << vtkMath::Norm(vector) << std::endl; 

  mafLogMessage(stringStream.str().c_str());
}

void mafCurvilinearAbscissaOnSkeletonHelper::LogPoint( double *point, const char *logMessage )
{
  std::ostringstream stringStream;
  if (logMessage) stringStream << logMessage << std::endl;
  stringStream << "Point coordinates: [" << point[0] << " , " << point[1] << " , " << point[2] << " ]" << std::endl;
  mafLogMessage(stringStream.str().c_str());
}

/** 
startup: 
set the local constrain polyline (so I need to decompose a graph into polylines)
vmegraph to polyline converter

converter-> set input (polyline)
converter-> get poly line (0)

graph->getnumberofpolylines
polyline graph->getpolyline(i)

list polyline->getbiforcationslist()


set the s on current polyline

While moving...

Set new s in local polyline 

have i passed a bifurcation point?

yes: check the projection to see if I have to change the local constrain polyline 

if changed:
set the local constrain polyline
set the new s 

no: set s

is the ca inside current polyline?

yes: ok
no: return out of polyline




*/



bool mafCurvilinearAbscissaOnSkeletonHelper::IsBifurcationVertex( mafPolylineGraph *inPG, int inVertexID )
{
    int vertexDegree = inPG->GetConstVertexPtr(inVertexID)->GetDegree();	 
   
    bool isBifurcation = false;

    if (vertexDegree >= 3)
    {
      isBifurcation = true;
    } 
    else
    {
      isBifurcation = false;
    }

    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      if (isBifurcation )
      {
        stringStream << "Vertex Degree: " << vertexDegree <<  " Must check for changing branch!!!" << std::endl;
      }
      else
      {
        stringStream << "Vertex Degree: " << vertexDegree << "No branch change check needed!!!" << std::endl;
      }

      mafLogMessage(stringStream.str().c_str());

    }

    return isBifurcation;
}

void mafCurvilinearAbscissaOnSkeletonHelper::FindTargetBranchAfterBifurcation( vtkIdType bifurcationVertexId, double moveAbsVector[3], 
vtkIdType &outputVertexId, vtkIdType &outputEdgeID, vtkIdType &outputBranchId)
{	
  std::ostringstream stringStream;
  stringStream << "Vertices connected to bifurcation vertex with ID: " << bifurcationVertexId << std::endl;

  // get near branches versor at endpoint
  vtkMAFSmartPointer<vtkIdList> radiatingPointsIdList;
  m_ConstraintPolylineGraph->GetConstVertexPtr(bifurcationVertexId)->GetVerticesIdList(radiatingPointsIdList);
  int radiatingPointsNumber = radiatingPointsIdList->GetNumberOfIds();

  int id  = 0;
  for (int vid = 0; vid < radiatingPointsNumber; vid++) 
  { 
    stringStream << id << " connected vertex ID: " << radiatingPointsIdList->GetId(vid) << std::endl;
    id++;
  }

  if (DEBUG_MODE)
  {
    mafLogMessage(stringStream.str().c_str());
  }

  // need abs versor for each possible branch to project motion abs versor on it

  // get bifurcation point local coords
  double bifurcationPoint[3] = {0,0,0};
  m_ConstraintPolylineGraph->GetVertexCoords(bifurcationVertexId, bifurcationPoint);

  vtkMAFSmartPointer<vtkPoints> neighborsLocalCoordinates ;
  vtkMAFSmartPointer<vtkPoints> localRadiatingVersors;

  double localPoint[3] = {0,0,0};
  double localVector[3] = {0,0,0};

  if (DEBUG_MODE)
  {
    mafLogMessage("neighbors local coords and local versors:");
  }

  for (int neighborID = 0; neighborID < radiatingPointsNumber; neighborID++) 
  { 
    vtkIdType polylineGraphVertexID = radiatingPointsIdList->GetId(neighborID);
    m_ConstraintPolylineGraph->GetVertexCoords(polylineGraphVertexID, localPoint);
    neighborsLocalCoordinates->InsertPoint(neighborID, localPoint );

    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "Near point with id: " << neighborID  << std::endl;
      mafLogMessage(stringStream.str().c_str());
      LogPoint(localPoint, "has coordinates:");
    }

    mafTransform::BuildVector(bifurcationPoint, localPoint, localVector);
    vtkMath::Normalize(localVector);
    localRadiatingVersors->InsertPoint(neighborID, localVector);

    if (DEBUG_MODE)
    {
      LogVector3(localVector, "bifPoint_localPoint_Versor:");
    }
  }

  std::vector<double> projectionsVector;
  int bifurcationsNumber = localRadiatingVersors->GetNumberOfPoints();

  // check move vector projection on each radiating versor
  for (int bifurcationId = 0; bifurcationId < bifurcationsNumber; bifurcationId++) 
  { 
    double currentBifurcationVersor[3] = {0,0,0};
    localRadiatingVersors->GetPoint(bifurcationId, currentBifurcationVersor);
    double currentProjection = mafTransform::ProjectVectorOnAxis(moveAbsVector, currentBifurcationVersor );
    projectionsVector.push_back(currentProjection);
  }

  // the biggest is the new active branch
  int maxProjectionId = -1;
  double maxProjectionValue = -1;

  for (int currentProjectionVersorsId = 0; currentProjectionVersorsId < bifurcationsNumber; currentProjectionVersorsId++) 
  { 
    double currentProjectionValue = projectionsVector.at(currentProjectionVersorsId);

    if (currentProjectionValue > maxProjectionValue)
    {
      maxProjectionId = currentProjectionVersorsId;
      maxProjectionValue = currentProjectionValue;
    }      
  }

  vtkIdType bifurcationArrowVertexId = radiatingPointsIdList->GetId(maxProjectionId) ;

  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Maximum projection value: " << maxProjectionValue  
      << " corresponding to edge [ bifId, radId ] = ["
      << bifurcationVertexId << " , " << bifurcationArrowVertexId << " ] "
      << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }
  
  vtkIdType outputVertexBifurcation = bifurcationVertexId;
  vtkIdType outputVertexArrow = bifurcationArrowVertexId;

  // find the branch ID corresponding to the greater projection

  // find the branch of the edge...

  int degree = m_ConstraintPolylineGraph->GetConstVertexPtr(bifurcationVertexId)->GetDegree();
    // assert(degree == 3);
  
  vtkIdType newActiveEdge = -1;

  for (int bifurcationArrowVertexId = 0; bifurcationArrowVertexId < radiatingPointsNumber ; bifurcationArrowVertexId++) 
  { 

    vtkIdType edgeId = m_ConstraintPolylineGraph->GetConstVertexPtr(bifurcationVertexId)->GetEdgeId(bifurcationArrowVertexId);
    
    if (m_ConstraintPolylineGraph->GetConstEdgePtr(edgeId)->GetVertexId(0) == outputVertexArrow)
    {
      newActiveEdge = edgeId;
      break;
    }
    else if (m_ConstraintPolylineGraph->GetConstEdgePtr(edgeId)->GetVertexId(1) == outputVertexArrow)
    {
      newActiveEdge = edgeId;
      break;
    }
  }

  vtkIdType newActiveBranchId = m_ConstraintPolylineGraph->GetConstEdgePtr(newActiveEdge)->GetBranchId();

  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << " edge Id: " 
      << outputEdgeID << " on branch: " <<  newActiveBranchId << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }
  
  outputVertexId = outputVertexArrow;
  outputEdgeID = newActiveEdge;
  outputBranchId = newActiveBranchId;

}
  

mafCurvilinearAbscissaOnSkeletonHelper::mafCurvilinearAbscissaOnSkeletonHelper( mafVME *inputVME, mafObserver *listener, bool testMode)
{
  m_InputVME = inputVME;
  m_ConstraintVMEPolylineGraph = NULL;
  m_TestMode = testMode;
  
  m_ConstraintPolylineGraph = NULL; 
  m_ConstraintPolylineGraph = new mafPolylineGraph();
  
  m_ActiveBranchId = 0;
  m_GUIActiveBranchId = m_ActiveBranchId;

  m_CurvilinearAbscissa = 0;
  m_GUICurvilinearAbscissa = m_CurvilinearAbscissa;

  m_Listener = listener;
  m_Gui = NULL;

  if (!m_TestMode)
  {
    CreateGui();
  }
}

mafCurvilinearAbscissaOnSkeletonHelper::~mafCurvilinearAbscissaOnSkeletonHelper()
{
  cppDEL(m_ConstraintPolylineGraph);
}

void mafCurvilinearAbscissaOnSkeletonHelper::SetConstraintPolylineGraph( mafVMEPolylineGraph* constraintPolylineGraph )
{
  assert(constraintPolylineGraph);
  m_ConstraintVMEPolylineGraph = constraintPolylineGraph;

  vtkPolyData *pd = vtkPolyData::SafeDownCast(m_ConstraintVMEPolylineGraph->GetOutput()->GetVTKData());
  assert(pd);

  pd->Update();

  m_ConstraintPolylineGraph->Clear();
  m_ConstraintPolylineGraph->CopyFromPolydata(pd);
  m_ConstraintPolylineGraph->MergeSimpleJoinedBranches();   //BES 1.5.2008 - added to support "not correct" polylines

  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Checking for mafVMEPolylineGraph " << constraintPolylineGraph->GetName()
      << " consistency..."  << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }

  bool isConsistent = m_ConstraintPolylineGraph->SelfCheck();

  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "The skeleton consistency check returned: " << isConsistent  << std::endl;
    std::string resultString = isConsistent?"IS":"IS NOT";
    stringStream << "Skeleton " << resultString << " consistent" << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }

  if (isConsistent == false)
  {
    wxMessageBox("Skeleton not consistent! See log area for more details.","", wxICON_WARNING);
  }

  
}

int mafCurvilinearAbscissaOnSkeletonHelper::SetCurvilinearAbscissa( vtkIdType branchId, double s )
{ 
  int numberOfBranches = m_ConstraintPolylineGraph->GetNumberOfBranches();

  if (branchId < 0)
  {
    std::ostringstream stringStream;
    stringStream << " please use an integer between 0 and " << numberOfBranches - 1 << std::endl;
    mafLogMessage(stringStream.str().c_str());
    return MAF_ERROR;          
  }
  if (branchId > numberOfBranches)
  {
    std::ostringstream stringStream;
    stringStream << "Constrain has: " << numberOfBranches  << std::endl;
    mafLogMessage(stringStream.str().c_str());      
    return MAF_ERROR;
  }

  double length = m_ConstraintPolylineGraph->GetBranchLength(branchId);
  
  if (s  < 0 || s > length)
  {
    std::ostringstream stringStream;
    stringStream << "Curvilinear abscissa must be between 0 and " << length\
    << " for current to branch" << std::endl;
    mafLogMessage(stringStream.str().c_str());          
    return MAF_ERROR;
  }

  m_CurvilinearAbscissa = s;
  m_ActiveBranchId = branchId;

  mafMatrix outputGizmoAbsMatrix;
  GetAbsPose(m_ConstraintVMEPolylineGraph, branchId, s, outputGizmoAbsMatrix);
  m_InputVME->SetAbsMatrix(outputGizmoAbsMatrix, -1);
  
  if(!m_TestMode)
  {
    // gui update
    m_GUIActiveBranchId = branchId;
    m_GUICurvilinearAbscissa = s;
    m_Gui->Update();
  }

  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  return MAF_OK;
}

void mafCurvilinearAbscissaOnSkeletonHelper::GetAbsPose( mafVMEPolylineGraph *inputConstrainVMEGraph, vtkIdType inBranchId, double s, mafMatrix &moverOutputAbsPose )
{

  std::ostringstream stringStream;

  const mafPolylineGraph::Branch *currentBranch = m_ConstraintPolylineGraph->GetConstBranchPtr(inBranchId);
  assert(currentBranch != NULL);

  double branchLength = m_ConstraintPolylineGraph->GetBranchLength(inBranchId);

  stringStream << "current branch ID: " << inBranchId << std::endl;
  stringStream << "current branch length: " << branchLength << std::endl;

  mafLogMessage(stringStream.str().c_str());

  // get the two boundary vertexes in current branch
  int p0ID = -1, p1ID = -1;
  double distP0s = -1;

  FindBoundaryVertices(inBranchId,s,p0ID,p1ID,distP0s);

  // calculate local point position between vertexes
  double pos[3] = {0,0,0};
  double pOut[3] = {0,0,0};

  ComputeLocalPointPositionBetweenVerticesForSkeletonBranch(distP0s,p0ID,p1ID,pOut);

  // compute the normal
  double normal1[3] = {0,0,0};
  double normal2[3] = {0,0,1};

  FindPerpendicularVersorsToSegment(p0ID, p1ID, normal2, normal1);

  // create the local pose matrix from position and normal
  double normal1_x_normal2[3] = {0,0,0};

  vtkMath::Normalize(normal2);
  vtkMath::Normalize(normal1);
  vtkMath::Cross(normal1, normal2, normal1_x_normal2);

  mafMatrix localGizmoPose;
  mafTransform::SetVersor(0, normal1_x_normal2, localGizmoPose);
  mafTransform::SetVersor(1, normal1, localGizmoPose);
  mafTransform::SetVersor(2, normal2, localGizmoPose);
  localGizmoPose.SetTimeStamp(-1);
  mafTransform::SetPosition(localGizmoPose, pOut);

  // get absolute pose for the constrain
  mafMatrix constrainAbsPose; // input
  constrainAbsPose = inputConstrainVMEGraph->GetAbsMatrixPipe()->GetMatrix();

  mafSmartPointer<mafTransform> trans; // output
  trans->SetMatrix(constrainAbsPose);
  trans->Concatenate(localGizmoPose, PRE_MULTIPLY);
  trans->RotateY(90, PRE_MULTIPLY);
  trans->Update();

  moverOutputAbsPose = trans->GetMatrix();
}


void mafCurvilinearAbscissaOnSkeletonHelper::ComputeLocalPointPositionBetweenVerticesForSkeletonBranch( double distP0s, int idP0, int idP1, double pOut[3] )
{
  // get the points from the two vertexes
  double p0[3] = {0,0,0};
  double p1[3] = {0,0,0};

  m_ConstraintPolylineGraph->GetVertexCoords(idP0, p0);
  m_ConstraintPolylineGraph->GetVertexCoords(idP1, p1 );

  // creates versor from two points
  double p0p1[3] = {0,0,0};
  mafTransform::BuildVector(p0,p1,p0p1);
  vtkMath::Normalize(p0p1);

  double p0s[3] = {0,0,0};
  mafTransform::BuildVector(distP0s, p0p1, p0s, mafRefSys::GLOBAL);

  mafTransform::AddVectors(p0,p0s,pOut);

}

void mafCurvilinearAbscissaOnSkeletonHelper::FindPerpendicularVersorsToSegment( int idP0, int idP1, double viewUp[3], double normal[3] )
{
  double p0[3] = {0,0,0};
  double p1[3] = {0,0,0};

  m_ConstraintPolylineGraph->GetVertexCoords(idP0, p0);
  m_ConstraintPolylineGraph->GetVertexCoords(idP1, p1);

  double p0p1[3] = {0,0,0};

  mafTransform::BuildVector(p0, p1, p0p1);
  vtkMath::Normalize(p0p1);
 
  mafTransform::FindPerpendicularVersors(p0p1, viewUp, normal) ;
  
}

double mafCurvilinearAbscissaOnSkeletonHelper::CheckS( vtkIdType inputBranchId, double inS )
{
  // calculate branch length 
  double branchLenght = m_ConstraintPolylineGraph->GetBranchLength(inputBranchId);

  if (inS <  0)
  {
    // check if I have to go on  another branch
    // what is the degree of the first branch point?
    // if >= 3 then check for projection

    inS = 0;
    mafLogMessage("out of constrain polyline: setting s to 0") ;    
  }
  else if (inS  > branchLenght)
  {
    // check if I have to go on  another branch
    // what is the degree of the last branch point?
    // if >= 3 then check for 5

    vtkIdType endVertexId = m_ConstraintPolylineGraph->GetConstBranchPtr(inputBranchId)->GetLastVertexId();
    int endVertexDegree = m_ConstraintPolylineGraph->GetConstVertexPtr(endVertexId)->GetDegree();
    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "branch with ID: " << inputBranchId << " with last vertex of ID: " << 
        endVertexId << " has degree: " << endVertexDegree << std::endl;

      mafLogMessage(stringStream.str().c_str());
    }

    if (endVertexDegree >= 3)
    {
      if (DEBUG_MODE)
      {

        // BuildRadiatingVersors(endVertexId);
      }
    }

    inS = branchLenght;
    mafLogMessage("out of constrain polyline: setting s to constraint length") ;
  }
  else
  { 
    // check if I have passed a branch point
    // what is the degree of the last branch point?
    // if >= 3 then check for projection

    assert(true);
  }

  return inS;
}

void mafCurvilinearAbscissaOnSkeletonHelper::MoveOnSkeletonInternal( vtkIdType inBranchId, double inS, double inMoveAbsVector[3], vtkIdType &outputBranch, double &outputS, mafMatrix &outputGizmoAbsMatrix )
{
  int lowerSVertexID = -1;
  int upperSVertexID = -1;
  double outSFromIdMin = -1;
  FindBoundaryVertices(inBranchId, inS, lowerSVertexID, upperSVertexID, outSFromIdMin );

  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Active branch ID: " << inBranchId
      << " S: " << inS << " between vertices with IDMin: " << lowerSVertexID << " and IDMax: " << upperSVertexID << std::endl;

    mafLogMessage(stringStream.str().c_str());
  }

  // build s local versor
  double p0[3] = {0,0,0};
  double p1[3] = {0,0,0};

  m_ConstraintPolylineGraph->GetVertexCoords(lowerSVertexID, p0);
  m_ConstraintPolylineGraph->GetVertexCoords(upperSVertexID, p1);

  double p0p1[3] = {0,0,0};
  mafTransform::BuildVector(p0, p1, p0p1);

  // project move vector on s poly

  double inMoveAbsVector_p0p1_ProjectionValue = 
  mafTransform::ProjectVectorOnAxis(inMoveAbsVector, p0p1);

  // compute distance between s and near/far vertex
  double p0p1Distance = vtkMath::Norm(p0p1);

  double dist_P0_S = outSFromIdMin;
  double dist_S_P1 = p0p1Distance - outSFromIdMin;

  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "dist_P0_S: " << dist_P0_S  << "dist_S_P1: " << dist_S_P1 << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }

  bool isBifurcationVertex = false;
  vtkIdType bifurcationPoindID = -1;

  // if passed upper vertex
  if (inMoveAbsVector_p0p1_ProjectionValue > 0  && inMoveAbsVector_p0p1_ProjectionValue > dist_S_P1)
  {
    mafLogMessage("passed upper vertex!!! ################################################### ");
    isBifurcationVertex = IsBifurcationVertex(m_ConstraintPolylineGraph, upperSVertexID);
    if (isBifurcationVertex)
    {
      bifurcationPoindID = upperSVertexID;
    }
  } 
  // if passed lower vertex
  else if (inMoveAbsVector_p0p1_ProjectionValue < 0 && abs(inMoveAbsVector_p0p1_ProjectionValue) > dist_P0_S )
  {
    mafLogMessage("passed lower vertex!!! ################################################### ");
    isBifurcationVertex = IsBifurcationVertex(m_ConstraintPolylineGraph, lowerSVertexID);
    if (isBifurcationVertex)
    {
      bifurcationPoindID = lowerSVertexID;
    }
  }
  // no vertex passed
  else
  {
    mafLogMessage("no vertex passed while dragging");
    outputS = inS + inMoveAbsVector_p0p1_ProjectionValue;
  }

  vtkIdType newEndVertexId = -1;
  vtkIdType newActiveEdgeId = -1;
  vtkIdType newActiveBranchId = -1;

  if (isBifurcationVertex)
  {
    FindTargetBranchAfterBifurcation(bifurcationPoindID, inMoveAbsVector, 
      newEndVertexId, newActiveEdgeId, newActiveBranchId );

    outputBranch = newActiveBranchId;

    double bifurcationPoint[3] = {0,0,0};
    m_ConstraintPolylineGraph->GetVertexCoords(bifurcationPoindID, bifurcationPoint);

    double bifurcationPoint_LastSId_Versor[3] = {0,0,0};
    mafTransform::BuildVector(bifurcationPoint,p0, bifurcationPoint_LastSId_Versor);

    vtkMath::Normalize(bifurcationPoint_LastSId_Versor);

    double bifurcationPoint_LastSId_Vector[3]
    = {0,0,0};
    mafTransform::BuildVector(abs(dist_S_P1), bifurcationPoint_LastSId_Versor, bifurcationPoint_LastSId_Vector, mafRefSys::GLOBAL);

    double outputMoveVector[3] = {0,0,0};
    outputMoveVector[0] = inMoveAbsVector[0] + bifurcationPoint_LastSId_Vector[0];
    outputMoveVector[1] = inMoveAbsVector[1] + bifurcationPoint_LastSId_Vector[1];
    outputMoveVector[2] = inMoveAbsVector[2] + bifurcationPoint_LastSId_Vector[2];

    if (DEBUG_MODE)
    {
      LogVector3(inMoveAbsVector, "inMoveAbsVector");
      LogVector3(bifurcationPoint_LastSId_Vector, "bifurcationPoint_LastSId_Vector");
      LogVector3(outputMoveVector, "sum");
    }    // to compute outputGizmoAbsMatrix 

    m_ConstraintPolylineGraph->GetVertexCoords(bifurcationPoindID,bifurcationPoint);

    double newEndVertex[3] = {0,0,0};
    m_ConstraintPolylineGraph->GetVertexCoords(newEndVertexId, newEndVertex);



    double bifurcationPoint_newEndVertex_Versor[3] = {0,0,0};
    mafTransform::BuildVector(bifurcationPoint,newEndVertex, bifurcationPoint_newEndVertex_Versor);
    vtkMath::Normalize(bifurcationPoint_newEndVertex_Versor);


    outputS = mafTransform::ProjectVectorOnAxis(outputMoveVector, bifurcationPoint_newEndVertex_Versor);
    // this can fail going back on 1
    assert(outputS >= 0);

    double startBranch_bifurcationPoint_Length = m_ConstraintPolylineGraph->GetBranchIntervalLength(newActiveBranchId, 
      m_ConstraintPolylineGraph->GetConstBranchPtr(newActiveBranchId)->GetVertexId(0),
      bifurcationPoindID);

    // I can be on branch end or middle point? how can i know??
    if (bifurcationPoindID > newEndVertexId)
    {
      mafLogMessage("going toward beginning of branch-------->BEG-------->BEG-------->");

      outputS = startBranch_bifurcationPoint_Length - outputS;
    }
    else if(bifurcationPoindID  <  newEndVertexId)
    {
      mafLogMessage("going toward end of branch-------->END-------->END-------->");
      outputS = startBranch_bifurcationPoint_Length + outputS;
    }

    if (DEBUG_MODE)
    {
      LogVector3(bifurcationPoint_newEndVertex_Versor, "bifurcationPoint_newEndVertex_Versor");
      std::ostringstream stringStream;
      stringStream << "Output s:" << outputS  << std::endl;
      stringStream << "Output branch id:" << outputBranch  << std::endl;
      mafLogMessage(stringStream.str().c_str());
    }
  }
  else
  {
    outputBranch = m_ActiveBranchId;
    outputS = inS + inMoveAbsVector_p0p1_ProjectionValue;

  }
}
void mafCurvilinearAbscissaOnSkeletonHelper::FindBoundaryVertices( vtkIdType inputSkeletonBranchId, double inS, int &outIdMin, int &outIdMax, double &outSFromIdMin )
{
  // get the boundary vertices on current branch
  outIdMin = outIdMax = -1;

  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Finding boundary vertices for s=" << inS << " on branch with ID: " << inputSkeletonBranchId << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }

  const mafPolylineGraph::Branch *currentBranch = m_ConstraintPolylineGraph->GetConstBranchPtr(inputSkeletonBranchId);
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Current Branch PrintSelf:" << std::endl;
    currentBranch->PrintSelf(stringStream, 0);
    mafLogMessage(stringStream.str().c_str());
  }

  inS = CheckS(inputSkeletonBranchId, inS);

  // get the vtk polyline cell
  vtkMAFSmartPointer<vtkIdList> branchPointsIdList;
  m_ConstraintPolylineGraph->GetConstBranchPtr(inputSkeletonBranchId)->GetVerticesIdList(branchPointsIdList);

  double endP[3], startP[3];

  int vtkId = 0;
  double lastSum = 0;
  double sum = 0;

  // Iterate over the points
  double branchLength = m_ConstraintPolylineGraph->GetBranchLength(inputSkeletonBranchId);
  do 
  {
    lastSum = sum;

    assert(sum <= branchLength);

    if (DEBUG_MODE)
    {
      int numberOfIds = branchPointsIdList->GetNumberOfIds();
      std::ostringstream stringStream;
      stringStream << "branch ID: " << inputSkeletonBranchId << "of length: " << branchLength  << " has: " << numberOfIds <<  " vertices"  << std::endl;
      mafLogMessage(stringStream.str().c_str());
    }

    vtkIdType startPBranchID = branchPointsIdList->GetId(vtkId);
    assert(vtkId < branchPointsIdList->GetNumberOfIds());
    assert(startPBranchID >= 0);
    m_ConstraintPolylineGraph->GetVertexCoords(startPBranchID, startP);

    vtkIdType endPBranchID = branchPointsIdList->GetId(vtkId + 1);
    assert(vtkId + 1  < branchPointsIdList->GetNumberOfIds());
    assert(endPBranchID >= 0);

    m_ConstraintPolylineGraph->GetVertexCoords(endPBranchID, endP);
    sum += sqrt(vtkMath::Distance2BetweenPoints(endP, startP));
    vtkId++;

  } while(sum < inS);

  outIdMin = branchPointsIdList->GetId(vtkId-1);
  outIdMax = branchPointsIdList->GetId(vtkId);
  outSFromIdMin = inS - lastSum;

  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Found skeleton vertex IdMin: " << outIdMin << " IdMax: " << outIdMax << std::endl;
    stringStream << "Distance from idMin is: " << outSFromIdMin << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }
}


void BuildVector( double p0[3],double p1[3],double vOut[3] )
{
  for (int i = 0;i < 3; i++)
  {
    vOut[i] = p1[i] - p0[i];
  }
}


void mafCurvilinearAbscissaOnSkeletonHelper::CreateGui()
{
  m_Gui = new mafGUI(this);

  m_Gui->Divider(2);
  // m_Gui->Label("skeleton gui", true);
  
  m_Gui->Integer(ID_BRANCH, "branch id", &m_GUIActiveBranchId, 0);
  m_Gui->Double(ID_S, "s",  &m_GUICurvilinearAbscissa, 0);
  m_Gui->Divider();
  m_Gui->Update();
}

void mafCurvilinearAbscissaOnSkeletonHelper::OnEvent(mafEventBase *maf_event)
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_BRANCH:
      {
        int result = MAF_ERROR;
        result = SetCurvilinearAbscissa(m_GUIActiveBranchId, 0);
        if (result == MAF_ERROR)
        {
          m_GUIActiveBranchId = m_ActiveBranchId;
          m_Gui->Update();
        }
      }
      break;

      case ID_S:
      {
        int result = MAF_ERROR;
        result = SetCurvilinearAbscissa(m_ActiveBranchId, m_GUICurvilinearAbscissa);
        if (result == MAF_ERROR)
        {
          m_GUICurvilinearAbscissa = m_CurvilinearAbscissa;
          m_Gui->Update();
        }

      }
      break;

      default:
        mafEventMacro(*e);
      break;
    }
  }
}

void mafCurvilinearAbscissaOnSkeletonHelper::EnableWidgets( bool enable )
{
  m_Gui->Enable(ID_BRANCH, enable);
  m_Gui->Enable(ID_S, enable);
  m_Gui->Update();
}
