/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFBridgeHoleFilter.cxx,v $
Language:  C++
Date:      $Date: 2009-10-08 13:44:29 $
Version:   $Revision: 1.1.2.5 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2012
University of Bedfordshire
=========================================================================*/

#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkMath.h"
#include "vtkIdList.h"
#include "vtkMEDMatrixVectorMath.h"
#include "vtkMEDPolyDataNavigator.h"
#include "vtkMEDHalfTubeRemoval.h"

#include "assert.h"
#include <algorithm>

vtkCxxRevisionMacro(vtkMEDHalfTubeRemoval, "$Revision: 1.61 $");
vtkStandardNewMacro(vtkMEDHalfTubeRemoval);



//------------------------------------------------------------------------------
// Constructor 
//------------------------------------------------------------------------------
vtkMEDHalfTubeRemoval::vtkMEDHalfTubeRemoval()
  : m_RemovalMode(REMOVE_FRONT), m_CenterLine(NULL)
{
  m_CameraPos[0] = 0.0 ;
  m_CameraPos[1] = 0.0 ;
  m_CameraPos[2] = 0.0 ;
}



//------------------------------------------------------------------------------
// Destructor 
//------------------------------------------------------------------------------
vtkMEDHalfTubeRemoval::~vtkMEDHalfTubeRemoval()
{
}



//------------------------------------------------------------------------------
// Overload standard modified time function. 
//------------------------------------------------------------------------------
unsigned long vtkMEDHalfTubeRemoval::GetMTime()
{
  unsigned long mTime1 = this->vtkPolyDataToPolyDataFilter::GetMTime() ;

  if (m_CenterLine != NULL){
    unsigned long mTime2 = m_CenterLine->GetMTime() ;
    return std::max(mTime1, mTime2) ;
  }
  else
    return mTime1 ;
}



//------------------------------------------------------------------------------
// Execute method
//------------------------------------------------------------------------------
void vtkMEDHalfTubeRemoval::Execute()
{
  vtkDebugMacro(<< "Executing vtkMEDHalfTubeRemoval Filter") ;

  // pointers to input and output
  m_Input = this->GetInput() ;
  m_Output = this->GetOutput() ;

  m_Output->DeepCopy(m_Input) ;

  if ((m_CenterLine == NULL) || (m_RemovalMode == REMOVE_NONE))
    return ;

  vtkMEDPolyDataNavigator* nav = vtkMEDPolyDataNavigator::New() ;
  vtkIdList* cellsToRemove = vtkIdList::New() ;

  m_CenterLine->Update() ;

  for (int i = 0 ;  i < m_Output->GetNumberOfCells() ;  i++){
    // get position of cell
    double x[3] ;
    nav->GetCenterOfCell(m_Output, i, x) ;

    // get nearest point on center line
    double xnear[3], rsq ;
    this->FindNearestPoint(x, xnear, rsq) ;

    // point is further than near point if (x-xnear).(xnear-xcam) > 0
    double dotpr = 0.0 ;
    //dotpr = -(x[0]-xnear[0]) ;
    for (int j = 0 ;  j < 3 ;  j++)
      dotpr += (x[j]-xnear[j])*(xnear[j]-m_CameraPos[j]) ;

    if (((m_RemovalMode == REMOVE_BACK) && (dotpr > 0.0)) ||
      ((m_RemovalMode == REMOVE_FRONT) && (dotpr < 0.0))){
        cellsToRemove->InsertNextId(i) ;
    }
  }

  nav->DeleteCells(m_Output, cellsToRemove) ;

  cellsToRemove->Delete() ;
  nav->Delete() ;
}



//------------------------------------------------------------------------------
// Set the center line
//------------------------------------------------------------------------------
void vtkMEDHalfTubeRemoval::SetCenterLine(vtkPolyData* centerline)
{
  m_CenterLine = centerline ;
  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set position of camera
//------------------------------------------------------------------------------
void vtkMEDHalfTubeRemoval::SetCameraPos(double camPos[3])
{
  m_CameraPos[0] = camPos[0] ;
  m_CameraPos[1] = camPos[1] ;
  m_CameraPos[2] = camPos[2] ;
  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set removal mode off (straight-through filter)
//------------------------------------------------------------------------------
void vtkMEDHalfTubeRemoval::SetRemovalModeOff()
{
  if (m_RemovalMode != REMOVE_NONE){
    m_RemovalMode = REMOVE_NONE ;
    this->Modified() ;
  }
}



//------------------------------------------------------------------------------
// Set mode to remove front
//------------------------------------------------------------------------------
void vtkMEDHalfTubeRemoval::SetRemovalModeFront()
{
  if (m_RemovalMode != REMOVE_FRONT){
    m_RemovalMode = REMOVE_FRONT ;
    this->Modified() ;
  }
}



//------------------------------------------------------------------------------
// Set mode to remove back
//------------------------------------------------------------------------------
void vtkMEDHalfTubeRemoval::SetRemovalModeBack()
{
  if (m_RemovalMode != REMOVE_BACK){
    m_RemovalMode = REMOVE_BACK ;
    this->Modified() ;
  }

}



//------------------------------------------------------------------------------
// Set automatic viewing position
//------------------------------------------------------------------------------
void vtkMEDHalfTubeRemoval::SetViewingPositionAuto(double pos[3], double focus[3], double upVector[3])
{
  m_Input = this->GetInput() ;
  m_Input->Update() ;

  if (m_CenterLine == NULL){
    std::cout << "vtkMEDHalfTubeRemoval: undefined center line\n" ;
    assert(false) ;
  }

  double r, b[6], s[3], c[3] ;
  m_Input->GetBounds(b) ;
  s[0] = b[1]-b[0] ;
  s[1] = b[3]-b[2] ;
  s[2] = b[5]-b[4] ;
  c[0] = (b[1]+b[0])/2.0 ;
  c[1] = (b[3]+b[2])/2.0 ;
  c[2] = (b[5]+b[4])/2.0 ;
  r = std::max(s[0], s[1]) ;
  r = std::max(r, s[2]) ;

  double u[3], v[3], w[3] ;
  this->PrincipalAxes(m_Input, u, v, w) ;

  // check that the sense points left to right
  vtkMEDMatrixVectorMath* mvec = vtkMEDMatrixVectorMath::New() ;
  mvec->SetHomogeneous(false) ;
  int n = m_CenterLine->GetPoints()->GetNumberOfPoints() ;
  double p0[3], pn[3] ;
  m_CenterLine->GetPoint(0, p0) ;
  m_CenterLine->GetPoint(n-1, pn) ;
  double p0u = mvec->DotProduct(p0, u) ;
  double pnu = mvec->DotProduct(pn, u) ;
  if (p0u > pnu){
    mvec->MultiplyVectorByScalar(-1.0, u, u) ;
    mvec->MultiplyVectorByScalar(-1.0, v, v) ;
  }
  mvec->Delete() ;

  for (int j = 0 ;  j < 3 ;  j++){
    focus[j] = c[j] ;
    upVector[j] = v[j] ;
    pos[j] = focus[j] + 2.0*r*w[j] ;
  }

  this->SetCameraPos(pos) ;
}


//------------------------------------------------------------------------------
// Find nearest point on center line. \n
// No point locator used - assumes that no. of points is small.
//------------------------------------------------------------------------------
int vtkMEDHalfTubeRemoval::FindNearestPoint(const double p[3], double pnearest[3], double& rsq) const
{
  int n = m_CenterLine->GetPoints()->GetNumberOfPoints() ;
  assert(n > 0) ;

  // start with distance of point 0
  int idmin = 0 ;
  double *pi = m_CenterLine->GetPoint(0) ;
  rsq = DistSq(p, pi) ;

  for (int i = 1 ;  i < n ;  i++){
    pi = m_CenterLine->GetPoint(i) ;
    double r2i = DistSq(p, pi) ;
    if (r2i < rsq){
      idmin = i ;
      rsq = r2i ;
    }
  }

  m_CenterLine->GetPoint(idmin, pnearest) ;
  return idmin ;
}



//------------------------------------------------------------------------------
// Dist squared between two points
//------------------------------------------------------------------------------
double vtkMEDHalfTubeRemoval::DistSq(const double p[3], const double q[3]) const
{
  return (p[0]-q[0])*(p[0]-q[0]) + (p[1]-q[1])*(p[1]-q[1]) + (p[2]-q[2])*(p[2]-q[2]) ;
}




//----------------------------------------------------------------------------
// Find eigenvalues and eigenvectors of 3x3 matrix
// eigenvalues are sorted in order of largest to smallest
// eigenvectors are the columns of V[row][col]
// Symmetric matrices only !
//----------------------------------------------------------------------------
void vtkMEDHalfTubeRemoval::EigenVectors3x3(double A[3][3], double lambda[3], double V[3][3]) const
{
  vtkMath *mth = vtkMath::New() ;

  // vtk function finds eigenvalues and eigenvectors of a symmetric matrix
  mth->Diagonalize3x3(A, lambda, V) ;

  // sort into order of increasing eigenvalue
  // irritating that the vtk function does not do this
  if (lambda[1] < lambda[2]){
    std::swap(lambda[1], lambda[2]) ;
    std::swap(V[0][1], V[0][2]) ;
    std::swap(V[1][1], V[1][2]) ;
    std::swap(V[2][1], V[2][2]) ;
  }

  if (lambda[0] < lambda[1]){
    std::swap(lambda[0], lambda[1]) ;
    std::swap(V[0][0], V[0][1]) ;
    std::swap(V[1][0], V[1][1]) ;
    std::swap(V[2][0], V[2][1]) ;
  }

  if (lambda[1] < lambda[2]){
    std::swap(lambda[1], lambda[2]) ;
    std::swap(V[0][1], V[0][2]) ;
    std::swap(V[1][1], V[1][2]) ;
    std::swap(V[2][1], V[2][2]) ;
  }
}





//------------------------------------------------------------------------------
// Find principal axes of polydata
//------------------------------------------------------------------------------
void vtkMEDHalfTubeRemoval::PrincipalAxes(vtkPolyData* polydata, double u[3], double v[3], double w[3]) const
{
  int n = polydata->GetPoints()->GetNumberOfPoints() ;
  assert(n > 0) ;

  vtkMEDMatrixVectorMath *matVecMath = vtkMEDMatrixVectorMath::New() ;
  matVecMath->SetHomogeneous(false) ;

  // get points
  double (*p)[3] = new double[n][3] ;
  for (int i = 0 ;  i < n ;  i++)
    polydata->GetPoint(i, p[i]) ;

  // get mean point
  double pm[3] ;
  matVecMath->SetVectorToZero(pm) ;
  for (int i = 0 ;  i < n ;  i++)
    matVecMath->AddVectors(p[i], pm, pm) ;
  matVecMath->DivideVectorByScalar((double)n, pm, pm) ;

  // get residuals
  double (*dp)[3] = new double[n][3] ;
  for (int i = 0 ;  i < n ;  i++)
    matVecMath->SubtractVectors(p[i], pm, dp[i]) ;

  // get covariance matrix
  double M[3][3] ;
  matVecMath->SetMatrixToZero3x3(M) ;
  for (int i = 0 ;  i < n ;  i++)
    for (int j = 0 ;  j < 3 ;  j++)
      for (int k = 0 ;  k < 3 ;  k++)
        M[j][k] += dp[i][j]*dp[i][k] ;
  matVecMath->DivideMatrixByScalar3x3((double)n, M, M) ;

  // solve for principal components
  double lambda[3], V[3][3] ;
  EigenVectors3x3(M, lambda, V) ;
  for (int k = 0 ;  k < 3 ;  k++){
    u[k] = V[k][0] ;
    v[k] = V[k][1] ;
    w[k] = V[k][2] ;
  }

  delete [] dp ;
  delete [] p ;
  matVecMath->Delete() ;
}

