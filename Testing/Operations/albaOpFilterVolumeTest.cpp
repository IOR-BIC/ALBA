/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFilterVolumeTest
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
#include "albaOpFilterVolumeTest.h"
#include "albaOpFilterVolume.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurface.h"
#include "albaVMEPolyline.h"
#include "albaVMEImage.h"
#include "albaVMEScalar.h"

#include "vtkALBASmartPointer.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageMedian3D.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void albaOpFilterVolumeTest::CreateDataTest()
//----------------------------------------------------------------------------
{
  vtkNEW(m_InputIM);
  m_InputIM->SetSpacing(0.5,1.0,1.5);
  m_InputIM->SetDimensions(10,15,5);
  vtkALBASmartPointer<vtkDoubleArray> scalarsIM;
  scalarsIM->SetNumberOfTuples(10*15*5);
  for(int i=0;i<10*15*5;i++)
  {
    scalarsIM->SetTuple1(i,i);
  }
  scalarsIM->SetName("SCALARS");
  m_InputIM->GetPointData()->SetScalars(scalarsIM);

  vtkNEW(m_InputRG);
  m_InputRG->SetDimensions(5,10,15);

  vtkALBASmartPointer<vtkDoubleArray> x;
  x->SetNumberOfTuples(5);
  for (int i=0;i<5;i++)
  {
    x->SetTuple1(i,i);
  }

  vtkALBASmartPointer<vtkDoubleArray> y;
  y->SetNumberOfTuples(10);
  for (int i=0;i<10;i++)
  {
    y->SetTuple1(i,i);
  }

  vtkALBASmartPointer<vtkDoubleArray> z;
  z->SetNumberOfTuples(15);
  for (int i=0;i<15;i++)
  {
    z->SetTuple1(i,i);
  }

  m_InputRG->SetXCoordinates(x);
  m_InputRG->SetYCoordinates(y);
  m_InputRG->SetZCoordinates(z);

  vtkALBASmartPointer<vtkDoubleArray> scalarsRG;
  scalarsRG->SetNumberOfTuples(10*15*5);
  for(int i=0;i<10*15*5;i++)
  {
    scalarsRG->SetTuple1(i,i);
  }
  scalarsRG->SetName("SCALARS");

  m_InputRG->GetPointData()->SetScalars(scalarsRG);

}
//----------------------------------------------------------------------------
void albaOpFilterVolumeTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaOpFilterVolumeTest::BeforeTest()
//----------------------------------------------------------------------------
{
  m_Result = false;
  m_InputIM = NULL;
  m_InputRG = NULL;

  CreateDataTest();
}
//----------------------------------------------------------------------------
void albaOpFilterVolumeTest::AfterTest()
//----------------------------------------------------------------------------
{
  vtkDEL(m_InputIM);
  vtkDEL(m_InputRG);
}
//----------------------------------------------------------------------------
void albaOpFilterVolumeTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaOpFilterVolume op;
}
//----------------------------------------------------------------------------
void albaOpFilterVolumeTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOpFilterVolume *op = new albaOpFilterVolume();
  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterVolumeTest::TestAccept()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volume;
  albaSmartPointer<albaVMESurface> surface;
  albaSmartPointer<albaVMEScalar> scalar;
  albaSmartPointer<albaVMEPolyline> polyline;
  albaSmartPointer<albaVMEImage> image;

  albaOpFilterVolume *op = new albaOpFilterVolume();
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

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterVolumeTest::TestOnSmooth()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volumeIM;
  volumeIM->SetData(m_InputIM,0.0);
  volumeIM->GetOutput()->Update();
  volumeIM->Update();

  albaOpFilterVolume *op = new albaOpFilterVolume();
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

  vtkImageData *outputIM = (vtkImageData*)albaVMEVolumeGray::SafeDownCast(op->GetInput())->GetOutput()->GetVTKData();

  vtkALBASmartPointer<vtkImageGaussianSmooth> filterSmooth;
  filterSmooth->SetInputData(m_InputIM);
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

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterVolumeTest::TestOnClear()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volumeIM;
  volumeIM->SetData(m_InputIM,0.0);
  volumeIM->GetOutput()->Update();
  volumeIM->Update();

  albaOpFilterVolume *op = new albaOpFilterVolume();
  op->TestModeOn();
  op->ApplyFiltersToInputDataOff();
  op->SetInput(volumeIM);
  op->OpRun();
  int kernelSize[3];
  op->OnMedian();
  op->OnSmooth();
  op->OnClear();
  op->OpDo();

  vtkImageData *outputIM = (vtkImageData*)albaVMEVolumeGray::SafeDownCast(op->GetInput())->GetOutput()->GetVTKData();

  m_Result = outputIM->GetNumberOfPoints() == m_InputIM->GetNumberOfPoints();
  TEST_RESULT;

  for (int i=0;i<outputIM->GetNumberOfPoints();i++)
  {
    m_Result = outputIM->GetPointData()->GetScalars()->GetTuple1(i) == m_InputIM->GetPointData()->GetScalars()->GetTuple1(i);
    TEST_RESULT;
  }

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterVolumeTest::TestApplyFiltersToInputData()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volumeIM;
  volumeIM->SetData(m_InputIM,0.0);
  volumeIM->GetOutput()->Update();
  volumeIM->Update();

  albaOpFilterVolume *op = new albaOpFilterVolume();
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

  vtkImageData *outputIM = (vtkImageData*)albaVMEVolumeGray::SafeDownCast(op->GetInput())->GetOutput()->GetVTKData();

  vtkALBASmartPointer<vtkImageGaussianSmooth> filterSmooth;
  filterSmooth->SetInputData(m_InputIM);
  filterSmooth->SetStandardDeviations(stDev);
  filterSmooth->SetRadiusFactors(radius);
  filterSmooth->Update();

  vtkALBASmartPointer<vtkImageMedian3D> filterMedian;
  filterMedian->SetInputConnection(filterSmooth->GetOutputPort());
  filterMedian->SetKernelSize(kernelSize[0],kernelSize[1],kernelSize[2]);
  filterMedian->Update();

  m_Result = outputIM->GetNumberOfPoints() == filterMedian->GetOutput()->GetNumberOfPoints();
  TEST_RESULT;

  for (int i=0;i<outputIM->GetNumberOfPoints();i++)
  {
    m_Result = outputIM->GetPointData()->GetScalars()->GetTuple1(i) == filterMedian->GetOutput()->GetPointData()->GetScalars()->GetTuple1(i);
    TEST_RESULT;
  }

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterVolumeTest::TestUndo()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volumeIM;
  volumeIM->SetData(m_InputIM,0.0);
  volumeIM->GetOutput()->Update();
  volumeIM->Update();

  albaOpFilterVolume *op = new albaOpFilterVolume();
  op->TestModeOn();
  op->ApplyFiltersToInputDataOff();
  op->SetInput(volumeIM);
  op->OpRun();
  int kernelSize[3];
  op->OnMedian();
  op->OnSmooth();
  op->OpDo();
  op->OpUndo();


  vtkImageData *outputIM = (vtkImageData*)albaVMEVolumeGray::SafeDownCast(op->GetInput())->GetOutput()->GetVTKData();

  for (int i=0;i<outputIM->GetNumberOfPoints();i++)
  {
    m_Result = (outputIM->GetPointData()->GetScalars()->GetTuple1(i) == m_InputIM->GetPointData()->GetScalars()->GetTuple1(i));
    TEST_RESULT;
  }

  albaDEL(op);
}
//----------------------------------------------------------------------------
void albaOpFilterVolumeTest::TestOnMedian()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEVolumeGray> volumeIM;
  volumeIM->SetData(m_InputIM,0.0);
  volumeIM->GetOutput()->Update();
  volumeIM->Update();

  albaOpFilterVolume *op = new albaOpFilterVolume();
  op->TestModeOn();
  op->ApplyFiltersToInputDataOff();
  op->SetInput(volumeIM);
  op->OpRun();
  int kernelSize[3];
  op->GetKernelSize(kernelSize);
  op->OnMedian();
  op->OpDo();

  vtkImageData *outputIM = (vtkImageData*)albaVMEVolumeGray::SafeDownCast(op->GetInput())->GetOutput()->GetVTKData();

  vtkALBASmartPointer<vtkImageMedian3D> filterMedian;
  filterMedian->SetInputData(m_InputIM);
  filterMedian->SetKernelSize(kernelSize[0],kernelSize[1],kernelSize[2]);
  filterMedian->Update();

  m_Result = outputIM->GetNumberOfPoints() == filterMedian->GetOutput()->GetNumberOfPoints();
  TEST_RESULT;

  for (int i=0;i<outputIM->GetNumberOfPoints();i++)
  {
    m_Result = outputIM->GetPointData()->GetScalars()->GetTuple1(i) == filterMedian->GetOutput()->GetPointData()->GetScalars()->GetTuple1(i);
    TEST_RESULT;
  }

  albaDEL(op);
}
