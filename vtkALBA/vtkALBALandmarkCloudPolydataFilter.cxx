/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkALBALandmarkCloudPolydataFilter.cxx,v $
Language:  C++
Date:      $Date: 2009-10-08 13:44:29 $
Version:   $Revision: 1.1.2.5 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2012
University of Bedfordshire
=========================================================================*/


#include "vtkPolyData.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkObjectFactory.h"
#include "vtkIdList.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"

#include "vtkALBALandmarkCloudPolydataFilter.h"
#include "vtkALBAAddScalarsFilter.h"
#include <algorithm>
#include "vtkInformationVector.h"
#include "vtkInformation.h"


vtkStandardNewMacro(vtkALBALandmarkCloudPolydataFilter);



//------------------------------------------------------------------------------
// Constructor 
//------------------------------------------------------------------------------
vtkALBALandmarkCloudPolydataFilter::vtkALBALandmarkCloudPolydataFilter()
  : m_Input(NULL), m_Output(NULL),
  m_Radius(1.0), m_ThetaRes(8), m_PhiRes(9)
{
  char defaultName[256] = "LandmarkScalars" ;
  bool istop = false ;
  for (int i = 0 ;  !istop && i < 256 ;   i++){
    m_ScalarName[i] = defaultName[i] ;
    istop = (defaultName[i] == '\0') ;
  }
}



//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
vtkALBALandmarkCloudPolydataFilter::~vtkALBALandmarkCloudPolydataFilter()
{
  Clear() ;
}




//------------------------------------------------------------------------------
int vtkALBALandmarkCloudPolydataFilter::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // pointers to input and output
  m_Input = input;
  m_Output = output;

  // update the positions from the polydata
  double x[3] ;
  for (int i = 0 ;  i < GetNumberOfLandmarks() ;  i++){
    int ptId = GetPolydataId(i) ;
    m_Input->GetPoint(ptId, x) ;
    SetPosition(i, x) ;
  }

  // merge individual spheres into one polydata
  vtkAppendPolyData* append = vtkAppendPolyData::New() ;
  for (int i = 0 ;  i < GetNumberOfLandmarks() ;  i++)
    append->AddInputData(m_AddScalarsList[i]->GetOutput()) ;
  append->Update() ;

  m_Output->DeepCopy(append->GetOutput()) ;

	return 1;
}



//------------------------------------------------------------------------------
// Clear list of landmarks
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::Clear()
{
  int n = GetNumberOfLandmarks() ;
  for (int i = 0 ;  i < n ;  i++){
    m_SphereList[i]->Delete() ;
    m_AddScalarsList[i]->Delete() ;
  }
  m_SphereList.clear() ;
  m_AddScalarsList.clear() ;
  m_PolydataIdsList.clear() ;
}





//------------------------------------------------------------------------------
// Add single point from polydata
//------------------------------------------------------------------------------
int vtkALBALandmarkCloudPolydataFilter::AddLandmark(int ptId)
{
  double x[3] = {0,0,0} ;

  vtkSphereSource* sphere = vtkSphereSource::New() ;
  sphere->SetCenter(x) ;
  sphere->SetRadius(m_Radius) ;
  sphere->SetThetaResolution(m_ThetaRes) ;
  sphere->SetPhiResolution(m_PhiRes) ;

  vtkALBAAddScalarsFilter* addScalars = vtkALBAAddScalarsFilter::New() ;
  addScalars->SetInputConnection(sphere->GetOutputPort()) ;

  m_SphereList.push_back(sphere) ;
  m_AddScalarsList.push_back(addScalars) ;
  m_PolydataIdsList.push_back(ptId) ;

  this->Modified() ;

  return GetNumberOfLandmarks()-1 ;
}



//------------------------------------------------------------------------------
// Add single point from polydata
//------------------------------------------------------------------------------
int vtkALBALandmarkCloudPolydataFilter::AddLandmark(int ptId, double col[3])
{
  int i = AddLandmark(ptId) ;
  SetColor(i,col); 
  return i ;
}



//------------------------------------------------------------------------------
// Add single point from polydata
//------------------------------------------------------------------------------
int vtkALBALandmarkCloudPolydataFilter::AddLandmark(int ptId, double r, double g, double b)
{
  int i = AddLandmark(ptId) ;
  SetColor(i,r,g,b); 
  return i ;
}



//------------------------------------------------------------------------------
// Add list of points from polydata
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::AddLandmarks(vtkIdList* ptIds)
{
  for (int i = 0 ;  i < ptIds->GetNumberOfIds() ;  i++){
    int id = ptIds->GetId(i) ;
    AddLandmark(id) ;
  }
}



//------------------------------------------------------------------------------
// Add list of points from polydata
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::AddLandmarks(vtkIdList* ptIds, double col[3])
{
  for (int i = 0 ;  i < ptIds->GetNumberOfIds() ;  i++){
    int id = ptIds->GetId(i) ;
    int j = AddLandmark(id) ;
    SetColor(j, col) ;
  }
}



//------------------------------------------------------------------------------
// Add list of points from polydata
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::AddLandmarks(vtkIdList* ptIds, double r, double g, double b)
{
  for (int i = 0 ;  i < ptIds->GetNumberOfIds() ;  i++){
    int id = ptIds->GetId(i) ;
    int j = AddLandmark(id) ;
    SetColor(j, r, g, b) ;
  }
}



//------------------------------------------------------------------------------
// Remove last landmark
// No effect if list is already empty.
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::DeleteLastLandmark()
{
  int i = GetNumberOfLandmarks() - 1 ;
  if (i >= 0){
    m_SphereList[i]->Delete() ;
    m_SphereList.pop_back() ;

    m_AddScalarsList[i]->Delete() ;
    m_AddScalarsList.pop_back() ;
  }

  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set theta and phi resolution
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::SetThetaPhiResolution(int thetaRes, int phiRes)
{
  m_ThetaRes = thetaRes ;
  m_PhiRes = phiRes ;

  for (int i = 0  ;  i < GetNumberOfLandmarks() ;  i++){
    m_SphereList[i]->SetThetaResolution(m_ThetaRes) ;
    m_SphereList[i]->SetPhiResolution(m_PhiRes) ;
  }

  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set radius
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::SetRadius(double r)
{
  m_Radius = r ;

  for (int i = 0  ;  i < GetNumberOfLandmarks() ;  i++)
    m_SphereList[i]->SetRadius(m_Radius) ;

  this->Modified() ;
}



//------------------------------------------------------------------------------
/// Multiply radius by f
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::MultiplyRadius(double f)
{
  SetRadius(f * GetRadius()) ;
}



//------------------------------------------------------------------------------
// Set estimated radius based on polydata bounds
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::SetRadiusAuto(vtkPolyData* polydata)
{
  const double SizeFactor = 0.015 ;

  double b[6], siz[3] ;
  polydata->GetBounds(b) ;
  siz[0] = b[1]-b[0] ;
  siz[1] = b[3]-b[2] ;
  siz[2] = b[5]-b[4] ;
  double totalSize = sqrt(siz[0]*siz[0] + siz[1]*siz[1] + siz[2]*siz[2]) ;

  SetRadius(SizeFactor * totalSize) ;
}



//------------------------------------------------------------------------------
// Get radius
//------------------------------------------------------------------------------
double vtkALBALandmarkCloudPolydataFilter::GetRadius() const
{
  return m_Radius ;
}



//------------------------------------------------------------------------------
// Set color of all landmarks
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::SetColor(double col[3])
{
  for (int i = 0  ;  i < GetNumberOfLandmarks() ;  i++)
    SetColor(i,col) ;
}



//------------------------------------------------------------------------------
// Set color of all landmarks
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::SetColor(double r, double g, double b)
{

  for (int i = 0  ;  i < GetNumberOfLandmarks() ;  i++)
    SetColor(i,r,g,b) ;
}



//------------------------------------------------------------------------------
// Set color of landmark i
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::SetColor(int i, double r, double g, double b)
{    
  m_AddScalarsList[i]->SetColor(r,g,b) ;
  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set color of landmark i
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::SetColor(int i, double col[3])
{    
  m_AddScalarsList[i]->SetColor(col) ;
  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set position of landmark i
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::SetPosition(int i, double x[3])
{
  m_SphereList[i]->SetCenter(x) ;
  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set position of landmark i
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::SetPosition(int i, double x, double y, double z)
{
  m_SphereList[i]->SetCenter(x,y,z) ;

  this->Modified() ;
}



//------------------------------------------------------------------------------
// Get position of landmark i
//------------------------------------------------------------------------------
void vtkALBALandmarkCloudPolydataFilter::GetPosition(int i, double x[3]) const
{
  m_SphereList[i]->GetCenter(x) ;
}



//------------------------------------------------------------------------------
// Get position of landmark i
//------------------------------------------------------------------------------
const double* vtkALBALandmarkCloudPolydataFilter::GetPosition(int i) const
{
  return m_SphereList[i]->GetCenter() ;
}




//------------------------------------------------------------------------------
// Get bounds of landmark cloud
//------------------------------------------------------------------------------
bool vtkALBALandmarkCloudPolydataFilter::GetBounds(double b[6]) const
{
  int n = GetNumberOfLandmarks() ;
  if (n == 0)
    return false ;

  b[0] = GetPosition(0)[0] ;
  b[1] = GetPosition(0)[0] ;
  b[2] = GetPosition(0)[1] ;
  b[3] = GetPosition(0)[1] ;
  b[4] = GetPosition(0)[2] ;
  b[5] = GetPosition(0)[2] ;

  for (int i = 0 ;  i < n ;  i++){
    double pos[3] ;
    GetPosition(i, pos) ;
    b[0] = std::min(pos[0], b[0]) ;
    b[1] = std::max(pos[0], b[1]) ;
    b[2] = std::min(pos[1], b[2]) ;
    b[3] = std::max(pos[1], b[3]) ;
    b[4] = std::min(pos[2], b[4]) ;
    b[5] = std::max(pos[2], b[5]) ;
  }

  return true ;
}



//------------------------------------------------------------------------------
// Get center of landmark cloud
//------------------------------------------------------------------------------
bool vtkALBALandmarkCloudPolydataFilter::GetCenter(double x[3]) const
{
  double b[6] ;
  bool ok = GetBounds(b) ;
  if (!ok)
    return false ;

  x[0] = (b[1]+b[0])/2.0 ;
  x[1] = (b[3]+b[2])/2.0 ;
  x[2] = (b[5]+b[4])/2.0 ;

  return true ;
}



//------------------------------------------------------------------------------
// Get centroid of landmark cloud
//------------------------------------------------------------------------------
bool vtkALBALandmarkCloudPolydataFilter::GetCentroid(double x[3]) const
{
  int n = GetNumberOfLandmarks() ;
  if (n == 0)
    return false ;

  for (int j = 0 ;  j < 3 ;  j++)
    x[j] = 0.0 ;

  for (int i = 0 ;  i < n ;  i++){
    double pos[3] ;
    GetPosition(i, pos) ;
    for (int j = 0 ;  j < 3 ;  j++)
      x[j] += pos[j] ;
  }

  for (int j = 0 ;  j < 3 ;  j++)
    x[j] /= (double)n ;

  return true ;
}

