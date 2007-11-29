/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFMeshCutterTest.cpp,v $
Language:  C++
Date:      $Date: 2007-11-29 10:38:35 $
Version:   $Revision: 1.3 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2007 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/


//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "mafDefines.h" 

#include <cppunit/config/SourcePrefix.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkLookupTable.h"
#include "vtkGeometryFilter.h"

#include "vtkTransform.h"
#include "vtkPlane.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkCell.h"

#include "vtkMAFMeshCutter.h"
#include "vtkMAFMeshCutterTest.h"

#include "mafConfigure.h"


//#define TESTDATA MAF_DATA_ROOT"/FEM/ANSYS"
#define FTOL 0.0000001

static const bool renderingOn = false ;  // switch interactive rendering on

void vtkMAFMeshCutterTest::setUp()
{
}

void vtkMAFMeshCutterTest::tearDown()
{
}


void vtkMAFMeshCutterTest::TestFixture()
{
}


//------------------------------------------------------------------------------
// return true if v = (1-lambda)*v0 + lambda*v1, within tolerance ftol
bool vtkMAFMeshCutterTest::ColinearVectors(double *v, double *v0, double *v1, double lambda, double ftol)
//------------------------------------------------------------------------------
{
  int i ;
  double dv[3], dist ;

  for (i = 0 ;  i < 3 ;  i++)
    dv[i] = v[i] - ((1.0-lambda)*v0[i] + lambda*v1[i]) ;

  for (i = 0, dist = 0.0 ;  i < 3 ;  i++)
    dist += dv[i]*dv[i] ;
  dist = sqrt(dist) ;

  return (dist <= ftol) ;
}


//------------------------------------------------------------------------------
// return true if point is in the plane within tolerance ftol
bool vtkMAFMeshCutterTest::PointInPlane(double *v, double *origin, double *norm, double ftol)
//------------------------------------------------------------------------------
{
  int i ;
  double dotprod ;

  for (i = 0, dotprod = 0.0 ;  i < 3 ;  i++)
    dotprod += (v[i]-origin[i])*norm[i] ;

  return ((dotprod >= -ftol) && (dotprod <= ftol)) ;
}


//------------------------------------------------------------------------------
// return true if points are equal within tolerance ftol
bool vtkMAFMeshCutterTest::PointsEqual(double *v0, double *v1, double ftol)
//------------------------------------------------------------------------------
{
  int i ;
  double dist ;

  for (i = 0, dist = 0.0 ;  i < 3 ;  i++)
    dist += (v1[i]-v0[i])*(v1[i]-v0[i]) ;
  dist = sqrt(dist) ;

  return (dist <= ftol) ;
}


//------------------------------------------------------------------------------
// render the input and output data
void vtkMAFMeshCutterTest::RenderPointScalars(vtkUnstructuredGrid *UG, vtkPolyData *polydata)
//------------------------------------------------------------------------------
{
  // Create a Renderer, a RenderWindow and a RenderWindowInteractor

  vtkRenderer *R = vtkRenderer::New();
  R->SetBackground(0.5 , 0.5 , 0.5);
  R->SetLightFollowCamera(1);

  vtkRenderWindow *RW = vtkRenderWindow::New();
  RW->AddRenderer(R);
  RW->AddRenderer(R);
  RW->SetSize(500 , 406);

  vtkRenderWindowInteractor *RWI = vtkRenderWindowInteractor::New();
  RWI->SetRenderWindow(RW);
  RWI->SetLightFollowCamera(1);


  // set up pipeline to visualize original data
  vtkGeometryFilter *GF = vtkGeometryFilter::New() ;
  GF->SetInput((vtkDataSet *)UG);

  // get center and scalar range of input data
  double range[2] ;
  GF->GetOutput()->Update() ;
  GF->GetOutput()->GetPointData()->GetArray(0)->GetRange(range) ;

  // set up scalar lut so that original data and slice can be viewed with same colour mapping
  vtkLookupTable *lut = vtkLookupTable::New() ;
  lut->SetTableRange(range[0], range[1]) ;
  lut->SetNumberOfColors(256) ;
  lut->Build() ;

  vtkPolyDataMapper *PDM1 = vtkPolyDataMapper::New();
  PDM1->SetInput(GF->GetOutput());
  PDM1->ScalarVisibilityOn() ;
  PDM1->SetColorModeToMapScalars() ;
  PDM1->SetScalarModeToUsePointFieldData() ;
  PDM1->ColorByArrayComponent(0, 0) ;
  PDM1->SetLookupTable(lut) ;
  PDM1->SetUseLookupTableScalarRange(1) ;

  // set the actor
  vtkActor *A1 = vtkActor::New();
  A1->SetMapper(PDM1);

  vtkProperty *prop1 = vtkProperty::New() ;
  prop1->SetOpacity(0.2) ;
  prop1->SetRepresentationToWireframe() ;
  prop1->SetLineWidth(1.0) ;
  A1->SetProperty(prop1) ;




// set up the pipeline to visulaize the outout polydata
  vtkPolyDataMapper *PDM2 = vtkPolyDataMapper::New();
  PDM2->SetInput((vtkPolyData *)polydata);
  PDM2->ScalarVisibilityOn() ;
  PDM2->SetColorModeToMapScalars() ;
  PDM2->SetScalarModeToUsePointFieldData() ;
  PDM2->ColorByArrayComponent(0, 0) ;
  PDM2->SetLookupTable(lut) ;
  PDM2->SetUseLookupTableScalarRange(1) ;

  vtkActor *A2 = vtkActor::New();
  A2->SetMapper(PDM2);



  // -------------------------------
  // Reset the camera and initialize
  // -------------------------------

  R->AddActor( A1 );
  R->AddActor( A2 );

  R->ResetCamera();
  R->ResetCameraClippingRange();
  RW->Render();
  RWI->Initialize();
  RWI->Start();


  R->Delete() ;
  RW->Delete() ;
  RWI->Delete() ;
  GF->Delete() ;
  lut->Delete() ;
  PDM1->Delete();
  A1->Delete();
  prop1->Delete() ;
  PDM2->Delete();
  A2->Delete();
}



//------------------------------------------------------------------------------
// render the input and output data
void vtkMAFMeshCutterTest::RenderCellScalars(vtkUnstructuredGrid *UG, vtkPolyData *polydata)
//------------------------------------------------------------------------------
{
  // Create a Renderer, a RenderWindow and a RenderWindowInteractor

  vtkRenderer *R = vtkRenderer::New();
  R->SetBackground(0.5 , 0.5 , 0.5);
  R->SetLightFollowCamera(1);

  vtkRenderWindow *RW = vtkRenderWindow::New();
  RW->AddRenderer(R);
  RW->AddRenderer(R);
  RW->SetSize(500 , 406);

  vtkRenderWindowInteractor *RWI = vtkRenderWindowInteractor::New();
  RWI->SetRenderWindow(RW);
  RWI->SetLightFollowCamera(1);


  // set up pipeline to visualize original data
  vtkGeometryFilter *GF = vtkGeometryFilter::New() ;
  GF->SetInput((vtkDataSet *)UG);

  // get center and scalar range of input data
  double range[2] ;
  GF->GetOutput()->Update() ;
  GF->GetOutput()->GetCellData()->GetArray(0)->GetRange(range) ;

  // set up scalar lut so that original data and slice can be viewed with same colour mapping
  vtkLookupTable *lut = vtkLookupTable::New() ;
  lut->SetTableRange(range[0], range[1]) ;
  lut->SetNumberOfColors(256) ;
  lut->Build() ;

  vtkPolyDataMapper *PDM1 = vtkPolyDataMapper::New();
  PDM1->SetInput(GF->GetOutput());
  PDM1->ScalarVisibilityOn() ;
  PDM1->SetColorModeToMapScalars() ;
  PDM1->SetScalarModeToUseCellFieldData() ;
  PDM1->ColorByArrayComponent(0, 0) ;
  PDM1->SetLookupTable(lut) ;
  PDM1->SetUseLookupTableScalarRange(1) ;

  // set the actor
  vtkActor *A1 = vtkActor::New();
  A1->SetMapper(PDM1);

  vtkProperty *prop1 = vtkProperty::New() ;
  prop1->SetOpacity(0.2) ;
  prop1->SetRepresentationToWireframe() ;
  prop1->SetLineWidth(1.0) ;
  A1->SetProperty(prop1) ;



  // set up the pipeline to visulaize the outout polydata
  vtkPolyDataMapper *PDM2 = vtkPolyDataMapper::New();
  PDM2->SetInput((vtkPolyData *)polydata);
  PDM2->ScalarVisibilityOn() ;
  PDM2->SetColorModeToMapScalars() ;
  PDM2->SetScalarModeToUseCellFieldData() ;
  PDM2->ColorByArrayComponent(0, 0) ;
  PDM2->SetLookupTable(lut) ;
  PDM2->SetUseLookupTableScalarRange(1) ;

  vtkActor *A2 = vtkActor::New();
  A2->SetMapper(PDM2);


  // -------------------------------
  // Reset the camera and initialize
  // -------------------------------

  R->AddActor( A1 );
  R->AddActor( A2 );

  R->ResetCamera();
  R->ResetCameraClippingRange();
  RW->Render();
  RWI->Initialize();
  RWI->Start();


  R->Delete() ;
  RW->Delete() ;
  RWI->Delete() ;
  GF->Delete() ;
  lut->Delete() ;
  PDM1->Delete();
  A1->Delete();
  prop1->Delete() ;
  PDM2->Delete();
  A2->Delete();
}




//------------------------------------------------------------------------------
// Test that the scalars were copied correctly
void vtkMAFMeshCutterTest::ScalarTest(vtkMAFMeshCutter *MeshCutter, vtkUnstructuredGrid *UG, vtkPolyData *polydata) 
//------------------------------------------------------------------------------
{
  int i, j, k ;
  vtkIdType id0, id1 ;
  double lambda ;

  // check that the point scalars were interpolated correctly
  double tuple[100], tup0[100], tup1[100] ;
  int narrays_in = UG->GetPointData()->GetNumberOfArrays() ;
  int narrays = polydata->GetPointData()->GetNumberOfArrays() ;
  CPPUNIT_ASSERT(narrays == narrays_in) ;

  for (i = 0 ;  i < narrays ;  i++){
    int dtype_in = UG->GetPointData()->GetArray(i)->GetDataType() ;
    int dtype = polydata->GetPointData()->GetArray(i)->GetDataType() ;
    CPPUNIT_ASSERT(dtype == dtype_in) ;

    int ncomp_in = UG->GetPointData()->GetArray(i)->GetNumberOfComponents() ;
    int ncomp = polydata->GetPointData()->GetArray(i)->GetNumberOfComponents() ;
    CPPUNIT_ASSERT(ncomp == ncomp_in) ;

    int ntuples = polydata->GetPointData()->GetArray(i)->GetNumberOfTuples() ;

    for (j = 0 ;  j < ntuples ;  j++){
      polydata->GetPointData()->GetArray(i)->GetTuple(j, tuple) ;
      if (MeshCutter->GetInputEdgeCutByPoint(j, &id0, &id1, &lambda)){
        // the output point cuts an edge - check the interpolation
        UG->GetPointData()->GetArray(i)->GetTuple(id0, tup0) ;
        UG->GetPointData()->GetArray(i)->GetTuple(id1, tup1) ;

        if ((dtype == VTK_FLOAT) || (dtype == VTK_DOUBLE)){
          // float types - look for exact match
          for (k = 0 ;  k < ncomp ;  k++){
            double interp = (1.0-lambda)*tup0[k] + lambda*tup1[k] ;
            if (tuple[k] != interp){
              std::cout << "point scalar interpolation problem at array " << i << " tuple " << j << " component " << k << std::endl ;
              std::cout << "value is " << tuple[k] << " expected " << interp << std::endl ;
              CPPUNIT_ASSERT(false) ;
            }
          }
        }
        else{
          // integer types - look for rounded match
          for (k = 0 ;  k < ncomp ;  k++){
            int interp = (int)((1.0-lambda)*(double)tup0[k] + lambda*(double)tup1[k] + 0.5) ;
            if (tuple[k] != interp){
              std::cout << "point scalar interpolation problem at array " << i << " tuple " << j << " component " << k << std::endl ;
              std::cout << "value is " << tuple[k] << " expected " << interp << std::endl ;
              CPPUNIT_ASSERT(false) ;
            }
          }
        }
      }
      else if (MeshCutter->GetInputPointCutByPoint(j, &id0)){
        // the output point cuts a point - check that scalars were copied
        UG->GetPointData()->GetArray(i)->GetTuple(id0, tup0) ;
        for (k = 0 ;  k < ncomp ;  k++){
          if (tuple[k] != tup0[k]){
            std::cout << "point scalar interpolation problem at array " << i << " tuple " << j << " component " << k << std::endl ;
            std::cout << "value is " << tuple[k] << " expected " << tup0[k] << std::endl ;
            CPPUNIT_ASSERT(false) ;
          }
        }
      }
      else{
        std::cout << "can't find mapping for output point " << j << std::endl ;
        CPPUNIT_ASSERT(false) ;
      }
    }
  }


  // check that the cell scalars were interpolated correctly
  int ncellarrays_in = UG->GetCellData()->GetNumberOfArrays() ;
  int ncellarrays = polydata->GetCellData()->GetNumberOfArrays() ;
  CPPUNIT_ASSERT(ncellarrays == ncellarrays_in) ;

  for (i = 0 ;  i < ncellarrays ;  i++){
    int dtype_in = UG->GetCellData()->GetArray(i)->GetDataType() ;
    int dtype = polydata->GetCellData()->GetArray(i)->GetDataType() ;
    CPPUNIT_ASSERT(dtype == dtype_in) ;

    int ncomp_in = UG->GetCellData()->GetArray(i)->GetNumberOfComponents() ;
    int ncomp = polydata->GetCellData()->GetArray(i)->GetNumberOfComponents() ;
    CPPUNIT_ASSERT(ncomp == ncomp_in) ;

    int ntuples = polydata->GetCellData()->GetArray(i)->GetNumberOfTuples() ;
    for (j = 0 ;  j < ntuples ;  j++){
      polydata->GetCellData()->GetArray(i)->GetTuple(j, tuple) ;
      id0 = MeshCutter->GetInputCellCutByOutputCell(j) ;
      UG->GetCellData()->GetArray(i)->GetTuple(id0, tup0) ;

      for (k = 0 ;  k < ncomp ;  k++){
        if (tuple[k] != tup0[k]){
          std::cout << "scalar interpolation problem at array " << i << " tuple " << j << " component " << k << std::endl ;
          std::cout << "value is " << tuple[k] << " expected " << tup0[k] << std::endl ;
          CPPUNIT_ASSERT(false) ;
        }
      }
    }
  }
}




//------------------------------------------------------------------------------
// Test filter with hex8 data (two cube cells)
// Basic slice through centre of both cubes
// The plane cuts the edges {0,2}, {3,8}, {1,10}, {11,4}, {7,5} and {6,9}
void vtkMAFMeshCutterTest::TestGetOutputHex8() 
//------------------------------------------------------------------------------
{
  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/FEM/vtk/hex8" << ".vtk" << std::ends ;

  // read the data
  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(fname.str());
  
  // set the implicit function which defines the cut
  vtkTransform *T = vtkTransform::New();

  vtkPlane *P = vtkPlane::New();
  P->SetTransform(T);
  double pnorm[3] = {0.0, 1.0, 0.0} ;
  double porigin[3] = {1.0, 0.7, 0.7} ;
  P->SetNormal(pnorm) ;
  P->SetOrigin(porigin);

  vtkMAFMeshCutter *MeshCutter = vtkMAFMeshCutter::New();
  MeshCutter->SetCutFunction(P);
  MeshCutter->SetInput(reader->GetOutput());

  MeshCutter->GetOutput()->Update() ;
  vtkPolyData *polydata = MeshCutter->GetOutput() ;
  vtkUnstructuredGrid *UG = reader->GetOutput() ;


  // Get the statistics of the polydata
  int i ;

  // check no. of points
  int npts = polydata->GetPoints()->GetNumberOfPoints() ;
  CPPUNIT_ASSERT(npts == 6) ;

  // check no. of cells and type
  int ncells = polydata->GetNumberOfCells() ;
  CPPUNIT_ASSERT(ncells == 2) ;
  for (i = 0 ;  i < ncells ;  i++)
    CPPUNIT_ASSERT(polydata->GetCell(i)->GetCellType() == VTK_QUAD) ;


  // compare the bounds of the input and output
  double boundsin[6], boundsout[6] ;
  UG->GetBounds(boundsin) ;
  polydata->GetBounds(boundsout) ;
  CPPUNIT_ASSERT((boundsout[0] >= boundsin[0]) && (boundsout[1] <= boundsin[1])) ;
  CPPUNIT_ASSERT((boundsout[2] >= boundsin[2]) && (boundsout[3] <= boundsin[3])) ;
  CPPUNIT_ASSERT((boundsout[4] >= boundsin[4]) && (boundsout[5] <= boundsin[5])) ;

  // check that the polydata points are in the plane
  double coords[3] ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;
    CPPUNIT_ASSERT(PointInPlane(coords, porigin, pnorm, FTOL)) ;
  }

  // check that the points are interpolated correctly
  double coords0[3], coords1[3] ;
  vtkIdType id0, id1 ;
  double lambda ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;

    // get coords of corresponding edge in input data
    MeshCutter->GetInputEdgeCutByPoint(i, &id0, &id1, &lambda) ;
    UG->GetPoint(id0, coords0) ;
    UG->GetPoint(id1, coords1) ;

    // test if polydata coords are in the interpolated position
    CPPUNIT_ASSERT(ColinearVectors(coords, coords0, coords1, lambda, FTOL)) ;
  }


  // test the scalars
  ScalarTest(MeshCutter, UG, polydata) ;


  // render the data
  if (renderingOn){
    RenderPointScalars(UG, polydata) ;
    RenderCellScalars(UG, polydata) ;
  }

  // delete vtk objects
  reader->Delete() ;
  T->Delete() ;
  P->Delete() ;
  MeshCutter->Delete() ;
}



//------------------------------------------------------------------------------
// Test filter with hex8 data (two cube cells)
// Vertical slice through one cube
// The plane cuts the edges {0,1}, {3,11}, {4,8}, {2,10}
void vtkMAFMeshCutterTest::TestGetOutputHex8_VerticalCut1() 
//------------------------------------------------------------------------------
{
  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/FEM/vtk/hex8" << ".vtk" << std::ends ;

  // read the data
  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(fname.str());

  // set the implicit function which defines the cut
  vtkTransform *T = vtkTransform::New();

  vtkPlane *P = vtkPlane::New();
  P->SetTransform(T);
  double pnorm[3] = {1.0, 0.0, 0.0} ;
  double porigin[3] = {0.5, 0, 0} ;
  P->SetNormal(pnorm) ;
  P->SetOrigin(porigin);

  vtkMAFMeshCutter *MeshCutter = vtkMAFMeshCutter::New();
  MeshCutter->SetCutFunction(P);
  MeshCutter->SetInput(reader->GetOutput());

  MeshCutter->GetOutput()->Update() ;
  vtkPolyData *polydata = MeshCutter->GetOutput() ;
  vtkUnstructuredGrid *UG = reader->GetOutput() ;


  // Get the statistics of the polydata
  int i ;

  // check no. of points
  int npts = polydata->GetPoints()->GetNumberOfPoints() ;
  CPPUNIT_ASSERT(npts == 4) ;

  // check no. of cells and type
  int ncells = polydata->GetNumberOfCells() ;
  CPPUNIT_ASSERT(ncells == 1) ;
  for (i = 0 ;  i < ncells ;  i++)
    CPPUNIT_ASSERT(polydata->GetCell(i)->GetCellType() == VTK_QUAD) ;


  // compare the bounds of the input and output
  double boundsin[6], boundsout[6] ;
  UG->GetBounds(boundsin) ;
  polydata->GetBounds(boundsout) ;
  CPPUNIT_ASSERT((boundsout[0] >= boundsin[0]) && (boundsout[1] <= boundsin[1])) ;
  CPPUNIT_ASSERT((boundsout[2] >= boundsin[2]) && (boundsout[3] <= boundsin[3])) ;
  CPPUNIT_ASSERT((boundsout[4] >= boundsin[4]) && (boundsout[5] <= boundsin[5])) ;

  // check that the polydata points are in the plane
  double coords[3] ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;
    CPPUNIT_ASSERT(PointInPlane(coords, porigin, pnorm, FTOL)) ;
  }

  // check that the points are interpolated correctly
  double coords0[3], coords1[3] ;
  vtkIdType id0, id1 ;
  double lambda ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;

    // get coords of corresponding edge in input data
    MeshCutter->GetInputEdgeCutByPoint(i, &id0, &id1, &lambda) ;
    UG->GetPoint(id0, coords0) ;
    UG->GetPoint(id1, coords1) ;

    // test if polydata coords are in the interpolated position
    CPPUNIT_ASSERT(ColinearVectors(coords, coords0, coords1, lambda, FTOL)) ;
  }


  // test the scalars
  ScalarTest(MeshCutter, UG, polydata) ;


  // render the data
  if (renderingOn){
    RenderPointScalars(UG, polydata) ;
    RenderCellScalars(UG, polydata) ;
  }

  // delete vtk objects
  reader->Delete() ;
  T->Delete() ;
  P->Delete() ;
  MeshCutter->Delete() ;
}



//------------------------------------------------------------------------------
// Test filter with hex8 data (two cube cells)
// Vertical slice through one cube
// The plane cuts the edges {4,9}, {5,10}, {6,11}, {7,9}
void vtkMAFMeshCutterTest::TestGetOutputHex8_VerticalCut2() 
//------------------------------------------------------------------------------
{
  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/FEM/vtk/hex8" << ".vtk" << std::ends ;

  // read the data
  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(fname.str());

  // set the implicit function which defines the cut
  vtkTransform *T = vtkTransform::New();

  vtkPlane *P = vtkPlane::New();
  P->SetTransform(T);
  double pnorm[3] = {1.0, 0.0, 0.0} ;
  double porigin[3] = {1.5, 0, 0} ;
  P->SetNormal(pnorm) ;
  P->SetOrigin(porigin);

  vtkMAFMeshCutter *MeshCutter = vtkMAFMeshCutter::New();
  MeshCutter->SetCutFunction(P);
  MeshCutter->SetInput(reader->GetOutput());

  MeshCutter->GetOutput()->Update() ;
  vtkPolyData *polydata = MeshCutter->GetOutput() ;
  vtkUnstructuredGrid *UG = reader->GetOutput() ;


  // Get the statistics of the polydata
  int i ;

  // check no. of points
  int npts = polydata->GetPoints()->GetNumberOfPoints() ;
  CPPUNIT_ASSERT(npts == 4) ;

  // check no. of cells and type
  int ncells = polydata->GetNumberOfCells() ;
  CPPUNIT_ASSERT(ncells == 1) ;
  for (i = 0 ;  i < ncells ;  i++)
    CPPUNIT_ASSERT(polydata->GetCell(i)->GetCellType() == VTK_QUAD) ;


  // compare the bounds of the input and output
  double boundsin[6], boundsout[6] ;
  UG->GetBounds(boundsin) ;
  polydata->GetBounds(boundsout) ;
  CPPUNIT_ASSERT((boundsout[0] >= boundsin[0]) && (boundsout[1] <= boundsin[1])) ;
  CPPUNIT_ASSERT((boundsout[2] >= boundsin[2]) && (boundsout[3] <= boundsin[3])) ;
  CPPUNIT_ASSERT((boundsout[4] >= boundsin[4]) && (boundsout[5] <= boundsin[5])) ;

  // check that the polydata points are in the plane
  double coords[3] ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;
    CPPUNIT_ASSERT(PointInPlane(coords, porigin, pnorm, FTOL)) ;
  }

  // check that the points are interpolated correctly
  double coords0[3], coords1[3] ;
  vtkIdType id0, id1 ;
  double lambda ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;

    // get coords of corresponding edge in input data
    MeshCutter->GetInputEdgeCutByPoint(i, &id0, &id1, &lambda) ;
    UG->GetPoint(id0, coords0) ;
    UG->GetPoint(id1, coords1) ;

    // test if polydata coords are in the interpolated position
    CPPUNIT_ASSERT(ColinearVectors(coords, coords0, coords1, lambda, FTOL)) ;
  }


  // test the scalars
  ScalarTest(MeshCutter, UG, polydata) ;


  // render the data
  if (renderingOn){
    RenderPointScalars(UG, polydata) ;
    RenderCellScalars(UG, polydata) ;
  }

  // delete vtk objects
  reader->Delete() ;
  T->Delete() ;
  P->Delete() ;
  MeshCutter->Delete() ;
}




//------------------------------------------------------------------------------
// Test filter when cutting plane contains the faces of the cubes
// The plane contains the faces {1,7,6,11} and {0,1,11,3}
void vtkMAFMeshCutterTest::TestGetOutputHex8_FaceInPlane() 
//------------------------------------------------------------------------------
{
  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/FEM/vtk/hex8" << ".vtk" << std::ends ;

  // read the data
  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(fname.str());

  // set the implicit function which defines the cut
  vtkTransform *T = vtkTransform::New();

  vtkPlane *P = vtkPlane::New();
  P->SetTransform(T);
  double pnorm[3] = {0.0, 1.0, 0.0} ;
  double porigin[3] = {0.0, 0.0, 0.0} ;
  P->SetNormal(pnorm) ;
  P->SetOrigin(porigin);

  vtkMAFMeshCutter *MeshCutter = vtkMAFMeshCutter::New();
  MeshCutter->SetCutFunction(P);
  MeshCutter->SetInput(reader->GetOutput());

  MeshCutter->GetOutput()->Update() ;
  vtkPolyData *polydata = MeshCutter->GetOutput() ;
  vtkUnstructuredGrid *UG = reader->GetOutput() ;


  // Get the statistics of the polydata
  int i ;

  // check no. of points
  int npts = polydata->GetPoints()->GetNumberOfPoints() ;
  CPPUNIT_ASSERT(npts == 6) ;

  // check no. of cells and type
  int ncells = polydata->GetNumberOfCells() ;
  CPPUNIT_ASSERT(ncells == 2) ;
  for (i = 0 ;  i < ncells ;  i++)
    CPPUNIT_ASSERT(polydata->GetCell(i)->GetCellType() == VTK_QUAD) ;


  // compare the bounds of the input and output
  double boundsin[6], boundsout[6] ;
  UG->GetBounds(boundsin) ;
  polydata->GetBounds(boundsout) ;
  CPPUNIT_ASSERT((boundsout[0] >= boundsin[0]) && (boundsout[1] <= boundsin[1])) ;
  CPPUNIT_ASSERT((boundsout[2] >= boundsin[2]) && (boundsout[3] <= boundsin[3])) ;
  CPPUNIT_ASSERT((boundsout[4] >= boundsin[4]) && (boundsout[5] <= boundsin[5])) ;

  // check that the polydata points are in the plane
  double coords[3] ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;
    CPPUNIT_ASSERT(PointInPlane(coords, porigin, pnorm, FTOL)) ;
  }

  // check that the points are interpolated correctly
  // note that the plane cuts points in this case - not edges
  double coords0[3] ;
  vtkIdType id0 ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;

    // get coords of corresponding edge in input data
    MeshCutter->GetInputPointCutByPoint(i, &id0) ;
    UG->GetPoint(id0, coords0) ;

    // test if polydata coords are in the interpolated position
    CPPUNIT_ASSERT(PointsEqual(coords, coords0, FTOL)) ;
  }


  // test the scalars
  ScalarTest(MeshCutter, UG, polydata) ;


  // render the data
  if (renderingOn){
    RenderPointScalars(UG, polydata) ;
    RenderCellScalars(UG, polydata) ;
  }

  // delete vtk objects
  reader->Delete() ;
  T->Delete() ;
  P->Delete() ;
  MeshCutter->Delete() ;
}





//------------------------------------------------------------------------------
// Test filter when plane includes an edge.
// The plane contains the edge {0,3} and cuts the edges {2,10} and {4,8}
void vtkMAFMeshCutterTest::TestGetOutputHex8_IncludesEdge1() 
//------------------------------------------------------------------------------
{
  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/FEM/vtk/hex8" << ".vtk" << std::ends ;

  // read the data
  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(fname.str());

  // set the implicit function which defines the cut
  vtkTransform *T = vtkTransform::New();

  vtkPlane *P = vtkPlane::New();
  P->SetTransform(T);
  double pnorm[3] = {0.8, -0.2, 0.0} ;
  double porigin[3] = {0.0, 0.0, 0.0} ;
  P->SetNormal(pnorm) ;
  P->SetOrigin(porigin);

  vtkMAFMeshCutter *MeshCutter = vtkMAFMeshCutter::New();
  MeshCutter->SetCutFunction(P);
  MeshCutter->SetInput(reader->GetOutput());

  MeshCutter->GetOutput()->Update() ;
  vtkPolyData *polydata = MeshCutter->GetOutput() ;
  vtkUnstructuredGrid *UG = reader->GetOutput() ;


  // Get the statistics of the polydata
  int i ;

  // check no. of points
  int npts = polydata->GetPoints()->GetNumberOfPoints() ;
  CPPUNIT_ASSERT(npts == 4) ;

  // check no. of cells and type
  int ncells = polydata->GetNumberOfCells() ;
  CPPUNIT_ASSERT(ncells == 1) ;
  for (i = 0 ;  i < ncells ;  i++)
    CPPUNIT_ASSERT(polydata->GetCell(i)->GetCellType() == VTK_QUAD) ;


  // compare the bounds of the input and output
  double boundsin[6], boundsout[6] ;
  UG->GetBounds(boundsin) ;
  polydata->GetBounds(boundsout) ;
  CPPUNIT_ASSERT((boundsout[0] >= boundsin[0]) && (boundsout[1] <= boundsin[1])) ;
  CPPUNIT_ASSERT((boundsout[2] >= boundsin[2]) && (boundsout[3] <= boundsin[3])) ;
  CPPUNIT_ASSERT((boundsout[4] >= boundsin[4]) && (boundsout[5] <= boundsin[5])) ;

  // check that the polydata points are in the plane
  double coords[3] ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;
    CPPUNIT_ASSERT(PointInPlane(coords, porigin, pnorm, FTOL)) ;
  }


  // Check that the points are interpolated correctly
  // In this case the mesh is cut through both points and edges
  double coords0[3], coords1[3] ;
  vtkIdType id0, id1 ;
  double lambda ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;

    // get coords of corresponding point or edge in input data
    if (MeshCutter->GetInputPointCutByPoint(i, &id0)){
      UG->GetPoint(id0, coords0) ;
      CPPUNIT_ASSERT(PointsEqual(coords, coords0, FTOL)) ;
    }
    else if (MeshCutter->GetInputEdgeCutByPoint(i, &id0, &id1, &lambda)){
      UG->GetPoint(id0, coords0) ;
      UG->GetPoint(id1, coords1) ;
      CPPUNIT_ASSERT(ColinearVectors(coords, coords0, coords1, lambda, FTOL)) ;
    }
    else
      CPPUNIT_ASSERT(false) ;
  }


  // test the scalars
  ScalarTest(MeshCutter, UG, polydata) ;


  // render the data
  if (renderingOn){
    RenderPointScalars(UG, polydata) ;
    RenderCellScalars(UG, polydata) ;
  }

  // delete vtk objects
  reader->Delete() ;
  T->Delete() ;
  P->Delete() ;
  MeshCutter->Delete() ;
}




//------------------------------------------------------------------------------
// Test filter when plane includes an edge.
// The plane contains the edge {0,3} and cuts the edges {4,11}, {1,10}, {4,9} and {5,10}
void vtkMAFMeshCutterTest::TestGetOutputHex8_IncludesEdge2() 
//------------------------------------------------------------------------------
{
  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/FEM/vtk/hex8" << ".vtk" << std::ends ;

  // read the data
  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(fname.str());

  // set the implicit function which defines the cut
  vtkTransform *T = vtkTransform::New();

  vtkPlane *P = vtkPlane::New();
  P->SetTransform(T);
  double pnorm[3] = {0.4, -0.6, 0.0} ;
  double porigin[3] = {0.0, 0.0, 0.0} ;
  P->SetNormal(pnorm) ;
  P->SetOrigin(porigin);

  vtkMAFMeshCutter *MeshCutter = vtkMAFMeshCutter::New();
  MeshCutter->SetCutFunction(P);
  MeshCutter->SetInput(reader->GetOutput());

  MeshCutter->GetOutput()->Update() ;
  vtkPolyData *polydata = MeshCutter->GetOutput() ;
  vtkUnstructuredGrid *UG = reader->GetOutput() ;



  // Get the statistics of the polydata
  int i ;

  // check no. of points
  int npts = polydata->GetPoints()->GetNumberOfPoints() ;
  CPPUNIT_ASSERT(npts == 6) ;

  // check no. of cells and type
  int ncells = polydata->GetNumberOfCells() ;
  CPPUNIT_ASSERT(ncells == 2) ;
  for (i = 0 ;  i < ncells ;  i++)
    CPPUNIT_ASSERT(polydata->GetCell(i)->GetCellType() == VTK_QUAD) ;

  // compare the bounds of the input and output
  double boundsin[6], boundsout[6] ;
  UG->GetBounds(boundsin) ;
  polydata->GetBounds(boundsout) ;
  CPPUNIT_ASSERT((boundsout[0] >= boundsin[0]) && (boundsout[1] <= boundsin[1])) ;
  CPPUNIT_ASSERT((boundsout[2] >= boundsin[2]) && (boundsout[3] <= boundsin[3])) ;
  CPPUNIT_ASSERT((boundsout[4] >= boundsin[4]) && (boundsout[5] <= boundsin[5])) ;

  // check that the polydata points are in the plane
  double coords[3] ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;
    CPPUNIT_ASSERT(PointInPlane(coords, porigin, pnorm, FTOL)) ;
  }


  // Check that the points are interpolated correctly
  // In this case the mesh is cut through both points and edges
  double coords0[3], coords1[3] ;
  vtkIdType id0, id1 ;
  double lambda ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;

    // get coords of corresponding point or edge in input data
    if (MeshCutter->GetInputPointCutByPoint(i, &id0)){
      UG->GetPoint(id0, coords0) ;
      CPPUNIT_ASSERT(PointsEqual(coords, coords0, FTOL)) ;
    }
    else if (MeshCutter->GetInputEdgeCutByPoint(i, &id0, &id1, &lambda)){
      UG->GetPoint(id0, coords0) ;
      UG->GetPoint(id1, coords1) ;
      CPPUNIT_ASSERT(ColinearVectors(coords, coords0, coords1, lambda, FTOL)) ;
    }
    else
      CPPUNIT_ASSERT(false) ;
  }


  // test the scalars
  ScalarTest(MeshCutter, UG, polydata) ;


  // render the data
  if (renderingOn){
    RenderPointScalars(UG, polydata) ;
    RenderCellScalars(UG, polydata) ;
  }

  // delete vtk objects
  reader->Delete() ;
  T->Delete() ;
  P->Delete() ;
  MeshCutter->Delete() ;
}




//------------------------------------------------------------------------------
// Test filter when cutting plane includes a corner of the cube
// The plane cuts the corner point 0, and the edges {8,3}, {8,4}, {10, 1}, {10, 4} and {10, 5}
void vtkMAFMeshCutterTest::TestGetOutputHex8_IncludesCorner() 
//------------------------------------------------------------------------------
{
  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/FEM/vtk/hex8" << ".vtk" << std::ends ;

  // read the data
  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(fname.str());

  // set the implicit function which defines the cut
  vtkTransform *T = vtkTransform::New();

  vtkPlane *P = vtkPlane::New();
  P->SetTransform(T);
  double pnorm[3] = {-0.8, 1.0, -0.8} ;
  double porigin[3] = {0.0, 0.0, 0.0} ;
  P->SetNormal(pnorm) ;
  P->SetOrigin(porigin);

  vtkMAFMeshCutter *MeshCutter = vtkMAFMeshCutter::New();
  MeshCutter->SetCutFunction(P);
  MeshCutter->SetInput(reader->GetOutput());

  MeshCutter->GetOutput()->Update() ;
  vtkPolyData *polydata = MeshCutter->GetOutput() ;
  vtkUnstructuredGrid *UG = reader->GetOutput() ;


  // Get the statistics of the polydata
  int i ;

  // check no. of points
  int npts = polydata->GetPoints()->GetNumberOfPoints() ;
  CPPUNIT_ASSERT(npts == 6) ;

  // check no. of cells and type
  int ncells = polydata->GetNumberOfCells() ;
  CPPUNIT_ASSERT(ncells == 2) ;
  CPPUNIT_ASSERT(polydata->GetCell(0)->GetCellType() == VTK_TRIANGLE) ;
  CPPUNIT_ASSERT(polydata->GetCell(1)->GetCellType() == VTK_POLYGON) ;

  // compare the bounds of the input and output
  double boundsin[6], boundsout[6] ;
  UG->GetBounds(boundsin) ;
  polydata->GetBounds(boundsout) ;
  CPPUNIT_ASSERT((boundsout[0] >= boundsin[0]) && (boundsout[1] <= boundsin[1])) ;
  CPPUNIT_ASSERT((boundsout[2] >= boundsin[2]) && (boundsout[3] <= boundsin[3])) ;
  CPPUNIT_ASSERT((boundsout[4] >= boundsin[4]) && (boundsout[5] <= boundsin[5])) ;

  // check that the polydata points are in the plane
  double coords[3] ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;
    CPPUNIT_ASSERT(PointInPlane(coords, porigin, pnorm, FTOL)) ;
  }

  // Check that the points are interpolated correctly
  // In this case the mesh is cut through both points and edges
  double coords0[3], coords1[3] ;
  vtkIdType id0, id1 ;
  double lambda ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;

    // get coords of corresponding point or edge in input data
    if (MeshCutter->GetInputPointCutByPoint(i, &id0)){
      UG->GetPoint(id0, coords0) ;
      CPPUNIT_ASSERT(PointsEqual(coords, coords0, FTOL)) ;
    }
    else if (MeshCutter->GetInputEdgeCutByPoint(i, &id0, &id1, &lambda)){
      UG->GetPoint(id0, coords0) ;
      UG->GetPoint(id1, coords1) ;
      CPPUNIT_ASSERT(ColinearVectors(coords, coords0, coords1, lambda, FTOL)) ;
    }
    else
      CPPUNIT_ASSERT(false) ;
  }


  // test the scalars
  ScalarTest(MeshCutter, UG, polydata) ;


  // render the data
  if (renderingOn){
    RenderPointScalars(UG, polydata) ;
    RenderCellScalars(UG, polydata) ;
  }

  // delete vtk objects
  reader->Delete() ;
  T->Delete() ;
  P->Delete() ;
  MeshCutter->Delete() ;
}




//------------------------------------------------------------------------------
// Test filter when cutting plane contains only one edge
// The plane contains the edge {0,3}
void vtkMAFMeshCutterTest::TestGetOutputHex8_EdgeOnly() 
//------------------------------------------------------------------------------
{
  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/FEM/vtk/hex8" << ".vtk" << std::ends ;

  // read the data
  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(fname.str());

  // set the implicit function which defines the cut
  vtkTransform *T = vtkTransform::New();

  vtkPlane *P = vtkPlane::New();
  P->SetTransform(T);
  double pnorm[3] = {1.0, 1.0, 0.0} ;
  double porigin[3] = {0.0, 0.0, 0.0} ;
  P->SetNormal(pnorm) ;
  P->SetOrigin(porigin);

  vtkMAFMeshCutter *MeshCutter = vtkMAFMeshCutter::New();
  MeshCutter->SetCutFunction(P);
  MeshCutter->SetInput(reader->GetOutput());

  MeshCutter->GetOutput()->Update() ;
  vtkPolyData *polydata = MeshCutter->GetOutput() ;
  vtkUnstructuredGrid *UG = reader->GetOutput() ;


  // Get the statistics of the polydata

  // check no. of points
  int npts = polydata->GetPoints()->GetNumberOfPoints() ;
  CPPUNIT_ASSERT(npts == 2) ;

  // check no. of cells and type
  int ncells = polydata->GetNumberOfCells() ;
  CPPUNIT_ASSERT(ncells == 0) ;

  // delete vtk objects
  reader->Delete() ;
  T->Delete() ;
  P->Delete() ;
  MeshCutter->Delete() ;
}


//------------------------------------------------------------------------------
// Test filter when cutting plane contains only one corner
// The plane contains the corner 0
void vtkMAFMeshCutterTest::TestGetOutputHex8_CornerOnly() 
//------------------------------------------------------------------------------
{
  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/FEM/vtk/hex8" << ".vtk" << std::ends ;

  // read the data
  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(fname.str());

  // set the implicit function which defines the cut
  vtkTransform *T = vtkTransform::New();

  vtkPlane *P = vtkPlane::New();
  P->SetTransform(T);
  double pnorm[3] = {1.0, 1.0, 1.0} ;
  double porigin[3] = {0.0, 0.0, 0.0} ;
  P->SetNormal(pnorm) ;
  P->SetOrigin(porigin);

  vtkMAFMeshCutter *MeshCutter = vtkMAFMeshCutter::New();
  MeshCutter->SetCutFunction(P);
  MeshCutter->SetInput(reader->GetOutput());

  MeshCutter->GetOutput()->Update() ;
  vtkPolyData *polydata = MeshCutter->GetOutput() ;
  vtkUnstructuredGrid *UG = reader->GetOutput() ;


  // Get the statistics of the polydata

  // check no. of points
  int npts = polydata->GetPoints()->GetNumberOfPoints() ;
  CPPUNIT_ASSERT(npts == 1) ;

  // check no. of cells and type
  int ncells = polydata->GetNumberOfCells() ;
  CPPUNIT_ASSERT(ncells == 0) ;

  // delete vtk objects
  reader->Delete() ;
  T->Delete() ;
  P->Delete() ;
  MeshCutter->Delete() ;
}



//------------------------------------------------------------------------------
// Test filter with test4 data (two tetrahedra)
void vtkMAFMeshCutterTest::TestGetOutputTet4() 
//------------------------------------------------------------------------------
{
  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/FEM/vtk/tet4" << ".vtk" << std::ends ;

  // read the data
  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(fname.str());

  // set the implicit function which defines the cut
  vtkTransform *T = vtkTransform::New();

  vtkPlane *P = vtkPlane::New();
  P->SetTransform(T);
  double pnorm[3] = {0.0, 1.0, 0.0} ;
  double porigin[3] = {0.5, 0.5, 0.5} ;
  P->SetNormal(pnorm) ;
  P->SetOrigin(porigin);

  vtkMAFMeshCutter *MeshCutter = vtkMAFMeshCutter::New();
  MeshCutter->SetCutFunction(P);
  MeshCutter->SetInput(reader->GetOutput());

  MeshCutter->GetOutput()->Update() ;
  vtkPolyData *polydata = MeshCutter->GetOutput() ;
  vtkUnstructuredGrid *UG = reader->GetOutput() ;



  // Get the statistics of the polydata
  int i ;

  // check no. of points
  int npts = polydata->GetPoints()->GetNumberOfPoints() ;
  CPPUNIT_ASSERT(npts == 5) ;

  // check no. of cells and type (should be one triangle and one quad)
  int ncells = polydata->GetNumberOfCells() ;
  CPPUNIT_ASSERT(ncells == 2) ;
  CPPUNIT_ASSERT(polydata->GetCell(0)->GetCellType() == VTK_TRIANGLE) ;
  CPPUNIT_ASSERT(polydata->GetCell(1)->GetCellType() == VTK_QUAD) ;

  // compare the bounds of the input and output
  double boundsin[6], boundsout[6] ;
  UG->GetBounds(boundsin) ;
  polydata->GetBounds(boundsout) ;
  CPPUNIT_ASSERT((boundsout[0] >= boundsin[0]) && (boundsout[1] <= boundsin[1])) ;
  CPPUNIT_ASSERT((boundsout[2] >= boundsin[2]) && (boundsout[3] <= boundsin[3])) ;
  CPPUNIT_ASSERT((boundsout[4] >= boundsin[4]) && (boundsout[5] <= boundsin[5])) ;

  // check that the polydata points are in the plane
  double coords[3] ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;
    CPPUNIT_ASSERT(PointInPlane(coords, porigin, pnorm, FTOL)) ;
  }

  // check that the points are interpolated correctly
  double coords0[3], coords1[3] ;
  vtkIdType id0, id1 ;
  double lambda ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;

    // get coords of corresponding edge in input data
    MeshCutter->GetInputEdgeCutByPoint(i, &id0, &id1, &lambda) ;
    UG->GetPoint(id0, coords0) ;
    UG->GetPoint(id1, coords1) ;

    // test if polydata coords are in the interpolated position
    CPPUNIT_ASSERT(ColinearVectors(coords, coords0, coords1, lambda, FTOL)) ;
  }


  // render the data
  if (renderingOn){
    RenderPointScalars(UG, polydata) ;
    RenderCellScalars(UG, polydata) ;
  }

  // delete vtk objects
  reader->Delete() ;
  T->Delete() ;
  P->Delete() ;
  MeshCutter->Delete() ;
}




//------------------------------------------------------------------------------
// Test that the cutter updates and executes properly when the cutting function changes
// It runs the tests GetOutputHex8_VerticalCut1() and GetOutputHex8_VerticalCut2() in series
void vtkMAFMeshCutterTest::TestUpdateChangeCutFunction() 
//------------------------------------------------------------------------------
{
  //----------------------------------------------------------------------------
  // 1. Run test Hex8_VerticalCut1()
  //----------------------------------------------------------------------------
  std::cout << std::endl ;
  std::cout << "  cutting plane position 1..." << std::endl ;

  // set filename
  std::ostrstream fname ;
  fname << MAF_DATA_ROOT << "/FEM/vtk/hex8" << ".vtk" << std::ends ;

  // read the data
  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(fname.str());

  // set the implicit function which defines the cut
  vtkTransform *T = vtkTransform::New();

  vtkPlane *P = vtkPlane::New();
  P->SetTransform(T);
  double pnorm1[3] = {1.0, 0.0, 0.0} ;
  double porigin1[3] = {0.5, 0, 0} ;
  P->SetNormal(pnorm1) ;
  P->SetOrigin(porigin1);

  vtkMAFMeshCutter *MeshCutter = vtkMAFMeshCutter::New();
  MeshCutter->SetCutFunction(P);
  MeshCutter->SetInput(reader->GetOutput());

  // force update so that the output is there
  MeshCutter->Update() ;

  vtkPolyData *polydata = MeshCutter->GetOutput() ;
  vtkUnstructuredGrid *UG = reader->GetOutput() ;


  // Get the statistics of the polydata
  int i ;

  // check no. of points
  int npts = polydata->GetPoints()->GetNumberOfPoints() ;
  CPPUNIT_ASSERT(npts == 4) ;

  // check no. of cells and type
  int ncells = polydata->GetNumberOfCells() ;
  CPPUNIT_ASSERT(ncells == 1) ;
  for (i = 0 ;  i < ncells ;  i++)
    CPPUNIT_ASSERT(polydata->GetCell(i)->GetCellType() == VTK_QUAD) ;


  // compare the bounds of the input and output
  double boundsin[6], boundsout[6] ;
  UG->GetBounds(boundsin) ;
  polydata->GetBounds(boundsout) ;
  CPPUNIT_ASSERT((boundsout[0] >= boundsin[0]) && (boundsout[1] <= boundsin[1])) ;
  CPPUNIT_ASSERT((boundsout[2] >= boundsin[2]) && (boundsout[3] <= boundsin[3])) ;
  CPPUNIT_ASSERT((boundsout[4] >= boundsin[4]) && (boundsout[5] <= boundsin[5])) ;

  // check that the polydata points are in the plane
  double coords[3] ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;
    CPPUNIT_ASSERT(PointInPlane(coords, porigin1, pnorm1, FTOL)) ;
  }

  // check that the points are interpolated correctly
  double coords0[3], coords1[3] ;
  vtkIdType id0, id1 ;
  double lambda ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;

    // get coords of corresponding edge in input data
    MeshCutter->GetInputEdgeCutByPoint(i, &id0, &id1, &lambda) ;
    UG->GetPoint(id0, coords0) ;
    UG->GetPoint(id1, coords1) ;

    // test if polydata coords are in the interpolated position
    CPPUNIT_ASSERT(ColinearVectors(coords, coords0, coords1, lambda, FTOL)) ;
  }


  // test the scalars
  ScalarTest(MeshCutter, UG, polydata) ;


  // render the data
  if (renderingOn){
    RenderCellScalars(UG, polydata) ;
  }

 

  //----------------------------------------------------------------------------
  // 2. Change plane and run test Hex8_VerticalCut2()
  //----------------------------------------------------------------------------
  std::cout << "  cutting plane position 2..." << std::endl ;

  // change the position of the plane
  double pnorm2[3] = {1.0, 0.0, 0.0} ;
  double porigin2[3] = {1.5, 0, 0} ;
  P->SetNormal(pnorm2) ;
  P->SetOrigin(porigin2);

  // force update so that the output is there
  MeshCutter->Update() ;



  // Get the statistics of the polydata

  // check no. of points
  npts = polydata->GetPoints()->GetNumberOfPoints() ;
  CPPUNIT_ASSERT(npts == 4) ;

  // check no. of cells and type
  ncells = polydata->GetNumberOfCells() ;
  CPPUNIT_ASSERT(ncells == 1) ;
  for (i = 0 ;  i < ncells ;  i++)
    CPPUNIT_ASSERT(polydata->GetCell(i)->GetCellType() == VTK_QUAD) ;


  // compare the bounds of the input and output
  UG->GetBounds(boundsin) ;
  polydata->GetBounds(boundsout) ;
  CPPUNIT_ASSERT((boundsout[0] >= boundsin[0]) && (boundsout[1] <= boundsin[1])) ;
  CPPUNIT_ASSERT((boundsout[2] >= boundsin[2]) && (boundsout[3] <= boundsin[3])) ;
  CPPUNIT_ASSERT((boundsout[4] >= boundsin[4]) && (boundsout[5] <= boundsin[5])) ;

  // check that the polydata points are in the plane
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;
    CPPUNIT_ASSERT(PointInPlane(coords, porigin2, pnorm2, FTOL)) ;
  }

  // check that the points are interpolated correctly
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;

    // get coords of corresponding edge in input data
    MeshCutter->GetInputEdgeCutByPoint(i, &id0, &id1, &lambda) ;
    UG->GetPoint(id0, coords0) ;
    UG->GetPoint(id1, coords1) ;

    // test if polydata coords are in the interpolated position
    CPPUNIT_ASSERT(ColinearVectors(coords, coords0, coords1, lambda, FTOL)) ;
  }


  // test the scalars
  ScalarTest(MeshCutter, UG, polydata) ;


  // render the data
  if (renderingOn){
    RenderCellScalars(UG, polydata) ;
  }

  // delete vtk objects
  reader->Delete() ;
  T->Delete() ;
  P->Delete() ;
  MeshCutter->Delete() ;
}





//------------------------------------------------------------------------------
// Test that the cutter updates and executes properly when the input changes
// It runs the tests GetOutputHex8 and GetOutputTet4 in series
void vtkMAFMeshCutterTest::TestUpdateChangeInput() 
//------------------------------------------------------------------------------
{
  //----------------------------------------------------------------------------
  // Run Hex8
  //----------------------------------------------------------------------------
  std::cout << std::endl ;
  std::cout << " running hex8 data..." << std::endl ;

  // set filename
  std::ostrstream fname1 ;
  fname1 << MAF_DATA_ROOT << "/FEM/vtk/hex8" << ".vtk" << std::ends ;

  // read the data
  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(fname1.str());

  // set the implicit function which defines the cut
  vtkTransform *T = vtkTransform::New();

  vtkPlane *P = vtkPlane::New();
  P->SetTransform(T);
  double pnorm[3] = {0.0, 1.0, 0.0} ;
  double porigin[3] = {1.0, 0.7, 0.7} ;
  P->SetNormal(pnorm) ;
  P->SetOrigin(porigin);

  vtkMAFMeshCutter *MeshCutter = vtkMAFMeshCutter::New();
  MeshCutter->SetCutFunction(P);
  MeshCutter->SetInput(reader->GetOutput());

  MeshCutter->GetOutput()->Update() ;
  vtkPolyData *polydata = MeshCutter->GetOutput() ;
  vtkUnstructuredGrid *UG = reader->GetOutput() ;


  // Get the statistics of the polydata
  int i ;

  // check no. of points
  int npts = polydata->GetPoints()->GetNumberOfPoints() ;
  CPPUNIT_ASSERT(npts == 6) ;

  // check no. of cells and type
  int ncells = polydata->GetNumberOfCells() ;
  CPPUNIT_ASSERT(ncells == 2) ;
  for (i = 0 ;  i < ncells ;  i++)
    CPPUNIT_ASSERT(polydata->GetCell(i)->GetCellType() == VTK_QUAD) ;


  // compare the bounds of the input and output
  double boundsin[6], boundsout[6] ;
  UG->GetBounds(boundsin) ;
  polydata->GetBounds(boundsout) ;
  CPPUNIT_ASSERT((boundsout[0] >= boundsin[0]) && (boundsout[1] <= boundsin[1])) ;
  CPPUNIT_ASSERT((boundsout[2] >= boundsin[2]) && (boundsout[3] <= boundsin[3])) ;
  CPPUNIT_ASSERT((boundsout[4] >= boundsin[4]) && (boundsout[5] <= boundsin[5])) ;

  // check that the polydata points are in the plane
  double coords[3] ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;
    CPPUNIT_ASSERT(PointInPlane(coords, porigin, pnorm, FTOL)) ;
  }

  // check that the points are interpolated correctly
  double coords0[3], coords1[3] ;
  vtkIdType id0, id1 ;
  double lambda ;
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;

    // get coords of corresponding edge in input data
    MeshCutter->GetInputEdgeCutByPoint(i, &id0, &id1, &lambda) ;
    UG->GetPoint(id0, coords0) ;
    UG->GetPoint(id1, coords1) ;

    // test if polydata coords are in the interpolated position
    CPPUNIT_ASSERT(ColinearVectors(coords, coords0, coords1, lambda, FTOL)) ;
  }


  // test the scalars
  ScalarTest(MeshCutter, UG, polydata) ;


  // render the data
  if (renderingOn){
    RenderCellScalars(UG, polydata) ;
  }



  //----------------------------------------------------------------------------
  // Run Tet4
  //----------------------------------------------------------------------------
  std::cout << " running tet4 data..." << std::endl ;

  // set filename
  std::ostrstream fname2 ;
  fname2 << MAF_DATA_ROOT << "/FEM/vtk/tet4" << ".vtk" << std::ends ;

  // read the data
  reader->SetFileName(fname2.str());

  // force update so that the output is there
  MeshCutter->Update() ;

  // Get the statistics of the polydata

  // check no. of points
  npts = polydata->GetPoints()->GetNumberOfPoints() ;
  CPPUNIT_ASSERT(npts == 5) ;

  // check no. of cells and type (should be one triangle and one quad)
  ncells = polydata->GetNumberOfCells() ;
  CPPUNIT_ASSERT(ncells == 2) ;
  CPPUNIT_ASSERT(polydata->GetCell(0)->GetCellType() == VTK_TRIANGLE) ;
  CPPUNIT_ASSERT(polydata->GetCell(1)->GetCellType() == VTK_QUAD) ;

  // compare the bounds of the input and output
  UG->GetBounds(boundsin) ;
  polydata->GetBounds(boundsout) ;
  CPPUNIT_ASSERT((boundsout[0] >= boundsin[0]) && (boundsout[1] <= boundsin[1])) ;
  CPPUNIT_ASSERT((boundsout[2] >= boundsin[2]) && (boundsout[3] <= boundsin[3])) ;
  CPPUNIT_ASSERT((boundsout[4] >= boundsin[4]) && (boundsout[5] <= boundsin[5])) ;

  // check that the polydata points are in the plane
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;
    CPPUNIT_ASSERT(PointInPlane(coords, porigin, pnorm, FTOL)) ;
  }

  // check that the points are interpolated correctly
  for (i = 0 ;  i < npts ;  i++){
    // get coords of polydata point
    polydata->GetPoint(i, coords) ;

    // get coords of corresponding edge in input data
    MeshCutter->GetInputEdgeCutByPoint(i, &id0, &id1, &lambda) ;
    UG->GetPoint(id0, coords0) ;
    UG->GetPoint(id1, coords1) ;

    // test if polydata coords are in the interpolated position
    //CPPUNIT_ASSERT(ColinearVectors(coords, coords0, coords1, lambda, FTOL)) ;
  }


  // render the data
  if (renderingOn){
    RenderCellScalars(UG, polydata) ;
  }

  // delete vtk objects
  reader->Delete() ;
  T->Delete() ;
  P->Delete() ;
  MeshCutter->Delete() ;
}
