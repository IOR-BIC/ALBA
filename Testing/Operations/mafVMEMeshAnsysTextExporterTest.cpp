/*=========================================================================

 Program: MAF2
 Module: mafVMEMeshAnsysTextExporterTest
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

#include "mafVMEMeshAnsysTextExporterTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "mafVMEMesh.h"
#include "mafVMEMeshAnsysTextExporter.h"

#include "vtkMAFSmartPointer.h"
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
void mafVMEMeshAnsysTextExporterTest::TestFixture()
{

}
//----------------------------------------------------------------------------------------
void mafVMEMeshAnsysTextExporterTest::BeforeTest()
{

}
//----------------------------------------------------------------------------------------
void mafVMEMeshAnsysTextExporterTest::AfterTest()
{

}
//----------------------------------------------------------------------------------------
void mafVMEMeshAnsysTextExporterTest::TestConstructorDestructor()
{
  mafVMEMeshAnsysTextExporter *exporter = new mafVMEMeshAnsysTextExporter  ;
  CPPUNIT_ASSERT(exporter);
  CPPUNIT_ASSERT(exporter->GetInput() == NULL);

  delete exporter;
}

//----------------------------------------------------------------------------------------
void mafVMEMeshAnsysTextExporterTest::Read( mafVMEMeshAnsysTextImporter *reader, \
mafString &dirPrefix, bool readMaterials /*= false*/, bool writeToDisk /*= false*/,\
mafString &outputFileName , mafString inputNLISTFileName /*= "NLIST.lis"*/, \
mafString inputELISTFileName /*= "ELIST.lis"*/, mafString inputMPLISTFileName /*= "MPLIST.LIS"*/ )
{
  CPPUNIT_ASSERT(reader != NULL);

  mafString nodesFileName = dirPrefix;
  nodesFileName << inputNLISTFileName.GetCStr();

  mafString elementsFileName = dirPrefix;
  elementsFileName << inputELISTFileName.GetCStr();

  cerr << std::endl << "nodes file:" << nodesFileName.GetCStr() << std::endl;
  reader->SetNodesFileName(nodesFileName.GetCStr());

  cerr << "elements file:" << elementsFileName.GetCStr() << std::endl;
  reader->SetElementsFileName(elementsFileName.GetCStr());

  mafString materialsFileName;

  if (readMaterials == true)
  {
    materialsFileName = dirPrefix;
    materialsFileName << inputMPLISTFileName.GetCStr();

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

//----------------------------------------------------------------------------------------
void mafVMEMeshAnsysTextExporterTest::SaveUnstructuredGridToFile(mafString &dirPrefix, mafString &fileName, vtkUnstructuredGrid *data)
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
//----------------------------------------------------------------------------------------
void mafVMEMeshAnsysTextExporterTest::ReadAndDisplay( mafString &dirPrefix, int dataType /*= 0*/, bool readMaterials /*= false*/, bool writeToDisk /*= false*/ )
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
    if (reader->GetElementType() == mafVMEMeshAnsysTextImporter::LINEAR)
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
void mafVMEMeshAnsysTextExporterTest::RenderData(  vtkUnstructuredGrid *data, int dataType)
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

//----------------------------------------------------------------------------------------
int mafVMEMeshAnsysTextExporterTest::GetRowsNumber(vtkUnstructuredGrid *inputUGrid)
{
  int numberOfPoints = inputUGrid->GetNumberOfPoints();

  int rowsNumber = numberOfPoints;
  return rowsNumber;
}

//----------------------------------------------------------------------------------------
void mafVMEMeshAnsysTextExporterTest::TestTetra10TrivialMeshExport()
{
	mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet10/";

  mafVMEMeshAnsysTextImporter *reader = new mafVMEMeshAnsysTextImporter;
  this->Read(reader, dirPrefix, false, true, mafString("tet10NoMaterials.vtk"));

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
  mafString outputNodesFileName = outputDir;
	outputNodesFileName << "/NLISTNoMaterialsWritten.txt";
  
  mafString outputElementsFileName = outputDir;
	outputElementsFileName << "/ELISTNoMaterialsWritten.txt";

  mafVMEMeshAnsysTextExporter *exporter = new mafVMEMeshAnsysTextExporter;
  exporter->SetInput(ugrid);
  exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
  exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());
 	
	mafOpExporterFEMCommon tmpFEMop1;
	tmpFEMop1.SetInput(reader->GetOutput());
	exporter->SetMaterialData(tmpFEMop1.GetMaterialData());
 	exporter->SetMatIdArray(tmpFEMop1.GetMatIdArray());

  exporter->Write();

  // mesh with materials
  this->Read(reader, dirPrefix, true, true, mafString("tet10WithMaterials.vtk"));
  numPoints = ugrid->GetNumberOfPoints();
  numCells = ugrid->GetNumberOfCells();

  CPPUNIT_ASSERT(numPoints == 14);
  CPPUNIT_ASSERT(numCells == 2);

  outputNodesFileName = outputDir;
  outputNodesFileName.Append("NLISTMaterialsWritten.txt");

  outputElementsFileName = outputDir;
  outputElementsFileName.Append("ELISTMaterialsWritten.txt");

  mafString outputMaterialsFileName = outputDir;
  outputMaterialsFileName.Append("MPLISTNoMaterialsWritten.txt");

  exporter->SetInput(ugrid);
  exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
  exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());

	mafOpExporterFEMCommon tmpFEMop2;
	tmpFEMop2.SetInput(reader->GetOutput());
	exporter->SetMaterialData(tmpFEMop2.GetMaterialData());
	exporter->SetMatIdArray(tmpFEMop2.GetMatIdArray());

  exporter->Write();

  // cleanup
  delete exporter;
  delete reader;
}

//----------------------------------------------------------------------------------------
void mafVMEMeshAnsysTextExporterTest::TestTetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarExport()
{
	mafString dirPrefix = MAF_DATA_ROOT;
	dirPrefix << "/FEM/ANSYS/tet10/Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar/";

	mafVMEMeshAnsysTextImporter *reader = new mafVMEMeshAnsysTextImporter;
	this->Read(reader, dirPrefix, false, true, mafString("tet10NoMaterials.vtk"));

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
	mafString outputNodesFileName = outputDir;
	outputNodesFileName << "/NLISTNoMaterialsWritten.txt";

	mafString outputElementsFileName = outputDir;
	outputElementsFileName << "/ELISTNoMaterialsWritten.txt";

	mafVMEMeshAnsysTextExporter *exporter = new mafVMEMeshAnsysTextExporter;
	exporter->SetInput(ugrid);
	exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
	exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());

	mafOpExporterFEMCommon tmpFEMop1;
	tmpFEMop1.SetInput(reader->GetOutput());
	exporter->SetMaterialData(tmpFEMop1.GetMaterialData());
	exporter->SetMatIdArray(tmpFEMop1.GetMatIdArray());

	exporter->Write();

	// mesh with materials
	this->Read(reader, dirPrefix, true, true, mafString("tet10WithMaterials.vtk"));
	numPoints = ugrid->GetNumberOfPoints();
	numCells = ugrid->GetNumberOfCells();

	CPPUNIT_ASSERT(numPoints == 86);
	CPPUNIT_ASSERT(numCells == 17);

	outputNodesFileName = outputDir;
	outputNodesFileName.Append("NLISTMaterialsWritten.txt");

	outputElementsFileName = outputDir;
	outputElementsFileName.Append("ELISTMaterialsWritten.txt");

	mafString outputMaterialsFileName = outputDir;
	outputMaterialsFileName.Append("MPLISTNoMaterialsWritten.txt");

	exporter->SetInput(ugrid);
	exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
	exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());

	mafOpExporterFEMCommon tmpFEMop2;
	tmpFEMop2.SetInput(reader->GetOutput());
	exporter->SetMaterialData(tmpFEMop2.GetMaterialData());
	exporter->SetMatIdArray(tmpFEMop2.GetMatIdArray());

	exporter->Write();

	// cleanup
	delete exporter;
	delete reader;
}

//----------------------------------------------------------------------------------------
void mafVMEMeshAnsysTextExporterTest::TestBonemattedTetra10ElementsIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarExport()
{
	mafString dirPrefix = MAF_DATA_ROOT;
	dirPrefix << "/FEM/ANSYS/tet10/bonemattedMeshTetra10ANSYS_ELEMENT_IDJumpingAnsysExportTest/WithMaterials/";

	CPPUNIT_ASSERT(wxDirExists(dirPrefix.GetCStr()));

	mafString fileName = dirPrefix;
	fileName.Append("BonemattedMeshWithMaterials.vtk");

	CPPUNIT_ASSERT(wxFileExists(fileName.GetCStr()));

	vtkMAFSmartPointer<vtkUnstructuredGridReader> ugr;
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
	mafString outputNodesFileName = outputDir;
	outputNodesFileName << "/NLISTMeshWithMaterialsWritten.txt";

	mafString outputElementsFileName = outputDir;
	outputElementsFileName << "/ELISTMeshWithMaterialsWritten.txt";

	mafString outputMaterialsFileName = outputDir;
	outputMaterialsFileName << "/MPLISTMeshWithMaterialsWritten.txt";

	mafVMEMeshAnsysTextExporter *exporter = new mafVMEMeshAnsysTextExporter;
	exporter->SetInput(ugrid);
	exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
	exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());
	exporter->SetOutputMaterialsFileName(outputMaterialsFileName.GetCStr());

 	mafVMEMeshAnsysTextImporter *reader = new mafVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(outputNodesFileName.GetCStr());
	reader->SetElementsFileName(outputElementsFileName.GetCStr());
	reader->SetMaterialsFileName(outputMaterialsFileName.GetCStr());
	reader->Read();
// 	this->Read(reader, dirPrefix, true, true, mafString("BonemattedMeshWithMaterials.vtk"));
// 
	mafOpExporterFEMCommon tmpFEMop1;
	tmpFEMop1.SetInput(reader->GetOutput());
	exporter->SetMaterialData(tmpFEMop1.GetMaterialData());
	exporter->SetMatIdArray(tmpFEMop1.GetMatIdArray());

	exporter->Write();

	// cleanup
	delete exporter;
	delete reader;
}

//----------------------------------------------------------------------------------------
void mafVMEMeshAnsysTextExporterTest::TestBonemattedTetra10ElementsIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarExportWithAppliedPose()
{
	mafString dirPrefix = MAF_DATA_ROOT;
	dirPrefix << "/FEM/ANSYS/tet10/bonemattedMeshTetra10ANSYS_ELEMENT_IDJumpingAnsysExportTest/WithMaterials/";

	CPPUNIT_ASSERT(wxDirExists(dirPrefix.GetCStr()));

	mafString fileName = dirPrefix;
	fileName.Append("BonemattedMeshWithMaterials.vtk");

	CPPUNIT_ASSERT(wxFileExists(fileName.GetCStr()));

	vtkMAFSmartPointer<vtkUnstructuredGridReader> ugr;
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
	mafString outputNodesFileName = outputDir;
	outputNodesFileName << "/NLISTWithAppliedPoseMeshWithMaterialsWritten.txt";

	mafString outputElementsFileName = outputDir;
	outputElementsFileName << "/ELISTWithAppliedPoseMeshWithMaterialsWritten.txt";

	mafString outputMaterialsFileName = outputDir;
	outputMaterialsFileName << "/MPLISTWithAppliedPoseMeshWithMaterialsWritten.txt";

	vtkMAFSmartPointer<vtkMatrix4x4> mat;
	mat->SetElement(0, 3, 10);
	mat->SetElement(1, 3, 20);
	mat->SetElement(2, 3, 30);

	mafVMEMeshAnsysTextExporter *exporter = new mafVMEMeshAnsysTextExporter;
	exporter->SetInput(ugrid);
	exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
	exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());
	exporter->SetOutputMaterialsFileName(outputMaterialsFileName.GetCStr());

	mafVMEMeshAnsysTextImporter *reader = new mafVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(outputNodesFileName.GetCStr());
	reader->SetElementsFileName(outputElementsFileName.GetCStr());
	reader->SetMaterialsFileName(outputMaterialsFileName.GetCStr());
	reader->Read();
	// 	this->Read(reader, dirPrefix, true, true, mafString("BonemattedMeshWithMaterials.vtk"));
	// 

	mafOpExporterFEMCommon tmpFEMop1;
	tmpFEMop1.SetInput(reader->GetOutput());
	exporter->SetMaterialData(tmpFEMop1.GetMaterialData());
	exporter->SetMatIdArray(tmpFEMop1.GetMatIdArray());

	exporter->ApplyMatrixOn();
	exporter->SetMatrix(mat);
	exporter->Write();

	// cleanup
	delete exporter;
	delete reader;
}

//----------------------------------------------------------------------------------------
void mafVMEMeshAnsysTextExporterTest::TestBonemattedTetra10ElementsIdJumpingNoMaterialsNoTimeVarExport()
{
	mafString dirPrefix = MAF_DATA_ROOT;
	dirPrefix << "/FEM/ANSYS/tet10/bonemattedMeshTetra10ANSYS_ELEMENT_IDJumpingAnsysExportTest/NoMaterials/";

	CPPUNIT_ASSERT(wxDirExists(dirPrefix.GetCStr()));

	mafString fileName = dirPrefix;
	fileName.Append("BonemattedMeshNoMaterials.vtk");

	CPPUNIT_ASSERT(wxFileExists(fileName.GetCStr()));

	vtkMAFSmartPointer<vtkUnstructuredGridReader> ugr;
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
	mafString outputNodesFileName = outputDir;
	outputNodesFileName << "/NLISTMeshWithMaterialsWritten.txt";

	mafString outputElementsFileName = outputDir;
	outputElementsFileName << "/ELISTMeshWithMaterialsWritten.txt";

	mafString outputMaterialsFileName = outputDir;
	outputMaterialsFileName << "/MPLISTMeshWithMaterialsWritten.txt";

	mafVMEMeshAnsysTextExporter *exporter = new mafVMEMeshAnsysTextExporter;
	exporter->SetInput(ugrid);
	exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
	exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());
	exporter->SetOutputMaterialsFileName(outputMaterialsFileName.GetCStr());

	mafVMEMeshAnsysTextImporter *reader = new mafVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(outputNodesFileName.GetCStr());
	reader->SetElementsFileName(outputElementsFileName.GetCStr());
	reader->SetMaterialsFileName(outputMaterialsFileName.GetCStr());
	reader->Read();
	// 	this->Read(reader, dirPrefix, true, true, mafString("BonemattedMeshWithMaterials.vtk"));
	// 
	mafOpExporterFEMCommon tmpFEMop1;
	tmpFEMop1.SetInput(reader->GetOutput());
	exporter->SetMaterialData(tmpFEMop1.GetMaterialData());
	exporter->SetMatIdArray(tmpFEMop1.GetMatIdArray());

	exporter->Write();

	// cleanup
	delete exporter;
	delete reader;
}

//----------------------------------------------------------------------------------------
void mafVMEMeshAnsysTextExporterTest::TestExportTetra10VtkWithoutAnsysInformation()
{
	mafString dirPrefix = MAF_DATA_ROOT;
	dirPrefix << "/FEM/ANSYS/tet10/";

	CPPUNIT_ASSERT(wxDirExists(dirPrefix.GetCStr()));

	mafString fileName = dirPrefix;
	fileName.Append("vtkTetra10WithoutAnsysInformations.vtk");

	CPPUNIT_ASSERT(wxFileExists(fileName.GetCStr()));

	vtkMAFSmartPointer<vtkUnstructuredGridReader> ugr;
	ugr->SetFileName(fileName.GetCStr());
	ugr->Update();

	vtkUnstructuredGrid *ugrid = ugr->GetOutput();

	int numPoints = ugrid->GetNumberOfPoints();
	int numCells = ugrid->GetNumberOfCells();

	cout << "number of points: " << numPoints << std::endl;
	cout << "number of cells: " << numCells << std::endl;

	CPPUNIT_ASSERT(numPoints == 14);
	CPPUNIT_ASSERT(numCells == 2);

	mafString outputDir = GET_TEST_DATA_DIR();
	mafString outputNodesFileName = outputDir;
	outputNodesFileName << "/NLISTvtkTetra10WithoutAnsysInformations.txt";

	mafString outputElementsFileName = outputDir;
	outputElementsFileName << "/ELISTvtkTetra10WithoutAnsysInformations.txt";

	mafString outputMaterialsFileName = outputDir;
	outputMaterialsFileName << "/MPLISTvtkTetra10WithoutAnsysInformations.txt";

	mafVMEMeshAnsysTextExporter *exporter = new mafVMEMeshAnsysTextExporter;
	exporter->SetInput(ugrid);
	exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
	exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());
	exporter->SetOutputMaterialsFileName(outputMaterialsFileName.GetCStr());

	mafVMEMeshAnsysTextImporter *reader = new mafVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(outputNodesFileName.GetCStr());
	reader->SetElementsFileName(outputElementsFileName.GetCStr());
	reader->SetMaterialsFileName(outputMaterialsFileName.GetCStr());
	reader->Read();
	// 	this->Read(reader, dirPrefix, true, true, mafString("BonemattedMeshWithMaterials.vtk"));
	// 
	mafOpExporterFEMCommon tmpFEMop1;
	tmpFEMop1.SetInput(reader->GetOutput());
	exporter->SetMaterialData(tmpFEMop1.GetMaterialData());
	exporter->SetMatIdArray(tmpFEMop1.GetMatIdArray());

	exporter->Write();

	// cleanup
	delete exporter;

	mafVMEMeshAnsysTextImporter *importer = new mafVMEMeshAnsysTextImporter;
	this->Read(importer, outputDir, false, true, mafString("/ImportedVTKTetra10WithoutAnsysInformations.vtk"),
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
void mafVMEMeshAnsysTextExporterTest::TestExportTetra4VtkWithoutAnsysInformation()
{
	mafString dirPrefix = MAF_DATA_ROOT;
	dirPrefix << "/FEM/ANSYS/tet4/";

	CPPUNIT_ASSERT(wxDirExists(dirPrefix.GetCStr()));

	mafString fileName = dirPrefix;
	fileName.Append("vtkTetra4WithoutAnsysInformations.vtk");

	CPPUNIT_ASSERT(wxFileExists(fileName.GetCStr()));

	vtkMAFSmartPointer<vtkUnstructuredGridReader> ugr;
	ugr->SetFileName(fileName.GetCStr());
	ugr->Update();

	vtkUnstructuredGrid *ugrid = ugr->GetOutput();

	int numPoints = ugrid->GetNumberOfPoints();
	int numCells = ugrid->GetNumberOfCells();

	cout << "number of points: " << numPoints << std::endl;
	cout << "number of cells: " << numCells << std::endl;

	CPPUNIT_ASSERT(numPoints == 5);
	CPPUNIT_ASSERT(numCells == 2);

	mafString outputDir = GET_TEST_DATA_DIR();
	mafString outputNodesFileName = outputDir;
	outputNodesFileName << "/NLISTvtkTetra4WithoutAnsysInformations.txt";

	mafString outputElementsFileName = outputDir;
	outputElementsFileName << "/ELISTvtkTetra4WithoutAnsysInformations.txt";

	mafString outputMaterialsFileName = outputDir;
	outputMaterialsFileName << "/MPLISTvtkTetra4WithoutAnsysInformations.txt";

	mafVMEMeshAnsysTextExporter *exporter = new mafVMEMeshAnsysTextExporter;
	exporter->SetInput(ugrid);
	exporter->SetOutputNodesFileName(outputNodesFileName.GetCStr());
	exporter->SetOutputElementsFileName(outputElementsFileName.GetCStr());
	exporter->SetOutputMaterialsFileName(outputMaterialsFileName.GetCStr());

	mafVMEMeshAnsysTextImporter *reader = new mafVMEMeshAnsysTextImporter;
	reader->SetNodesFileName(outputNodesFileName.GetCStr());
	reader->SetElementsFileName(outputElementsFileName.GetCStr());
	reader->SetMaterialsFileName(outputMaterialsFileName.GetCStr());
	reader->Read();
	// 	this->Read(reader, dirPrefix, true, true, mafString("BonemattedMeshWithMaterials.vtk"));
	// 
	mafOpExporterFEMCommon tmpFEMop1;
	tmpFEMop1.SetInput(reader->GetOutput());
	exporter->SetMaterialData(tmpFEMop1.GetMaterialData());
	exporter->SetMatIdArray(tmpFEMop1.GetMatIdArray());

	exporter->Write();

	// cleanup
	delete exporter;

	mafVMEMeshAnsysTextImporter *importer = new mafVMEMeshAnsysTextImporter;
	this->Read(importer, outputDir, false, true, mafString("/ImportedVTKTetra4WithoutAnsysInformations.vtk"),
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
