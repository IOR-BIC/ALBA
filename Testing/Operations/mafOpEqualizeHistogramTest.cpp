/*=========================================================================

 Program: MAF2
 Module: mafOpEqualizeHistogramTest
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

#include "mafOpEqualizeHistogramTest.h"
#include "mafOpEqualizeHistogram.h"

#include "mafVMEImage.h"
#include "mafVMEVolumeGray.h"
#include "vtkMAFSmartPointer.h"

#include "vtkDataSetReader.h"
#include "vtkImageData.h"
#include "vtkImageCast.h"
#include "vtkStructuredPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkDataSet.h"
#include "vtkDataArray.h"

#include "mafOpVolumeResample.h"
#include "mafOpImporterVTK.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"

class OpEqualizeHistogramDerivedTestClass: public mafOpEqualizeHistogram
{
public:

  OpEqualizeHistogramDerivedTestClass(const wxString &label = "Equalize Histogram");
  virtual ~OpEqualizeHistogramDerivedTestClass();

  mafTypeMacro(OpEqualizeHistogramDerivedTestClass, mafOpEqualizeHistogram);

  void ExecuteOpRun();
  void ExecuteAlgorithm();
  void ExecuteOpStop();
  mafVMEVolumeGray *GetVolumeOutput();

protected:
  mafOpVolumeResample *op;

};

//----------------------------------------------------------------------------
mafCxxTypeMacro(OpEqualizeHistogramDerivedTestClass);
//----------------------------------------------------------------------------*/

//----------------------------------------------------------------------------
OpEqualizeHistogramDerivedTestClass::OpEqualizeHistogramDerivedTestClass(const wxString &label) :
mafOpEqualizeHistogram(label)
//---------------------------------------------------------------------------*/
{
  op = new mafOpVolumeResample();
}

//----------------------------------------------------------------------------
OpEqualizeHistogramDerivedTestClass::~OpEqualizeHistogramDerivedTestClass()
//----------------------------------------------------------------------------*/
{
  cppDEL(op);
  mafDEL(m_VolumeOutput);
}
//----------------------------------------------------------------------------
mafVMEVolumeGray *OpEqualizeHistogramDerivedTestClass::GetVolumeOutput()
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
  m_VolumeInput = mafVMEVolumeGray::SafeDownCast(m_Input);

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
  op->SetBounds(outputBounds,mafOpVolumeResample::CUSTOMBOUNDS);

  op->Resample();

  mafVMEVolumeGray *Output = mafVMEVolumeGray::SafeDownCast(op->GetOutput());
  Output->GetOutput()->Update();
  Output->Update();

  m_VolumeInput = Output;




  /*vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  if (sp == NULL)
  {
    op->SetInput(m_VolumeInput);
    op->TestModeOn();
    op->OpRun();
    op->AutoSpacing();
    op->Resample();
    mafVMEVolumeGray *volOut=mafVMEVolumeGray::SafeDownCast(op->GetOutput());
    volOut->GetOutput()->Update();
    volOut->Update();

    m_VolumeInput=volOut;
  }*/

  m_VolumeInput->Update();

  mafNEW(m_VolumeOutput);
  m_VolumeOutput->SetData(vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData()),m_VolumeInput->GetTimeStamp());
  mafString name = m_VolumeInput->GetName();
  name<<" - Equalized Histogram";
  m_VolumeOutput->SetName(name);
  m_VolumeOutput->ReparentTo(m_Input);
  m_VolumeOutput->Update();

}


//-----------------------------------------------------------
void mafOpEqualizeHistogramTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  mafOpEqualizeHistogram *op = new mafOpEqualizeHistogram();
  cppDEL(op);
}
//-----------------------------------------------------------
void mafOpEqualizeHistogramTest::TestStaticAllocation() 
//-----------------------------------------------------------
{
  mafOpEqualizeHistogramTest op;  
}

//-----------------------------------------------------------
void mafOpEqualizeHistogramTest::TestAccept()
//-----------------------------------------------------------
{

  OpEqualizeHistogramDerivedTestClass *m_OpEqualizeHistogram = new OpEqualizeHistogramDerivedTestClass();

  mafVMEVolumeGray *volume;
  mafNEW(volume);

  mafVMEImage *image;
  mafNEW(image);

  CPPUNIT_ASSERT(m_OpEqualizeHistogram->Accept(volume));
  CPPUNIT_ASSERT(!m_OpEqualizeHistogram->Accept(image));
  CPPUNIT_ASSERT(!m_OpEqualizeHistogram->Accept(NULL));

  cppDEL(m_OpEqualizeHistogram);
  mafDEL(image);
  mafDEL(volume);

}

//-----------------------------------------------------------
void mafOpEqualizeHistogramTest::TestOpExecute()
//-----------------------------------------------------------
{
  OpEqualizeHistogramDerivedTestClass *m_OpEqualizeHistogram = new OpEqualizeHistogramDerivedTestClass();

  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafOpImporterVTK *importerVTK=new mafOpImporterVTK("importerVTK");
  importerVTK->TestModeOn();
  importerVTK->SetInput(storage->GetRoot());

  mafString absPathFilename=MAF_DATA_ROOT;
  absPathFilename<<"/Test_VolumeResample/";
  absPathFilename.Append("volumeRG_dim_10_10_10_bounds_1_10_1_10_1_10.vtk");
  importerVTK->SetFileName(absPathFilename);
  importerVTK->OpRun();

  mafVMEVolumeGray *inputVolume = mafVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
  inputVolume->ReparentTo(storage->GetRoot());
  CPPUNIT_ASSERT(inputVolume!=NULL);

  m_OpEqualizeHistogram->TestModeOn();
  m_OpEqualizeHistogram->SetInput(inputVolume);
  m_OpEqualizeHistogram->ExecuteOpRun();
  //mafEventMacro(mafEvent(this, wxOK));
  m_OpEqualizeHistogram->ExecuteAlgorithm();
  m_OpEqualizeHistogram->OpDo();
  mafVMEVolumeGray *volumeOperationOutput = NULL;
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
  mafDEL(storage); 
}

//-----------------------------------------------------------
void mafOpEqualizeHistogramTest::TestOpUndo()
//-----------------------------------------------------------
{

  OpEqualizeHistogramDerivedTestClass *m_OpEqualizeHistogram = new OpEqualizeHistogramDerivedTestClass();

  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  mafOpImporterVTK *importerVTK=new mafOpImporterVTK("importerVTK");
  importerVTK->TestModeOn();
  importerVTK->SetInput(storage->GetRoot());

  mafString absPathFilename=MAF_DATA_ROOT;
  absPathFilename<<"/Test_VolumeResample/";
  absPathFilename.Append("volumeRG_dim_10_10_10_bounds_1_10_1_10_1_10.vtk");
  importerVTK->SetFileName(absPathFilename);
  importerVTK->OpRun();

  mafVMEVolumeGray *inputVolume = mafVMEVolumeGray::SafeDownCast(importerVTK->GetOutput());
  inputVolume->ReparentTo(storage->GetRoot());
  CPPUNIT_ASSERT(inputVolume!=NULL);

  m_OpEqualizeHistogram->TestModeOn();
  m_OpEqualizeHistogram->SetInput(inputVolume);
  m_OpEqualizeHistogram->ExecuteOpRun();
  m_OpEqualizeHistogram->ExecuteAlgorithm();
  m_OpEqualizeHistogram->OpDo();
  mafVMEVolumeGray *volumeOperationOutput = NULL;
  volumeOperationOutput = m_OpEqualizeHistogram->GetVolumeOutput();

  if(volumeOperationOutput)
  {
    m_OpEqualizeHistogram->OpUndo();

    CPPUNIT_ASSERT(volumeOperationOutput->GetParent() == NULL);
  }

  cppDEL(m_OpEqualizeHistogram);
  cppDEL(importerVTK);
  mafDEL(storage); 

}
