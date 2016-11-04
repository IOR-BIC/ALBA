/*=========================================================================

 Program: MAF2
 Module: mafVMEMeshAnsysTextImporterTest
 Authors: Stefano Perticoni
 
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

#include "mafVMEMeshAnsysTextImporterTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "mafVMEMesh.h"
#include "mafVMEMeshAnsysTextImporter.h"

#include "vtkMAFSmartPointer.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkCell.h"

#include "vtkFileOutputWindow.h"
#include "vtkPointData.h"
#include "vtkCellData.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkDataSetMapper.h"
#include "vtkLookupTable.h"
#include "vtkActor.h"

void mafVMEMeshAnsysTextImporterTest::TestFixture()
{

}

void mafVMEMeshAnsysTextImporterTest::BeforeTest()
{
 
}

void mafVMEMeshAnsysTextImporterTest::AfterTest()
{

}

void mafVMEMeshAnsysTextImporterTest::TestConstructor()
{
  mafVMEMeshAnsysTextImporter *importer = new mafVMEMeshAnsysTextImporter  ;
  CPPUNIT_ASSERT(importer);
  CPPUNIT_ASSERT(importer->GetOutput() == NULL);

  delete importer;
}


void mafVMEMeshAnsysTextImporterTest::Read(mafVMEMeshAnsysTextImporter* reader, mafString &dirPrefix, bool readMaterials /*= false*/, bool writeToDisk /*= false*/, mafString &outputFileName )
{
  CPPUNIT_ASSERT(reader != NULL);

  mafString nodesFileName = dirPrefix;
  nodesFileName << "NLIST.lis";

  mafString elementsFileName = dirPrefix;
  elementsFileName << "ELIST.lis";

  cerr << std::endl << "nodes file:" << nodesFileName.GetCStr() << std::endl;
  reader->SetNodesFileName(nodesFileName.GetCStr());

  cerr << "elements file:" << elementsFileName.GetCStr() << std::endl;
  reader->SetElementsFileName(elementsFileName.GetCStr());

  mafString materialsFileName;

  if (readMaterials == true)
  {
    materialsFileName = dirPrefix;
    materialsFileName << "MPLIST.lis";

    cerr << "materials file:" << materialsFileName.GetCStr() << std::endl;
    reader->SetMaterialsFileName(materialsFileName.GetCStr());
  }

  CPPUNIT_ASSERT(reader->Read() == MAF_OK);

  CPPUNIT_ASSERT(reader->GetOutput() != NULL);

  vtkUnstructuredGrid* data = reader->GetOutput()->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  data->Update();
  
	mafString outputDir = GET_TEST_DATA_DIR();
  SaveUnstructuredGridToFile(outputDir, outputFileName, data);
}

void mafVMEMeshAnsysTextImporterTest::SaveUnstructuredGridToFile(mafString &dirPrefix, mafString &fileName, vtkUnstructuredGrid *data)
{
  // save output to file
  vtkMAFSmartPointer<vtkUnstructuredGridWriter> writer;
  writer->SetInput(data) ;
  writer->SetFileTypeToASCII();

  mafString gridFileName = dirPrefix;
  // gridFileName << "vtkUnstructuredGrid.vtk";
  gridFileName << fileName.GetCStr();

  writer->SetFileName(gridFileName.GetCStr());
  writer->Write();
}
void mafVMEMeshAnsysTextImporterTest::ReadAndDisplay( mafString &dirPrefix, int dataType /*= 0*/, bool readMaterials /*= false*/, bool writeToDisk /*= false*/ )
{
  mafVMEMeshAnsysTextImporter* reader = new mafVMEMeshAnsysTextImporter;
  CPPUNIT_ASSERT(reader != NULL);

  mafString nodesFileName = dirPrefix;
  nodesFileName << "NLIST.lis";

  mafString elementsFileName = dirPrefix;
  elementsFileName << "ELIST.lis";

  cerr << std::endl << "nodes file:" << nodesFileName.GetCStr() << std::endl;
  reader->SetNodesFileName(nodesFileName.GetCStr());

  cerr << "elements file:" << elementsFileName.GetCStr() << std::endl;
  reader->SetElementsFileName(elementsFileName.GetCStr());

  mafString materialsFileName;
  
  if (readMaterials == true)
  {
    materialsFileName = dirPrefix;
    materialsFileName << "MPLIST.lis";

    cerr << "materials file:" << materialsFileName.GetCStr() << std::endl;
    reader->SetMaterialsFileName(materialsFileName.GetCStr());
  }
    
  CPPUNIT_ASSERT(reader->Read() == MAF_OK);

  CPPUNIT_ASSERT(reader->GetOutput() != NULL);

  vtkUnstructuredGrid *ugrid = vtkUnstructuredGrid::New();
  vtkUnstructuredGrid* data = reader->GetOutput()->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  data->Update();

  // some tests on the geometry...

  if (reader->GetMeshType() == mafVMEMeshAnsysTextImporter::LINEAR)
  {
    if (reader->GetElementType() == mafVMEMeshAnsysTextImporter::TETRA4)
    {
      CPPUNIT_ASSERT(data->GetCell(0)->GetNumberOfPoints() == 4);
    }
    else if (reader->GetElementType() == mafVMEMeshAnsysTextImporter::HEXA8) 
    {
      CPPUNIT_ASSERT(data->GetCell(0)->GetNumberOfPoints() == 8);
    }
    else
    {
      CPPUNIT_ASSERT(false);
    }
  } 
  else if (reader->GetMeshType() == mafVMEMeshAnsysTextImporter::PARABOLIC)
  {
    if (reader->GetElementType() == mafVMEMeshAnsysTextImporter::TETRA10)
    {
      CPPUNIT_ASSERT(data->GetCell(0)->GetNumberOfPoints() == 10);
    }
    else if (reader->GetElementType() == mafVMEMeshAnsysTextImporter::HEXA20) 
    {
      CPPUNIT_ASSERT(data->GetCell(0)->GetNumberOfPoints() == 20);
    }
    else
    {
      CPPUNIT_ASSERT(false);
    }
  }
  else
  {
    CPPUNIT_ASSERT(false);
  }
  ugrid->DeepCopy(vtkUnstructuredGrid::SafeDownCast(data));  

  if (writeToDisk)
  {
		mafString outputDir = GET_TEST_DATA_DIR();
    SaveUnstructuredGridToFile(outputDir, mafString("vtkUnstructuredGrid.vtk"), data);
  }

  if (dataType == POINT_DATA)
  {
    ugrid->GetPointData()->SetActiveScalars("Id");
    int ntuples = ugrid->GetPointData()->GetArray("Id")->GetNumberOfTuples();
    CPPUNIT_ASSERT(ntuples == ugrid->GetNumberOfPoints());

    // always render point data
    RenderData(ugrid, dataType);
  } 
  else if (dataType == CELL_DATA)
  {
    ugrid->GetCellData()->SetActiveScalars("Material");
    int ntuples = ugrid->GetCellData()->GetArray("Material")->GetNumberOfTuples();
    CPPUNIT_ASSERT(ntuples == ugrid->GetNumberOfCells());

    // render cell data for linear cells only (vtk 4.4 bug)
    if (reader->GetMeshType() == mafVMEMeshAnsysTextImporter::LINEAR)
    {
      RenderData(ugrid, dataType);
    }

  }
  else
  {
    CPPUNIT_ASSERT(false);
  }
  
  delete reader;
  ugrid->Delete();
}

void mafVMEMeshAnsysTextImporterTest::RenderData(  vtkUnstructuredGrid *data, int dataType)
{
  //----------------------------
  // lookup table stuff
  //----------------------------

  // i reverse the color table  to use blue for the lowest values and red for the highest
  vtkMAFSmartPointer<vtkLookupTable> defaultLut;
  vtkMAFSmartPointer<vtkLookupTable> reverseLut;

  int colNum = 256;

  defaultLut->SetNumberOfColors(colNum);
  defaultLut->SetHueRange(0.0 ,.66667);
  double range[2] = {0,0};

  if (dataType == POINT_DATA)
  {
    data->GetPointData()->GetScalars("Id")->GetRange(range);
    cout << "point ID range: [" << range[0] <<":" << range[1] << "]" << std::endl ;

  } 
  else if (dataType == CELL_DATA)
  {
    data->GetCellData()->GetScalars("Material")->GetRange(range);
    cout << "material range: [" << range[0] <<":" << range[1] << "]"<<  std::endl;
  }
  else
  {
    CPPUNIT_ASSERT(false);
  }

  defaultLut->SetTableRange(range);
  defaultLut->Build();

  reverseLut->SetNumberOfColors(colNum);
  reverseLut->SetTableRange(defaultLut->GetTableRange());
  for(int i = 0;i < colNum; i++)
  {
    reverseLut->SetTableValue(i, defaultLut->GetTableValue(colNum-i));
  }

  //----------------------------

  vtkMAFSmartPointer<vtkRenderer> renderer;
  renderer->SetBackground(0.1, 0.1, 0.1);

  vtkMAFSmartPointer<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetPosition(400,0);

  vtkMAFSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkMAFSmartPointer<vtkDataSetMapper> mapper;
  mapper->ScalarVisibilityOn();

  if (dataType == POINT_DATA)
  {
    mapper->SetScalarModeToUsePointData();
  } 
  else
  {
    mapper->SetScalarModeToUseCellData();
  }
  
  mapper->UseLookupTableScalarRangeOff();
  mapper->SetScalarRange(reverseLut->GetTableRange());
  mapper->SetLookupTable(reverseLut);
  mapper->SetInput(data);

  vtkMAFSmartPointer<vtkActor> actor;
  actor->SetMapper(mapper);

  renderer->AddActor(actor);
  renderWindow->Render();
  
  // remove comment below for user interaction...
  // renderWindowInteractor->Start();
}
  
void mafVMEMeshAnsysTextImporterTest::TestTetra4()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet4/";

  // test without materials
  ReadAndDisplay(dirPrefix, POINT_DATA);
  ReadAndDisplay(dirPrefix, CELL_DATA);
  
  // test with materials
  ReadAndDisplay(dirPrefix, POINT_DATA, true);
  ReadAndDisplay(dirPrefix, CELL_DATA, true,true);
}

void mafVMEMeshAnsysTextImporterTest::TestTetra10()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet10/";
 
  // test without materials
  ReadAndDisplay(dirPrefix, POINT_DATA);
  
  // DISPLAY NOT WORKING!!! in vtk 4.4
  ReadAndDisplay(dirPrefix, CELL_DATA);

  // test with materials
  ReadAndDisplay(dirPrefix, POINT_DATA, true);
  // DISPLAY NOT WORKING!!! in vtk 4.4
  ReadAndDisplay(dirPrefix, CELL_DATA, true,true);
}

void mafVMEMeshAnsysTextImporterTest::TestTetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet10/Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar/";

  // test without materials
  ReadAndDisplay(dirPrefix, POINT_DATA);

  // DISPLAY NOT WORKING!!! in vtk 4.4
  ReadAndDisplay(dirPrefix, CELL_DATA);

  // test with materials
  ReadAndDisplay(dirPrefix, POINT_DATA, true);
  // DISPLAY NOT WORKING!!! in vtk 4.4
  ReadAndDisplay(dirPrefix, CELL_DATA, true,true);

}

int mafVMEMeshAnsysTextImporterTest::GetRowsNumber(vtkUnstructuredGrid *inputUGrid)
{
  int numberOfPoints = inputUGrid->GetNumberOfPoints();

  int rowsNumber = numberOfPoints;
  return rowsNumber;
}
void mafVMEMeshAnsysTextImporterTest::TestHexa8()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/hex8/";
  
  // test without materials
  ReadAndDisplay(dirPrefix, POINT_DATA);
  ReadAndDisplay(dirPrefix, CELL_DATA);

  // test with materials
  ReadAndDisplay(dirPrefix, POINT_DATA, true);
  ReadAndDisplay(dirPrefix, CELL_DATA, true,true);
}

void mafVMEMeshAnsysTextImporterTest::TestHexa20()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/hex20/";

  // test without materials
  ReadAndDisplay(dirPrefix, POINT_DATA);
  // DISPLAY NOT WORKING!!! in vtk 4.4
  ReadAndDisplay(dirPrefix, CELL_DATA);

  // test with materials
  ReadAndDisplay(dirPrefix, POINT_DATA, true);  
  // DISPLAY NOT WORKING!!! in vtk 4.4
  ReadAndDisplay(dirPrefix, CELL_DATA, true,true);
}
