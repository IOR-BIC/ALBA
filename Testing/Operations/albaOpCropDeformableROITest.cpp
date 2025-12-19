/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCropDeformableROITest
 Authors: Eleonora Mambrini
 
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

#include "albaOpCropDeformableROITest.h"
#include "albaOpCropDeformableROI.h"

#include "albaSmartPointer.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"
#include "vtkALBASmartPointer.h"

#include "vtkDataSet.h"
#include "vtkMaskPolyDataFilter.h"
#include "vtkPolyDataReader.h"
#include "vtkRectilinearGrid.h"
#include "vtkRectilinearGridReader.h"
#include "vtkStructuredPointsReader.h"

#include <string.h>
#include "vtkImageData.h"

class OpCropDeformableROIDummy: public albaOpCropDeformableROI
{
public:
  void SetMask(albaVME *mask);
  
  void SetDistance(double d);
  void SetInsideOut(int i);
	void SetInsideValue(double fillValue);
	void SetOutsideValue(double fillValue);

  void OpExecute();

  albaVMEVolumeGray *GetResult();
};
//----------------------------------------------------------------------------
void OpCropDeformableROIDummy::SetMask(albaVME *mask)
//----------------------------------------------------------------------------
{
  if(OutputSurfaceAccept(mask))
    m_PNode = mask;
}

//----------------------------------------------------------------------------
void OpCropDeformableROIDummy::SetDistance(double d)
//----------------------------------------------------------------------------
{
  m_Distance = d;
}

//----------------------------------------------------------------------------
void OpCropDeformableROIDummy::SetInsideValue(double fillValue)
//----------------------------------------------------------------------------
{
  m_InsideValue = fillValue;
}

//----------------------------------------------------------------------------
void OpCropDeformableROIDummy::SetOutsideValue(double fillValue)
{
	m_OutsideValue = fillValue;
}

//----------------------------------------------------------------------------
void OpCropDeformableROIDummy::SetInsideOut(int i)
//----------------------------------------------------------------------------
{
  m_Modality = i;
}


//----------------------------------------------------------------------------
albaVMEVolumeGray *OpCropDeformableROIDummy::GetResult()
//----------------------------------------------------------------------------
{
  return m_ResultVme;
}

//----------------------------------------------------------------------------
void OpCropDeformableROIDummy::OpExecute()
//----------------------------------------------------------------------------
{  
  TestModeOn();

  albaNEW(m_ResultVme);
  m_ResultVme->DeepCopy(m_Input);

  vtkNEW(m_MaskPolydataFilter);

  //assert(m_PNode);
  Algorithm(m_PNode);

  m_Output = m_ResultVme;

}

//----------------------------------------------------------------------------
void albaOpCropDeformableROITest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOpCropDeformableROI *op = new albaOpCropDeformableROI();
  cppDEL(op);
}
//----------------------------------------------------------------------------
void albaOpCropDeformableROITest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaOpCropDeformableROI op();
}

//----------------------------------------------------------------------------
void albaOpCropDeformableROITest::TestOpExecute()
//----------------------------------------------------------------------------
{
  double distance = 0.0;
  int insideOut = 0;
  double fillValue = 0.0;

  // mask surface
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/Surface/Sphere.vtk";
  vtkALBASmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(filename.GetCStr());
  reader->Update();

  albaSmartPointer<albaVMESurface>surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->Update();
  CPPUNIT_ASSERT(surface);

  // volume input
  filename = ALBA_DATA_ROOT;
  filename<<"/VTK_Volumes/volume.vtk";
  vtkALBASmartPointer<vtkStructuredPointsReader> volumeReader;
  volumeReader->SetFileName(filename.GetCStr());
  volumeReader->Update();

  albaSmartPointer<albaVMEVolumeGray> volume;
  volume->SetData((vtkImageData*)volumeReader->GetOutput(),0.0);
  volume->GetOutput()->GetVTKData()->Update();
  volume->Update();
  CPPUNIT_ASSERT(volume);

  // test filtering
  vtkMaskPolyDataFilter *filter;
  vtkNEW(filter);
  filter->SetInput(volume->GetOutput()->GetVTKData());
  filter->SetDistance(distance);
  filter->SetInsideValue(fillValue);
  filter->SetInsideOut(insideOut);

  vtkPolyData *polyData;
  polyData = vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData());

  filter->SetMask(polyData);
  filter->Update();

  vtkDataSet *dataset;
  dataset = filter->GetOutput();

  /// Operation Execute
  OpCropDeformableROIDummy *op = new OpCropDeformableROIDummy();
  op->SetDistance(distance);
  op->SetInsideValue(fillValue);
  op->SetInsideOut(insideOut);

  //set mask
  op->SetMask(surface);
  //set input
  op->SetInput(volume);

  //Execute
  op->OpExecute();
  op->OpDo();


  op->GetResult()->GetVolumeOutput()->Update();
  vtkDataSet *opResult = op->GetResult()->GetVolumeOutput()->GetVTKData();
  
  CPPUNIT_ASSERT(opResult);
  CPPUNIT_ASSERT(opResult->GetNumberOfCells() == dataset->GetNumberOfCells());
  CPPUNIT_ASSERT(opResult->GetMaxCellSize() == dataset->GetMaxCellSize());
  CPPUNIT_ASSERT(opResult->GetNumberOfPoints() == dataset->GetNumberOfPoints());

  vtkDEL(filter);
  cppDEL(op);
}