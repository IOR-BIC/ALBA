/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFilterSurfaceTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaOpFilterSurfaceTest.h"

#include "albaOpFilterSurface.h"
#include "albaVMEGroup.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEImage.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMEGizmo.h"
#include "albaVMERefSys.h"
#include "albaVMESlicer.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMEPolyline.h"
#include "albaVMESurface.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"
#include "vtkCleanPolyData.h"
#include "vtkCell.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkDecimatePro.h"
#include "vtkTriangleFilter.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkStripper.h"
#include "vtkPolyDataNormals.h"

//----------------------------------------------------------------------------
void albaOpFilterSurfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpFilterSurfaceTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaOpFilterSurface op;
}
//----------------------------------------------------------------------------
void albaOpFilterSurfaceTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOpFilterSurface *op = new albaOpFilterSurface();
  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterSurfaceTest::CheckPolydata(vtkPolyData *polydataFromOperation,vtkPolyData *polydataFromFilter)
//----------------------------------------------------------------------------
{
  CPPUNIT_ASSERT( polydataFromOperation->GetNumberOfPoints() == polydataFromFilter->GetNumberOfPoints() );

  //Check point to point if the surface are equals
  for (int i=0;i<polydataFromFilter->GetNumberOfPoints();i++)
  {
    CPPUNIT_ASSERT( polydataFromOperation->GetPoint(i)[0] == polydataFromFilter->GetPoint(i)[0] );
    CPPUNIT_ASSERT( polydataFromOperation->GetPoint(i)[1] == polydataFromFilter->GetPoint(i)[1] );
    CPPUNIT_ASSERT( polydataFromOperation->GetPoint(i)[2] == polydataFromFilter->GetPoint(i)[2] );
  }

  CPPUNIT_ASSERT( polydataFromOperation->GetNumberOfCells() == polydataFromFilter->GetNumberOfCells() );

  //Check cell to cell if the surface are equals
  for (int i=0;i<polydataFromFilter->GetNumberOfCells();i++)
  {
    vtkCell *cellVtkFilter = polydataFromFilter->GetCell(i);
    vtkCell *cellOp = polydataFromOperation->GetCell(i);

    CPPUNIT_ASSERT( cellOp->GetNumberOfPoints() == cellVtkFilter->GetNumberOfPoints() );

    for (int j=0;cellOp->GetNumberOfPoints()>j;j++)
    {
      CPPUNIT_ASSERT( cellOp->GetPointId(j) == cellVtkFilter->GetPointId(j) );
    }

  }
}
//----------------------------------------------------------------------------
void albaOpFilterSurfaceTest::CheckNormals(vtkPolyData *polydataFromOperation,vtkPolyData *polydataFromFilter)
//----------------------------------------------------------------------------
{
  if (polydataFromOperation->GetCellData()->GetNormals())
  {
    CPPUNIT_ASSERT(polydataFromOperation->GetCellData()->GetNormals()->GetNumberOfTuples() == polydataFromFilter->GetCellData()->GetNormals()->GetNumberOfTuples());
    vtkDataArray *normalsVtkFilter = polydataFromFilter->GetCellData()->GetNormals();
    vtkDataArray *normalsOp = polydataFromOperation->GetCellData()->GetNormals();

    for (int i=0;i<normalsOp->GetNumberOfTuples();i++)
    {
      CPPUNIT_ASSERT( normalsOp->GetTuple3(i)[0] == normalsVtkFilter->GetTuple3(i)[0] );
      CPPUNIT_ASSERT( normalsOp->GetTuple3(i)[1] == normalsVtkFilter->GetTuple3(i)[1] );
      CPPUNIT_ASSERT( normalsOp->GetTuple3(i)[2] == normalsVtkFilter->GetTuple3(i)[2] );
    }
  }
  else
  {
    CPPUNIT_ASSERT(polydataFromOperation->GetPointData()->GetNormals()->GetNumberOfTuples() == polydataFromFilter->GetPointData()->GetNormals()->GetNumberOfTuples());

    vtkDataArray *normalsVtkFilter = polydataFromFilter->GetPointData()->GetNormals();
    vtkDataArray *normalsOp = polydataFromOperation->GetPointData()->GetNormals();

    for (int i=0;i<normalsOp->GetNumberOfTuples();i++)
    {
      CPPUNIT_ASSERT( normalsOp->GetTuple3(i)[0] == normalsVtkFilter->GetTuple3(i)[0] );
      CPPUNIT_ASSERT( normalsOp->GetTuple3(i)[1] == normalsVtkFilter->GetTuple3(i)[1] );
      CPPUNIT_ASSERT( normalsOp->GetTuple3(i)[2] == normalsVtkFilter->GetTuple3(i)[2] );
    }
  }
}
//----------------------------------------------------------------------------
void albaOpFilterSurfaceTest::TestAccept()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMESurface> surface;
  albaOpFilterSurface *op = new albaOpFilterSurface();

  CPPUNIT_ASSERT( op->Accept(surface) );

  albaSmartPointer<albaVMEVolumeGray> volume;
  albaSmartPointer<albaVMEGroup> group;
  albaSmartPointer<albaVMESlicer> slicer;
  albaSmartPointer<albaVMEPolyline> poly;
  albaSmartPointer<albaVMEImage> image;
  albaSmartPointer<albaVMERefSys> refSys;
  albaSmartPointer<albaVMESurfaceParametric> paramSurface;
  albaSmartPointer<albaVMEGizmo> gizmo;
  albaSmartPointer<albaVMELandmark> landmark;
  albaSmartPointer<albaVMELandmarkCloud> landmarkCloud;

  //Only surface is accepted
  CPPUNIT_ASSERT( !(op->Accept(landmark) && op->Accept(landmarkCloud) && op->Accept(gizmo) && op->Accept(paramSurface) && op->Accept(volume) && op->Accept(group) && op->Accept(slicer) && op->Accept(poly) && op->Accept(image) && op->Accept(refSys)) );

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterSurfaceTest::TestOnClean()
//----------------------------------------------------------------------------
{
  albaOpFilterSurface *op = new albaOpFilterSurface();
  op->TestModeOn();

  albaString fileName = ALBA_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToClean.vtk";
  vtkALBASmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT( polyIn->GetNumberOfPoints() == 24 );

  vtkALBASmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);

  op->SetInput(surface);
  op->OpRun();
  op->OnClean();
  op->OpDo();

  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  CPPUNIT_ASSERT( polyOut->GetNumberOfPoints() == 8 );

  vtkALBASmartPointer<vtkCleanPolyData> cleanFilter;
  cleanFilter->SetTolerance(0.0);
  cleanFilter->SetInputData(inputVtkFilter);
  cleanFilter->Update();

  vtkPolyData *outputVtkFilter = cleanFilter->GetOutput();

  CheckPolydata(polyOut,outputVtkFilter);

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterSurfaceTest::TestOnSmooth()
//----------------------------------------------------------------------------
{
  albaOpFilterSurface *op = new albaOpFilterSurface();
  op->TestModeOn();

  albaString fileName = ALBA_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToSmooth.vtk";
  vtkALBASmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  vtkALBASmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);

  op->SetInput(surface);
  op->OpRun();
  op->OnSmooth();
  op->OpDo();

  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  vtkALBASmartPointer<vtkSmoothPolyDataFilter> smoothFilter;
  smoothFilter->SetNumberOfIterations(op->GetNumberOfIterations());
  smoothFilter->SetInputData(inputVtkFilter);
  smoothFilter->Update();

  vtkPolyData *outputVtkFilter = smoothFilter->GetOutput();

  CheckPolydata(polyOut,outputVtkFilter);
  CheckNormals(polyOut,outputVtkFilter);

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterSurfaceTest::TestOnDecimate()
//----------------------------------------------------------------------------
{
  albaOpFilterSurface *op = new albaOpFilterSurface();
  op->TestModeOn();

  albaString fileName = ALBA_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToDecimate.vtk";
  vtkALBASmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  vtkALBASmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);

  op->SetInput(surface);
  op->OpRun();
  op->OnDecimate();
  op->OpDo();

  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  vtkALBASmartPointer<vtkDecimatePro> decimateFilter;
  decimateFilter->SetPreserveTopology(op->GetPreserveTopology());
  decimateFilter->SetTargetReduction(op->GetTargetReduction()/100.0);
  decimateFilter->SetInputData(inputVtkFilter);
  decimateFilter->Update();

  vtkPolyData *outputVtkFilter = decimateFilter->GetOutput();

  CheckPolydata(polyOut,outputVtkFilter);

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterSurfaceTest::TestOnTriangulate()
//----------------------------------------------------------------------------
{
  albaOpFilterSurface *op = new albaOpFilterSurface();
  op->TestModeOn();

  albaString fileName = ALBA_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToTriangulate.vtk";
  vtkALBASmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  vtkALBASmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);

  op->SetInput(surface);
  op->OpRun();
  op->OnTriangulate();
  op->OpDo();

  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  vtkALBASmartPointer<vtkTriangleFilter> triangleFilter;
  triangleFilter->SetInputData(inputVtkFilter);
  triangleFilter->Update();

  vtkPolyData *outputVtkFilter = triangleFilter->GetOutput();
  
  CheckPolydata(polyOut,outputVtkFilter);
  
  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterSurfaceTest::TestOnVtkConnect()
//----------------------------------------------------------------------------
{
  albaOpFilterSurface *op = new albaOpFilterSurface();
  op->TestModeOn();

  albaString fileName = ALBA_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToVtkConnect.vtk";
  vtkALBASmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  vtkALBASmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);

  op->SetInput(surface);
  op->OpRun();
  op->OnVtkConnect();
  op->OpDo();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  vtkALBASmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
	connectivityFilter->SetInputData(inputVtkFilter);
  connectivityFilter->Update();

  vtkPolyData *outputVtkFilter = connectivityFilter->GetOutput();

  CheckPolydata(polyOut,outputVtkFilter);

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterSurfaceTest::TestOnStripper()
//----------------------------------------------------------------------------
{
  albaOpFilterSurface *op = new albaOpFilterSurface();
  op->TestModeOn();

  albaString fileName = ALBA_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToStripper.vtk";
  vtkALBASmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  vtkALBASmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);

  op->SetInput(surface);
  op->OpRun();
  op->OnStripper();
  op->OpDo();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  vtkALBASmartPointer<vtkStripper> stripperFilter;
  stripperFilter->SetInputData(inputVtkFilter);
  stripperFilter->Update();

  vtkPolyData *outputVtkFilter = stripperFilter->GetOutput();

  CheckPolydata(polyOut,outputVtkFilter);

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterSurfaceTest::TestOnGenerateNormals()
//----------------------------------------------------------------------------
{
  albaOpFilterSurface *op = new albaOpFilterSurface();
  op->TestModeOn();

  albaString fileName = ALBA_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToGenerateNormals.vtk";
  vtkALBASmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  vtkALBASmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);

  op->SetInput(surface);
  op->OpRun();
  op->OnGenerateNormals();
  op->OpDo();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  vtkALBASmartPointer<vtkPolyDataNormals> normalsFilter;
  normalsFilter->SetFlipNormals(op->GetFlipNormals());
  normalsFilter->SetSplitting(op->GetEdgeSplit());
  normalsFilter->SetFeatureAngle(op->GetFeatureAngle());
  normalsFilter->SetInputData(inputVtkFilter);
  normalsFilter->Update();

  vtkPolyData *outputVtkFilter = normalsFilter->GetOutput();

  CheckPolydata(polyOut,outputVtkFilter);
  CheckNormals(polyOut,outputVtkFilter);

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterSurfaceTest::TestUndo()
//----------------------------------------------------------------------------
{
  albaOpFilterSurface *op = new albaOpFilterSurface();
  op->TestModeOn();

  albaString fileName = ALBA_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToGenerateNormals.vtk";
  vtkALBASmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  albaSmartPointer<albaVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  vtkALBASmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);

  op->SetInput(surface);
  op->OpRun();
  op->OnClean();
  op->OnTriangulate();
  op->OnDecimate();
  op->OnSmooth();
  op->OnGenerateNormals();
  op->OpDo();
  op->OpUndo();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  CheckPolydata(polyOut,polyIn);

  //The normal shouldn't be present
  CPPUNIT_ASSERT( polyOut->GetPointData()->GetNormals() == NULL && polyOut->GetCellData()->GetNormals() == NULL);

  albaDEL(op);
}
