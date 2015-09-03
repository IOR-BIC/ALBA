/*=========================================================================

 Program: MAF2
 Module: mafOpLabelExtractorTest
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

#include "mafOpLabelExtractorTest.h"
#include "mafOpLabelExtractor.h"

#include "mafVMESurface.h"
#include "mafVMEFactory.h"
#include "vtkPolyData.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"

//----------------------------------------------------------------------------
void mafOpLabelExtractorTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//---------------------------------------------------------------
void mafOpLabelExtractorTest::TestDynamicAllocation()
//---------------------------------------------------------------
{
  mafOpLabelExtractor *extractor = new mafOpLabelExtractor("label extractor");
  mafDEL(extractor);
}

//---------------------------------------------------------------
void mafOpLabelExtractorTest::TestLabelRG()
//---------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_LabelExtractor/Test_LabelExtractor.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);

  mafNode *node = storage->GetRoot();
  mafNode *volume = node->FindInTreeByName("CropTestVolumeRG");

  mafOpLabelExtractor *extractor = new mafOpLabelExtractor("label extractor");
  extractor->SetInput(volume);
  extractor->SetLabel(100);
  extractor->SmoothMode(false);
  extractor->ExtractLabel();
  extractor->OpDo();
 
  mafNode *surfaceExtracted = extractor->GetOutput();
  mafString name = surfaceExtracted->GetName();
  mafNode *surfaceNode = surfaceExtracted->FindInTreeByName(name.GetCStr());
  mafVMESurface *surfaceLabeled = mafVMESurface::SafeDownCast(surfaceNode);

  mafNode *surface = node->FindInTreeByName("label100RG");

  mafVMESurface *surfaceOriginal = mafVMESurface::SafeDownCast(surface);

  mafVMEOutputSurface *surfaceOutputLab = mafVMEOutputSurface::SafeDownCast(surfaceLabeled->GetOutput());
  assert(surfaceOutputLab);
  surfaceOutputLab->Update();
  vtkPolyData *polydataLab = vtkPolyData::SafeDownCast(surfaceOutputLab->GetVTKData());
  CPPUNIT_ASSERT(polydataLab);
  polydataLab->Update();


  mafVMEOutputSurface *surfaceOutput = mafVMEOutputSurface::SafeDownCast(surfaceOriginal->GetOutput());
  assert(surfaceOutput);
  surfaceOutput->Update();
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(surfaceOutput->GetVTKData());
  CPPUNIT_ASSERT(polydata);
  polydata->Update();

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

  mafDEL(extractor);
  mafDEL(storage);
}


//---------------------------------------------------------------
void mafOpLabelExtractorTest::TestLabelSP()
//---------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_LabelExtractor/Test_LabelExtractor.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);

  mafNode *node = storage->GetRoot();
  mafNode *volume = node->FindInTreeByName("CropTestVolumeSP");

  mafOpLabelExtractor *extractor = new mafOpLabelExtractor("label extractor");
  extractor->SetInput(volume);
  extractor->SetLabel(100);
  extractor->SmoothMode(false);
  extractor->ExtractLabel();
  extractor->OpDo();

  mafNode *surfaceExtracted = extractor->GetOutput();
  mafString name = surfaceExtracted->GetName();
  mafNode *surfaceNode = surfaceExtracted->FindInTreeByName(name.GetCStr());
  mafVMESurface *surfaceLabeled = mafVMESurface::SafeDownCast(surfaceNode);

  mafNode *surface = node->FindInTreeByName("label100SP");

  mafVMESurface *surfaceOriginal = mafVMESurface::SafeDownCast(surface);

  mafVMEOutputSurface *surfaceOutputLab = mafVMEOutputSurface::SafeDownCast(surfaceLabeled->GetOutput());
  assert(surfaceOutputLab);
  surfaceOutputLab->Update();
  vtkDataSet *data = surfaceOutputLab->GetVTKData();
  vtkPolyData *polydataLab = vtkPolyData::SafeDownCast(surfaceOutputLab->GetVTKData());
  CPPUNIT_ASSERT(polydataLab);
  polydataLab->Update();


  mafVMEOutputSurface *surfaceOutput = mafVMEOutputSurface::SafeDownCast(surfaceOriginal->GetOutput());
  assert(surfaceOutput);
  surfaceOutput->Update();
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(surfaceOutput->GetVTKData());
  CPPUNIT_ASSERT(polydata);
  polydata->Update();

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

  mafDEL(extractor);
  mafDEL(storage);
}

//---------------------------------------------------------------
void mafOpLabelExtractorTest::TestLabelSmoothRG()
//---------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_LabelExtractor/Test_LabelExtractor.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);

  mafNode *node = storage->GetRoot();
  mafNode *volume = node->FindInTreeByName("CropTestVolumeRG");

  mafOpLabelExtractor *extractor = new mafOpLabelExtractor("label extractor");
  extractor->SetInput(volume);
  extractor->SetLabel(100);
  //set smooth mode on
  extractor->SmoothMode(true);
  extractor->ExtractLabel();
  extractor->OpDo();

  mafNode *surfaceExtracted = extractor->GetOutput();
  mafString name = surfaceExtracted->GetName();
  mafNode *surfaceNode = surfaceExtracted->FindInTreeByName(name.GetCStr());
  mafVMESurface *surfaceLabeled = mafVMESurface::SafeDownCast(surfaceNode);

  mafNode *surface = node->FindInTreeByName("label100smoothRG");

  mafVMESurface *surfaceOriginal = mafVMESurface::SafeDownCast(surface);

  mafVMEOutputSurface *surfaceOutputLab = mafVMEOutputSurface::SafeDownCast(surfaceLabeled->GetOutput());
  assert(surfaceOutputLab);
  surfaceOutputLab->Update();
  vtkPolyData *polydataLab = vtkPolyData::SafeDownCast(surfaceOutputLab->GetVTKData());
  CPPUNIT_ASSERT(polydataLab);
  polydataLab->Update();


  mafVMEOutputSurface *surfaceOutput = mafVMEOutputSurface::SafeDownCast(surfaceOriginal->GetOutput());
  assert(surfaceOutput);
  surfaceOutput->Update();
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(surfaceOutput->GetVTKData());
  CPPUNIT_ASSERT(polydata);
  polydata->Update();

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

  mafDEL(extractor);
  mafDEL(storage);
}


//---------------------------------------------------------------
void mafOpLabelExtractorTest::TestLabelSmoothSP()
//---------------------------------------------------------------
{
  // in order to create VME from storage we need the factory to initialize 
  mafVMEFactory::Initialize();

  mafString filename = MAF_DATA_ROOT;
  filename << "/Test_LabelExtractor/Test_LabelExtractor.msf";

  //Initialize storage
  mafVMEStorage *storage = mafVMEStorage::New();
  storage->GetRoot()->SetName("root");
  storage->GetRoot()->Initialize();
  storage->SetURL(filename);

  int res = storage->Restore();
  CPPUNIT_ASSERT(res == MAF_OK);

  mafNode *node = storage->GetRoot();
  mafNode *volume = node->FindInTreeByName("CropTestVolumeSP");

  mafOpLabelExtractor *extractor = new mafOpLabelExtractor("label extractor");
  extractor->SetInput(volume);
  extractor->SetLabel(100);
  //set smooth mode on
  extractor->SmoothMode(true);
  extractor->ExtractLabel();
  extractor->OpDo();

  mafNode *surfaceExtracted = extractor->GetOutput();
  mafString name = surfaceExtracted->GetName();
  mafNode *surfaceNode = surfaceExtracted->FindInTreeByName(name.GetCStr());
  mafVMESurface *surfaceLabeled = mafVMESurface::SafeDownCast(surfaceNode);

  mafNode *surface = node->FindInTreeByName("label100smoothSP");

  mafVMESurface *surfaceOriginal = mafVMESurface::SafeDownCast(surface);

  mafVMEOutputSurface *surfaceOutputLab = mafVMEOutputSurface::SafeDownCast(surfaceLabeled->GetOutput());
  assert(surfaceOutputLab);
  surfaceOutputLab->Update();
  vtkDataSet *data = surfaceOutputLab->GetVTKData();
  vtkPolyData *polydataLab = vtkPolyData::SafeDownCast(surfaceOutputLab->GetVTKData());
  CPPUNIT_ASSERT(polydataLab);
  polydataLab->Update();


  mafVMEOutputSurface *surfaceOutput = mafVMEOutputSurface::SafeDownCast(surfaceOriginal->GetOutput());
  assert(surfaceOutput);
  surfaceOutput->Update();
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(surfaceOutput->GetVTKData());
  CPPUNIT_ASSERT(polydata);
  polydata->Update();

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

  mafDEL(extractor);
  mafDEL(storage);
}

