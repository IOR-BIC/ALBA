/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESurfaceEditorTest
 Authors: Roberto Mucci
 
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
#include "albaVMESurfaceEditorTest.h"
#include <iostream>
#include "albaOpImporterVTK.h"
#include "albaVMEStorage.h"
#include "albaVMERoot.h"

#include "vtkALBASmartPointer.h"
#include "vtkPolyData.h"
//#include "vtkdataSet.h"

#include "albaVMESurfaceEditor.h"


#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//---------------------------------------------------------
void albaVMESurfaceEditorTest::TestDynamicAllocation()
//---------------------------------------------------------
{
  albaVMESurfaceEditor *editor = NULL;
  albaNEW(editor);
  albaDEL(editor);
}
//---------------------------------------------------------
void albaVMESurfaceEditorTest::TestSetData()
//---------------------------------------------------------
{
  //Create storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();

  //import surface
  albaOpImporterVTK *importer1=new albaOpImporterVTK("importer");
  albaString filename=ALBA_DATA_ROOT;
  filename<<"/Test_SurfaceEditor/surface.vtk";
  importer1->TestModeOn();
  importer1->SetFileName(filename);
  importer1->SetInput(storage->GetRoot());
  importer1->ImportFile();
  albaSmartPointer<albaVMESurface> surface = albaVMESurface::SafeDownCast(importer1->GetOutput());
  surface->Update();
  surface->GetOutput()->GetVTKData()->Update();

  albaVMESurfaceEditor *editor = NULL;
  albaNEW(editor);
  
  vtkALBASmartPointer<vtkPolyData> inputPolydata;
  inputPolydata->DeepCopy(vtkPolyData::SafeDownCast(surface->GetOutput()->GetVTKData()));
  int pointsNumberIn = inputPolydata->GetNumberOfPoints();
  editor->SetData(inputPolydata,0.0);
  editor->Modified();
  editor->Update();

  vtkALBASmartPointer<vtkPolyData> outputPolydata=vtkPolyData::SafeDownCast(editor->GetOutput()->GetVTKData());
  outputPolydata->Modified();
  outputPolydata->Update();
  int pointsNumberOut = outputPolydata->GetNumberOfPoints();

  m_Result = pointsNumberIn == pointsNumberOut;

  m_Result = true;
  TEST_RESULT;

  albaDEL(editor);
  albaDEL(importer1);
  albaDEL(storage);
  
}
//---------------------------------------------------------
void albaVMESurfaceEditorTest::TestSetGetVisualPipe()
//---------------------------------------------------------
{
  albaVMESurfaceEditor *editor = NULL;
  albaNEW(editor);

  albaString pipeIn = "albaPipeSurfaceEditor";

  editor->SetVisualPipe(pipeIn);

  albaString pipeOut = editor->GetVisualPipe();

  m_Result = pipeIn.Compare(pipeOut) == 0;
  TEST_RESULT;

  albaDEL(editor);
}
