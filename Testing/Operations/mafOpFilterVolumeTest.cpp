/*=========================================================================

 Program: MAF2
 Module: mafOpFilterVolumeTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafOpFilterVolumeTest.h"
#include "mafOpFilterVolume.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafVMEPolyline.h"
#include "mafVMEImage.h"
#include "mafVMEScalar.h"

#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageMedian3D.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mafOpFilterVolumeTest::CreateDataTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_InputIM);
  m_InputIM->SetSpacing(0.5,1.0,1.5);
  m_InputIM->SetDimensions(10,15,5);
  vtkMAFSmartPointer<vtkDoubleArray> scalarsIM;
  scalarsIM->SetNumberOfTuples(10*15*5);
  for(int i=0;i<10*15*5;i++)
  {
    scalarsIM->SetTuple1(i,i);
  }
  scalarsIM->SetName("SCALARS");
  m_InputIM->GetPointData()->SetScalars(scalarsIM);
  m_InputIM->Update();

  vtkNEW(m_InputRG);
  m_InputRG->SetDimensions(5,10,15);

  vtkMAFSmartPointer<vtkDoubleArray> x;
  x->SetNumberOfTuples(5);
  for (int i=0;i<5;i++)
  {
    x->SetTuple1(i,i);
  }

  vtkMAFSmartPointer<vtkDoubleArray> y;
  y->SetNumberOfTuples(10);
  for (int i=0;i<10;i++)
  {
    y->SetTuple1(i,i);
  }

  vtkMAFSmartPointer<vtkDoubleArray> z;
  z->SetNumberOfTuples(15);
  for (int i=0;i<15;i++)
  {
    z->SetTuple1(i,i);
  }

  m_InputRG->SetXCoordinates(x);
  m_InputRG->SetYCoordinates(y);
  m_InputRG->SetZCoordinates(z);

  vtkMAFSmartPointer<vtkDoubleArray> scalarsRG;
  scalarsRG->SetNumberOfTuples(10*15*5);
  for(int i=0;i<10*15*5;i++)
  {
    scalarsRG->SetTuple1(i,i);
  }
  scalarsRG->SetName("SCALARS");

  m_InputRG->GetPointData()->SetScalars(scalarsRG);
  m_InputRG->Update();

}
//----------------------------------------------------------------------------
void mafOpFilterVolumeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafOpFilterVolumeTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_Result = false;
  m_InputIM = NULL;
  m_InputRG = NULL;

  CreateDataTest();
}
//----------------------------------------------------------------------------
void mafOpFilterVolumeTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_InputIM);
  vtkDEL(m_InputRG);
}
//----------------------------------------------------------------------------
void mafOpFilterVolumeTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafOpFilterVolume op;
}
//----------------------------------------------------------------------------
void mafOpFilterVolumeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpFilterVolume *op = new mafOpFilterVolume();
  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterVolumeTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volume;
  mafSmartPointer<mafVMESurface> surface;
  mafSmartPointer<mafVMEScalar> scalar;
  mafSmartPointer<mafVMEPolyline> polyline;
  mafSmartPointer<mafVMEImage> image;

  mafOpFilterVolume *op = new mafOpFilterVolume();
  op->TestModeOn();

  m_Result = op->Accept(volume);
  TEST_RESULT;

  m_Result = !op->Accept(surface);
  TEST_RESULT;

  m_Result = !op->Accept(scalar);
  TEST_RESULT;

  m_Result = !op->Accept(polyline);
  TEST_RESULT;

  m_Result = !op->Accept(image);
  TEST_RESULT;

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterVolumeTest::TestOnSmooth()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volumeIM;
  volumeIM->SetData(m_InputIM,0.0);
  volumeIM->GetOutput()->GetVTKData()->Update();
  volumeIM->GetOutput()->Update();
  volumeIM->Update();

  mafOpFilterVolume *op = new mafOpFilterVolume();
  op->TestModeOn();
  op->ApplyFiltersToInputDataOff();
  op->SetInput(volumeIM);
  op->OpRun();
  double radius[3];
  op->GetSmoothRadius(radius);
  double stDev[3];
  op->GetStandardDeviation(stDev);
  op->OnSmooth();
  op->OpDo();

  vtkImageData *outputIM = (vtkImageData*)mafVMEVolumeGray::SafeDownCast(op->GetInput())->GetOutput()->GetVTKData();
  outputIM->Update();

  vtkMAFSmartPointer<vtkImageGaussianSmooth> filterSmooth;
  filterSmooth->SetInput(m_InputIM);
  filterSmooth->SetStandardDeviations(stDev);
  filterSmooth->SetRadiusFactors(radius);
  filterSmooth->Update();

  m_Result = outputIM->GetNumberOfPoints() == filterSmooth->GetOutput()->GetNumberOfPoints();
  TEST_RESULT;

  for (int i=0;i<outputIM->GetNumberOfPoints();i++)
  {
    m_Result = outputIM->GetPointData()->GetScalars()->GetTuple1(i) == filterSmooth->GetOutput()->GetPointData()->GetScalars()->GetTuple1(i);
    TEST_RESULT;
  }

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterVolumeTest::TestOnClear()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volumeIM;
  volumeIM->SetData(m_InputIM,0.0);
  volumeIM->GetOutput()->GetVTKData()->Update();
  volumeIM->GetOutput()->Update();
  volumeIM->Update();

  mafOpFilterVolume *op = new mafOpFilterVolume();
  op->TestModeOn();
  op->ApplyFiltersToInputDataOff();
  op->SetInput(volumeIM);
  op->OpRun();
  int kernelSize[3];
  op->OnMedian();
  op->OnSmooth();
  op->OnClear();
  op->OpDo();

  vtkImageData *outputIM = (vtkImageData*)mafVMEVolumeGray::SafeDownCast(op->GetInput())->GetOutput()->GetVTKData();
  outputIM->Update();

  m_Result = outputIM->GetNumberOfPoints() == m_InputIM->GetNumberOfPoints();
  TEST_RESULT;

  for (int i=0;i<outputIM->GetNumberOfPoints();i++)
  {
    m_Result = outputIM->GetPointData()->GetScalars()->GetTuple1(i) == m_InputIM->GetPointData()->GetScalars()->GetTuple1(i);
    TEST_RESULT;
  }

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterVolumeTest::TestApplyFiltersToInputData()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volumeIM;
  volumeIM->SetData(m_InputIM,0.0);
  volumeIM->GetOutput()->GetVTKData()->Update();
  volumeIM->GetOutput()->Update();
  volumeIM->Update();

  mafOpFilterVolume *op = new mafOpFilterVolume();
  op->TestModeOn();
  op->ApplyFiltersToInputDataOn();
  op->SetInput(volumeIM);
  op->OpRun();
  int kernelSize[3];
  op->GetKernelSize(kernelSize);
  double radius[3];
  op->GetSmoothRadius(radius);
  double stDev[3];
  op->GetStandardDeviation(stDev);
  op->OnSmooth();
  op->OnMedian();
  op->OpDo();
  op->OpUndo();

  vtkImageData *outputIM = (vtkImageData*)mafVMEVolumeGray::SafeDownCast(op->GetInput())->GetOutput()->GetVTKData();
  outputIM->Update();

  vtkMAFSmartPointer<vtkImageGaussianSmooth> filterSmooth;
  filterSmooth->SetInput(m_InputIM);
  filterSmooth->SetStandardDeviations(stDev);
  filterSmooth->SetRadiusFactors(radius);
  filterSmooth->Update();

  vtkMAFSmartPointer<vtkImageMedian3D> filterMedian;
  filterMedian->SetInput(filterSmooth->GetOutput());
  filterMedian->SetKernelSize(kernelSize[0],kernelSize[1],kernelSize[2]);
  filterMedian->Update();

  m_Result = outputIM->GetNumberOfPoints() == filterMedian->GetOutput()->GetNumberOfPoints();
  TEST_RESULT;

  for (int i=0;i<outputIM->GetNumberOfPoints();i++)
  {
    m_Result = outputIM->GetPointData()->GetScalars()->GetTuple1(i) == filterMedian->GetOutput()->GetPointData()->GetScalars()->GetTuple1(i);
    TEST_RESULT;
  }

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterVolumeTest::TestUndo()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volumeIM;
  volumeIM->SetData(m_InputIM,0.0);
  volumeIM->GetOutput()->GetVTKData()->Update();
  volumeIM->GetOutput()->Update();
  volumeIM->Update();

  mafOpFilterVolume *op = new mafOpFilterVolume();
  op->TestModeOn();
  op->ApplyFiltersToInputDataOff();
  op->SetInput(volumeIM);
  op->OpRun();
  int kernelSize[3];
  op->OnMedian();
  op->OnSmooth();
  op->OpDo();
  op->OpUndo();


  vtkImageData *outputIM = (vtkImageData*)mafVMEVolumeGray::SafeDownCast(op->GetInput())->GetOutput()->GetVTKData();
  outputIM->Update();

  for (int i=0;i<outputIM->GetNumberOfPoints();i++)
  {
    m_Result = (outputIM->GetPointData()->GetScalars()->GetTuple1(i) == m_InputIM->GetPointData()->GetScalars()->GetTuple1(i));
    TEST_RESULT;
  }

  mafDEL(op);
}
//----------------------------------------------------------------------------
void mafOpFilterVolumeTest::TestOnMedian()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volumeIM;
  volumeIM->SetData(m_InputIM,0.0);
  volumeIM->GetOutput()->GetVTKData()->Update();
  volumeIM->GetOutput()->Update();
  volumeIM->Update();

  mafOpFilterVolume *op = new mafOpFilterVolume();
  op->TestModeOn();
  op->ApplyFiltersToInputDataOff();
  op->SetInput(volumeIM);
  op->OpRun();
  int kernelSize[3];
  op->GetKernelSize(kernelSize);
  op->OnMedian();
  op->OpDo();

  vtkImageData *outputIM = (vtkImageData*)mafVMEVolumeGray::SafeDownCast(op->GetInput())->GetOutput()->GetVTKData();
  outputIM->Update();

  vtkMAFSmartPointer<vtkImageMedian3D> filterMedian;
  filterMedian->SetInput(m_InputIM);
  filterMedian->SetKernelSize(kernelSize[0],kernelSize[1],kernelSize[2]);
  filterMedian->Update();

  m_Result = outputIM->GetNumberOfPoints() == filterMedian->GetOutput()->GetNumberOfPoints();
  TEST_RESULT;

  for (int i=0;i<outputIM->GetNumberOfPoints();i++)
  {
    m_Result = outputIM->GetPointData()->GetScalars()->GetTuple1(i) == filterMedian->GetOutput()->GetPointData()->GetScalars()->GetTuple1(i);
    TEST_RESULT;
  }

  mafDEL(op);
}
