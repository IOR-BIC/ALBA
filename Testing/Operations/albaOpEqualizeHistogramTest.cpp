/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpEqualizeHistogramTest
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

#include "albaOpEqualizeHistogramTest.h"
#include "albaOpEqualizeHistogram.h"

#include "albaVMEImage.h"
#include "albaVMEVolumeGray.h"
#include "vtkALBASmartPointer.h"

#include "vtkDataSetReader.h"
#include "vtkImageData.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkDataSet.h"
#include "vtkDataArray.h"

#include "albaOpVolumeResample.h"
#include "albaOpImporterVTK.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"

class OpEqualizeHistogramDerivedTestClass: public albaOpEqualizeHistogram
{
public:

  OpEqualizeHistogramDerivedTestClass(const wxString &label = "Equalize Histogram");
  virtual ~OpEqualizeHistogramDerivedTestClass();

  albaTypeMacro(OpEqualizeHistogramDerivedTestClass, albaOpEqualizeHistogram);

  void ExecuteOpRun();
  void ExecuteAlgorithm();
  void ExecuteOpStop();
  albaVMEVolumeGray *GetVolumeOutput();

protected:
  albaOpVolumeResample *op;

};

//----------------------------------------------------------------------------
albaCxxTypeMacro(OpEqualizeHistogramDerivedTestClass);
//----------------------------------------------------------------------------*/

//----------------------------------------------------------------------------
OpEqualizeHistogramDerivedTestClass::OpEqualizeHistogramDerivedTestClass(const wxString &label) :
albaOpEqualizeHistogram(label)
//---------------------------------------------------------------------------*/
{
  op = new albaOpVolumeResample();
}

//----------------------------------------------------------------------------
OpEqualizeHistogramDerivedTestClass::~OpEqualizeHistogramDerivedTestClass()
//----------------------------------------------------------------------------*/
{
  cppDEL(op);
  albaDEL(m_VolumeOutput);
}
//----------------------------------------------------------------------------
albaVMEVolumeGray *OpEqualizeHistogramDerivedTestClass::GetVolumeOutput()
//----------------------------------------------------------------------------*/
{ 
  return m_VolumeOutput;
}

//----------------------------------------------------------------------------
void OpEqualizeHistogramDerivedTestClass::ExecuteAlgorithm()
//----------------------------------------------------------------------------
{
  Superclass::Algorithm();
}

//----------------------------------------------------------------------------
void OpEqualizeHistogramDerivedTestClass::ExecuteOpStop()
//----------------------------------------------------------------------------
{
  Superclass::OpStop(OP_RUN_OK);
}
//----------------------------------------------------------------------------
void OpEqualizeHistogramDerivedTestClass::ExecuteOpRun()
//----------------------------------------------------------------------------
{
  m_VolumeInput = albaVMEVolumeGray::SafeDownCast(m_Input);

  op->SetInput(m_VolumeInput);
  op->TestModeOn();

  vtkDataSet *inputDataSet = m_VolumeInput->GetOutput()->GetVTKData();

  double outputSpacing[3];
  double inputDataOrigin[3];

  vtkRectilinearGrid *rg = vtkRectilinearGrid::SafeDownCast(inputDataSet);

  outputSpacing[0] = rg->GetXCoordinates()->GetComponent(1,0)-rg->GetXCoordinates()->GetComponent(0,0);
  outputSpacing[1] = rg->GetYCoordinates()->GetComponent(1,0)-rg->GetYCoordinates()->GetComponent(0,0);
  outputSpacing[2] = rg->GetZCoordinates()->GetComponent(1,0)-rg->GetZCoordinates()->GetComponent(0,0);

  inputDataOrigin[0] = rg->GetXCoordinates()->GetComponent(0,0);
  inputDataOrigin[1] = rg->GetYCoordinates()->GetComponent(0,0);
  inputDataOrigin[2] = rg->GetZCoordinates()->GetComponent(0,0);

  op->SetSpacing(outputSpacing);

  double outputBounds[6];
  m_VolumeInput->GetOutput()->GetVMEBounds(outputBounds);
  op->SetBounds(outputBounds,albaOpVolumeResample::CUSTOMBOUNDS);

  op->Resample();

  albaVMEVolumeGray *Output = albaVMEVolumeGray::SafeDownCast(op->GetOutput());
  Output->GetOutput()->Update();
  Output->Update();

  m_VolumeInput = Output;




  /*vtkImageData *sp = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  if (sp == NULL)
  {
    op->SetInput(m_VolumeInput);
    op->TestModeOn();
    op->OpRun();
    op->AutoSpacing();
    op->Resample();
    albaVMEVolumeGray *volOut=albaVMEVolumeGray::SafeDownCast(op->GetOutput());
    volOut->GetOutput()->Update();
    volOut->Update();

    m_VolumeInput=volOut;
  }*/

  m_VolumeInput->Update();

  albaNEW(m_VolumeOutput);
  m_VolumeOutput->SetData(vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData()),m_VolumeInput->GetTimeStamp());
  albaString name = m_VolumeInput->GetName();
  name<<" - Equalized Histogram";
  m_VolumeOutput->SetName(name);
  m_VolumeOutput->ReparentTo(m_Input);
  m_VolumeOutput->Update();

}


//-----------------------------------------------------------
void albaOpEqualizeHistogramTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpEqualizeHistogram *op = new albaOpEqualizeHistogram();
  cppDEL(op);
}
//-----------------------------------------------------------
void albaOpEqualizeHistogramTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  albaOpEqualizeHistogramTest op;  
}

//-----------------------------------------------------------
void albaOpEqualizeHistogramTest::TestAccept()
//-----------------------------------------------------------
{

  OpEqualizeHistogramDerivedTestClass *m_OpEqualizeHistogram = new OpEqualizeHistogramDerivedTestClass();

  albaVMEVolumeGray *volume;
  albaNEW(volume);

  albaVMEImage *image;
  albaNEW(image);

  CPPUNIT_ASSERT(m_OpEqualizeHistogram->Accept(volume));
  CPPUNIT_ASSERT(!m_OpEqualizeHistogram->Accept(image));
  CPPUNIT_ASSERT(!m_OpEqualizeHistogram->Accept(NULL));

  cppDEL(m_OpEqualizeHistogram);
  albaDEL(image);
  albaDEL(volume);

}

//-----------------------------------------------------------
void albaOpEqualizeHistogramTest::TestOpExecute()
//-----------------------------------------------------------
{
  OpEqualizeHistogramDerivedTestClass *m_OpEqualizeHistogram = new OpEqualizeHistogramDerivedTestClass();

  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaOpImporterVTK *importerVTK=new albaOpImporterVTK("importerVTK");
  importerVTK->TestModeOn();
  importerVTK->SetInput(storage->GetRoot());

  albaString absPathFilename=ALBA_DATA_ROOT;
  absPathFilename<<"/Test_VolumeResample/";
  absPathFilename.Append("volumeRG_dim_10_10_10_bounds_1_10_1_10_1_10.vtk");
  importerVTK->SetFileName(absPathFilename);
  importerVTK->OpRun();

  albaVMEVolumeGray *inputVolume = albaVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
  inputVolume->ReparentTo(storage->GetRoot());
  CPPUNIT_ASSERT(inputVolume!=NULL);

  m_OpEqualizeHistogram->TestModeOn();
  m_OpEqualizeHistogram->SetInput(inputVolume);
  m_OpEqualizeHistogram->ExecuteOpRun();
  //albaEventMacro(albaEvent(this, wxOK));
  m_OpEqualizeHistogram->ExecuteAlgorithm();
  m_OpEqualizeHistogram->OpDo();
  albaVMEVolumeGray *volumeOperationOutput = NULL;
  volumeOperationOutput = m_OpEqualizeHistogram->GetVolumeOutput();

  CPPUNIT_ASSERT(volumeOperationOutput);
  if(volumeOperationOutput)
  {
    volumeOperationOutput->GetOutput()->Update();

    double inBounds[6];
    double outBounds[6];

    inputVolume->GetOutput()->GetBounds(inBounds);
    volumeOperationOutput->GetOutput()->GetBounds(outBounds);

    for(int i=0;i<6;i++)
      CPPUNIT_ASSERT(inBounds[i] == outBounds[i]);

  }

  inputVolume = NULL;
  volumeOperationOutput = NULL;

  cppDEL(m_OpEqualizeHistogram);
  cppDEL(importerVTK);
  albaDEL(storage); 
}

//-----------------------------------------------------------
void albaOpEqualizeHistogramTest::TestOpUndo()
//-----------------------------------------------------------
{

  OpEqualizeHistogramDerivedTestClass *m_OpEqualizeHistogram = new OpEqualizeHistogramDerivedTestClass();

  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  albaOpImporterVTK *importerVTK=new albaOpImporterVTK("importerVTK");
  importerVTK->TestModeOn();
  importerVTK->SetInput(storage->GetRoot());

  albaString absPathFilename=ALBA_DATA_ROOT;
  absPathFilename<<"/Test_VolumeResample/";
  absPathFilename.Append("volumeRG_dim_10_10_10_bounds_1_10_1_10_1_10.vtk");
  importerVTK->SetFileName(absPathFilename);
  importerVTK->OpRun();

  albaVMEVolumeGray *inputVolume = albaVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
  inputVolume->ReparentTo(storage->GetRoot());
  CPPUNIT_ASSERT(inputVolume!=NULL);

  m_OpEqualizeHistogram->TestModeOn();
  m_OpEqualizeHistogram->SetInput(inputVolume);
  m_OpEqualizeHistogram->ExecuteOpRun();
  m_OpEqualizeHistogram->ExecuteAlgorithm();
  m_OpEqualizeHistogram->OpDo();
  albaVMEVolumeGray *volumeOperationOutput = NULL;
  volumeOperationOutput = m_OpEqualizeHistogram->GetVolumeOutput();

  if(volumeOperationOutput)
  {
    m_OpEqualizeHistogram->OpUndo();

    CPPUNIT_ASSERT(volumeOperationOutput->GetParent() == NULL);
  }

  cppDEL(m_OpEqualizeHistogram);
  cppDEL(importerVTK);
  albaDEL(storage); 

}
