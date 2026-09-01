/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMeshAnsysTextExporterTest
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

#include "albaVMEMeshAnsysTextExporterTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "albaVMEMesh.h"
#include "albaVMEMeshAnsysTextExporter.h"

#include "vtkALBASmartPointer.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkCell.h"

#include "vtkFileOutputWindow.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGridReader.h"

// render window stuff
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkDataSetMapper.h"
#include "vtkLookupTable.h"
#include "vtkActor.h"

//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::TestFixture()
{

}
//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::BeforeTest()
{

}
//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::AfterTest()
{

}
//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::TestConstructorDestructor()
{
  albaVMEMeshAnsysTextExporter *exporter = new albaVMEMeshAnsysTextExporter  ;
  CPPUNIT_ASSERT(exporter);
  CPPUNIT_ASSERT(exporter->GetInput() == NULL);

  delete exporter;
}

//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::Read( albaVMEMeshAnsysTextImporter *reader, \
albaString &dirPrefix, bool readMaterials /*= false*/, bool writeToDisk /*= false*/,\
albaString &outputFileName , albaString inputNLISTFileName /*= "NLIST.lis"*/, \
albaString inputELISTFileName /*= "ELIST.lis"*/, albaString inputMPLISTFileName /*= "MPLIST.LIS"*/ )
{
  CPPUNIT_ASSERT(reader != NULL);

  albaString nodesFileName = dirPrefix;
  nodesFileName << inputNLISTFileName.GetCStr();

  albaString elementsFileName = dirPrefix;
  elementsFileName << inputELISTFileName.GetCStr();

  cerr << std::endl << "nodes file:" << nodesFileName.GetCStr() << std::endl;
  reader->SetNodesFileName(nodesFileName.GetCStr());

  cerr << "elements file:" << elementsFileName.GetCStr() << std::endl;
  reader->SetElementsFileName(elementsFileName.GetCStr());

  albaString materialsFileName;

  if (readMaterials == true)
  {
    materialsFileName = dirPrefix;
    materialsFileName << inputMPLISTFileName.GetCStr();

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

//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::SaveUnstructuredGridToFile(albaString &dirPrefix, albaString &fileName, vtkUnstructuredGrid *data)
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
//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::ReadAndDisplay( albaString &dirPrefix, int dataType /*= 0*/, bool readMaterials /*= false*/, bool writeToDisk /*= false*/ )
{
  albaVMEMeshAnsysTextImporter* reader = new albaVMEMeshAnsysTextImporter;
  CPPUNIT_ASSERT(reader != NULL);

  albaString nodesFileName = dirPrefix;
  nodesFileName << "NLIST.lis";

  albaString elementsFileName = dirPrefix;
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
    ugrid->GetCellData()->SetActiveScalars("Material");
    int ntuples = ugrid->GetCellData()->GetArray("Material")->GetNumberOfTuples();
    CPPUNIT_ASSERT(ntuples == ugrid->GetNumberOfCells());

    // render cell data for linear cells only (vtk 4.4 bug)
    if (reader->GetElementType() == albaVMEMeshAnsysTextImporter::LINEAR)
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

//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::RenderData(  vtkUnstructuredGrid *data, int dataType)
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
    data->GetCellData()->GetScalars("Material")->GetRange(range);
    cout << "Material range: [" << range[0] <<":" << range[1] << "]"<<  std::endl;
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

//----------------------------------------------------------------------------------------
int albaVMEMeshAnsysTextExporterTest::GetRowsNumber(vtkUnstructuredGrid *inputUGrid)
{
  int numberOfPoints = inputUGrid->GetNumberOfPoints();

  int rowsNumber = numberOfPoints;
  return rowsNumber;
}

//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::TestTetra10TrivialMeshExport()
{
	albaString dirPrefix = ALBA_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet10/";

  albaVMEMeshAnsysTextImporter *reader = new albaVMEMeshAnsysTextImporter;
  this->Read(reader, dirPrefix, false, true, albaString("tet10NoMaterials.vtk"));

  vtkUnstructuredGrid *ugrid = reader->GetOutput()->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  int numPoints = ugrid->GetNumberOfPoints();
  int numCells = ugrid->GetNumberOfCells();

  cout << "number of points: " << numPoints << std::endl;
  cout << "number of cells: " << numCells << std::endl;

  // mesh without materials
  CPPUNIT_ASSERT(numPoints == 14);
  CPPUNIT_ASSERT(numCells == 2);

  // create text exporter
	wxString outputDir = GET_TEST_DATA_DIR();
  albaString outputNodesFileName = outputDir;
	outputNodesFileName << "/NLISTNoMaterialsWritten.txt";
  
  albaString outputElementsFileName = outputDir;
	outputElementsFileName << "/ELISTNoMaterialsWritten.txt";

  albaVMEMeshAnsysTextExporter *exporter = new albaVMEMeshAnsysTextExporter;
  exporter->SetInput(ugrid);
  exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
  exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());

  exporter->Write();

  // mesh with materials
  this->Read(reader, dirPrefix, true, true, albaString("tet10WithMaterials.vtk"),"NLIST.lis","ELIST_MAT.lis");
  numPoints = ugrid->GetNumberOfPoints();
  numCells = ugrid->GetNumberOfCells();

  CPPUNIT_ASSERT(numPoints == 14);
  CPPUNIT_ASSERT(numCells == 2);

  outputNodesFileName = outputDir;
  outputNodesFileName.Append("NLISTMaterialsWritten.txt");

  outputElementsFileName = outputDir;
  outputElementsFileName.Append("ELISTMaterialsWritten.txt");

  albaString outputMaterialsFileName = outputDir;
  outputMaterialsFileName.Append("MPLISTNoMaterialsWritten.txt");

  exporter->SetInput(ugrid);
  exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
  exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());

  exporter->Write();

  // cleanup
  delete exporter;
  delete reader;
}

//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::TestTetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarExport()
{
	albaString dirPrefix = ALBA_DATA_ROOT;
	dirPrefix << "/FEM/ANSYS/tet10/Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar/";

	albaVMEMeshAnsysTextImporter *reader = new albaVMEMeshAnsysTextImporter;
	this->Read(reader, dirPrefix, false, true, albaString("tet10NoMaterials.vtk"));

	vtkUnstructuredGrid *ugrid = reader->GetOutput()->GetUnstructuredGridOutput()->GetUnstructuredGridData();
	int numPoints = ugrid->GetNumberOfPoints();
	int numCells = ugrid->GetNumberOfCells();

	cout << "number of points: " << numPoints << std::endl;
	cout << "number of cells: " << numCells << std::endl;

	// mesh without materials
	CPPUNIT_ASSERT(numPoints == 86);
	CPPUNIT_ASSERT(numCells == 17);

	// create text exporter
	wxString outputDir = GET_TEST_DATA_DIR();
	albaString outputNodesFileName = outputDir;
	outputNodesFileName << "/NLISTNoMaterialsWritten.txt";

	albaString outputElementsFileName = outputDir;
	outputElementsFileName << "/ELISTNoMaterialsWritten.txt";

	albaVMEMeshAnsysTextExporter *exporter = new albaVMEMeshAnsysTextExporter;
	exporter->SetInput(ugrid);
	exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
	exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());

	exporter->Write();

	// mesh with materials
	this->Read(reader, dirPrefix, true, true, albaString("tet10WithMaterials.vtk"),"NLIST.lis","ELIST_MAT.lis");
	numPoints = ugrid->GetNumberOfPoints();
	numCells = ugrid->GetNumberOfCells();

	CPPUNIT_ASSERT(numPoints == 86);
	CPPUNIT_ASSERT(numCells == 17);

	outputNodesFileName = outputDir;
	outputNodesFileName.Append("NLISTMaterialsWritten.txt");

	outputElementsFileName = outputDir;
	outputElementsFileName.Append("ELISTMaterialsWritten.txt");

	albaString outputMaterialsFileName = outputDir;
	outputMaterialsFileName.Append("MPLISTNoMaterialsWritten.txt");

	exporter->SetInput(ugrid);
	exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
	exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());

	exporter->Write();

	// cleanup
	delete exporter;
	delete reader;
}

//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::TestBonemattedTetra10ElementsIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarExport()
{
	albaString dirPrefix = ALBA_DATA_ROOT;
	dirPrefix << "/FEM/ANSYS/tet10/bonemattedMeshTetra10ANSYS_ELEMENT_IDJumpingAnsysExportTest/WithMaterials/";

	CPPUNIT_ASSERT(wxDirExists(dirPrefix.GetCStr()));

	albaString fileName = dirPrefix;
	fileName.Append("BonemattedMeshWithMaterials.vtk");

	CPPUNIT_ASSERT(wxFileExists(fileName.GetCStr()));

	vtkALBASmartPointer<vtkUnstructuredGridReader> ugr;
	ugr->SetFileName(fileName.GetCStr());
	ugr->Update();

	vtkUnstructuredGrid *ugrid = ugr->GetOutput();

	int numPoints = ugrid->GetNumberOfPoints();
	int numCells = ugrid->GetNumberOfCells();

	cout << "number of points: " << numPoints << std::endl;
	cout << "number of cells: " << numCells << std::endl;

	CPPUNIT_ASSERT(numPoints == 57);
	CPPUNIT_ASSERT(numCells == 15);

	wxString outputDir = GET_TEST_DATA_DIR();
	albaString outputNodesFileName = outputDir;
	outputNodesFileName << "/NLISTMeshWithMaterialsWritten.txt";

	albaString outputElementsFileName = outputDir;
	outputElementsFileName << "/ELISTMeshWithMaterialsWritten.txt";

	albaString outputMaterialsFileName = outputDir;
	outputMaterialsFileName << "/MPLISTMeshWithMaterialsWritten.txt";

	albaVMEMeshAnsysTextExporter *exporter = new albaVMEMeshAnsysTextExporter;
	exporter->SetInput(ugrid);
	exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
	exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());
	exporter->SetOutputMaterialsFileName(outputMaterialsFileName.GetCStr());
	exporter->Write();

 	albaVMEMeshAnsysTextImporter *reader = new albaVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(outputNodesFileName.GetCStr());
	reader->SetElementsFileName(outputElementsFileName.GetCStr());
	reader->SetMaterialsFileName(outputMaterialsFileName.GetCStr());
	reader->Read();
// 	this->Read(reader, dirPrefix, true, true, albaString("BonemattedMeshWithMaterials.vtk"));
// 
	exporter->Write();

	// cleanup
	delete exporter;
	delete reader;
}

//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::TestBonemattedTetra10ElementsIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarExportWithAppliedPose()
{
	albaString dirPrefix = ALBA_DATA_ROOT;
	dirPrefix << "/FEM/ANSYS/tet10/bonemattedMeshTetra10ANSYS_ELEMENT_IDJumpingAnsysExportTest/WithMaterials/";

	CPPUNIT_ASSERT(wxDirExists(dirPrefix.GetCStr()));

	albaString fileName = dirPrefix;
	fileName.Append("BonemattedMeshWithMaterials.vtk");

	CPPUNIT_ASSERT(wxFileExists(fileName.GetCStr()));

	vtkALBASmartPointer<vtkUnstructuredGridReader> ugr;
	ugr->SetFileName(fileName.GetCStr());
	ugr->Update();

	vtkUnstructuredGrid *ugrid = ugr->GetOutput();

	int numPoints = ugrid->GetNumberOfPoints();
	int numCells = ugrid->GetNumberOfCells();

	cout << "number of points: " << numPoints << std::endl;
	cout << "number of cells: " << numCells << std::endl;

	CPPUNIT_ASSERT(numPoints == 57);
	CPPUNIT_ASSERT(numCells == 15);

	wxString outputDir = GET_TEST_DATA_DIR();
	albaString outputNodesFileName = outputDir;
	outputNodesFileName << "/NLISTWithAppliedPoseMeshWithMaterialsWritten.txt";

	albaString outputElementsFileName = outputDir;
	outputElementsFileName << "/ELISTWithAppliedPoseMeshWithMaterialsWritten.txt";

	albaString outputMaterialsFileName = outputDir;
	outputMaterialsFileName << "/MPLISTWithAppliedPoseMeshWithMaterialsWritten.txt";

	vtkALBASmartPointer<vtkMatrix4x4> mat;
	mat->SetElement(0, 3, 10);
	mat->SetElement(1, 3, 20);
	mat->SetElement(2, 3, 30);

	albaVMEMeshAnsysTextExporter *exporter = new albaVMEMeshAnsysTextExporter;
	exporter->SetInput(ugrid);
	exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
	exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());
	exporter->SetOutputMaterialsFileName(outputMaterialsFileName.GetCStr());
	exporter->Write();

	albaVMEMeshAnsysTextImporter *reader = new albaVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(outputNodesFileName.GetCStr());
	reader->SetElementsFileName(outputElementsFileName.GetCStr());
	reader->SetMaterialsFileName(outputMaterialsFileName.GetCStr());
	reader->Read();
	// 	this->Read(reader, dirPrefix, true, true, albaString("BonemattedMeshWithMaterials.vtk"));

	exporter->ApplyMatrixOn();
	exporter->SetMatrix(mat);
	exporter->Write();

	// cleanup
	delete exporter;
	delete reader;
}

//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::TestBonemattedTetra10ElementsIdJumpingNoMaterialsNoTimeVarExport()
{
	albaString dirPrefix = ALBA_DATA_ROOT;
	dirPrefix << "/FEM/ANSYS/tet10/bonemattedMeshTetra10ANSYS_ELEMENT_IDJumpingAnsysExportTest/NoMaterials/";

	CPPUNIT_ASSERT(wxDirExists(dirPrefix.GetCStr()));

	albaString fileName = dirPrefix;
	fileName.Append("BonemattedMeshNoMaterials.vtk");

	CPPUNIT_ASSERT(wxFileExists(fileName.GetCStr()));

	vtkALBASmartPointer<vtkUnstructuredGridReader> ugr;
	ugr->SetFileName(fileName.GetCStr());
	ugr->Update();

	vtkUnstructuredGrid *ugrid = ugr->GetOutput();

	int numPoints = ugrid->GetNumberOfPoints();
	int numCells = ugrid->GetNumberOfCells();

	cout << "number of points: " << numPoints << std::endl;
	cout << "number of cells: " << numCells << std::endl;

	CPPUNIT_ASSERT(numPoints == 57);
	CPPUNIT_ASSERT(numCells == 15);

	wxString outputDir = GET_TEST_DATA_DIR();
	albaString outputNodesFileName = outputDir;
	outputNodesFileName << "/NLISTMeshWithMaterialsWritten.txt";

	albaString outputElementsFileName = outputDir;
	outputElementsFileName << "/ELISTMeshWithMaterialsWritten.txt";

	albaString outputMaterialsFileName = outputDir;
	outputMaterialsFileName << "/MPLISTMeshWithMaterialsWritten.txt";

	albaVMEMeshAnsysTextExporter *exporter = new albaVMEMeshAnsysTextExporter;
	exporter->SetInput(ugrid);
	exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
	exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());
	exporter->SetOutputMaterialsFileName(outputMaterialsFileName.GetCStr());
	exporter->Write();
	
	albaVMEMeshAnsysTextImporter *reader = new albaVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(outputNodesFileName.GetCStr());
	reader->SetElementsFileName(outputElementsFileName.GetCStr());
	reader->SetMaterialsFileName(outputMaterialsFileName.GetCStr());
	reader->Read();
	// 	this->Read(reader, dirPrefix, true, true, albaString("BonemattedMeshWithMaterials.vtk"));
	// 

	exporter->Write();

	// cleanup
	delete exporter;
	delete reader;
}

//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::TestExportTetra10VtkWithoutAnsysInformation()
{
	albaString dirPrefix = ALBA_DATA_ROOT;
	dirPrefix << "/FEM/ANSYS/tet10/";

	CPPUNIT_ASSERT(wxDirExists(dirPrefix.GetCStr()));

	albaString fileName = dirPrefix;
	fileName.Append("vtkTetra10WithoutAnsysInformations.vtk");

	CPPUNIT_ASSERT(wxFileExists(fileName.GetCStr()));

	vtkALBASmartPointer<vtkUnstructuredGridReader> ugr;
	ugr->SetFileName(fileName.GetCStr());
	ugr->Update();

	vtkUnstructuredGrid *ugrid = ugr->GetOutput();

	int numPoints = ugrid->GetNumberOfPoints();
	int numCells = ugrid->GetNumberOfCells();

	cout << "number of points: " << numPoints << std::endl;
	cout << "number of cells: " << numCells << std::endl;

	CPPUNIT_ASSERT(numPoints == 14);
	CPPUNIT_ASSERT(numCells == 2);

	albaString outputDir = GET_TEST_DATA_DIR();
	albaString outputNodesFileName = outputDir;
	outputNodesFileName << "/NLISTvtkTetra10WithoutAnsysInformations.txt";

	albaString outputElementsFileName = outputDir;
	outputElementsFileName << "/ELISTvtkTetra10WithoutAnsysInformations.txt";

	albaVMEMeshAnsysTextExporter *exporter = new albaVMEMeshAnsysTextExporter;
	exporter->SetInput(ugrid);
	exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
	exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());
	exporter->SetMode(albaVMEMeshAnsysTextExporter::WITHOUT_MAT_MODE);
	exporter->Write();

	albaVMEMeshAnsysTextImporter *reader = new albaVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(outputNodesFileName.GetCStr());
	reader->SetElementsFileName(outputElementsFileName.GetCStr());
	reader->SetMode(albaVMEMeshAnsysTextExporter::WITHOUT_MAT_MODE);
	reader->Read();
	

	// cleanup
	delete exporter;

	albaVMEMeshAnsysTextImporter *importer = new albaVMEMeshAnsysTextImporter;
	this->Read(importer, outputDir, false, true, albaString("/ImportedVTKTetra10WithoutAnsysInformations.vtk"),
		"/NLISTvtkTetra10WithoutAnsysInformations.txt", "/ELISTvtkTetra10WithoutAnsysInformations.txt");

	ugrid = importer->GetOutput()->GetUnstructuredGridOutput()->GetUnstructuredGridData();
	numPoints = ugrid->GetNumberOfPoints();
	numCells = ugrid->GetNumberOfCells();

	cout << "number of points: " << numPoints << std::endl;
	cout << "number of cells: " << numCells << std::endl;

	// mesh without materials
	CPPUNIT_ASSERT(numPoints == 14);
	CPPUNIT_ASSERT(numCells == 2);

	delete importer;
	delete reader;
}

//----------------------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporterTest::TestExportTetra4VtkWithoutAnsysInformation()
{
	albaString dirPrefix = ALBA_DATA_ROOT;
	dirPrefix << "/FEM/ANSYS/tet4/";

	CPPUNIT_ASSERT(wxDirExists(dirPrefix.GetCStr()));

	albaString fileName = dirPrefix;
	fileName.Append("vtkTetra4WithoutAnsysInformations.vtk");

	CPPUNIT_ASSERT(wxFileExists(fileName.GetCStr()));

	vtkALBASmartPointer<vtkUnstructuredGridReader> ugr;
	ugr->SetFileName(fileName.GetCStr());
	ugr->Update();

	vtkUnstructuredGrid *ugrid = ugr->GetOutput();

	int numPoints = ugrid->GetNumberOfPoints();
	int numCells = ugrid->GetNumberOfCells();

	cout << "number of points: " << numPoints << std::endl;
	cout << "number of cells: " << numCells << std::endl;

	CPPUNIT_ASSERT(numPoints == 5);
	CPPUNIT_ASSERT(numCells == 2);

	albaString outputDir = GET_TEST_DATA_DIR();
	albaString outputNodesFileName = outputDir;
	outputNodesFileName << "/NLISTvtkTetra4WithoutAnsysInformations.txt";

	albaString outputElementsFileName = outputDir;
	outputElementsFileName << "/ELISTvtkTetra4WithoutAnsysInformations.txt";


	albaVMEMeshAnsysTextExporter *exporter = new albaVMEMeshAnsysTextExporter;
	exporter->SetInput(ugrid);
	exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
	exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());
	exporter->SetMode(albaVMEMeshAnsysTextExporter::WITHOUT_MAT_MODE);
	exporter->Write();

	albaVMEMeshAnsysTextImporter *reader = new albaVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(outputNodesFileName.GetCStr());
	reader->SetElementsFileName(outputElementsFileName.GetCStr());
	reader->SetMode(albaVMEMeshAnsysTextImporter::WITHOUT_MAT_MODE);
	reader->Read();

	// cleanup
	delete exporter;

	albaVMEMeshAnsysTextImporter *importer = new albaVMEMeshAnsysTextImporter;
	this->Read(importer, outputDir, false, true, albaString("/ImportedVTKTetra4WithoutAnsysInformations.vtk"),
		"/NLISTvtkTetra4WithoutAnsysInformations.txt", "/ELISTvtkTetra4WithoutAnsysInformations.txt");

	ugrid = importer->GetOutput()->GetUnstructuredGridOutput()->GetUnstructuredGridData();
	numPoints = ugrid->GetNumberOfPoints();
	numCells = ugrid->GetNumberOfCells();

	cout << "number of points: " << numPoints << std::endl;
	cout << "number of cells: " << numCells << std::endl;

	// mesh without materials
	CPPUNIT_ASSERT(numPoints == 5);
	CPPUNIT_ASSERT(numCells == 2);

	delete importer;
	delete reader;
}
