/*=========================================================================

 Program: MAF2
 Module: mafVMEDataSetAttributesImporterTest
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

#include "mafVMEDataSetAttributesImporterTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "mafVMEMesh.h"
#include "mafVMEMeshAnsysTextImporter.h"
#include "mafVMEDataSetAttributesImporter.h"
#include "mafParabolicMeshToLinearMeshFilter.h"

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


mafVMEDataSetAttributesImporterTest::mafVMEDataSetAttributesImporterTest()
{
  m_AnsysTextReader = NULL;
}

mafVMEDataSetAttributesImporterTest::~mafVMEDataSetAttributesImporterTest()
{
}

void mafVMEDataSetAttributesImporterTest::TestFixture()
{
  
}

void mafVMEDataSetAttributesImporterTest::setUp()
{
  CPPUNIT_ASSERT(m_AnsysTextReader == NULL);
  m_AnsysTextReader = new mafVMEMeshAnsysTextImporter;
}

void mafVMEDataSetAttributesImporterTest::tearDown()
{
  cppDEL(m_AnsysTextReader);
}

void mafVMEDataSetAttributesImporterTest::TestConstructor()
{
  mafVMEDataSetAttributesImporter *importer = new mafVMEDataSetAttributesImporter;
  
  CPPUNIT_ASSERT(importer);
  cppDEL(importer);
}

void mafVMEDataSetAttributesImporterTest::ReadMesh( mafString &dirPrefix, bool readMaterials )
{
  // read the mesh
  mafString nodesFileName = dirPrefix;
  nodesFileName << "NLIST.lis";

  mafString elementsFileName = dirPrefix;
  elementsFileName << "ELIST.lis";

  cerr << std::endl << "nodes file:" << nodesFileName.GetCStr() << std::endl;
  m_AnsysTextReader->SetNodesFileName(nodesFileName.GetCStr());

  cerr << "elements file:" << elementsFileName.GetCStr() << std::endl;
  m_AnsysTextReader->SetElementsFileName(elementsFileName.GetCStr());

  if (readMaterials)
  {
    mafString materialsFileName;

    materialsFileName = dirPrefix;
    materialsFileName << "MPLIST.lis";

    cerr << "materials file:" << materialsFileName.GetCStr() << std::endl;
    m_AnsysTextReader->SetMaterialsFileName(materialsFileName.GetCStr());
  }

  CPPUNIT_ASSERT(m_AnsysTextReader->Read() == MAF_OK);
  CPPUNIT_ASSERT(m_AnsysTextReader->GetOutput() != NULL);
}

void mafVMEDataSetAttributesImporterTest::SaveToDiskAndDisplay( mafVMEMesh *vmeMesh,mafString &dirPrefix, int dataType /*= 0*/, const char *attributeLabel /*= "UNDEFINED"*/, bool writeToDisk /*= false*/, const char *outputVtkDataName /*= "defaultFileName.vtk"*/, bool render /*= true*/, bool linearize /*= false*/ )
{
  // get vtk output
  vtkUnstructuredGrid* data = vmeMesh->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  data->Update();

  // some tests on the geometry...
  if (writeToDisk)
  {
    // save output to file
    vtkMAFSmartPointer<vtkUnstructuredGridWriter> writer;
    writer->SetInput(data) ;
    writer->SetFileTypeToASCII();

    mafString gridFileName = dirPrefix;
    gridFileName << outputVtkDataName;

    writer->SetFileName(gridFileName.GetCStr());
    writer->Write();
  }

  if (dataType == POINT_DATA)
  {
    int res = -1;
    res = data->GetPointData()->SetActiveScalars(attributeLabel);
    assert(res != -1);

    int ntuples = data->GetPointData()->GetArray("id")->GetNumberOfTuples();
    CPPUNIT_ASSERT(ntuples == data->GetNumberOfPoints());
  } 
  else if (dataType == CELL_DATA)
  {
    int res = -1;
    res = data->GetCellData()->SetActiveScalars(attributeLabel);
    assert(res != -1);

    int ntuples = data->GetCellData()->GetArray("material")->GetNumberOfTuples();
    CPPUNIT_ASSERT(ntuples == data->GetNumberOfCells());
  }
  else
  {
    CPPUNIT_ASSERT(false);
  }

  // linearise before
  if (render)RenderData(data, dataType, linearize);
  
}

void mafVMEDataSetAttributesImporterTest::SaveToDiskForTimeVarying(  mafVMEMesh *vmeMesh,mafString &dirPrefix, int dataType /*= 0*/, const char *attributeLabel /*= "UNDEFINED"*/, bool writeToDisk /*= false*/, const char *outputDataName /*= "defaultFileName.vtk"*/  )
{
  
  for (int ts = 0; ts < 3; ts++)
  { 
    vmeMesh->SetTimeStamp(ts);

    // get vtk output
    vtkUnstructuredGrid* data = vmeMesh->GetUnstructuredGridOutput()->GetUnstructuredGridData();
    data->Update();

    // some tests on the geometry...
    if (writeToDisk)
    {
      // save output to file
      vtkMAFSmartPointer<vtkUnstructuredGridWriter> writer;
      writer->SetInput(data) ;
      writer->SetFileTypeToASCII();

      mafString gridFileName = dirPrefix;
      gridFileName << outputDataName;
      gridFileName << ts+1;
      gridFileName << ".vtk";

      writer->SetFileName(gridFileName.GetCStr());
      writer->Write();
    }

    if (dataType == POINT_DATA)
    {
      int res = -1;
      res = data->GetPointData()->SetActiveScalars(attributeLabel);
      assert(res != -1);

      int ntuples = data->GetPointData()->GetArray(attributeLabel)->GetNumberOfTuples();
      CPPUNIT_ASSERT(ntuples == data->GetNumberOfPoints());
    } 
    else if (dataType == CELL_DATA)
    {
      int res = -1;
      res = data->GetCellData()->SetActiveScalars(attributeLabel);
      assert(res != -1);

      int ntuples = data->GetCellData()->GetArray(attributeLabel)->GetNumberOfTuples();
      CPPUNIT_ASSERT(ntuples == data->GetNumberOfCells());
    }
    else
    {
      CPPUNIT_ASSERT(false);
    }
  }
  
}

void mafVMEDataSetAttributesImporterTest::RenderData( vtkUnstructuredGrid *data, int dataType /*= 0*/, bool linearize /*= false*/ )
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
    data->GetPointData()->GetScalars("id")->GetRange(range);
  } 
  else if (dataType == CELL_DATA)
  {
    data->GetCellData()->GetScalars("material")->GetRange(range);
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


  mafParabolicMeshToLinearMeshFilter *linearizationFilter  = NULL;
  
  mapper->UseLookupTableScalarRangeOff();
  mapper->SetScalarRange(reverseLut->GetTableRange());
  mapper->SetLookupTable(reverseLut);

  if (linearize)
  {
    linearizationFilter = mafParabolicMeshToLinearMeshFilter::New()  ;
    linearizationFilter->SetInput(data);
    linearizationFilter->Update()  ;
    mapper->SetInput(linearizationFilter->GetOutput());
  }
  else
  {
    mapper->SetInput(data);
  }

  vtkMAFSmartPointer<vtkActor> actor;
  actor->SetMapper(mapper);

  renderer->AddActor(actor);
  renderWindow->Render();
  
  // remove comment below for user interaction...
  // renderWindowInteractor->Start();

  mafDEL(linearizationFilter);
}
  

void mafVMEDataSetAttributesImporterTest::TestTetra4()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet4/";

  // test without materials
    // SaveToDiskAndDisplay(dirPrefix, false, );
  
}

void mafVMEDataSetAttributesImporterTest::TestTetra10SingleTime3CellAttributes()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet10/";
 
  mafVMEMesh *mesh = NULL;

  // test without materials
  ReadMesh(dirPrefix, false);

  mesh = m_AnsysTextReader->GetOutput();
  assert(mesh);

  mafVMEDataSetAttributesImporter *importer = new mafVMEDataSetAttributesImporter;

  mafString attributesFileName;
  attributesFileName << dirPrefix;
  attributesFileName << "EPTO1_S1_UZ_element_matrix.lis";

  importer->SetInput(mesh);
  importer->SetFileName(attributesFileName.GetCStr());
  importer->SetUseTSFile(false);
  importer->SetAttributeTypeToCellData();
  importer->Read();

  SaveToDiskAndDisplay(mesh,dirPrefix,CELL_DATA, "EPTO1", 
    true, "EPTO1_S1_UZ_element_matrix.vtk",false);
  
  cppDEL(importer);
}


void mafVMEDataSetAttributesImporterTest::TestTetra10SingleTime1CellAttributes()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet10/";

  mafVMEMesh *mesh = NULL;

  // test without materials
  ReadMesh(dirPrefix, false);

  mesh = m_AnsysTextReader->GetOutput();
  assert(mesh);

  mafVMEDataSetAttributesImporter *importer = new mafVMEDataSetAttributesImporter;

  mafString attributesFileName;
  attributesFileName << dirPrefix;
  attributesFileName << "EPTO1_element.lis";

  importer->SetInput(mesh);
  importer->SetFileName(attributesFileName.GetCStr());
  importer->SetUseTSFile(false);
  importer->SetAttributeTypeToCellData();
  importer->Read();

  SaveToDiskAndDisplay(mesh,dirPrefix,CELL_DATA, "EPTO1", 
    true, "EPTO1_element.vtk",false);

  cppDEL(importer);
}

void mafVMEDataSetAttributesImporterTest::TestTetra10SingleTime1PointsAttributes()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet10/";

  mafVMEMesh *mesh = NULL;

  // test without materials
  ReadMesh(dirPrefix, false);

  mesh = m_AnsysTextReader->GetOutput();
  assert(mesh);

  mafVMEDataSetAttributesImporter *importer = new mafVMEDataSetAttributesImporter;

  mafString attributesFileName;
  attributesFileName << dirPrefix;
  attributesFileName << "UZ_nodal.lis";

  importer->SetInput(mesh);
  importer->SetFileName(attributesFileName.GetCStr());
  importer->SetUseTSFile(false);
  importer->SetAttributeTypeToPointData();
  importer->Read();

  SaveToDiskAndDisplay(mesh,dirPrefix,POINT_DATA, "UZ", 
    true, "UZ_nodal.vtk",false);

  cppDEL(importer);
}

void mafVMEDataSetAttributesImporterTest::TestTetra10MultiTime1CellAttributes()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet10/";

  mafVMEMesh *mesh = NULL;

  // test without materials
  ReadMesh(dirPrefix, false);

  dirPrefix << "tet10_timevar/";
  
  mesh = m_AnsysTextReader->GetOutput();
  assert(mesh);

  mafVMEDataSetAttributesImporter *importer = new mafVMEDataSetAttributesImporter;

  mafString attributesFileName;
  attributesFileName << dirPrefix;
  attributesFileName << "EPTO1_element_0001.lis";

  importer->SetInput(mesh);
  importer->TimeVaryingOn();
  importer->SetFileName(attributesFileName.GetCStr());
  importer->SetFilePrefix("EPTO1_element_");
  importer->SetUseTSFile(false);
  importer->SetAttributeTypeToCellData();
  importer->Read();

  SaveToDiskForTimeVarying(mesh,dirPrefix,CELL_DATA, "EPTO1", 
    true, "EPTO1_element");

  cppDEL(importer);
}


void mafVMEDataSetAttributesImporterTest::TestTetra10MultiTime3CellAttributes()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet10/";

  mafVMEMesh *mesh = NULL;

  // test without materials
  ReadMesh(dirPrefix, false);

  dirPrefix << "tet10_timevar/";

  mesh = m_AnsysTextReader->GetOutput();
  assert(mesh);

  mafVMEDataSetAttributesImporter *importer = new mafVMEDataSetAttributesImporter;

  mafString attributesFileName;
  attributesFileName << dirPrefix;
  attributesFileName << "EPTO1_S1_UZ_element_matrix_0001.lis";

  importer->SetInput(mesh);
  importer->TimeVaryingOn();
  importer->SetFileName(attributesFileName.GetCStr());
  importer->SetFilePrefix("EPTO1_S1_UZ_element_matrix_");
  importer->SetUseTSFile(false);
  importer->SetAttributeTypeToCellData();
  importer->Read();

  SaveToDiskForTimeVarying(mesh,dirPrefix,CELL_DATA, "EPTO1", 
    true, "EPTO1_S1_UZ_element_matrix_");

  cppDEL(importer);
}


void mafVMEDataSetAttributesImporterTest::TestTetra10MultiTime1PointAttributes()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet10/";

  mafVMEMesh *mesh = NULL;

  // test without materials
  ReadMesh(dirPrefix, false);

  dirPrefix << "tet10_timevar/";

  mesh = m_AnsysTextReader->GetOutput();
  assert(mesh);

  mafVMEDataSetAttributesImporter *importer = new mafVMEDataSetAttributesImporter;

  mafString attributesFileName;
  attributesFileName << dirPrefix;
  attributesFileName << "UZ_nodal_0001.lis";

  importer->SetInput(mesh);
  importer->TimeVaryingOn();
  importer->SetFileName(attributesFileName.GetCStr());
  importer->SetFilePrefix("UZ_nodal_");
  importer->SetUseTSFile(false);
  importer->SetAttributeTypeToPointData();
  importer->Read();

  SaveToDiskForTimeVarying(mesh,dirPrefix, POINT_DATA, "UZ", 
    true, "UZ_nodal_");

  cppDEL(importer);
}


void mafVMEDataSetAttributesImporterTest::TestHexa8()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/hex8/";
  
  // test without materials
    // ReadMesh(dirPrefix, false);

}

void mafVMEDataSetAttributesImporterTest::TestHexa20()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/hex20/";

  // test without materials
    // ReadMesh(dirPrefix, false);
  
}

void mafVMEDataSetAttributesImporterTest::TestHexa8SingleTime3CellAttributes()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/hex8/";

  mafVMEMesh *mesh = NULL;

  // test without materials
  ReadMesh(dirPrefix, false);

  mesh = m_AnsysTextReader->GetOutput();
  assert(mesh);

  mafVMEDataSetAttributesImporter *importer = new mafVMEDataSetAttributesImporter;

  mafString attributesFileName;
  attributesFileName << dirPrefix;
  attributesFileName << "S1_UZ_EPTO1_matrix.lis";

  importer->SetInput(mesh);
  importer->SetFileName(attributesFileName.GetCStr());
  importer->SetUseTSFile(false);
  importer->SetAttributeTypeToCellData();
  importer->Read();

  SaveToDiskAndDisplay(mesh,dirPrefix,CELL_DATA, "EPTO1", 
    true, "EPTO1_S1_UZ_element_matrix.vtk",false);

  cppDEL(importer);
}


void mafVMEDataSetAttributesImporterTest::TestHexa8SingleTime1CellAttributes()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/hex8/";

  mafVMEMesh *mesh = NULL;

  // test without materials
  ReadMesh(dirPrefix, false);

  mesh = m_AnsysTextReader->GetOutput();
  assert(mesh);

  mafVMEDataSetAttributesImporter *importer = new mafVMEDataSetAttributesImporter;

  mafString attributesFileName;
  attributesFileName << dirPrefix;
  attributesFileName << "EPTO1_element.lis";

  importer->SetInput(mesh);
  importer->SetFileName(attributesFileName.GetCStr());
  importer->SetUseTSFile(false);
  importer->SetAttributeTypeToCellData();
  importer->Read();

  SaveToDiskAndDisplay(mesh,dirPrefix,CELL_DATA, "EPTO1", 
    true, "EPTO1_element.vtk",false);

  cppDEL(importer);
}

void mafVMEDataSetAttributesImporterTest::TestHexa8SingleTime1PointsAttributes()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/hex8/";

  mafVMEMesh *mesh = NULL;

  // test without materials
  ReadMesh(dirPrefix, false);

  mesh = m_AnsysTextReader->GetOutput();
  assert(mesh);

  mafVMEDataSetAttributesImporter *importer = new mafVMEDataSetAttributesImporter;

  mafString attributesFileName;
  attributesFileName << dirPrefix;
  attributesFileName << "UZ_nodal.lis";

  importer->SetInput(mesh);
  importer->SetFileName(attributesFileName.GetCStr());
  importer->SetUseTSFile(false);
  importer->SetAttributeTypeToPointData();
  importer->Read();

  SaveToDiskAndDisplay(mesh,dirPrefix,POINT_DATA, "UZ", 
    true, "UZ_nodal.vtk",false);

  cppDEL(importer);
}


void mafVMEDataSetAttributesImporterTest::TestHexa20SingleTime1PointsAttributes()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/hex20/";

  mafVMEMesh *mesh = NULL;

  // test without materials
  ReadMesh(dirPrefix, false);

  mesh = m_AnsysTextReader->GetOutput();
  assert(mesh);

  mafVMEDataSetAttributesImporter *importer = new mafVMEDataSetAttributesImporter;

  mafString attributesFileName;
  attributesFileName << dirPrefix;
  attributesFileName << "UZ_nodal.lis";

  importer->SetInput(mesh);
  importer->SetFileName(attributesFileName.GetCStr());
  importer->SetUseTSFile(false);
  importer->SetAttributeTypeToPointData();
  importer->Read();

  SaveToDiskAndDisplay(mesh,dirPrefix,POINT_DATA, "UZ", 
    true, "UZ_nodal.vtk",false);

  cppDEL(importer);
}

void mafVMEDataSetAttributesImporterTest::TestHexa20SingleTime1CellAttributes()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/hex20/";

  mafVMEMesh *mesh = NULL;

    // test without materials
    ReadMesh(dirPrefix, false);

  mesh = m_AnsysTextReader->GetOutput();
  assert(mesh);

  mafVMEDataSetAttributesImporter *importer = new mafVMEDataSetAttributesImporter;

  mafString attributesFileName;
  attributesFileName << dirPrefix;
  attributesFileName << "EPTO1_element.lis";

  importer->SetInput(mesh);
  importer->SetFileName(attributesFileName.GetCStr());
  importer->SetUseTSFile(false);
  importer->SetAttributeTypeToCellData();
  importer->Read();

  SaveToDiskAndDisplay(mesh,dirPrefix,CELL_DATA, "EPTO1", 
    true, "EPTO1_element.vtk",false);

  cppDEL(importer);
}

void mafVMEDataSetAttributesImporterTest::TestHexa20SingleTime3CellAttributes()
{
  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/hex20/";

  mafVMEMesh *mesh = NULL;

  // test without materials
  ReadMesh(dirPrefix, false);

  mesh = m_AnsysTextReader->GetOutput();
  assert(mesh);

  mafVMEDataSetAttributesImporter *importer = new mafVMEDataSetAttributesImporter;

  mafString attributesFileName;
  attributesFileName << dirPrefix;
  attributesFileName << "S1_EPTO1_UZ_element_matrix.lis";

  importer->SetInput(mesh);
  importer->SetFileName(attributesFileName.GetCStr());
  importer->SetUseTSFile(false);
  importer->SetAttributeTypeToCellData();
  importer->Read();

  SaveToDiskAndDisplay(mesh,dirPrefix,CELL_DATA, "EPTO1", 
    true, "S1_EPTO1_UZ_element_matrix.vtk",false);

  cppDEL(importer);
}

void mafVMEDataSetAttributesImporterTest::TestTetra10ANSYS_ELEMENT_IDJumpingSingleTime1CellAttributes()
{  
  /*
  // IN: ansys elements id with values
  ELEM	EPEL1
  49445	0.0004
  41871	0.0035
  41995	0.0045
  42142   0.007
  41670	0.00005
  42788	0.009
  41430	0.002
  40639	0.0008
  41049	0.0045
  45054	0.01
  45194	0.0004
  40241	0.00009
  49740	0.00024
  38714	0.0038
  38842	0.0068

  create an hash table for elemid->value

  // IN: ansys cells id
  ANSYS_ELEMENT_ID 1 15 int
    38714 38842 40241 40639 41049 41430 41670 41871 41995 
    42142 42788 45054 45194 49445 49740 

  if use the cell array then set its name:
  set the array name to ANSYS_ELEMENT_ID
  
  get the 0th id which is 38714
  EPEL1[0] = ht[ANSYS_ELEMENT_ID[0]]


  // OUT (wrong): cells value
  EPEL1 1 15 double
    0.0004 0.0035 0.0045 0.007 5e-005 0.009 0.002 0.0008 0.0045 
    0.01 0.0004 9e-005 0.00024 0.0038 0.0068 
  
  */

  mafString dirPrefix = MAF_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet10/meshTetra10ANSYS_ELEMENT_IDJumpingDatasetAttributtesImporting/";

  // create the mesh  
  mafString preBonematVTKMeshFileName;
  preBonematVTKMeshFileName << dirPrefix;
  preBonematVTKMeshFileName << "meshTetra10_ANSYS_ELEMENT_ID_Jumping_PostBonemat.vtk";

  vtkUnstructuredGridReader *reader = vtkUnstructuredGridReader::New();
  reader->SetFileName(preBonematVTKMeshFileName.GetCStr());
  reader->Update();

  mafVMEMesh *mesh = NULL;
  mesh = mafVMEMesh::New();
  assert(mesh);
  mesh->SetData(reader->GetOutput(), -1);

  // this is strange but...
  CPPUNIT_ASSERT(mesh->GetUnstructuredGridOutput()->GetVTKData()->GetNumberOfCells() ==  0);
  // ... after MAF magic update :P ...
  mesh->GetUnstructuredGridOutput()->GetVTKData()->Update();
  // ... everything works :D
  CPPUNIT_ASSERT(mesh->GetUnstructuredGridOutput()->GetVTKData()->GetNumberOfCells() != 0);

  // add the attributes
  mafVMEDataSetAttributesImporter *importer = new mafVMEDataSetAttributesImporter;  
  importer->SetInput(mesh);
  
  mafString resultsFile;
  resultsFile << dirPrefix;
  resultsFile << "resultsMeshTetra10_ANSYS_ELEMENT_ID_Jumping.txt";
  importer->SetFileName(resultsFile.GetCStr());

  importer->SetUseTSFile(false);
  importer->UseIdArrayOn();
  importer->SetIdArrayName("ANSYS_ELEMENT_ID");
  importer->SetAttributeTypeToCellData();
  int result = importer->Read();

  CPPUNIT_ASSERT(result == MAF_OK);

  SaveToDiskAndDisplay(mesh,dirPrefix,CELL_DATA, "EPEL1", 
    true, "EPEL1_element.vtk", false, true);

  vtkUnstructuredGrid *outputData = mesh->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  vtkDataArray *dataArray = outputData->GetCellData()->GetArray("EPEL1");
  CPPUNIT_ASSERT(dataArray != NULL);
  CPPUNIT_ASSERT(dataArray->GetNumberOfComponents() == 1);
  
  int numCells = mesh->GetUnstructuredGridOutput()->GetVTKData()->GetNumberOfCells();
  CPPUNIT_ASSERT(dataArray->GetNumberOfTuples() == numCells);

  vtkDEL(reader);
  cppDEL(importer);
  mafDEL(mesh);
}
