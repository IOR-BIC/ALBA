/*=========================================================================

 Program: MAF2Medical
 Module: mafOpCropDeformableROITest
 Authors: Eleonora Mambrini
 
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

#include "mafOpCropDeformableROITest.h"
#include "mafOpCropDeformableROI.h"

#include "mafSmartPointer.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "vtkMAFSmartPointer.h"

#include "vtkDataSet.h"
#include "vtkMaskPolyDataFilter.h"
#include "vtkPolyDataReader.h"
#include "vtkRectilinearGrid.h"
#include "vtkRectilinearGridReader.h"
#include "vtkStructuredPointsReader.h"

#include <string.h>

class OpCropDeformableROIDummy: public mafOpCropDeformableROI
{
public:
  void SetMask(mafNode *mask);
  
  void SetDistance(double d);
  void SetInsideOut(int i);
  void SetMaxDistance(double d);
  void SetFillValue(double fillValue);

  void OpExecute();

  mafVMEVolumeGray *GetResult();
};
//----------------------------------------------------------------------------
void OpCropDeformableROIDummy::SetMask(mafNode *mask)
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
void OpCropDeformableROIDummy::SetFillValue(double fillValue)
//----------------------------------------------------------------------------
{
  m_FillValue = fillValue;
}

//----------------------------------------------------------------------------
void OpCropDeformableROIDummy::SetInsideOut(int i)
//----------------------------------------------------------------------------
{
  m_InsideOut = i;
}

//----------------------------------------------------------------------------
void OpCropDeformableROIDummy::SetMaxDistance(double d)
//----------------------------------------------------------------------------
{
  m_MaxDistance = d;
}

//----------------------------------------------------------------------------
mafVMEVolumeGray *OpCropDeformableROIDummy::GetResult()
//----------------------------------------------------------------------------
{
  return m_ResultVme;
}

//----------------------------------------------------------------------------
void OpCropDeformableROIDummy::OpExecute()
//----------------------------------------------------------------------------
{  
  TestModeOn();

  mafNEW(m_ResultVme);
  m_ResultVme->DeepCopy(m_Input);

  vtkNEW(m_MaskPolydataFilter);

  //assert(m_PNode);
  Algorithm(mafVME::SafeDownCast(m_PNode));

  m_Output = m_ResultVme;

}

//----------------------------------------------------------------------------
void mafOpCropDeformableROITest::setUp()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void mafOpCropDeformableROITest::tearDown()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void mafOpCropDeformableROITest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpCropDeformableROI *op = new mafOpCropDeformableROI();
  cppDEL(op);
}
//----------------------------------------------------------------------------
void mafOpCropDeformableROITest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafOpCropDeformableROI op();
}

//----------------------------------------------------------------------------
void mafOpCropDeformableROITest::TestOpExecute()
//----------------------------------------------------------------------------
{
  double distance = 0.0;
  int insideOut = 0;
  double maxDistance = sqrt(1.0e29)/3.0;
  double fillValue = 0.0;

  // mask surface
  mafString filename=MAF_DATA_ROOT;
  filename<<"/Surface/Sphere.vtk";
  vtkMAFSmartPointer<vtkPolyDataReader> reader;
  reader->SetFileName(filename.GetCStr());
  reader->Update();

  mafSmartPointer<mafVMESurface>surface;
  surface->SetData(reader->GetOutput(),0.0);
  surface->GetOutput()->GetVTKData()->Update();
  surface->Update();
  CPPUNIT_ASSERT(surface);

  // volume input
  filename = MAF_DATA_ROOT;
  filename<<"/VTK_Volumes/volume.vtk";
  vtkMAFSmartPointer<vtkStructuredPointsReader> volumeReader;
  volumeReader->SetFileName(filename.GetCStr());
  volumeReader->Update();

  mafSmartPointer<mafVMEVolumeGray> volume;
  volume->SetData((vtkImageData*)volumeReader->GetOutput(),0.0);
  volume->GetOutput()->GetVTKData()->Update();
  volume->Update();
  CPPUNIT_ASSERT(volume);

  // test filtering
  vtkMaskPolyDataFilter *filter;
  vtkNEW(filter);
  filter->SetInput(volume->GetOutput()->GetVTKData());
  filter->SetDistance(distance);
  filter->SetFillValue(fillValue);
  filter->SetMaximumDistance(maxDistance);
  filter->SetFillValue(fillValue);
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
  op->SetFillValue(fillValue);
  op->SetInsideOut(insideOut);
  op->SetMaxDistance(maxDistance);

  //set mask
  op->SetMask(mafNode::SafeDownCast(surface));
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