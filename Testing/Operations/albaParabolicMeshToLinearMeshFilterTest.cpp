/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaParabolicMeshToLinearMeshFilterTest
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

#include "albaParabolicMeshToLinearMeshFilterTest.h"

#include <cppunit/config/SourcePrefix.h>

#include "albaVMEMesh.h"
#include "albaVMEMeshAnsysTextImporter.h"
#include "albaParabolicMeshToLinearMeshFilter.h"

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

void albaParabolicMeshToLinearMeshFilterTest::TestFixture()
{

}

void albaParabolicMeshToLinearMeshFilterTest::setUp()
{
  // single test case log
  m_Log = vtkFileOutputWindow::New();
  m_Log->SetInstance(m_Log);
  m_Log->SetFileName("test.log");
  m_Log->AppendOn();   
}

void albaParabolicMeshToLinearMeshFilterTest::tearDown()
{
  m_Log->Delete();
}

void albaParabolicMeshToLinearMeshFilterTest::TestConstructor()
{
  albaVMEMeshAnsysTextImporter *importer = new albaVMEMeshAnsysTextImporter  ;
  CPPUNIT_ASSERT(importer);
  CPPUNIT_ASSERT(importer->GetOutput() == NULL);

  delete importer;
}

void albaParabolicMeshToLinearMeshFilterTest::ReadLinearizeAndDisplay( albaString &dirPrefix, int dataType /*= 0*/, bool readMaterials /*= false*/, bool writeToDisk /*= false*/ )
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


  vtkUnstructuredGrid* dataBeforeLinearization = vtkUnstructuredGrid::New();
  vtkUnstructuredGrid *data = reader->GetOutput()->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  data->Update();

  dataBeforeLinearization->DeepCopy(data)  ;

  // some tests on the geometry...

  if (reader->GetMeshType() == albaVMEMeshAnsysTextImporter::LINEAR)
  {
    if (reader->GetElementType() == albaVMEMeshAnsysTextImporter::TETRA4)
    {
      CPPUNIT_ASSERT(dataBeforeLinearization->GetCell(0)->GetNumberOfPoints() == 4);
    }
    else if (reader->GetElementType() == albaVMEMeshAnsysTextImporter::HEXA8) 
    {
      CPPUNIT_ASSERT(dataBeforeLinearization->GetCell(0)->GetNumberOfPoints() == 8);
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
      CPPUNIT_ASSERT(dataBeforeLinearization->GetCell(0)->GetNumberOfPoints() == 10);
    }
    else if (reader->GetElementType() == albaVMEMeshAnsysTextImporter::HEXA20) 
    {
      CPPUNIT_ASSERT(dataBeforeLinearization->GetCell(0)->GetNumberOfPoints() == 20);
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

  // create the linearization filter
  vtkALBASmartPointer<albaParabolicMeshToLinearMeshFilter> linearizationFilter;   
  linearizationFilter->SetInput(dataBeforeLinearization);
  linearizationFilter->Update();

  if (writeToDisk)
  {
    // save non lineairzed output to file
    vtkALBASmartPointer<vtkUnstructuredGridWriter> nonLinearizedGridWriter;
    nonLinearizedGridWriter->SetInput(dataBeforeLinearization) ;
    nonLinearizedGridWriter->SetFileTypeToASCII();
    
    albaString gridFileName = dirPrefix;
    if (readMaterials)
    {
      gridFileName << "vtkUnstructuredGridWithMaterialsBeforeLinearization.vtk";
    } 
    else
    {
      gridFileName << "vtkUnstructuredGridWithoutMaterialsBeforeLinearization.vtk";
    }
    
    nonLinearizedGridWriter->SetFileName(gridFileName.GetCStr());
    nonLinearizedGridWriter->Write();
 
    // save linearized output to file
    vtkALBASmartPointer<vtkUnstructuredGridWriter> linearizedGridWriter;
    linearizedGridWriter->SetInput(linearizationFilter->GetOutput()) ;
    linearizedGridWriter->SetFileTypeToASCII();

    albaString linearizedGridFileName = dirPrefix;
    if (readMaterials)
    {
      linearizedGridFileName << "vtkUnstructuredGridWithMaterialsAfterLinearization.vtk";
    } 
    else
    {
      linearizedGridFileName << "vtkUnstructuredGridWithoutMaterialsAfterLinearization.vtk";
    }
    
    linearizedGridWriter->SetFileName(linearizedGridFileName.GetCStr());
    linearizedGridWriter->Write();
  }

  vtkUnstructuredGrid *linearizedData = NULL;

  linearizedData = linearizationFilter->GetOutput();
  CPPUNIT_ASSERT(linearizedData);

  if (dataType == POINT_DATA)
  {
    linearizedData->GetPointData()->SetActiveScalars("id");
    int ntuples = linearizedData->GetPointData()->GetArray("id")->GetNumberOfTuples();
    CPPUNIT_ASSERT(ntuples == linearizedData->GetNumberOfPoints());
  } 
  else if (dataType == CELL_DATA)
  {
    linearizedData->GetCellData()->SetActiveScalars("material");
    int ntuples = linearizedData->GetCellData()->GetArray("material")->GetNumberOfTuples();
    CPPUNIT_ASSERT(ntuples == linearizedData->GetNumberOfCells());
  }
  else
  {
    CPPUNIT_ASSERT(false);
  }

  // render linearized data
  RenderData(linearizedData, dataType);

  dataBeforeLinearization->Delete();
  delete reader;
}

void albaParabolicMeshToLinearMeshFilterTest::RenderData(  vtkUnstructuredGrid *data, int dataType)
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
  mapper->SetInput(data);

  vtkALBASmartPointer<vtkActor> actor;
  actor->SetMapper(mapper);

  renderer->AddActor(actor);
  renderWindow->Render();
  
  // remove comment below for user interaction...
  // renderWindowInteractor->Start();

  albaSleep(500);
}
  
void albaParabolicMeshToLinearMeshFilterTest::TestTetra4()
{
  albaString dirPrefix = ALBA_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet4/";

  // test without materials
  ReadLinearizeAndDisplay(dirPrefix, POINT_DATA);
  ReadLinearizeAndDisplay(dirPrefix, CELL_DATA, false, true);
  
  // test with materials
  ReadLinearizeAndDisplay(dirPrefix, POINT_DATA, true);
  ReadLinearizeAndDisplay(dirPrefix, CELL_DATA, true,true);

}

void albaParabolicMeshToLinearMeshFilterTest::TestTetra10()
{
  albaString dirPrefix = ALBA_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/tet10/";
 
  // test without materials
  ReadLinearizeAndDisplay(dirPrefix, POINT_DATA);
  ReadLinearizeAndDisplay(dirPrefix, CELL_DATA, false, true);

  // test with materials
  ReadLinearizeAndDisplay(dirPrefix, POINT_DATA, true);
  ReadLinearizeAndDisplay(dirPrefix, CELL_DATA, true,true);
}

void albaParabolicMeshToLinearMeshFilterTest::TestHexa8()
{
  albaString dirPrefix = ALBA_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/hex8/";
  
  // test without materials
  ReadLinearizeAndDisplay(dirPrefix, POINT_DATA);
  ReadLinearizeAndDisplay(dirPrefix, CELL_DATA, false, true);

  // test with materials
  ReadLinearizeAndDisplay(dirPrefix, POINT_DATA, true);
  ReadLinearizeAndDisplay(dirPrefix, CELL_DATA, true,true);
}

void albaParabolicMeshToLinearMeshFilterTest::TestHexa20()
{
  albaString dirPrefix = ALBA_DATA_ROOT;
  dirPrefix << "/FEM/ANSYS/hex20/";

  // test without materials
  ReadLinearizeAndDisplay(dirPrefix, POINT_DATA);
  ReadLinearizeAndDisplay(dirPrefix, CELL_DATA,false,true);

  // test with materials
  ReadLinearizeAndDisplay(dirPrefix, POINT_DATA, true);  
  ReadLinearizeAndDisplay(dirPrefix, CELL_DATA, true,true);
}

