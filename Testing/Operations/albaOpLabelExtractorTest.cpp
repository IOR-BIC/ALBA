/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpLabelExtractorTest
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

#include "albaOpLabelExtractorTest.h"
#include "albaOpLabelExtractor.h"

#include "albaVMESurface.h"
#include "albaVMEFactory.h"
#include "vtkPolyData.h"
#include "albaVMERoot.h"
#include "albaVMEStorage.h"

//----------------------------------------------------------------------------
void albaOpLabelExtractorTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//---------------------------------------------------------------
void albaOpLabelExtractorTest::TestDynamicAllocation()
//---------------------------------------------------------------
{
  albaOpLabelExtractor *extractor = new albaOpLabelExtractor("label extractor");
  albaDEL(extractor);
}

//---------------------------------------------------------------
void albaOpLabelExtractorTest::TestLabelRG()
//---------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_LabelExtractor/Test_LabelExtractor.alba";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);

  albaVME *node = storage->GetRoot();
  albaVME *volume = node->FindInTreeByName("CropTestVolumeRG");

  albaOpLabelExtractor *extractor = new albaOpLabelExtractor("label extractor");
  extractor->SetInput(volume);
  extractor->SetLabel(100);
  extractor->SmoothMode(false);
  extractor->ExtractLabel();
  extractor->OpDo();
 
  albaVME *surfaceExtracted = extractor->GetOutput();
  albaString name = surfaceExtracted->GetName();
  albaVME *surfaceNode = surfaceExtracted->FindInTreeByName(name.GetCStr());
  albaVMESurface *surfaceLabeled = albaVMESurface::SafeDownCast(surfaceNode);

  albaVME *surface = node->FindInTreeByName("label100RG");

  albaVMESurface *surfaceOriginal = albaVMESurface::SafeDownCast(surface);

  albaVMEOutputSurface *surfaceOutputLab = albaVMEOutputSurface::SafeDownCast(surfaceLabeled->GetOutput());
  assert(surfaceOutputLab);
  surfaceOutputLab->Update();
  vtkPolyData *polydataLab = vtkPolyData::SafeDownCast(surfaceOutputLab->GetVTKData());
  CPPUNIT_ASSERT(polydataLab);


  albaVMEOutputSurface *surfaceOutput = albaVMEOutputSurface::SafeDownCast(surfaceOriginal->GetOutput());
  assert(surfaceOutput);
  surfaceOutput->Update();
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(surfaceOutput->GetVTKData());
  CPPUNIT_ASSERT(polydata);

  CPPUNIT_ASSERT(polydataLab->GetNumberOfPoints() == polydata->GetNumberOfPoints());

  double boundsLab[6];
  double bounds[6];

  polydataLab->GetBounds(boundsLab);
  polydata->GetBounds(bounds);

  CPPUNIT_ASSERT(
    fabs(boundsLab[0] - bounds[0]) < 0.0001 &&
    fabs(boundsLab[1] - bounds[1]) < 0.0001 &&
    fabs(boundsLab[2] - bounds[2]) < 0.0001 &&
    fabs(boundsLab[3] - bounds[3]) < 0.0001 &&
    fabs(boundsLab[4] - bounds[4]) < 0.0001 &&
    fabs(boundsLab[5] - bounds[5]) < 0.0001 
    );

  albaDEL(extractor);
  albaDEL(storage);
}


//---------------------------------------------------------------
void albaOpLabelExtractorTest::TestLabelSP()
//---------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_LabelExtractor/Test_LabelExtractor.alba";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);

  albaVME *node = storage->GetRoot();
  albaVME *volume = node->FindInTreeByName("CropTestVolumeSP");

  albaOpLabelExtractor *extractor = new albaOpLabelExtractor("label extractor");
  extractor->SetInput(volume);
  extractor->SetLabel(100);
  extractor->SmoothMode(false);
  extractor->ExtractLabel();
  extractor->OpDo();

  albaVME *surfaceExtracted = extractor->GetOutput();
  albaString name = surfaceExtracted->GetName();
  albaVME *surfaceNode = surfaceExtracted->FindInTreeByName(name.GetCStr());
  albaVMESurface *surfaceLabeled = albaVMESurface::SafeDownCast(surfaceNode);

  albaVME *surface = node->FindInTreeByName("label100SP");

  albaVMESurface *surfaceOriginal = albaVMESurface::SafeDownCast(surface);

  albaVMEOutputSurface *surfaceOutputLab = albaVMEOutputSurface::SafeDownCast(surfaceLabeled->GetOutput());
  assert(surfaceOutputLab);
  surfaceOutputLab->Update();
  vtkDataSet *data = surfaceOutputLab->GetVTKData();
  vtkPolyData *polydataLab = vtkPolyData::SafeDownCast(surfaceOutputLab->GetVTKData());
  CPPUNIT_ASSERT(polydataLab);


  albaVMEOutputSurface *surfaceOutput = albaVMEOutputSurface::SafeDownCast(surfaceOriginal->GetOutput());
  assert(surfaceOutput);
  surfaceOutput->Update();
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(surfaceOutput->GetVTKData());
  CPPUNIT_ASSERT(polydata);

  CPPUNIT_ASSERT(polydataLab->GetNumberOfPoints() == polydata->GetNumberOfPoints());

  double boundsLab[6];
  double bounds[6];

  polydataLab->GetBounds(boundsLab);
  polydata->GetBounds(bounds);

  CPPUNIT_ASSERT(
    fabs(boundsLab[0] - bounds[0]) < 0.0001 &&
    fabs(boundsLab[1] - bounds[1]) < 0.0001 &&
    fabs(boundsLab[2] - bounds[2]) < 0.0001 &&
    fabs(boundsLab[3] - bounds[3]) < 0.0001 &&
    fabs(boundsLab[4] - bounds[4]) < 0.0001 &&
    fabs(boundsLab[5] - bounds[5]) < 0.0001 
    );

  albaDEL(extractor);
  albaDEL(storage);
}

//---------------------------------------------------------------
void albaOpLabelExtractorTest::TestLabelSmoothRG()
//---------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_LabelExtractor/Test_LabelExtractor.alba";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);

  albaVME *node = storage->GetRoot();
  albaVME *volume = node->FindInTreeByName("CropTestVolumeRG");

  albaOpLabelExtractor *extractor = new albaOpLabelExtractor("label extractor");
  extractor->SetInput(volume);
  extractor->SetLabel(100);
  //set smooth mode on
  extractor->SmoothMode(true);
  extractor->ExtractLabel();
  extractor->OpDo();

  albaVME *surfaceExtracted = extractor->GetOutput();
  albaString name = surfaceExtracted->GetName();
  albaVME *surfaceNode = surfaceExtracted->FindInTreeByName(name.GetCStr());
  albaVMESurface *surfaceLabeled = albaVMESurface::SafeDownCast(surfaceNode);

  albaVME *surface = node->FindInTreeByName("label100smoothRG");

  albaVMESurface *surfaceOriginal = albaVMESurface::SafeDownCast(surface);

  albaVMEOutputSurface *surfaceOutputLab = albaVMEOutputSurface::SafeDownCast(surfaceLabeled->GetOutput());
  assert(surfaceOutputLab);
  surfaceOutputLab->Update();
  vtkPolyData *polydataLab = vtkPolyData::SafeDownCast(surfaceOutputLab->GetVTKData());
  CPPUNIT_ASSERT(polydataLab);


  albaVMEOutputSurface *surfaceOutput = albaVMEOutputSurface::SafeDownCast(surfaceOriginal->GetOutput());
  assert(surfaceOutput);
  surfaceOutput->Update();
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(surfaceOutput->GetVTKData());
  CPPUNIT_ASSERT(polydata);

  CPPUNIT_ASSERT(polydataLab->GetNumberOfPoints() == polydata->GetNumberOfPoints());

  double boundsLab[6];
  double bounds[6];

  polydataLab->GetBounds(boundsLab);
  polydata->GetBounds(bounds);

  CPPUNIT_ASSERT(
    fabs(boundsLab[0] - bounds[0]) < 0.0001 &&
    fabs(boundsLab[1] - bounds[1]) < 0.0001 &&
    fabs(boundsLab[2] - bounds[2]) < 0.0001 &&
    fabs(boundsLab[3] - bounds[3]) < 0.0001 &&
    fabs(boundsLab[4] - bounds[4]) < 0.0001 &&
    fabs(boundsLab[5] - bounds[5]) < 0.0001 
    );

  albaDEL(extractor);
  albaDEL(storage);
}


//---------------------------------------------------------------
void albaOpLabelExtractorTest::TestLabelSmoothSP()
//---------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  albaVMEFactory::Initialize();

  albaString filename = ALBA_DATA_ROOT;
  filename << "/Test_LabelExtractor/Test_LabelExtractor.alba";

  //Initialize storage
  albaVMEStorage *storage = albaVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == ALBA_OK);

  albaVME *node = storage->GetRoot();
  albaVME *volume = node->FindInTreeByName("CropTestVolumeSP");

  albaOpLabelExtractor *extractor = new albaOpLabelExtractor("label extractor");
  extractor->SetInput(volume);
  extractor->SetLabel(100);
  //set smooth mode on
  extractor->SmoothMode(true);
  extractor->ExtractLabel();
  extractor->OpDo();

  albaVME *surfaceExtracted = extractor->GetOutput();
  albaString name = surfaceExtracted->GetName();
  albaVME *surfaceNode = surfaceExtracted->FindInTreeByName(name.GetCStr());
  albaVMESurface *surfaceLabeled = albaVMESurface::SafeDownCast(surfaceNode);

  albaVME *surface = node->FindInTreeByName("label100smoothSP");

  albaVMESurface *surfaceOriginal = albaVMESurface::SafeDownCast(surface);

  albaVMEOutputSurface *surfaceOutputLab = albaVMEOutputSurface::SafeDownCast(surfaceLabeled->GetOutput());
  assert(surfaceOutputLab);
  surfaceOutputLab->Update();
  vtkDataSet *data = surfaceOutputLab->GetVTKData();
  vtkPolyData *polydataLab = vtkPolyData::SafeDownCast(surfaceOutputLab->GetVTKData());
  CPPUNIT_ASSERT(polydataLab);


  albaVMEOutputSurface *surfaceOutput = albaVMEOutputSurface::SafeDownCast(surfaceOriginal->GetOutput());
  assert(surfaceOutput);
  surfaceOutput->Update();
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(surfaceOutput->GetVTKData());
  CPPUNIT_ASSERT(polydata);

  CPPUNIT_ASSERT(polydataLab->GetNumberOfPoints() == polydata->GetNumberOfPoints());

  double boundsLab[6];
  double bounds[6];

  polydataLab->GetBounds(boundsLab);
  polydata->GetBounds(bounds);

  CPPUNIT_ASSERT(
    fabs(boundsLab[0] - bounds[0]) < 0.0001 &&
    fabs(boundsLab[1] - bounds[1]) < 0.0001 &&
    fabs(boundsLab[2] - bounds[2]) < 0.0001 &&
    fabs(boundsLab[3] - bounds[3]) < 0.0001 &&
    fabs(boundsLab[4] - bounds[4]) < 0.0001 &&
    fabs(boundsLab[5] - bounds[5]) < 0.0001 
    );

  albaDEL(extractor);
  albaDEL(storage);
}

