/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpCropDeformableROITest.cpp,v $
Language:  C++
Date:      $Date: 2010-04-12 07:36:36 $
Version:   $Revision: 1.1.2.1 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpCropDeformableROITest.h"
#include "medOpCropDeformableROI.h"

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

class OpCropDeformableROIDummy: public medOpCropDeformableROI
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
void medOpCropDeformableROITest::setUp()
//----------------------------------------------------------------------------
{

}
//----------------------------------------------------------------------------
void medOpCropDeformableROITest::tearDown()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
void medOpCropDeformableROITest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  medOpCropDeformableROI *op = new medOpCropDeformableROI();
  cppDEL(op);
}
//----------------------------------------------------------------------------
void medOpCropDeformableROITest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  medOpCropDeformableROI op();
}

//----------------------------------------------------------------------------
void medOpCropDeformableROITest::TestOpExecute()
//----------------------------------------------------------------------------
{
  double distance = 0.0;
  int insideOut = 0;
  double maxDistance = sqrt(1.0e29)/3.0;
  double fillValue = 0.0;

  // mask surface
  mafString filename=MED_DATA_ROOT;
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
  filename = MED_DATA_ROOT;
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