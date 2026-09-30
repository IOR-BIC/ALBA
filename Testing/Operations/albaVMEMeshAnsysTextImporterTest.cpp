/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMeshAnsysTextImporterTest
 Authors: Stefano Perticoni
 
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

#include "albaVMEMeshAnsysTextImporterTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "albaVMEMesh.h"
#include "albaVMEMeshAnsysTextImporter.h"

#include "vtkALBASmartPointer.h"
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

void albaVMEMeshAnsysTextImporterTest::TestFixture()
{

}

void albaVMEMeshAnsysTextImporterTest::BeforeTest()
{
 
}

void albaVMEMeshAnsysTextImporterTest::AfterTest()
{

}

void albaVMEMeshAnsysTextImporterTest::TestConstructor()
{
  albaVMEMeshAnsysTextImporter *importer = new albaVMEMeshAnsysTextImporter  ;
  CPPUNIT_ASSERT(importer);
  CPPUNIT_ASSERT(importer->GetOutput() == NULL);

  delete importer;
}


void albaVMEMeshAnsysTextImporterTest::Read(albaVMEMeshAnsysTextImporter* reader, albaString &dirPrefix, bool readMaterials /*= false*/, bool writeToDisk /*= false*/, albaString &outputFileName )
{
  CPPUNIT_ASSERT(reader != NULL);

  albaString nodesFileName = dirPrefix;
  nodesFileName << "NLIST.lis";

  albaString elementsFileName = dirPrefix;
	if(readMaterials)
	  elementsFileName << "ELIST_MAT.lis";
	else
		elementsFileName << "ELIST.lis";

  cerr << std::endl << "nodes file:" << nodesFileName.GetCStr() << std::endl;
  reader->SetNodesFileName(nodesFileName.GetCStr());

  cerr << "elements file:" << elementsFileName.GetCStr() << std::endl;
  reader->SetElementsFileName(elementsFileName.GetCStr());

  albaString materialsFileName;

  if (readMaterials == true)
  {
    materialsFileName = dirPrefix;
    materialsFileName << "MPLIST.lis";

    cerr << "materials file:" << materialsFileName.GetCStr() << std::endl;
    reader->SetMaterialsFileName(materialsFileName.GetCStr());
		reader->SetMode(albaVMEMeshAnsysTextImporter::WITH_MAT_MODE);
	}
	else
	{
		reader->SetMode(albaVMEMeshAnsysTextImporter::WITHOUT_MAT_MODE);
	}

	CPPUNIT_ASSERT(reader->Read() == ALBA_OK);

	CPPUNIT_ASSERT(reader->GetOutput() != NULL);

	vtkUnstructuredGrid* data = reader->GetOutput()->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  
	albaString outputDir = GET_TEST_DATA_DIR();
  SaveUnstructuredGridToFile(outputDir, outputFileName, data);
}

void albaVMEMeshAnsysTextImporterTest::SaveUnstructuredGridToFile(albaString &dirPrefix, albaString &fileName, vtkUnstructuredGrid *data)
{
  // save output to file
  vtkALBASmartPointer<vtkUnstructuredGridWriter> writer;
  writer->SetInputData(data) ;
  writer->SetFileTypeToASCII();

  albaString gridFileName = dirPrefix;
  // gridFileName << "vtkUnstructuredGrid.vtk";
  gridFileName << fileName.GetCStr();

  writer->SetFileName(gridFileName.GetCStr());
  writer->Write();
}
void albaVMEMeshAnsysTextImporterTest::ReadAndDisplay( albaString &dirPrefix, int dataType /*= 0*/, bool readMaterials /*= false*/, bool writeToDisk /*= false*/ )
{
  albaVMEMeshAnsysTextImporter* reader = new albaVMEMeshAnsysTextImporter;
  CPPUNIT_ASSERT(reader != NULL);

  albaString nodesFileName = dirPrefix;
  nodesFileName << "NLIST.lis";

  albaString elementsFileName = dirPrefix;
	if (readMaterials)
		elementsFileName << "ELIST_MAT.lis";
	else
		elementsFileName << "ELIST.lis";

  cerr << std::endl << "nodes file:" << nodesFileName.GetCStr() << std::endl;
  reader->SetNodesFileName(nodesFileName.GetCStr());

  cerr << "elements file:" << elementsFileName.GetCStr() << std::endl;
  reader->SetElementsFileName(elementsFileName.GetCStr());

  albaString materialsFileName;
  
  if (readMaterials == true)
  {
    materialsFileName = dirPrefix;
    materialsFileName << "MPLIST.lis";

    cerr << "materials file:" << materialsFileName.GetCStr() << std::endl;
    reader->SetMaterialsFileName(materialsFileName.GetCStr());
		reader->SetMode(albaVMEMeshAnsysTextImporter::WITH_MAT_MODE);
  }
	else
	{
		reader->SetMode(albaVMEMeshAnsysTextImporter::WITHOUT_MAT_MODE);
	}
    
  CPPUNIT_ASSERT(reader->Read() == ALBA_OK);

  CPPUNIT_ASSERT(reader->GetOutput() != NULL);

  vtkUnstructuredGrid *ugrid = vtkUnstructuredGrid::New();
  vtkUnstructuredGrid* data = reader->GetOutput()->GetUnstructuredGridOutput()->GetUnstructuredGridData();

  // some tests on the geometry...

  if (reader->GetMeshType() == albaVMEMeshAnsysTextImporter::LINEAR)
  {
    if (reader->GetElementType() == albaVMEMeshAnsysTextImporter::TETRA4)
    {
      CPPUNIT_ASSERT(data->GetCell(0)->GetNumberOfPoints() == 4);
    }
    else if (reader->GetElementType() == albaVMEMeshAnsysTextImporter::HEXA8) 
    {
      CPPUNIT_ASSERT(data->GetCell(0)->GetNumberOfPoints() == 8);
    }
    else
    {
      CPPUNIT_ASSERT(false);
    }
  } 
  else if (reader->GetMeshType() == albaVMEMeshAnsysTextImporter::PARABOLIC)
  {
    if (reader->GetElementType() == albaVMEMeshAnsysTextImporter::TETRA10)
    {
      CPPUNIT_ASSERT(data->GetCell(0)->GetNumberOfPoints() == 10);
    }
    else if (reader->GetElementType() == albaVMEMeshAnsysTextImporter::HEXA20) 
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
		albaString outputDir = GET_TEST_DATA_DIR();
    SaveUnstructuredGridToFile(outputDir, albaString("vtkUnstructuredGrid.vtk"), data);
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
		if (readMaterials)
		{
			ugrid->GetCellData()->SetActiveScalars("Material");
			int ntuples = ugrid->GetCellData()->GetArray("Material")->GetNumberOfTuples();
			CPPUNIT_ASSERT(ntuples == ugrid->GetNumberOfCells());
		}

    // render cell data for linear cells only (vtk 4.4 bug)
    if (reader->GetMeshType() == albaVMEMeshAnsysTextImporter::LINEAR)
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

void albaVMEMeshAnsysTextImporterTest::RenderData(  vtkUnstructuredGrid *data, int dataType)
{
  //----------------------------
  // lookup table stuff
  //----------------------------

  // i reverse the color table  to use blue for the lowest values and red for the highest
  vtkALBASmartPointer<vtkLookupTable> defaultLut;
  vtkALBASmartPointer<vtkLookupTable> reverseLut;

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
		if (data->GetCellData()->GetScalars("Material"))
		{
			data->GetCellData()->GetScalars("Material")->GetRange(range);
			cout << "material range: [" << range[0] << ":" << range[1] << "]" << std::endl;
		}
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

  vtkALBASmartPointer<vtkRenderer> renderer;
  renderer->SetBackground(0.1, 0.1, 0.1);

  vtkALBASmartPointer<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);
  renderWindow->SetPosition(400,0);

  vtkALBASmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkALBASmartPointer<vtkDataSetMapper> mapper;
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
  mapper->SetInputData(data);

  vtkALBASmartPointer<vtkActor> actor;
  actor->SetMapper(mapper);

  renderer->AddActor(actor);
  renderWindow->Render();
  
  // remove comment below for user interaction...
  // renderWindowInteractor->Start();
}
  
void albaVMEMeshAnsysTextImporterTest::TestTetra4()
{
  albaString dirPrefix = ALBA_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet4/";

  // test without materials
  ReadAndDisplay(dirPrefix, POINT_DATA);
  ReadAndDisplay(dirPrefix, CELL_DATA);
  
  // test with materials
  ReadAndDisplay(dirPrefix, POINT_DATA, true);
  ReadAndDisplay(dirPrefix, CELL_DATA, true,true);
}

void albaVMEMeshAnsysTextImporterTest::TestTetra10()
{
  albaString dirPrefix = ALBA_DATA_ROOT;
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

void albaVMEMeshAnsysTextImporterTest::TestTetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar()
{
  albaString dirPrefix = ALBA_DATA_ROOT;
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

int albaVMEMeshAnsysTextImporterTest::GetRowsNumber(vtkUnstructuredGrid *inputUGrid)
{
  int numberOfPoints = inputUGrid->GetNumberOfPoints();

  int rowsNumber = numberOfPoints;
  return rowsNumber;
}
void albaVMEMeshAnsysTextImporterTest::TestHexa8()
{
  albaString dirPrefix = ALBA_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/hex8/";
  
  // test without materials
  ReadAndDisplay(dirPrefix, POINT_DATA);
  ReadAndDisplay(dirPrefix, CELL_DATA);

  // test with materials
  ReadAndDisplay(dirPrefix, POINT_DATA, true);
  ReadAndDisplay(dirPrefix, CELL_DATA, true,true);
}

void albaVMEMeshAnsysTextImporterTest::TestHexa20()
{
  albaString dirPrefix = ALBA_DATA_ROOT;
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
