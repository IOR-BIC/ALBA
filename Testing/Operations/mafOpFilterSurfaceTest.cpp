/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafOpFilterSurfaceTest.cpp,v $
Language:  C++
Date:      $Date: 2009-10-08 07:26:33 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafOpFilterSurfaceTest.h"

#include "mafOpFilterSurface.h"
#include "mafVMEGroup.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEImage.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMEGizmo.h"
#include "mafVMERefSys.h"
#include "mafVMESlicer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMEPolyline.h"
#include "mafVMESurface.h"

#include "vtkMAFSmartPointer.h"
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
void mafOpFilterSurfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpFilterSurfaceTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;
}
//----------------------------------------------------------------------------
void mafOpFilterSurfaceTest::tearDown()
//----------------------------------------------------------------------------
{
  delete wxLog::SetActiveTarget(NULL);
}
//----------------------------------------------------------------------------
void mafOpFilterSurfaceTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafOpFilterSurface op;
}
//----------------------------------------------------------------------------
void mafOpFilterSurfaceTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpFilterSurface *op = new mafOpFilterSurface();
  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterSurfaceTest::CheckPolydata(vtkPolyData *polydataFromOperation,vtkPolyData *polydataFromFilter)
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
void mafOpFilterSurfaceTest::CheckNormals(vtkPolyData *polydataFromOperation,vtkPolyData *polydataFromFilter)
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
void mafOpFilterSurfaceTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> surface;
  mafOpFilterSurface *op = new mafOpFilterSurface();

  CPPUNIT_ASSERT( op->Accept(surface) );

  mafSmartPointer<mafVMEVolumeGray> volume;
  mafSmartPointer<mafVMEGroup> group;
  mafSmartPointer<mafVMESlicer> slicer;
  mafSmartPointer<mafVMEPolyline> poly;
  mafSmartPointer<mafVMEImage> image;
  mafSmartPointer<mafVMERefSys> refSys;
  mafSmartPointer<mafVMESurfaceParametric> paramSurface;
  mafSmartPointer<mafVMEGizmo> gizmo;
  mafSmartPointer<mafVMELandmark> landmark;
  mafSmartPointer<mafVMELandmarkCloud> landmarkCloud;

  //Only surface is accepted
  CPPUNIT_ASSERT( !(op->Accept(landmark) && op->Accept(landmarkCloud) && op->Accept(gizmo) && op->Accept(paramSurface) && op->Accept(volume) && op->Accept(group) && op->Accept(slicer) && op->Accept(poly) && op->Accept(image) && op->Accept(refSys)) );

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterSurfaceTest::TestOnClean()
//----------------------------------------------------------------------------
{
  mafOpFilterSurface *op = new mafOpFilterSurface();
  op->TestModeOn();

  mafString fileName = MAF_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToClean.vtk";
  vtkMAFSmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyIn->Update();

  CPPUNIT_ASSERT( polyIn->GetNumberOfPoints() == 24 );

  vtkMAFSmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);
  inputVtkFilter->Update();

  op->SetInput(surface);
  op->OpRun();
  op->OnClean();
  op->OpDo();

  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyOut->Update();

  CPPUNIT_ASSERT( polyOut->GetNumberOfPoints() == 8 );

  vtkMAFSmartPointer<vtkCleanPolyData> cleanFilter;
  cleanFilter->SetTolerance(0.0);
  cleanFilter->SetInput(inputVtkFilter);
  cleanFilter->Update();

  vtkPolyData *outputVtkFilter = cleanFilter->GetOutput();

  CheckPolydata(polyOut,outputVtkFilter);

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterSurfaceTest::TestOnSmooth()
//----------------------------------------------------------------------------
{
  mafOpFilterSurface *op = new mafOpFilterSurface();
  op->TestModeOn();

  mafString fileName = MAF_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToSmooth.vtk";
  vtkMAFSmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyIn->Update();

  vtkMAFSmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);
  inputVtkFilter->Update();

  op->SetInput(surface);
  op->OpRun();
  op->OnSmooth();
  op->OpDo();

  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyOut->Update();

  vtkMAFSmartPointer<vtkSmoothPolyDataFilter> smoothFilter;
  smoothFilter->SetNumberOfIterations(op->GetNumberOfIterations());
  smoothFilter->SetInput(inputVtkFilter);
  smoothFilter->Update();

  vtkPolyData *outputVtkFilter = smoothFilter->GetOutput();

  CheckPolydata(polyOut,outputVtkFilter);
  CheckNormals(polyOut,outputVtkFilter);

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterSurfaceTest::TestOnDecimate()
//----------------------------------------------------------------------------
{
  mafOpFilterSurface *op = new mafOpFilterSurface();
  op->TestModeOn();

  mafString fileName = MAF_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToDecimate.vtk";
  vtkMAFSmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyIn->Update();

  vtkMAFSmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);
  inputVtkFilter->Update();

  op->SetInput(surface);
  op->OpRun();
  op->OnDecimate();
  op->OpDo();

  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyOut->Update();

  vtkMAFSmartPointer<vtkDecimatePro> decimateFilter;
  decimateFilter->SetPreserveTopology(op->GetPreserveTopology());
  decimateFilter->SetTargetReduction(op->GetTargetReduction()/100.0);
  decimateFilter->SetInput(inputVtkFilter);
  decimateFilter->Update();

  vtkPolyData *outputVtkFilter = decimateFilter->GetOutput();
  outputVtkFilter->Update();

  CheckPolydata(polyOut,outputVtkFilter);

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterSurfaceTest::TestOnTriangulate()
//----------------------------------------------------------------------------
{
  mafOpFilterSurface *op = new mafOpFilterSurface();
  op->TestModeOn();

  mafString fileName = MAF_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToTriangulate.vtk";
  vtkMAFSmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyIn->Update();

  vtkMAFSmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);
  inputVtkFilter->Update();

  op->SetInput(surface);
  op->OpRun();
  op->OnTriangulate();
  op->OpDo();

  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyOut->Update();

  vtkMAFSmartPointer<vtkTriangleFilter> triangleFilter;
  triangleFilter->SetInput(inputVtkFilter);
  triangleFilter->Update();

  vtkPolyData *outputVtkFilter = triangleFilter->GetOutput();
  outputVtkFilter->Update();
  
  CheckPolydata(polyOut,outputVtkFilter);
  
  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterSurfaceTest::TestOnVtkConnect()
//----------------------------------------------------------------------------
{
  mafOpFilterSurface *op = new mafOpFilterSurface();
  op->TestModeOn();

  mafString fileName = MAF_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToVtkConnect.vtk";
  vtkMAFSmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyIn->Update();

  vtkMAFSmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);
  inputVtkFilter->Update();

  op->SetInput(surface);
  op->OpRun();
  op->OnVtkConnect();
  op->OpDo();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyOut->Update();

  vtkMAFSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
  connectivityFilter->SetInput(inputVtkFilter);
  connectivityFilter->Update();

  vtkPolyData *outputVtkFilter = connectivityFilter->GetOutput();
  outputVtkFilter->Update();

  CheckPolydata(polyOut,outputVtkFilter);

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterSurfaceTest::TestOnStripper()
//----------------------------------------------------------------------------
{
  mafOpFilterSurface *op = new mafOpFilterSurface();
  op->TestModeOn();

  mafString fileName = MAF_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToStripper.vtk";
  vtkMAFSmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyIn->Update();

  vtkMAFSmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);
  inputVtkFilter->Update();

  op->SetInput(surface);
  op->OpRun();
  op->OnStripper();
  op->OpDo();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyOut->Update();

  vtkMAFSmartPointer<vtkStripper> stripperFilter;
  stripperFilter->SetInput(inputVtkFilter);
  stripperFilter->Update();

  vtkPolyData *outputVtkFilter = stripperFilter->GetOutput();
  outputVtkFilter->Update();

  CheckPolydata(polyOut,outputVtkFilter);

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterSurfaceTest::TestOnGenerateNormals()
//----------------------------------------------------------------------------
{
  mafOpFilterSurface *op = new mafOpFilterSurface();
  op->TestModeOn();

  mafString fileName = MAF_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToGenerateNormals.vtk";
  vtkMAFSmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyIn->Update();

  vtkMAFSmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);
  inputVtkFilter->Update();

  op->SetInput(surface);
  op->OpRun();
  op->OnGenerateNormals();
  op->OpDo();

  vtkPolyData *polyOut = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyOut->Update();

  vtkMAFSmartPointer<vtkPolyDataNormals> normalsFilter;
  normalsFilter->SetFlipNormals(op->GetFlipNormals());
  normalsFilter->SetSplitting(op->GetEdgeSplit());
  normalsFilter->SetFeatureAngle(op->GetFeatureAngle());
  normalsFilter->SetInput(inputVtkFilter);
  normalsFilter->Update();

  vtkPolyData *outputVtkFilter = normalsFilter->GetOutput();
  outputVtkFilter->Update();

  CheckPolydata(polyOut,outputVtkFilter);
  CheckNormals(polyOut,outputVtkFilter);

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterSurfaceTest::TestUndo()
//----------------------------------------------------------------------------
{
  mafOpFilterSurface *op = new mafOpFilterSurface();
  op->TestModeOn();

  mafString fileName = MAF_DATA_ROOT;
  fileName<<"\\Test_FilterSurface\\SurfaceToGenerateNormals.vtk";
  vtkMAFSmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(fileName.GetCStr());
  reader->Update();

  mafSmartPointer<mafVMESurface> surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->Update();
  surface->Update();

  vtkPolyData *polyIn = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());
  polyIn->Update();

  vtkMAFSmartPointer<vtkPolyData> inputVtkFilter;
  inputVtkFilter->DeepCopy(polyIn);
  inputVtkFilter->Update();

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
  polyOut->Update();

  CheckPolydata(polyOut,polyIn);

  //The normal shouldn't be present
  CPPUNIT_ASSERT( polyOut->GetPointData()->GetNormals() == NULL && polyOut->GetCellData()->GetNormals() == NULL);

  mafDEL(op);
}
