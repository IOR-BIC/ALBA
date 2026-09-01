/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMML3ModelView
 Authors: Mel Krokos, Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h"
#include "albaSmartPointer.h"

#include "albaOpMML3ModelView.h"
#include "albaOpMML3ModelView2DPipe.h"
#include "albaOpMML3ModelView3DPipe.h"
#include "vtkALBAMatrixVectorMath.h"

#include "vtkMath.h"
#include "vtkInteractorStyleImage.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkCamera.h"

#include <ostream>
#include <vector>
#include <algorithm>




//------------------------------------------------------------------------------
// Constructor for model view
//------------------------------------------------------------------------------
albaOpMML3ModelView::albaOpMML3ModelView( vtkRenderWindow *rw, vtkRenderer *ren, vtkPolyData *muscleIn, vtkPolyData *muscleOut,
                                       vtkDataSet* volume, int numberOfScans)
: m_RenderWindow(rw), m_Renderer(ren), m_MuscleInput(muscleIn),  m_MuscleOutput(muscleOut),
m_Scans(volume), m_NumberOfScans(numberOfScans), m_Landmarks4(0), m_ScalingOccured(false), m_ScansGrain(1)
{ 
  m_Math = vtkALBAMatrixVectorMath::New() ;

  //----------------------------------------------------------------------------
  // Set up slices and their pose matrices
  //----------------------------------------------------------------------------
  m_Alpha = new double[m_NumberOfScans] ;
  m_Zeta = new double[m_NumberOfScans] ;
  m_SlicePositions = new double[m_NumberOfScans][3] ;
  m_SliceNormals = new double[m_NumberOfScans][3] ;
  m_OriginalContourCenters = new double[m_NumberOfScans][3] ;
  m_OriginalContourBounds = new double[m_NumberOfScans][6] ;

  m_SlicePoseMat = new (vtkMatrix4x4 * [m_NumberOfScans]) ;
  for (int i = 0 ;  i < m_NumberOfScans ;  i++)
    m_SlicePoseMat[i] = vtkMatrix4x4::New() ;

  m_SlicePoseInvMat = new (vtkMatrix4x4 * [m_NumberOfScans]) ;
  for (int i = 0 ;  i < m_NumberOfScans ;  i++)
    m_SlicePoseInvMat[i] = vtkMatrix4x4::New() ;


  //----------------------------------------------------------------------------
  // Create visual pipes
  //----------------------------------------------------------------------------
  m_VisualPipe2D = new albaOpMML3ModelView2DPipe(ren, muscleIn, volume, m_NumberOfScans) ;
  m_VisualPipe3D = new albaOpMML3ModelView3DPipe(ren, muscleIn, volume, m_NumberOfScans) ;
  m_VisualPipePreview = new albaOpMML3ModelView3DPipe(ren, muscleOut, volume, m_NumberOfScans) ;

  


  //----------------------------------------------------------------------------
  // Set style and select 2d or 3d
  //----------------------------------------------------------------------------

  int *size = m_RenderWindow->GetSize();
  assert(size[0] > 0);
  assert(size[1] > 0);

  m_RenderWindow->LineSmoothingOn();

  m_Style2D = vtkInteractorStyleImage::New() ;
  m_Style3D = vtkInteractorStyleTrackballCamera::New() ;

  SetDisplay2D() ;



  //----------------------------------------------------------------------------
  // Set up spline functions.
  // NB The center op splines are not set by user interaction, nor displayed.
  //----------------------------------------------------------------------------
  m_PlaceHorizontalOffsetSpline = vtkKochanekSpline::New();
  m_PlaceHorizontalOffsetSpline->ClosedOff();

  m_PlaceVerticalOffsetSpline = vtkKochanekSpline::New();
  m_PlaceVerticalOffsetSpline->ClosedOff();

  m_TwistSpline = vtkKochanekSpline::New();
  m_TwistSpline->ClosedOff();

  m_HorizontalTranslationSpline = vtkKochanekSpline::New();
  m_HorizontalTranslationSpline->ClosedOff();

  m_VerticalTranslationSpline = vtkKochanekSpline::New();
  m_VerticalTranslationSpline->ClosedOff();

  m_NorthScalingSpline = vtkKochanekSpline::New();
  m_NorthScalingSpline->ClosedOff();
 
  m_SouthScalingSpline = vtkKochanekSpline::New();
  m_SouthScalingSpline->ClosedOff();

  m_EastScalingSpline = vtkKochanekSpline::New();
  m_EastScalingSpline->ClosedOff();

  m_WestScalingSpline = vtkKochanekSpline::New();
  m_WestScalingSpline->ClosedOff();

  m_CenterHorizontalOffsetSpline = vtkKochanekSpline::New();
  m_CenterHorizontalOffsetSpline->ClosedOff();

  m_CenterVerticalOffsetSpline = vtkKochanekSpline::New();
  m_CenterVerticalOffsetSpline->ClosedOff();




  //----------------------------------------------------------------------------
  // Stack
  //----------------------------------------------------------------------------
  m_ScalingFlagStack = vtkIntArray::New();
  m_ScalingFlagStack->SetNumberOfComponents(1);

  // allocate the operations stack, used for undo purposes
  AllocateOperationsStack(5, 2000) ;
}




//----------------------------------------------------------------------------
// Destructor
albaOpMML3ModelView::~albaOpMML3ModelView()
//----------------------------------------------------------------------------
{
  m_Math->Delete() ;

  // slice positions etc
  delete [] m_Alpha ;
  delete [] m_Zeta ;
  delete [] m_SlicePositions ;
  delete [] m_SliceNormals ;
  delete [] m_OriginalContourCenters ;
  delete [] m_OriginalContourBounds ;

  for (int i = 0 ;  i < m_NumberOfScans ;  i++)
    m_SlicePoseMat[i]->Delete() ;
  delete [] m_SlicePoseMat ;

  for (int i = 0 ;  i < m_NumberOfScans ;  i++)
    m_SlicePoseInvMat[i]->Delete() ;
  delete [] m_SlicePoseInvMat ;


  // visual pipes and styles
  delete m_VisualPipe2D ;
  delete m_VisualPipe3D ;
  delete m_VisualPipePreview ;
  m_Style2D->Delete() ;
  m_Style3D->Delete() ;


  // splines
  m_PlaceHorizontalOffsetSpline->Delete() ;
  m_PlaceVerticalOffsetSpline->Delete() ;
  m_TwistSpline->Delete() ;
  m_HorizontalTranslationSpline->Delete() ;
  m_VerticalTranslationSpline->Delete() ;
  m_NorthScalingSpline->Delete() ;
  m_SouthScalingSpline->Delete() ;
  m_EastScalingSpline->Delete() ;
  m_WestScalingSpline->Delete() ;
  m_CenterHorizontalOffsetSpline->Delete() ;
  m_CenterVerticalOffsetSpline->Delete() ;

 
  // scaling stack
  m_ScalingFlagStack->Delete() ;


  DeleteOperationsStack() ;

}




//------------------------------------------------------------------------------
// initialize after input parameters have been set
void albaOpMML3ModelView::Initialize()
//------------------------------------------------------------------------------
{
  // Get position, orientation, size and resolution of scans
  // These methods only set member variables and do not put any results in the visual pipes !
  CalculatePositionsOfScans() ;
  CalculateNormalsOfScans() ;
  CalculatePoseMatricesOfScans() ;
  FindSizeAndResolutionOfScans();


  // transfer landmarks to 3d pipe
  m_VisualPipe3D->AddLandmark(m_PatientLandmark1) ;
  m_VisualPipe3D->AddLandmark(m_PatientLandmark2) ;
  m_VisualPipe3D->AddLandmark(m_PatientLandmark3) ;
  if (m_Landmarks4)
    m_VisualPipe3D->AddLandmark(m_PatientLandmark4) ;
  m_VisualPipe3D->AddAxisLandmark(m_AxisLandmark1) ;
  m_VisualPipe3D->AddAxisLandmark(m_AxisLandmark2) ;
  if (m_MuscleType == 2)
    m_VisualPipe3D->AddAxisLandmark(m_AxisLandmark3) ;


  // transfer landmarks to preview pipe
  m_VisualPipePreview->AddLandmark(m_PatientLandmark1) ;
  m_VisualPipePreview->AddLandmark(m_PatientLandmark2) ;
  m_VisualPipePreview->AddLandmark(m_PatientLandmark3) ;
  if (m_Landmarks4)
    m_VisualPipePreview->AddLandmark(m_PatientLandmark4) ;
  m_VisualPipePreview->AddAxisLandmark(m_AxisLandmark1) ;
  m_VisualPipePreview->AddAxisLandmark(m_AxisLandmark2) ;
  if (m_MuscleType == 2)
    m_VisualPipePreview->AddAxisLandmark(m_AxisLandmark3) ;



  // transfer slice parameters to the visual pipes
  m_VisualPipe2D->SetSliceSize(m_ScanSizeX, m_ScanSizeY) ;
  m_VisualPipe2D->SetSliceResolution(m_ScansResolutionX, m_ScansResolutionY) ;
  for (int i = 0 ;  i < m_NumberOfScans ;  i++)
    m_VisualPipe2D->SetSliceTransform(i, m_SlicePoseMat[i]) ;

  m_VisualPipe3D->SetSliceSize(m_ScanSizeX, m_ScanSizeY) ;
  m_VisualPipe3D->SetSliceResolution(m_ScansResolutionX/4, m_ScansResolutionY/4) ;
  for (int i = 0 ;  i < m_NumberOfScans ;  i++)
    m_VisualPipe3D->SetSliceTransform(i, m_SlicePoseMat[i]) ;

  m_VisualPipePreview->SetSliceSize(m_ScanSizeX, m_ScanSizeY) ;
  m_VisualPipePreview->SetSliceResolution(m_ScansResolutionX/4, m_ScansResolutionY/4) ;
  for (int i = 0 ;  i < m_NumberOfScans ;  i++)
    m_VisualPipePreview->SetSliceTransform(i, m_SlicePoseMat[i]) ;



  // set slice id
  SetCurrentIdOfScans(0) ;

  // pre-process the slices
  //m_VisualPipe2D->UpdateAllSlices() ;
  //m_VisualPipe3D->UpdateAllSlices() ;
  //m_VisualPipePreview->UpdateAllSlices() ;

  // update visual pipes
  m_VisualPipe2D->Update() ;
  m_VisualPipe3D->Update() ;
  m_VisualPipePreview->Update() ;


  // Calculate the centers and bounds of the contours before any transformations take place
  // and while the default identity transforms are still valid.
  CalculateOriginalContourCenters() ;

  // The contour centers affect the transforms, so we need to update again.
  Update() ;

  
  // initialize camera and render
  GetRenderer()->ResetCamera(); // must do this before changing the camera settings
  this->ResetCameraPosition() ;


  // copy the input muscle to the output
  m_MuscleOutput->DeepCopy(m_MuscleInput) ;


  // initialize scaling flag stack
  m_ScalingFlagStack->SetNumberOfTuples(m_NumberOfScans);
  float a = 0;
  for(int ii = 0 ;  ii < m_NumberOfScans ;  ii++)
    m_ScalingFlagStack->SetTuple(ii, &a);

}





//------------------------------------------------------------------------------
// Set display to 2d visual pipe
void albaOpMML3ModelView::SetDisplay2D()
//------------------------------------------------------------------------------
{
  m_Display3D = 0 ;
  m_Renderer->GetRenderWindow()->GetInteractor()->SetInteractorStyle(m_Style2D) ;
  m_VisualPipe2D->SetVisibility(1) ;
  m_VisualPipe3D->SetVisibility(0) ;
  m_VisualPipePreview->SetVisibility(0) ;
  ResetCameraPosition() ;
  Render() ;
}



//------------------------------------------------------------------------------
// Set display to 3d visual pipe
void albaOpMML3ModelView::SetDisplay3D()
//------------------------------------------------------------------------------
{
  m_Display3D = 1 ;
  m_Renderer->GetRenderWindow()->GetInteractor()->SetInteractorStyle(m_Style3D) ;
  m_VisualPipe2D->SetVisibility(0) ;
  m_VisualPipe3D->SetVisibility(1) ;
  m_VisualPipePreview->SetVisibility(0) ;
  ResetCameraPosition() ;
  Render() ;
}


//------------------------------------------------------------------------------
// Set display to preview visual pipe
void albaOpMML3ModelView::SetDisplayToPreview()
//------------------------------------------------------------------------------
{
  m_Display3D = 2 ;

  // apply registration ops to update the output muscle
  ApplyRegistrationOps() ;

  m_Renderer->GetRenderWindow()->GetInteractor()->SetInteractorStyle(m_Style3D) ;
  m_VisualPipe2D->SetVisibility(0) ;
  m_VisualPipe3D->SetVisibility(0) ;
  m_VisualPipePreview->SetVisibility(1) ;
  ResetCameraPosition() ;
  Render() ;
}



//------------------------------------------------------------------------------
// reset the camera position
void albaOpMML3ModelView::ResetCameraPosition()
//------------------------------------------------------------------------------
{
  double maxSize = std::max(m_ScanSizeX, m_ScanSizeY) ;
  double r = 2*maxSize ;

  if (m_Display3D == 0){
    // if 2d, set camera to point at (0,0,0) along z axis
    GetRenderer()->GetActiveCamera()->SetFocalPoint(0,0,0) ;
    GetRenderer()->GetActiveCamera()->SetPosition(0, 0, r) ;
    GetRenderer()->GetActiveCamera()->SetViewUp(0,1,0) ;
  }
  else{
    // if 3d or preview, set to a diagonal viewing position
    double bnds[6], center[3] ;
    m_VisualPipe3D->GetContourBounds(bnds) ;
    m_VisualPipe3D->GetContourCenter(center) ;
    GetRenderer()->GetActiveCamera()->SetFocalPoint(center) ;
    GetRenderer()->GetActiveCamera()->SetPosition(r, r, r) ;
    GetRenderer()->GetActiveCamera()->SetViewUp(-1,-1,2) ;
  }

  GetRenderer()->ResetCameraClippingRange();

}



//------------------------------------------------------------------------------
// set the current scan id
void albaOpMML3ModelView::SetCurrentIdOfScans(int i) 
//------------------------------------------------------------------------------
{
  m_ScansCurrentId = i ;
  m_VisualPipe2D->SetCurrentSliceId(i) ;
  m_VisualPipe3D->SetCurrentSliceId(i) ;
  m_VisualPipePreview->SetCurrentSliceId(i) ;
}





//------------------------------------------------------------------------------
// Calculate size and resolution from user-defined grain
// Must be called prior to CreateScans
//
// Size is determined by the bounds of muscle in slice coords
//
// Resolution is the no. of cells when the polydata slice is created from vtkPlaneSource().
// The resolution is a fraction of the image size, which is calculated from the grain
void albaOpMML3ModelView::FindSizeAndResolutionOfScans()
//------------------------------------------------------------------------------
{
  double Factor = 1.1 ; // 10% extra

  //----------------------------------------------------------------------------
  // Get the bounds of the muscle in slice coords
  // Can't get it from the visual pipe because it might not be set up yet
  //----------------------------------------------------------------------------
  vtkTransform *tr = vtkTransform::New() ;
  tr->SetMatrix(m_SlicePoseInvMat[0]) ;

  vtkTransformPolyDataFilter *tpdf = vtkTransformPolyDataFilter::New() ;
  tpdf->SetInputData(m_MuscleInput) ;
  tpdf->SetTransform(tr) ;

  double bounds[6];
  tpdf->GetOutput()->GetBounds(bounds) ;

  tr->Delete() ;
  tpdf->Delete() ;



  // set size to (x,y) bounds of muscle
  m_ScanSizeX = Factor * fabs(bounds[1]-bounds[0]) ;
  m_ScanSizeY = Factor * fabs(bounds[3]-bounds[2]) ;


  // set resolution to that requested by user.
  // resolution is no. of quads in the polydata probe.
  double aspectRatio = m_ScanSizeY / m_ScanSizeX ;

  if (m_ScanSizeX > m_ScanSizeY){
    m_ScansResolutionX = m_ScansGrain ;
    m_ScansResolutionY = (int)((double)m_ScansGrain * aspectRatio) ;
  }
  else if (m_ScanSizeX < m_ScanSizeY){
    m_ScansResolutionX = (int)((double)m_ScansGrain / aspectRatio) ;
    m_ScansResolutionY = m_ScansGrain ;
  }
  else{
    m_ScansResolutionX = m_ScansGrain ;
    m_ScansResolutionY = m_ScansGrain ;
  }
}




//----------------------------------------------------------------------------
// Calculate total length of axis or axes
double albaOpMML3ModelView::LengthOfAxis() const
//----------------------------------------------------------------------------
{
  double pstart[3] ; // low insertion
  double pmid[3] ; // mid landmark if two-part axis
  double pend[3] ; // high insertion

  double lenTotal, len12, len23 ;

  int n = GetTotalNumberOfScans();

  vtkMath *pMath = vtkMath::New();


  switch(m_MuscleType){
    case 1:
      // single axis
      GetLandmark1OfAxis(pstart);
      GetLandmark2OfAxis(pend);

      lenTotal = sqrt(pMath->Distance2BetweenPoints(pstart, pend));

      break ;

    case 2:
      // two-part axis
      GetLandmark1OfAxis(pstart);
      GetLandmark2OfAxis(pmid);
      GetLandmark3OfAxis(pend);

      len12 = sqrt(pMath->Distance2BetweenPoints(pstart, pmid));
      len23 = sqrt(pMath->Distance2BetweenPoints(pmid, pend));
      lenTotal = len12 + len23 ;

      break ;

    default:
      // unknown type
      assert(false) ;
  }

  pMath->Delete() ;

  return lenTotal ;
}




//------------------------------------------------------------------------------
// set positions (fractional and actual) of scans along axis
void albaOpMML3ModelView::SetFractionalPosOfScans(double *alpha)
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < GetTotalNumberOfScans() ;  i++){
    m_Alpha[i] = alpha[i] ;
    m_Zeta[i] = alpha[i] * LengthOfAxis() ;
  }
}



//------------------------------------------------------------------------------
// calculate the positions of the scans in patient coords, given the fractional positions
// The positions are always along the axis
void albaOpMML3ModelView::CalculatePositionsOfScans()
//------------------------------------------------------------------------------
{
  double pstart[3] ; // low insertion
  double pmid[3] ;   // mid landmark if two-part axis
  double pend[3] ;   // high insertion

  int n = GetTotalNumberOfScans();

  vtkMath *pMath = vtkMath::New();

  switch(m_MuscleType){
    case 1:
      {
        // single axis
        GetLandmark1OfAxis(pstart);
        GetLandmark2OfAxis(pend);

        for (int i = 0 ;  i < n ;  i++){
          for (int j = 0 ;  j < 3 ;  j++){
            double f = m_Alpha[i] ;
            m_SlicePositions[i][j] = (1.0 - f)*pstart[j] + f*pend[j] ;
          }
        }

        break ;
      }

    case 2:
      {
        // two-part axis
        GetLandmark1OfAxis(pstart);
        GetLandmark2OfAxis(pmid);
        GetLandmark3OfAxis(pend);

        // calculate the fractional distance of pmid, the breakpoint in the axes
        // so we can know which part a given f is on
        double len12 = sqrt(pMath->Distance2BetweenPoints(pstart, pmid));
        double len23 = sqrt(pMath->Distance2BetweenPoints(pmid, pend));
        double lenTotal = len12 + len23 ; 
        double fmid = len12/lenTotal ;

        // expensive calculation so save result for future use
        m_AlphaMidPoint = fmid ;
        m_ZetaMidPoint = fmid*LengthOfAxis() ;

        // calculate the positions
        for (int i = 0 ;  i < n ;  i++){
          double f = m_Alpha[i] ;
          if (f <= fmid){
            // pos goes from pstart to pmid as f goes from 0 to fmid
            for (int j = 0 ;  j < 3 ;  j++)
              m_SlicePositions[i][j] = (1.0 - f/fmid)*pstart[j] + (f/fmid)*pmid[j] ;
          }
          else{
            // pos goes from pmid to pend as f goes from fmid to 1
            for (int j = 0 ;  j < 3 ;  j++)
              m_SlicePositions[i][j] = ((1.0-f)/(1-fmid))*pmid[j] + ((f-fmid)/(1.0-fmid))*pend[j] ;
          }
        }

        break ;
      }

    default:
      // unknown type
      assert(false) ;
  }

  pMath->Delete() ;
}



//------------------------------------------------------------------------------
// calculate the normals of the scans
void albaOpMML3ModelView::CalculateNormalsOfScans()
//------------------------------------------------------------------------------
{
  double pstart[3] ; // low insertion
  double pmid[3] ;   // mid landmark if two-part axis
  double pend[3] ;   // high insertion

  int n = GetTotalNumberOfScans();

  vtkMath *pMath = vtkMath::New();

  switch(m_MuscleType){
    case 1:
      {
        // single axis
        GetLandmark1OfAxis(pstart);
        GetLandmark2OfAxis(pend);

        // get axis direction
        double direc[3] ;
        for (int j = 0 ;  j < 3 ;  j++)
          direc[j] = pend[j] - pstart[j] ;
        pMath->Normalize(direc) ;

        // copy axis direction to all normals
        for (int i = 0 ;  i < n ;  i++)
          for (int j = 0 ;  j < 3 ;  j++)
            m_SliceNormals[i][j] = direc[j] ;

        break ;
      }

    case 2:
      {
        // two-part axis
        GetLandmark1OfAxis(pstart);
        GetLandmark2OfAxis(pmid);
        GetLandmark3OfAxis(pend);

        // get axis directions
        double direc12[3], direc23[3], direcMid[3] ;
        for (int j = 0 ;  j < 3 ;  j++){
          direc12[j] = pmid[j] - pstart[j] ;
          direc23[j] = pend[j] - pmid[j] ;
          direcMid[j] = (direc12[j] + direc23[j]) / 2.0 ; // mean direction at pmid
        }
        pMath->Normalize(direc12) ;
        pMath->Normalize(direc23) ;
        pMath->Normalize(direcMid) ;

        // expensive calculation so save results for future use
        for (int j = 0 ;  j < 3 ;  j++){
          m_NormalStart[j] = direc12[j] ;
          m_NormalMidPoint[j] = direcMid[j] ;
          m_NormalEnd[j] = direc23[j] ;
        }

        // calculate the fractional distance of pmid, the breakpoint in the axes
        // so we can know which part a given f is on
        double len12 = sqrt(pMath->Distance2BetweenPoints(pstart, pmid));
        double len23 = sqrt(pMath->Distance2BetweenPoints(pmid, pend));
        double lenTotal = len12 + len23 ; 
        double fmid = len12/lenTotal ;

        // calculate the normals
        for (int i = 0 ;  i < n ;  i++){
          double f = m_Alpha[i] ;
          if (f <= fmid){
            // direction goes from direc12 to direcMid as f goes from 0 to fmid
            for (int j = 0 ;  j < 3 ;  j++)
              m_SliceNormals[i][j] = (1.0 - f/fmid)*direc12[j] + (f/fmid)*direcMid[j] ;
          }
          else{
            // direction goes from direcMid to direc23 as f goes from fmid to 1
            for (int j = 0 ;  j < 3 ;  j++)
              m_SliceNormals[i][j] = ((1.0-f)/(1-fmid))*direcMid[j] + ((f-fmid)/(1.0-fmid))*direc23[j] ;
          }
        }

        // renormalise normals
        for (int i = 0 ;  i < n ;  i++)
          pMath->Normalize(m_SliceNormals[i]) ;

        break ;
      }

    default:
      // unknown type
      assert(false) ;
  } 

  pMath->Delete() ;

}




//----------------------------------------------------------------------------
// calculate the pose matrices of the scans
void albaOpMML3ModelView::CalculatePoseMatricesOfScans()
//----------------------------------------------------------------------------
{
  for (int i = 0 ;  i < m_NumberOfScans ;  i++){
    vtkMatrix4x4 *mat = m_SlicePoseMat[i] ;
    mat->Identity() ;

    // set position
    for (int j = 0 ;  j < 3 ;  j++)
      mat->SetElement(j, 3, m_SlicePositions[i][j]) ;

    // calculate x and y axes
    double u[3], v[3] ;
    m_Math->CalculateNormalsToW(u, v, m_SliceNormals[i]) ;
    m_Math->NormalizeVector(u) ;
    m_Math->NormalizeVector(v) ;

    // set x, y and z axes
    for (int j = 0 ;  j < 3 ;  j++){
      mat->SetElement(j, 0, u[j]) ;
      mat->SetElement(j, 1, v[j]) ;
      mat->SetElement(j, 2, m_SliceNormals[i][j]) ;
    }

    // also calculate the inverse matrix
    mat->Invert(mat, m_SlicePoseInvMat[i]) ;
  }
}







//------------------------------------------------------------------------------
// Calculate and save the original centers and bounds of contours on each slice.
// Do this when everything else is set up, and before any transforms have been applied.
void albaOpMML3ModelView::CalculateOriginalContourCenters()
//------------------------------------------------------------------------------
{
  // save current slice id
  int saveId = m_ScansCurrentId ;

  for (int i = 0 ;  i < m_NumberOfScans ;  i++){
    SetCurrentIdOfScans(i) ;
    GetVisualPipe2D()->GetCurrentContourBounds(m_OriginalContourBounds[i]) ;
    GetVisualPipe2D()->CalculateRobustCenterOfContour(m_OriginalContourCenters[i]) ; // returns (0,0,0) if no data, which is ok
 
    // add value to center op spline
    // This allows us to interpolate the center between slices later
    double zeta = GetZetaOfCurrentSlice() ;
    GetCHSpline()->AddPoint(zeta, m_OriginalContourCenters[i][0]) ;
    GetCVSpline()->AddPoint(zeta, m_OriginalContourCenters[i][1]) ;
  }

  // restore slice id
  SetCurrentIdOfScans(saveId) ;
}



//------------------------------------------------------------------------------
// Get the bounds of the untransformed contour on the current slice
// Returns the value saved by CalculateOriginalContourCenters()
void albaOpMML3ModelView::GetOriginalContourBounds(double bounds[6])
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 6 ;  i++)
    bounds[i] = m_OriginalContourBounds[m_ScansCurrentId][i] ;
}


//------------------------------------------------------------------------------
// Get the center of the untransformed contour on the current slice
// Returns the value saved by CalculateOriginalContourCenters()
void albaOpMML3ModelView::GetOriginalContourCenter(double center[3])
//------------------------------------------------------------------------------
{
  for (int i = 0 ;  i < 3 ;  i++)
    center[i] = m_OriginalContourCenters[m_ScansCurrentId][i] ;
}




//----------------------------------------------------------------------------
// Get distance zeta along slice axis
double albaOpMML3ModelView::GetZetaOfSlice(int sliceId) const
//----------------------------------------------------------------------------
{
  return m_Zeta[sliceId] ;
}



//----------------------------------------------------------------------------
// Get distance zeta along slice axis
double albaOpMML3ModelView::GetZetaOfCurrentSlice() const
//----------------------------------------------------------------------------
{
  return m_Zeta[m_ScansCurrentId] ;
}






//------------------------------------------------------------------------------
// Calculate the position of the slice for an arbitrary value of zeta
// This assumes that CalculatePositionsOfScans() has been called
void albaOpMML3ModelView::CalculateSlicePosition(double zeta, double *pos) const
//------------------------------------------------------------------------------
{
  double pstart[3] ; // low insertion
  double pmid[3] ;   // mid landmark if two-part axis
  double pend[3] ;   // high insertion

  double alpha = zeta / LengthOfAxis() ;

  switch(m_MuscleType){
    case 1:
      {
        // single axis
        GetLandmark1OfAxis(pstart);
        GetLandmark2OfAxis(pend);

        for (int j = 0 ;  j < 3 ;  j++)
          pos[j] = (1.0 - alpha)*pstart[j] + alpha*pend[j] ;

        break ;
      }

    case 2:
      {
        // two-part axis
        GetLandmark1OfAxis(pstart);
        GetLandmark2OfAxis(pmid);
        GetLandmark3OfAxis(pend);

        // calculate the position
        if (alpha <= m_AlphaMidPoint){
          // pos goes from pstart to pmid as alpha goes from 0 to m_AlphaMidPoint
          for (int j = 0 ;  j < 3 ;  j++)
            pos[j] = (1.0 - alpha/m_AlphaMidPoint)*pstart[j] + (alpha/m_AlphaMidPoint)*pmid[j] ;
        }
        else{
          // pos goes from pmid to pend as alpha goes from m_AlphaMidPoint to 1
          for (int j = 0 ;  j < 3 ;  j++)
            pos[j] = ((1.0-alpha)/(1-m_AlphaMidPoint))*pmid[j] + ((alpha-m_AlphaMidPoint)/(1.0-m_AlphaMidPoint))*pend[j] ;
        }


        break ;
      }

    default:
      // unknown type
      assert(false) ;
  }
}




//------------------------------------------------------------------------------
// Calculate the normal of the slice for an arbitrary value of zeta
// This assumes that CalculateNormalsOfScans() has already been called
void albaOpMML3ModelView::CalculateSliceNormal(double zeta, double *normal) const
//------------------------------------------------------------------------------
{
  double alpha = zeta / LengthOfAxis() ;

  int n = GetTotalNumberOfScans();

  vtkMath *pMath = vtkMath::New();

  switch(m_MuscleType){
    case 1:
      {
        // single straight axis
        // all slices have same normal, so just return result for slice 0
        for (int j = 0 ;  j < 3 ;  j++)
          normal[j] = m_SliceNormals[0][j] ;

        break ;
      }

    case 2:
      {
        // two-part axis
        if (alpha <= 0.0){
          // clamp value to that at alpha = 0.0
          for (int j = 0 ;  j < 3 ;  j++)
            normal[j] = m_NormalStart[j] ;
        }
        else if (alpha <= m_AlphaMidPoint){
          // Direction goes from normalStart to normalMid as alpha goes from 0 to m_AlphaMidPoint
          // We don't use values at slice 0 because it might not be at alpha = 0.0
          for (int j = 0 ;  j < 3 ;  j++)
            normal[j] = (1.0 - alpha/m_AlphaMidPoint)*m_NormalStart[j] + (alpha/m_AlphaMidPoint)*m_NormalMidPoint[j] ;
        }
        else if (alpha < 1.0){
          // direction goes from normalMid to normalEnd as alpha goes from m_AlphaMidPoint to 1
          for (int j = 0 ;  j < 3 ;  j++)
            normal[j] = ((1.0-alpha)/(1-m_AlphaMidPoint))*m_NormalMidPoint[j] + ((alpha-m_AlphaMidPoint)/(1.0-m_AlphaMidPoint))*m_NormalEnd[j] ;
        }
        else{
          // clamp value to that at alpha = 1.0
          for (int j = 0 ;  j < 3 ;  j++)
            normal[j] = m_NormalEnd[j] ;
        }

        pMath->Normalize(normal) ;

        break ;
      }

    default:
      // unknown type
      assert(false) ;
  } 

  pMath->Delete() ;

}



//------------------------------------------------------------------------------
// calculate the pose matrix of the slice for an arbitrary value of zeta
void albaOpMML3ModelView::CalculateSlicePoseMatrix(double zeta, vtkMatrix4x4 *mat) const
//------------------------------------------------------------------------------
{
  double pos[3], normal[3] ;
  CalculateSlicePosition(zeta, pos) ;
  CalculateSliceNormal(zeta, normal) ;

  mat->Identity() ;

  // set position
  for (int j = 0 ;  j < 3 ;  j++)
    mat->SetElement(j, 3, pos[j]) ;

  // calculate x and y axes
  double u[3], v[3] ;
  m_Math->CalculateNormalsToW(u, v, normal) ;
  m_Math->NormalizeVector(u) ;
  m_Math->NormalizeVector(v) ;

  // set x, y and z axes
  for (int j = 0 ;  j < 3 ;  j++){
    mat->SetElement(j, 0, u[j]) ;
    mat->SetElement(j, 1, v[j]) ;
    mat->SetElement(j, 2, normal[j]) ;
  }
}



//------------------------------------------------------------------------------
// Calculate height of point x above plane
// the value is zero for a point in the plane.
double albaOpMML3ModelView::HeightAbovePlane(double *normal, double *origin, double *x) const
//------------------------------------------------------------------------------
{
  return (x[0]-origin[0])*normal[0] +  (x[1]-origin[1])*normal[1] + (x[2]-origin[2])*normal[2] ;
}


//------------------------------------------------------------------------------
// Calculate height of point x above plane defined by zeta
// the value is zero for a point in the plane.
double albaOpMML3ModelView::HeightAbovePlane(double zeta, double *x) const
//------------------------------------------------------------------------------
{
  double origin[3], normal[3] ;
  CalculateSlicePosition(zeta, origin) ;
  CalculateSliceNormal(zeta, normal) ;
  return HeightAbovePlane(normal, origin, x) ;
}



//----------------------------------------------------------------------------
// calculate zeta of a point, i.e. which slice plane does point belong to
double albaOpMML3ModelView::CalculateZetaOfPoint(double *x) const
//----------------------------------------------------------------------------
{
  double zeta ;

  switch(m_MuscleType){
    case 1:
      {
        // straight axis is very simple - just project point onto axis
        double pstart[3] ;
        GetLandmark1OfAxis(pstart);

        zeta = 0.0 ;
        for (int j = 0 ;  j < 3 ;  j++)
          zeta += (x[j]-pstart[j])*m_SliceNormals[0][j] ;

        break ;
      }
    case 2:
      {
        // piecewise axis is more complicated - need to iterate to solution
        int i, ifound ;
        bool found ;
        double htLo, htHi ;
        double zetaLo, zetaHi ;

        // Try to bracket the zero value by evaluating at integer slice positions
        // The height ht(zeta) should be a decreasing function.
        htHi = HeightAbovePlane(m_SliceNormals[0], m_SlicePositions[0], x) ;
        for (i = 1, found = false ;  i < m_NumberOfScans && !found ;  i++){
          htLo = htHi ;
          htHi = HeightAbovePlane(m_SliceNormals[i], m_SlicePositions[i], x) ;
          if ((htLo >= 0.0) && (htHi <= 0.0)){
            ifound = i ;
            found = true ;
          }
        }

        if (found){
          zetaLo = GetZetaOfSlice(ifound-1) ;
          zetaHi = GetZetaOfSlice(ifound) ;
        }
        else if ((htLo <= 0.0) && (htHi <= 0.0)){  
          // point is below the stack of planes
          zetaLo = -LengthOfAxis() ;
          zetaHi = m_Zeta[0] ;
          htLo = HeightAbovePlane(zetaLo, x) ;
        }
        else if ((htLo >= 0.0) && (htHi >= 0.0)){  
          // point is above the stack of planes
          zetaLo = m_Zeta[m_NumberOfScans-1] ;
          zetaHi = m_Zeta[m_NumberOfScans-1] + LengthOfAxis() ;
          htHi = HeightAbovePlane(zetaHi, x) ;
        }
        else{
          // something went wrong
          assert(false) ;
        }

        // check that root is now bracketed
        assert((htLo >= 0.0) && (htHi <= 0.0)) ;

        // now iterate using regula falsi
        int maxIterations = 100 ;
        double tol = 0.0001*LengthOfAxis() ;
        bool foundRoot = false ;
        double zetaNext ;
        double htNext ;
        for (int i = 0 ;  i < maxIterations && !foundRoot ;  i++) {
          zetaNext = (htHi*zetaLo - htLo*zetaHi) / (htHi-htLo) ;
          htNext = HeightAbovePlane(zetaNext, x) ;

          // stop when required accuracy reached
          foundRoot = ((htNext >= -tol) && (htNext <= tol)) ;

          if (htNext >= 0.0){
            // new point becomes low point
            zetaLo = zetaNext ;
            htLo = htNext ;
          }
          else{
            // new point becomes high point
            zetaHi = zetaNext ;
            htHi = htNext ;
          }
        }

        zeta = zetaNext ;
        break ;
      }
  }

  return zeta ;
}



//----------------------------------------------------------------------------
// Apply the registration ops to the muscle
void albaOpMML3ModelView::ApplyRegistrationOps()
//----------------------------------------------------------------------------
{
  // number of points
  int n = this->m_MuscleInput->GetNumberOfPoints();

  vtkMatrix4x4 *slicePlaneMat = vtkMatrix4x4::New() ;
  vtkMatrix4x4 *slicePlaneInvMat = vtkMatrix4x4::New() ;
  vtkTransform *transform = vtkTransform::New() ;

  int cnt[4] = {0,0,0,0} ;

  // deform each point
  for(int i = 0 ;  i < n ;  i++){
    // get ith point
    double pt[3] ;
    m_MuscleInput->GetPoints()->GetPoint(i, pt);

    // get zeta and evaluate the splines
    double zeta = CalculateZetaOfPoint(pt) ;

    if (zeta > 255.6 && zeta < 256.0)
      std::cout << "hello" << std::endl ;

    // get spline values
    double ph = GetPHSpline()->Evaluate(zeta);
    double pv = GetPVSpline()->Evaluate(zeta);
    double th = GetTHSpline()->Evaluate(zeta);
    double tv = GetTVSpline()->Evaluate(zeta);
    double ra = GetRASpline()->Evaluate(zeta);
    double se = GetSESpline()->Evaluate(zeta);
    double sw = GetSWSpline()->Evaluate(zeta);
    double sn = GetSNSpline()->Evaluate(zeta);
    double ss = GetSSSpline()->Evaluate(zeta);
    double ch = GetCHSpline()->Evaluate(zeta);
    double cv = GetCVSpline()->Evaluate(zeta);


    // transform the point from patient space into the plane of the slice
    CalculateSlicePoseMatrix(zeta, slicePlaneMat) ;
    vtkMatrix4x4::Invert(slicePlaneMat, slicePlaneInvMat) ;
    MultiplyMatrixPoint(slicePlaneInvMat, pt, pt) ;
    

    // test transform to identify which scaling quadrant to use
    double temppt[3] ;
    transform->Identity() ;
    transform->Translate(th,tv,0.0) ;
    transform->Translate(-ch,-cv,0.0) ;
    transform->TransformPoint(pt, temppt);


    // transform point
    transform->Identity() ;
    transform->Translate(ch,cv,0.0) ;
    transform->Translate(ph,pv,0.0) ;
    transform->RotateZ(ra) ;

    if (temppt[0] > 0.0 && temppt[1] > 0.0){ // 1st quadrant (+, +)
      transform->Scale(se, sn, 1.0);
      cnt[0]++ ;
    }
    else if (temppt[0] < 0.0 && temppt[1] > 0.0){ // 2nd quadrant (-, +)
      transform->Scale(sw, sn, 1.0);
      cnt[1]++ ;
    }
    else if (temppt[0] < 0.0 && temppt[1] < 0.0){ // 3rd quadrant (-, -)
      transform->Scale(sw, ss, 1.0);
      cnt[2]++ ;
    }
    else if (temppt[0] > 0.0 && temppt[1] < 0.0){ // 4th quadrant (+, -)
      transform->Scale(se, ss, 1.0);
      cnt[3]++ ;
    }
    else if (temppt[0] > 0.0 && temppt[1] == 0.0) // positive x axis
      transform->Scale(se, 1.0, 1.0);
    else if (temppt[0] < 0.0 && temppt[1] == 0.0) // negative x axis
      transform->Scale(sw, 1.0, 1.0);
    else if (temppt[0] == 0.0 && temppt[1] > 0.0) // positive y axis
      transform->Scale(1.0, sn, 1.0);
    else if (temppt[0] == 0.0 && temppt[1] < 0.0) // negative y axis
      transform->Scale(1.0, ss, 1.0);

    transform->Translate(th,tv,0.0) ;
    transform->Translate(-ch,-cv,0.0) ;
    transform->TransformPoint(pt, pt) ;


    // transform back to patient space
    MultiplyMatrixPoint(slicePlaneMat, pt, pt) ;


    // write to output polydata
    m_MuscleOutput->GetPoints()->SetPoint(i, pt) ;
  }

  m_MuscleOutput->Modified() ;

  
  slicePlaneMat->Delete() ;
  slicePlaneInvMat->Delete() ;
  transform->Delete() ;
}



//----------------------------------------------------------------------------
void albaOpMML3ModelView::Render()
//----------------------------------------------------------------------------
{
  m_RenderWindow->Render();
}


//----------------------------------------------------------------------------
/** m = 1, 2 for text actor 1,2 */
void albaOpMML3ModelView::SetText(int m, double n, int d, int s)
//----------------------------------------------------------------------------
{
  char text[10];

  char *buffer;
  int decimal, sign;

  double nfloor;

  // scaling display
  if (s == 1) // scaling flag
  {
    if (n >= 1.0)
    {
      if (n > 9.0) n = 9.0;
      strcpy(text, "X");
      buffer = _ecvt(n, 1, &decimal, &sign);
      strcat(text, buffer);
      strcat(text, ".0");
    }
    else
    {	
      n = 10*n;
      if (n > 9.0) n = 9.0;
      strcpy(text, "X0.");
      buffer = _ecvt(n, 1, &decimal, &sign);
      strcat(text, buffer);
      //strcat(text, ".0");
    }

  }
  // non-scaling display
  else
  {
    //remove decimals
    nfloor = floor(n);
    if (n < nfloor + 0.5)
      n = nfloor;
    else
      n = nfloor + 1.0;

    // 2 digit display
    if (d == 2) 
    {
      if (fabs(n) < 10.0)
      {
        buffer = _ecvt(n, 1, &decimal, &sign);

        // sign
        if (sign == 0)
          strcpy(text, "+");
        else
          strcpy(text, "-");

        // pad zero
        strcat(text, "0");
      }
      else
        if (fabs(n) < 100.0)
        {
          buffer = _ecvt(n, 2, &decimal, &sign);

          // sign
          if (sign == 0)
            strcpy(text, "+");
          else
            strcpy(text, "-");
        }
        else
          return;
      strcat(text, buffer);
    }
  }

  // 3 digit display
  if (d == 3) 
  {
    if (fabs(n) < 10)
    {
      buffer = _ecvt(n, 1, &decimal, &sign);

      // sign
      if (sign == 0)
        strcpy(text, "+");
      else
        strcpy(text, "-");

      // pad zero
      strcat(text, "00");
    }
    else
      if (fabs(n) < 100)
      {
        buffer = _ecvt(n, 2, &decimal, &sign);

        // sign
        if (sign == 0)
          strcpy(text, "+");
        else
          strcpy(text, "-");

        // pad zero
        strcat(text, "0");
      }
      else
      {
        buffer = _ecvt(n, 3, &decimal, &sign);

        // sign
        if (sign == 0)
          strcpy(text, "+");
        else
          strcpy(text, "-");
      }

      strcat(text, buffer);
  }

  // assign to text mapper
  if (m == 1)
    GetVisualPipe2D()->SetTextY(text) ;
  else if (m == 2)
    GetVisualPipe2D()->SetTextX(text) ;
}









//----------------------------------------------------------------------------
int albaOpMML3ModelView::GetTotalNumberOfScans() const
//----------------------------------------------------------------------------
{
  assert(m_NumberOfScans >= 3); // at least 3 synthetic scans
  return m_NumberOfScans;
}


//----------------------------------------------------------------------------
void albaOpMML3ModelView::GetResolutionOfScans(int *x, int *y) const
//----------------------------------------------------------------------------
{
  *x = m_ScansResolutionX;
  *y = m_ScansResolutionY;
}


//----------------------------------------------------------------------------
void albaOpMML3ModelView::GetSizeOfScans(double *x, double *y) const
//----------------------------------------------------------------------------
{
  *x = m_ScanSizeX;
  *y = m_ScanSizeY;
}




//------------------------------------------------------------------------------
// Update everything
void albaOpMML3ModelView::Update()
//------------------------------------------------------------------------------
{
  // update vertical cuts
  UpdateCuttingPlanesTransform();

  // update transform of contour axes and clipping planes
  UpdateContourAxesTransform();

  // update transform of global axes
  UpdateGlobalAxesTransform();

  // update north east segment
  UpdateSegmentNorthEastTransform();

  // update north west segment
  UpdateSegmentNorthWestTransform();

  // update south east segment
  UpdateSegmentSouthEastTransform();

  // update south west segment
  UpdateSegmentSouthWestTransform();

  // update visual pipes, if they haven't been done already
  m_VisualPipe2D->Update() ;
  m_VisualPipe3D->Update() ;
  m_VisualPipePreview->Update() ;
}




//------------------------------------------------------------------------------
// Update the transform which controls the cutting planes which divide the contour into quadrants.
// Applies the current values of the splines.
// Applies the operation C - T to the cutting planes, which places them correctly on the original contour.
void albaOpMML3ModelView::UpdateCuttingPlanesTransform()
//------------------------------------------------------------------------------
{
  vtkTransform* Transform = vtkTransform::New();
  Transform->Identity();

  // get zeta level
  double zeta = GetZetaOfCurrentSlice();

  // get h/v translation
  double trans[2];
  trans[0] = GetTHSpline()->Evaluate(zeta);
  trans[1] = GetTVSpline()->Evaluate(zeta);

  // original center
  double center[3];
  GetOriginalContourCenter(center) ;

  Transform->Translate(center[0] - trans[0], center[1] - trans[1], 0.0) ;

  GetVisualPipe2D()->SetCuttingPlanesTransform(Transform) ;
  GetVisualPipe2D()->Update();

  Transform->Delete() ;
}




//------------------------------------------------------------------------------
// Update the transform which controls the contour axes.
// Applies the current values of the splines.
// This does PCR, where C is the translation to the original contour center.
// There is no S and T, which are applied to the contour but not the axes.
// NB remember that the transforms added to vtkTransform are executed in reverse order, like a stack.
void albaOpMML3ModelView::UpdateContourAxesTransform()
//------------------------------------------------------------------------------
{
  // get zeta level
  double zeta = GetZetaOfCurrentSlice();

  // get center h/v translation
  double ctrans[2];
  ctrans[0] = GetPHSpline()->Evaluate(zeta);
  ctrans[1] = GetPVSpline()->Evaluate(zeta);

  // get twist
  double twist = GetRASpline()->Evaluate(zeta);

  // Get original center
  double center[3] ;
  GetOriginalContourCenter(center) ;

  // set transform to identity
  vtkTransform* Transform = vtkTransform::New();
  Transform->Identity();

  // P operation - move contour and widget
  Transform->Translate(ctrans[0], ctrans[1], 0.0);

  // C operation - move axes to center of contour
  Transform->Translate(center[0], center[1], 0.0);

  // R operation - rotate about origin
  Transform->RotateZ(twist);

  GetVisualPipe2D()->SetContourAxesTransform(Transform) ;
  GetVisualPipe2D()->Update();

  Transform->Delete();
}





//----------------------------------------------------------------------------
// Update the transform of the global axes
void albaOpMML3ModelView::UpdateGlobalAxesTransform()
//----------------------------------------------------------------------------
{
  // set transform to identity
  vtkTransform* Transform = vtkTransform::New();
  Transform->Identity();
  GetVisualPipe2D()->SetGlobalAxesTransform(Transform) ;
  GetVisualPipe2D()->Update();
  Transform->Delete() ;
}




//----------------------------------------------------------------------------
// Apply current state of transforms to the NE contour quadrant
// Mel's code is (CPRS R-1 P-1 C-1) * (CPR P-1 C-1) *TP
//                = P C R S (C-1 T)
// where C-1 T puts the center of rotation at the origin (opposite of UpdateCuttingPlanesTransform)
// Then scale and rotate, undo the C-1 translation and do P.
void albaOpMML3ModelView::UpdateSegmentNorthEastTransform()
//----------------------------------------------------------------------------
{
  // get zeta level
  double zeta = GetZetaOfCurrentSlice();

  // get center h/v translation
  double ctrans[2];
  ctrans[0] = GetPHSpline()->Evaluate(zeta);
  ctrans[1] = GetPVSpline()->Evaluate(zeta);

  // get twist
  double twist = GetRASpline()->Evaluate(zeta);

  // get h/v translation
  double trans[2];
  trans[0] = GetTHSpline()->Evaluate(zeta);
  trans[1] = GetTVSpline()->Evaluate(zeta);

  // get scaling
  double scale[4];
  scale[0] = GetSESpline()->Evaluate(zeta); // east
  scale[1] = GetSWSpline()->Evaluate(zeta); // west
  scale[2] = GetSNSpline()->Evaluate(zeta); // north
  scale[3] = GetSSSpline()->Evaluate(zeta); // south;


  // original center
  double center[3] ;
  GetOriginalContourCenter(center) ;


  // initialize
  vtkTransform* Transform = vtkTransform::New();
  Transform->Identity();

  // transform
  Transform->Translate(center[0], center[1], 0.0); // axes origin
  Transform->Translate(ctrans[0], ctrans[1], 0.0); // p operation
  Transform->RotateZ(twist); // r operation
  Transform->Scale(scale[0], scale[2], 1.0); // s operation
  Transform->Translate(trans[0], trans[1], 0.0); // axes origin
  Transform->Translate(-center[0], -center[1], 0.0); // axes origin

  GetVisualPipe2D()->SetNEContourTransform(Transform);
  GetVisualPipe2D()->Update();

  // clean up
  Transform->Delete();
}



//----------------------------------------------------------------------------
// Apply current state of transforms to the NW contour quadrant
void albaOpMML3ModelView::UpdateSegmentNorthWestTransform()
//----------------------------------------------------------------------------
{
  // get zeta level
  double zeta = GetZetaOfCurrentSlice();

  // get center h/v translation
  double ctrans[2];
  ctrans[0] = GetPHSpline()->Evaluate(zeta);
  ctrans[1] = GetPVSpline()->Evaluate(zeta);

  // get twist
  double twist = GetRASpline()->Evaluate(zeta);

  // get h/v translation
  double trans[2];
  trans[0] = GetTHSpline()->Evaluate(zeta);
  trans[1] = GetTVSpline()->Evaluate(zeta);

  // get scaling
  double scale[4];
  scale[0] = GetSESpline()->Evaluate(zeta); // east
  scale[1] = GetSWSpline()->Evaluate(zeta); // west
  scale[2] = GetSNSpline()->Evaluate(zeta); // north
  scale[3] = GetSSSpline()->Evaluate(zeta); // south;


  // original center
  double center[3] ;
  GetOriginalContourCenter(center) ;


  // initialize
  vtkTransform* Transform = vtkTransform::New();
  Transform->Identity();

  // transform
  Transform->Translate(center[0], center[1], 0.0); // axes origin
  Transform->Translate(ctrans[0], ctrans[1], 0.0); // p operation
  Transform->RotateZ(twist); // r operation
  Transform->Scale(scale[1], scale[2], 1.0); // s operation
  Transform->Translate(trans[0], trans[1], 0.0); // axes origin
  Transform->Translate(-center[0], -center[1], 0.0); // axes origin

  GetVisualPipe2D()->SetNWContourTransform(Transform) ;
  GetVisualPipe2D()->Update() ;

  // clean up
  Transform->Delete();
}


//----------------------------------------------------------------------------
// Apply current state of transforms to the SE contour quadrant
void albaOpMML3ModelView::UpdateSegmentSouthEastTransform()
//----------------------------------------------------------------------------
{
  // get zeta level
  double zeta = GetZetaOfCurrentSlice();

  // get center h/v translation
  double ctrans[2];
  ctrans[0] = GetPHSpline()->Evaluate(zeta);
  ctrans[1] = GetPVSpline()->Evaluate(zeta);

  // get twist
  double twist = GetRASpline()->Evaluate(zeta);

  // get h/v translation
  double trans[2];
  trans[0] = GetTHSpline()->Evaluate(zeta);
  trans[1] = GetTVSpline()->Evaluate(zeta);

  // get scaling
  double scale[4];
  scale[0] = GetSESpline()->Evaluate(zeta); // east
  scale[1] = GetSWSpline()->Evaluate(zeta); // west
  scale[2] = GetSNSpline()->Evaluate(zeta); // north
  scale[3] = GetSSSpline()->Evaluate(zeta); // south;


  // original center
  double center[3] ;
  GetOriginalContourCenter(center) ;


  // initialize
  vtkTransform* Transform = vtkTransform::New();
  Transform->Identity();

  // transform
  Transform->Translate(center[0], center[1], 0.0); // axes origin
  Transform->Translate(ctrans[0], ctrans[1], 0.0); // p operation
  Transform->RotateZ(twist); // r operation
  Transform->Scale(scale[0], scale[3], 1.0); // s operation
  Transform->Translate(trans[0], trans[1], 0.0); // axes origin
  Transform->Translate(-center[0], -center[1], 0.0); // axes origin

  GetVisualPipe2D()->SetSEContourTransform(Transform) ;
  GetVisualPipe2D()->Update() ;

  // clean up
  Transform->Delete();
}


//----------------------------------------------------------------------------
// Apply current state of transforms to the SW contour quadrant
void albaOpMML3ModelView::UpdateSegmentSouthWestTransform()
//----------------------------------------------------------------------------
{
  // get zeta level
  double zeta = GetZetaOfCurrentSlice();

  // get center h/v translation
  double ctrans[2];
  ctrans[0] = GetPHSpline()->Evaluate(zeta);
  ctrans[1] = GetPVSpline()->Evaluate(zeta);

  // get twist
  double twist = GetRASpline()->Evaluate(zeta);

  // get h/v translation
  double trans[2];
  trans[0] = GetTHSpline()->Evaluate(zeta);
  trans[1] = GetTVSpline()->Evaluate(zeta);

  // get scaling
  double scale[4];
  scale[0] = GetSESpline()->Evaluate(zeta); // east
  scale[1] = GetSWSpline()->Evaluate(zeta); // west
  scale[2] = GetSNSpline()->Evaluate(zeta); // north
  scale[3] = GetSSSpline()->Evaluate(zeta); // south;


  // original center
  double center[3] ;
  GetOriginalContourCenter(center) ;


  // initialize
  vtkTransform* Transform = vtkTransform::New();
  Transform->Identity();

  // transform
  Transform->Translate(center[0], center[1], 0.0); // axes origin
  Transform->Translate(ctrans[0], ctrans[1], 0.0); // p operation
  Transform->RotateZ(twist); // r operation
  Transform->Scale(scale[1], scale[3], 1.0); // s operation
  Transform->Translate(trans[0], trans[1], 0.0); // axes origin
  Transform->Translate(-center[0], -center[1], 0.0); // axes origin

  GetVisualPipe2D()->SetSWContourTransform(Transform) ;
  GetVisualPipe2D()->Update() ;

  // clean up
  Transform->Delete();
}





//----------------------------------------------------------------------------
void albaOpMML3ModelView::SetLandmark1OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
  m_AtlasLandmark1[0] = xyz[0];
  m_AtlasLandmark1[1] = xyz[1];
  m_AtlasLandmark1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::SetLandmark2OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
  m_AtlasLandmark2[0] = xyz[0];
  m_AtlasLandmark2[1] = xyz[1];
  m_AtlasLandmark2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::SetLandmark3OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
  m_AtlasLandmark3[0] = xyz[0];
  m_AtlasLandmark3[1] = xyz[1];
  m_AtlasLandmark3[2] = xyz[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::SetLandmark4OfAtlas(double *xyz)
//----------------------------------------------------------------------------
{
  m_AtlasLandmark4[0] = xyz[0];
  m_AtlasLandmark4[1] = xyz[1];
  m_AtlasLandmark4[2] = xyz[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::GetLandmark1OfAtlas(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_AtlasLandmark1[0];
  xyz[1] = m_AtlasLandmark1[1];
  xyz[2] = m_AtlasLandmark1[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::GetLandmark2OfAtlas(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_AtlasLandmark2[0];
  xyz[1] = m_AtlasLandmark2[1];
  xyz[2] = m_AtlasLandmark2[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::GetLandmark3OfAtlas(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_AtlasLandmark3[0];
  xyz[1] = m_AtlasLandmark3[1];
  xyz[2] = m_AtlasLandmark3[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::GetLandmark4OfAtlas(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_AtlasLandmark4[0];
  xyz[1] = m_AtlasLandmark4[1];
  xyz[2] = m_AtlasLandmark4[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::SetLandmark1OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
  m_PatientLandmark1[0] = xyz[0];
  m_PatientLandmark1[1] = xyz[1];
  m_PatientLandmark1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::SetLandmark2OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
  m_PatientLandmark2[0] = xyz[0];
  m_PatientLandmark2[1] = xyz[1];
  m_PatientLandmark2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::SetLandmark3OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
  m_PatientLandmark3[0] = xyz[0];
  m_PatientLandmark3[1] = xyz[1];
  m_PatientLandmark3[2] = xyz[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::SetLandmark4OfPatient(double *xyz)
//----------------------------------------------------------------------------
{
  m_PatientLandmark4[0] = xyz[0];
  m_PatientLandmark4[1] = xyz[1];
  m_PatientLandmark4[2] = xyz[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::GetLandmark1OfPatient(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_PatientLandmark1[0];
  xyz[1] = m_PatientLandmark1[1];
  xyz[2] = m_PatientLandmark1[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::GetLandmark2OfPatient(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_PatientLandmark2[0];
  xyz[1] = m_PatientLandmark2[1];
  xyz[2] = m_PatientLandmark2[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::GetLandmark3OfPatient(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_PatientLandmark3[0];
  xyz[1] = m_PatientLandmark3[1];
  xyz[2] = m_PatientLandmark3[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::GetLandmark4OfPatient(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_PatientLandmark4[0];
  xyz[1] = m_PatientLandmark4[1];
  xyz[2] = m_PatientLandmark4[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::SetLandmark1OfAxis(double *xyz)
//----------------------------------------------------------------------------
{
  m_AxisLandmark1[0] = xyz[0];
  m_AxisLandmark1[1] = xyz[1];
  m_AxisLandmark1[2] = xyz[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::SetLandmark2OfAxis(double *xyz)
//----------------------------------------------------------------------------
{
  m_AxisLandmark2[0] = xyz[0];
  m_AxisLandmark2[1] = xyz[1];
  m_AxisLandmark2[2] = xyz[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::SetLandmark3OfAxis(double *xyz)
//----------------------------------------------------------------------------
{
  m_AxisLandmark3[0] = xyz[0];
  m_AxisLandmark3[1] = xyz[1];
  m_AxisLandmark3[2] = xyz[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::GetLandmark1OfAxis(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_AxisLandmark1[0];
  xyz[1] = m_AxisLandmark1[1];
  xyz[2] = m_AxisLandmark1[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::GetLandmark2OfAxis(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_AxisLandmark2[0];
  xyz[1] = m_AxisLandmark2[1];
  xyz[2] = m_AxisLandmark2[2];
}

//----------------------------------------------------------------------------
void albaOpMML3ModelView::GetLandmark3OfAxis(double *xyz) const 
//----------------------------------------------------------------------------
{
  xyz[0] = m_AxisLandmark3[0];
  xyz[1] = m_AxisLandmark3[1];
  xyz[2] = m_AxisLandmark3[2];
}







//------------------------------------------------------------------------------
// Convenience method for multiplying a non-homo 3-point with a 4x4 matrix
// multiply point by matrix c = Ab. \n
// c can be the same as b
void albaOpMML3ModelView::MultiplyMatrixPoint(vtkMatrix4x4* A, double b[3], double c[3]) const
//------------------------------------------------------------------------------
{
  int i, j ;
  double ctemp[4] ;

  for (i = 0 ;  i < 4 ;  i++){
    for (j = 0, ctemp[i] = 0.0 ;  j < 3 ;  j++)
      ctemp[i] += A->GetElement(i,j) * b[j] ;
    ctemp[i] += A->GetElement(i,3) ;
  }

  // copy to output matrix
  for (i = 0 ;  i < 3 ;  i++)
    c[i] = ctemp[i] ;
}




//------------------------------------------------------------------------------
// Calculate center of any plane created by vtkPlaneSource and scaled by sizx, sizy
void albaOpMML3ModelView::CalculateCenterOfVtkPlane(double sizx, double sizy, double p[3]) const
//------------------------------------------------------------------------------
{
  vtkPlaneSource *plane = vtkPlaneSource::New() ;

  vtkTransform *t = vtkTransform::New() ;
  t->Scale(sizx, sizy, 1) ;

  vtkTransformPolyDataFilter *tpdf = vtkTransformPolyDataFilter::New() ;
  tpdf->SetInputConnection(plane->GetOutputPort()) ;
  tpdf->SetTransform(t) ;

  tpdf->GetOutput()->GetCenter(p) ;
  double b[6] ;
  tpdf->GetOutput()->GetBounds(b) ;

  t->Delete() ;
  tpdf->Delete() ;
  plane->Delete() ;
}






//----------------------------------------------------------------------------
void albaOpMML3ModelView::Print(vtkObject *obj, wxString msg) const
//----------------------------------------------------------------------------
{
  wxLogMessage("%s",msg);
  std::stringstream ss1;

  obj->Print(ss1);
  wxString message=ss1.str().c_str();
  wxLogMessage("[%s PRINTOUT:]\n", obj->GetClassName());

  for (int pos=message.Find('\n');pos>=0;pos=message.Find('\n'))
  {
    wxString tmp=message.Mid(0,pos);
    wxLogMessage(tmp);
    message=message.Mid(pos+1);
  }

}






//------------------------------------------------------------------------------
// allocate operations stack
void albaOpMML3ModelView::AllocateOperationsStack(int numberOfComponents, int numberOfTuples)
//------------------------------------------------------------------------------
{
  m_OperationsStack = vtkDoubleArray::New() ;
  m_OperationsStack->SetNumberOfComponents(numberOfComponents) ;
  m_OperationsStack->SetNumberOfTuples(numberOfTuples) ;
}



//------------------------------------------------------------------------------
// Print self
void albaOpMML3ModelView::PrintSelf(ostream &os, int indent)
//------------------------------------------------------------------------------
{
  os << "MML Model View" << std::endl ;
  os << std::endl ;

  os << "muscle type = " << m_MuscleType << std::endl ;
  os << std::endl ;

  os << "atlas landmark 1 " << m_AtlasLandmark1[0] << " " << m_AtlasLandmark1[1] << " " << m_AtlasLandmark1[2] << std::endl ;
  os << "atlas landmark 2 " << m_AtlasLandmark2[0] << " " << m_AtlasLandmark2[1] << " " << m_AtlasLandmark2[2] << std::endl ;
  os << "atlas landmark 3 " << m_AtlasLandmark3[0] << " " << m_AtlasLandmark3[1] << " " << m_AtlasLandmark3[2] << std::endl ;
  if (m_Landmarks4)
    os << "atlas landmark 4 " << m_AtlasLandmark4[0] << " " << m_AtlasLandmark4[1] << " " << m_AtlasLandmark4[2] << std::endl ;

  os << "patient landmark 1 " << m_PatientLandmark1[0] << " " << m_PatientLandmark1[1] << " " << m_PatientLandmark1[2] << std::endl ;
  os << "patient landmark 2 " << m_PatientLandmark2[0] << " " << m_PatientLandmark2[1] << " " << m_PatientLandmark2[2] << std::endl ;
  os << "patient landmark 3 " << m_PatientLandmark3[0] << " " << m_PatientLandmark3[1] << " " << m_PatientLandmark3[2] << std::endl ;
  if (m_Landmarks4)
    os << "patient landmark 4 " << m_PatientLandmark4[0] << " " << m_PatientLandmark4[1] << " " << m_PatientLandmark4[2] << std::endl ;

  os << "axis landmark 1 " << m_AxisLandmark1[0] << " " << m_AxisLandmark1[1] << " " << m_AxisLandmark1[2] << std::endl ;
  os << "axis landmark 2 " << m_AxisLandmark2[0] << " " << m_AxisLandmark2[1] << " " << m_AxisLandmark2[2] << std::endl ;
  if (m_MuscleType == 2)
    os << "axis landmark 3 " << m_AxisLandmark3[0] << " " << m_AxisLandmark3[1] << " " << m_AxisLandmark3[2] << std::endl ;
  os << std::endl ;

  if (m_Display3D == 0)
    os << "display mode = 2D" << std::endl ;
  else if (m_Display3D == 1)
    os << "display mode = 3D" << std::endl ;
  else
    os << "display mode = Preview" << std::endl ;
  os << std::endl ;

  os << "scaling occurred = " << m_ScalingOccured << " op = " << m_ScalingOccuredOperationId << std::endl ;
  os << std::endl ;

  os << "no. of scans = " << m_NumberOfScans << std::endl ;
  os << "current scan id = " << m_ScansCurrentId << std::endl ;
  os << "scans grain = " << m_ScansGrain << std::endl ;
  os << "scans resolution = " << m_ScansResolutionX << " " << m_ScansResolutionY << std::endl ;
  os << "scans size = " << m_ScanSizeX << " " << m_ScanSizeY << std::endl ;
  os << std::endl ;

  os << "alpha and zeta values of slices" << std::endl ;
  for (int i = 0 ;  i < m_NumberOfScans ;  i++){
    os << "slice " << i << " " << m_Alpha[i] << " " << m_Zeta[i] << std::endl ;
  }
  os << std::endl ;

  os << "positions of slices in patient space" << std::endl ;
  for (int i = 0 ;  i < m_NumberOfScans ;  i++){
    os << "slice " << i << " " << m_SlicePositions[i][0] << " " << m_SlicePositions[i][1] << " " << m_SlicePositions[i][2] << " " << std::endl ;
  }
  os << std::endl ;

  os << "normals of slices in patient space" << std::endl ;
  for (int i = 0 ;  i < m_NumberOfScans ;  i++){
    os << "slice " << i << " " << m_SliceNormals[i][0] << " " << m_SliceNormals[i][1] << " " << m_SliceNormals[i][2] << std::endl ;
  }
  os << std::endl ;

  os << "initial centers of contours in slice coords" << std::endl ;
  for (int i = 0 ;  i < m_NumberOfScans ;  i++){
    os << "slice " << i << " " 
      << m_OriginalContourCenters[i][0] << " " 
      << m_OriginalContourCenters[i][1] << " " 
      << m_OriginalContourCenters[i][2] << std::endl ;
  }
  os << std::endl ;
  os << std::endl ;

}
