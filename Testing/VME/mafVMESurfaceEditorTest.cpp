/*=========================================================================

 Program: MAF2
 Module: mafVMESurfaceEditorTest
 Authors: Roberto Mucci
 
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
#include "mafVMESurfaceEditorTest.h"
#include <iostream>
#include "mafOpImporterVTK.h"
#include "mafVMEStorage.h"
#include "mafVMERoot.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
//#include "vtkdataSet.h"

#include "mafVMESurfaceEditor.h"


#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//---------------------------------------------------------
void mafVMESurfaceEditorTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  mafVMESurfaceEditor *editor = NULL;
  mafNEW(editor);
  mafDEL(editor);
}
//---------------------------------------------------------
void mafVMESurfaceEditorTest::TestSetData()
//---------------------------------------------------------
{
  //Create storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  //import surface
  mafOpImporterVTK *importer1=new mafOpImporterVTK("importer");
  mafString filename=MAF_DATA_ROOT;
  filename<<"/Test_SurfaceEditor/surface.vtk";
  importer1->TestModeOn();
  importer1->SetFileName(filename);
  importer1->SetInput(storage->GetRoot());
  importer1->ImportVTK();
  mafSmartPointer<mafVMESurface> surface = mafVMESurface::SafeDownCast(importer1->GetOutput());
  surface->Update();
  surface->GetOutput()->GetVTKData()->Update();

  mafVMESurfaceEditor *editor = NULL;
  mafNEW(editor);
  
  vtkMAFSmartPointer<vtkPolyData> inputPolydata;
  inputPolydata->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData()));
  int pointsNumberIn = inputPolydata->GetNumberOfPoints();
  editor->SetData(inputPolydata,0.0);
  editor->Modified();
  editor->Update();

  vtkMAFSmartPointer<vtkPolyData> outputPolydata=vtkPolyData::SafeDownCast(editor->GetOutput()->GetVTKData());
  outputPolydata->Modified();
  outputPolydata->Update();
  int pointsNumberOut = outputPolydata->GetNumberOfPoints();

  m_Result = pointsNumberIn == pointsNumberOut;

  m_Result = true;
  TEST_RESULT;

  mafDEL(editor);
  mafDEL(importer1);
  mafDEL(storage);
  
}
//---------------------------------------------------------
void mafVMESurfaceEditorTest::TestSetGetVisualPipe()
//---------------------------------------------------------
{
  mafVMESurfaceEditor *editor = NULL;
  mafNEW(editor);

  mafString pipeIn = "mafPipeSurfaceEditor";

  editor->SetVisualPipe(pipeIn);

  mafString pipeOut = editor->GetVisualPipe();

  m_Result = pipeIn.Compare(pipeOut) == 0;
  TEST_RESULT;

  mafDEL(editor);
}
