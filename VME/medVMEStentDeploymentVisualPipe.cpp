/*=========================================================================
Program:   MAF2Medical
Module:    $RCSfile: medVMEStentDeploymentVisualPipe.cpp,v $
Language:  C++
Date:      $Date: 2012-10-23 10:15:31 $
Version:   $Revision: 1.1.2.8 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2013
University of Bedfordshire UK
=========================================================================*/

#include "mafDefines.h" 

#include "medVMEStentDeploymentVisualPipe.h"

#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkPolyDataMapper.h"
#include "vtkCamera.h"
#include "vtkDepthSortPolyData.h"
#include "vtkTubeFilter.h"
#include "vtkIdList.h"



//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
medVMEStentDeploymentVisualPipe::medVMEStentDeploymentVisualPipe(vtkRenderer *renderer)
  : m_Renderer(renderer), m_FirstCall(true), m_Visibility(1),    
  m_DefinedCatheter(0), m_CatheterPolydata(NULL), m_VisibilityCatheter(0),
  m_DefinedCenterLine(0), m_CenterLinePolydata(NULL), m_VisibilityCenterLine(0),
  m_DefinedVessel(0), m_VesselPolydata(NULL), m_VisibilityVessel(0),
  m_DefinedStent(0), m_StentPolydata(NULL), m_VisibilityStent(0), m_StentTubeRadius(0.06),
  m_VesselOpacity(0.2)
{
  // catheter pipeline
  m_CatheterDepthSort = vtkDepthSortPolyData::New() ;  // <--------- start of pipeline
  m_CatheterDepthSort->SetCamera(m_Renderer->GetActiveCamera()) ;

  m_CatheterMapper = vtkPolyDataMapper::New() ;
  m_CatheterMapper->SetInput(m_CatheterDepthSort->GetOutput()) ;
  m_CatheterActor = vtkActor::New() ;
  m_CatheterActor->SetMapper(m_CatheterMapper) ;
  m_CatheterActor->GetProperty()->SetColor(1, 0.4, 1) ;
  m_CatheterActor->GetProperty()->SetOpacity(0.7) ;
  m_CatheterActor->SetVisibility(0) ;
  m_Renderer->AddActor(m_CatheterActor) ;


  // center line pipeline
  m_CenterLineMapper = vtkPolyDataMapper::New() ;  // <--------- start of pipeline
  m_CenterLineActor = vtkActor::New() ;
  m_CenterLineActor->SetMapper(m_CenterLineMapper) ;
  m_CenterLineActor->GetProperty()->SetColor(1, 1, 1) ;
  m_CenterLineActor->SetVisibility(0) ;
  m_Renderer->AddActor(m_CenterLineActor) ;


  // vessel pipeline
  m_VesselDepthSort = vtkDepthSortPolyData::New() ;  // <--------- start of pipeline
  m_VesselDepthSort->SetCamera(m_Renderer->GetActiveCamera()) ;

  m_VesselMapper = vtkPolyDataMapper::New() ;
  m_VesselMapper->SetInput(m_VesselDepthSort->GetOutput()) ;
  m_VesselMapper->SetScalarModeToUsePointData() ;
  m_VesselMapper->ScalarVisibilityOn() ;

  m_VesselActor = vtkActor::New() ;
  m_VesselActor->SetMapper(m_VesselMapper) ;
  m_VesselActor->GetProperty()->SetColor(0, 1, 0) ;
  m_VesselActor->GetProperty()->SetOpacity(m_VesselOpacity) ;
  m_VesselActor->SetVisibility(0) ;
  m_Renderer->AddActor(m_VesselActor) ;


  // stent pipeline
  m_StentTubeFilter = vtkTubeFilter::New() ;  // <--------- start of pipeline
  m_StentTubeFilter->SetNumberOfSides(12) ;
  m_StentTubeFilter->SetRadius(m_StentTubeRadius) ;

  m_StentMapper = vtkPolyDataMapper::New() ;
  m_StentMapper->SetInput(m_StentTubeFilter->GetOutput()) ;
  m_StentActor = vtkActor::New() ;
  m_StentActor->SetMapper(m_StentMapper) ;
  m_StentActor->GetProperty()->SetColor(1, 1, 1) ;
  m_StentActor->SetVisibility(0) ;
  m_Renderer->AddActor(m_StentActor) ;


  // simplex pipeline
  m_SimplexTubeFilter = vtkTubeFilter::New() ;  // <--------- start of pipeline
  m_SimplexTubeFilter->SetNumberOfSides(12) ;
  m_SimplexTubeFilter->SetRadius(m_StentTubeRadius/3.0) ;

  m_SimplexMapper = vtkPolyDataMapper::New() ;
  m_SimplexMapper->SetInput(m_SimplexTubeFilter->GetOutput()) ;
  m_SimplexActor = vtkActor::New() ;
  m_SimplexActor->SetMapper(m_SimplexMapper) ;
  m_SimplexActor->GetProperty()->SetColor(1, 0, 0) ;
  m_SimplexActor->SetVisibility(0) ;
  m_Renderer->AddActor(m_SimplexActor) ;

}



//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
medVMEStentDeploymentVisualPipe::~medVMEStentDeploymentVisualPipe()
{
  // catheter
  m_CatheterDepthSort->Delete() ;
  m_CatheterMapper->Delete() ;
  m_CatheterActor->Delete() ;

  // center line
  m_CenterLineMapper->Delete() ;
  m_CenterLineActor->Delete() ;

  // vessel
  m_VesselDepthSort->Delete() ;
  m_VesselMapper->Delete() ;
  m_VesselActor->Delete() ;

  // stent
  m_StentTubeFilter->Delete() ;
  m_StentMapper->Delete() ;
  m_StentActor->Delete() ;

  // simplex
  m_SimplexTubeFilter->Delete() ;
  m_SimplexMapper->Delete() ;
  m_SimplexActor->Delete() ;
}



//------------------------------------------------------------------------------
// Update visibility
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::UpdateVisibility()
{
  m_CatheterActor->SetVisibility(m_VisibilityCatheter && m_Visibility) ;
  m_CenterLineActor->SetVisibility(m_VisibilityCenterLine && m_Visibility) ;
  m_VesselActor->SetVisibility(m_VisibilityVessel && m_Visibility) ;
  m_StentActor->SetVisibility(m_VisibilityStent && m_Visibility) ;
  m_SimplexActor->SetVisibility(m_VisibilitySimplex && m_Visibility) ;
}



//------------------------------------------------------------------------------
// Set visibility
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::SetVisibility(int visibility)
{
  m_Visibility = visibility ;
  UpdateVisibility() ;
}



//------------------------------------------------------------------------------
// Set the catheter
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::SetCatheter(vtkPolyData *catheter)
{
  // input pipeline
  m_CatheterPolydata = catheter ;
  m_CatheterDepthSort->SetInput(m_CatheterPolydata) ;

  m_DefinedCatheter = 1 ;
  m_VisibilityCatheter = 1 ;
  UpdateVisibility() ;
}



//------------------------------------------------------------------------------
// Show catheter
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::ShowCatheter(int visibility)
{
  if (m_DefinedCatheter){
    m_VisibilityCatheter = visibility ;
    UpdateVisibility() ;
  }
}



//------------------------------------------------------------------------------
// Set the center line
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::SetCenterLine(vtkPolyData *centerLine)
{
  // input pipeline
  m_CenterLinePolydata = centerLine ;
  m_CenterLineMapper->SetInput(m_CenterLinePolydata) ;

  m_DefinedCenterLine = 1 ;
  m_VisibilityCenterLine = 1 ;
  UpdateVisibility() ;
}



//------------------------------------------------------------------------------
// Show center line
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::ShowCenterLine(int visibility)
{
  if (m_DefinedCenterLine){
    m_VisibilityCenterLine = visibility ;
    UpdateVisibility() ;
  }
}




//------------------------------------------------------------------------------
// Set the vessel
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::SetVessel(vtkPolyData *vessel)
{
  // input pipeline
  m_VesselPolydata = vessel ;
  m_VesselDepthSort->SetInput(m_VesselPolydata) ;

  m_DefinedVessel = 1 ;
  m_VisibilityVessel = 1 ;
  UpdateVisibility() ;
}



//------------------------------------------------------------------------------
// Show vessel
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::ShowVessel(int visibility)
{
  if (m_DefinedVessel){
    m_VisibilityVessel = visibility ;
    UpdateVisibility() ;
  }
}


//------------------------------------------------------------------------------
// Set the stent
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::SetStent(vtkPolyData *stent)
{
  // input pipeline
  m_StentPolydata = stent ;
  m_StentTubeFilter->SetInput(m_StentPolydata) ;

  m_DefinedStent = 1 ;
  m_VisibilityStent = 1 ;
  UpdateVisibility() ;
}



//------------------------------------------------------------------------------
// Show stent
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::ShowStent(int visibility)
{
  if (m_DefinedStent){
    m_VisibilityStent = visibility ;
    UpdateVisibility() ;
  }
}



//------------------------------------------------------------------------------
// Set the stent tube Radius
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::SetStentTubeRadius(double rad)
{
  m_StentTubeRadius = rad ;
  m_StentTubeFilter->SetRadius(rad) ;
  Render() ;
}




//------------------------------------------------------------------------------
// Set the stent
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::SetSimplex(vtkPolyData *stent)
{
  // input pipeline
  m_SimplexPolydata = stent ;
  m_SimplexTubeFilter->SetInput(m_SimplexPolydata) ;

  m_DefinedSimplex = 1 ;
  m_VisibilitySimplex = 1 ;
  UpdateVisibility() ;
}



//------------------------------------------------------------------------------
// Show stent
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::ShowSimplex(int visibility)
{
  if (m_DefinedSimplex){
    m_VisibilitySimplex = visibility ;
    UpdateVisibility() ;
  }
}



//------------------------------------------------------------------------------
// Set vessel wireframe on
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::SetWireframeOn()
{
  m_VesselActor->GetProperty()->SetRepresentationToWireframe() ;
  m_VesselActor->GetProperty()->SetOpacity(1.0) ;
}



//------------------------------------------------------------------------------
// Set vessel wireframe off
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::SetWireframeOff()
{
  m_VesselActor->GetProperty()->SetRepresentationToSurface() ;
  m_VesselActor->GetProperty()->SetOpacity(m_VesselOpacity) ;
}



//-------------------------------------------------------------------------
// Get the highest index of the valid points,
// ie points which are members of cells.
/// Needed because the stent contains unused and undefined points.
//-------------------------------------------------------------------------
int medVMEStentDeploymentVisualPipe::GetHighestValidPointIndex(vtkPolyData *pd) const
{
  vtkIdList *ptIds = vtkIdList::New() ;
  int ncells = pd->GetNumberOfCells() ;
  int idLast = -1 ;
  for (int i = 0 ;  i < ncells ;  i++){
    pd->GetCellPoints(i, ptIds) ;
    for (int j = 0 ;  j < ptIds->GetNumberOfIds() ;  j++){
      int id = ptIds->GetId(j) ;
      if (id > idLast)
        idLast = id ;
    }
  }
  ptIds->Delete() ;
  return idLast ;
}



//------------------------------------------------------------------------------
// Reset the camera position
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::ResetCameraPosition()
{
  // select available polydata for setting the camera
  vtkPolyData *selectedPolydata ;
  vtkActor *selectedActor ;
  if (m_DefinedStent){
    selectedPolydata = m_StentPolydata ;
    selectedActor = m_StentActor ;
  }
  else if (m_DefinedVessel){
    selectedPolydata = m_VesselPolydata ;
    selectedActor = m_VesselActor ;
  }
  else if (m_DefinedCatheter){
    selectedPolydata = m_CatheterPolydata ;
    selectedActor = m_CatheterActor ;
  }
  else if (m_DefinedCenterLine){
    selectedPolydata = m_CenterLinePolydata ;
    selectedActor = m_CenterLineActor ;
  }
  else
    return ;

  // get bounds, center and size
  double bounds[6], siz[3], *center ;
  selectedActor->GetBounds(bounds) ;
  center = selectedActor->GetCenter() ;
  siz[0] = bounds[1] - bounds[0] ;
  siz[1] = bounds[3] - bounds[2] ;
  siz[2] = bounds[5] - bounds[4] ;

  // save center position
  for (int j = 0 ;  j < 3 ;  j++)
    m_CenterOld[j] = center[j] ;

  // find longest axis
  int longestAxis ;
  if ((siz[0] >= siz[1]) && (siz[0] >= siz[2]))
    longestAxis = 0 ;
  else if (siz[1] >= siz[2])
    longestAxis = 1 ;
  else
    longestAxis = 2 ;

  // find whether item points left or right
  int direc ;
  double x0[3], xLast[3] ;
  int idLast = GetHighestValidPointIndex(selectedPolydata) ;
  selectedPolydata->GetPoint(0, x0) ;
  selectedPolydata->GetPoint(idLast, xLast) ;
  if (x0[longestAxis] < xLast[longestAxis])
    direc = 1 ;
  else
    direc = -1 ;

  double focalPt[3] ;
  for (int j = 0 ;  j < 3 ;  j++)
    focalPt[j] = 0.9*x0[j] + 0.1*xLast[j] ;

  m_Renderer->GetActiveCamera()->SetFocalPoint(focalPt) ;

  // This sets the camera view so that the longest axis is horizontal
  double r = direc*siz[longestAxis] ;
  switch(longestAxis){
  case 0:
    m_Renderer->GetActiveCamera()->SetPosition(focalPt[0], focalPt[1], focalPt[2]+r) ;
    m_Renderer->GetActiveCamera()->SetViewUp(0,1,0) ;
    break ;
  case 1:
    m_Renderer->GetActiveCamera()->SetPosition(focalPt[0]+r, focalPt[1], focalPt[2]) ;
    m_Renderer->GetActiveCamera()->SetViewUp(0,0,1) ;
    break ;
  case 2:
    m_Renderer->GetActiveCamera()->SetPosition(focalPt[0]-r, focalPt[1], focalPt[2]) ;
    m_Renderer->GetActiveCamera()->SetViewUp(0,1,0) ;
    break ;
  }

  m_Renderer->ResetCameraClippingRange() ;
}



//------------------------------------------------------------------------------
// Reset the camera focal point
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::ResetCameraFocalPoint()
{
  // select available polydata for setting the camera
  vtkPolyData *selectedPolydata ;
  vtkActor *selectedActor ;
  if (m_DefinedStent){
    selectedPolydata = m_StentPolydata ;
    selectedActor = m_StentActor ;
  }
  else if (m_DefinedVessel){
    selectedPolydata = m_VesselPolydata ;
    selectedActor = m_VesselActor ;
  }
  else if (m_DefinedCatheter){
    selectedPolydata = m_CatheterPolydata ;
    selectedActor = m_CatheterActor ;
  }
  else if (m_DefinedCenterLine){
    selectedPolydata = m_CenterLinePolydata ;
    selectedActor = m_CenterLineActor ;
  }
  else
    return ;

  // move camera focal point and position by the change in center position
  double focalPt[3], cameraPos[3] ;
  double *center = selectedActor->GetCenter() ;
  m_Renderer->GetActiveCamera()->GetFocalPoint(focalPt) ;
  m_Renderer->GetActiveCamera()->GetPosition(cameraPos) ;
  for (int j = 0 ;  j < 3 ;  j++){
    focalPt[j] += 0.9*(center[j] - m_CenterOld[j]) ;
    cameraPos[j] += 0.9*(center[j] - m_CenterOld[j]) ;
  }
  m_Renderer->GetActiveCamera()->SetFocalPoint(focalPt) ;
  m_Renderer->GetActiveCamera()->SetPosition(cameraPos) ;

  m_Renderer->ResetCameraClippingRange() ;

  // save center position
  for (int j = 0 ;  j < 3 ;  j++)
    m_CenterOld[j] = center[j] ;
}




//------------------------------------------------------------------------------
// Update and render
//------------------------------------------------------------------------------
void medVMEStentDeploymentVisualPipe::Render() 
{
  if (m_DefinedCatheter)
    m_CatheterPolydata->Update() ;
  if (m_DefinedCenterLine)
    m_CenterLinePolydata->Update() ;
  if (m_DefinedVessel)
    m_VesselPolydata->Update() ;
  if (m_DefinedStent)
    m_StentPolydata->Update() ;

  m_Renderer->GetRenderWindow()->Render() ;

}


