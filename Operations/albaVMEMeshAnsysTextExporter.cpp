/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMeshAnsysTextExporter
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

#include "albaVMEMeshAnsysTextExporter.h"

#include "vtkALBASmartPointer.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkFieldData.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"

#include "albaVMEMesh.h"
#include "albaTagArray.h"


// vcl includes
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vcl_map.h>
#include <vcl_vector.h>

//----------------------------------------------------------------------------
albaVMEMeshAnsysTextExporter::albaVMEMeshAnsysTextExporter()
{
  m_Input = NULL;
  m_OutputNodesFileName = "NLIST.txt";
  m_OutputElementsFileName = "ELIST.txt";
  m_OutputMaterialsFileName = "MPLIST.txt";
  m_ApplyMatrixFlag = 0;
  m_MatrixToBeAppliedToGeometry = NULL;

	m_MaterialData = NULL;
	m_MatIdArray = NULL;

	m_WriteMode = WITH_MAT_MODE;
}
//----------------------------------------------------------------------------
albaVMEMeshAnsysTextExporter::~albaVMEMeshAnsysTextExporter()
{
  
}

//----------------------------------------------------------------------------
int albaVMEMeshAnsysTextExporter::Write()
{
  int writeResult = ALBA_ERROR;

  writeResult = WriteNodesFile(m_Input, m_OutputNodesFileName);

  if (writeResult == ALBA_ERROR)
  {
    return ALBA_ERROR;
  }

  writeResult = WriteElementsFile(m_Input, m_OutputElementsFileName);
  if (writeResult == ALBA_ERROR)
  {
    return ALBA_ERROR;
  }

	if (m_WriteMode == WITH_MAT_MODE)
		WriteMaterialsFile(m_Input, m_OutputMaterialsFileName);
  
  return ALBA_OK;
}

//----------------------------------------------------------------------------
int albaVMEMeshAnsysTextExporter::WriteNodesFile( vtkUnstructuredGrid *inputUGrid, const char *outputFileName )
{
  // check this is a valid nodes id field data: 
  vtkIntArray *nodesIDArray = vtkIntArray::SafeDownCast(inputUGrid->GetPointData()->GetArray("Id"));
  
	if(nodesIDArray==NULL)
    nodesIDArray = vtkIntArray::SafeDownCast(inputUGrid->GetPointData()->GetArray("id"));
  
  vtkIntArray *syntheticNodesIDArray = NULL;

  if (nodesIDArray == NULL)
  {
    albaLogMessage("nodesID informations not found in vtk unstructured grid!\
    Temporary nodes id array will be created in order to export the data.");
    
    int numPoints = inputUGrid->GetNumberOfPoints();
    syntheticNodesIDArray = vtkIntArray::New();
    
    int offset = 1 ;
    for (int i = 0; i < numPoints; i++) 
    {
      syntheticNodesIDArray->InsertNextValue(i + offset);
    }
    
    nodesIDArray = syntheticNodesIDArray;
  }

  assert(nodesIDArray != NULL);

  // get the pointsToBeExported
  int columnsNumber;
  int rowsNumber = inputUGrid->GetNumberOfPoints();
  columnsNumber = 4; // point ID + point coordinates
  
  vtkPoints *pointsToBeExported = NULL;

  vtkTransform *transform = NULL;
  vtkTransformFilter *transformFilter = NULL;
  vtkUnstructuredGrid *inUGDeepCopy = NULL;
  
  if (m_ApplyMatrixFlag)
  {
    // apply abs matrix to geometry
    assert(m_MatrixToBeAppliedToGeometry);

    transform = vtkTransform::New();
    transform->SetMatrix(m_MatrixToBeAppliedToGeometry);

    transformFilter = vtkTransformFilter::New();
    inUGDeepCopy = vtkUnstructuredGrid::New();
    inUGDeepCopy->DeepCopy(inputUGrid);

    transformFilter->SetInputData(inUGDeepCopy);
    transformFilter->SetTransform(transform);
    transformFilter->Update();

    pointsToBeExported = transformFilter->GetOutput()->GetPoints();
  } 
  else
  {
    // do not transform geometry
    pointsToBeExported = inputUGrid->GetPoints();
  }
  
  // read all the pointsToBeExported in memory (vnl_matrix)
  
  int pointIDColumn = 0;

  double pointCoordinates[3] = {-9999, -9999, -9999};

	FILE *file=fopen(outputFileName,"w");

  for (int rowID = 0 ; rowID < rowsNumber ; rowID++)
  {
    pointsToBeExported->GetPoint(rowID, pointCoordinates);

		fprintf(file, "%d ", nodesIDArray->GetValue(rowID));

		fprintf(file, "%f ", pointCoordinates[0]);
		fprintf(file, "%f ", pointCoordinates[1]);
		fprintf(file, "%f ", pointCoordinates[2]);
		fprintf(file, "\n");	
  }

	fclose(file);
	  
  // clean up
  vtkDEL(inUGDeepCopy);
  vtkDEL(transform);
  vtkDEL(transformFilter);
  vtkDEL(syntheticNodesIDArray);

  nodesIDArray = NULL;

  return ALBA_OK;

}

//----------------------------------------------------------------------------
int albaVMEMeshAnsysTextExporter::WriteElementsFile( vtkUnstructuredGrid *inputUGrid, const char *outputFileName )
{
  // create elements matrix 
  int cellsNumber = inputUGrid->GetNumberOfCells();

  // since we do not support hybrid meshes I use the first cell as a probe
  int numberOfPointsPerCell = inputUGrid->GetCell(0)->GetNumberOfPoints();

  int rowsNumber = cellsNumber;

  //  tetra 10 example
  //  offset                  |                     pointsToBeExported ID
  //  1   3   3   3   0   1      5     2     4     3    11    10    14    13    9    12

  // read all the elements with their attribute data in memory (vnl_matrix)

  // get the ELEMENT_ID array
  vtkIntArray *elementIdArray = albaVMEMesh::GetElementsIDArray(inputUGrid);

  vtkIntArray *syntheticElementsIDArray = NULL;

  if (elementIdArray == NULL)
  {
    albaLogMessage("Id information not found in vtk unstructured grid!\
    Temporary elements id array will be created in order to export the data.");

    int numCells = inputUGrid->GetNumberOfCells();
    syntheticElementsIDArray = vtkIntArray::New();

    int offset = 1 ;
    for (int i = 0; i < numCells; i++) 
    {
      syntheticElementsIDArray->InsertNextValue(i + offset);
    }

    elementIdArray = syntheticElementsIDArray;
  }

  // get the Ansys Nodes Id array
  vtkIntArray *nodesIDArray = albaVMEMesh::GetNodesIDArray(inputUGrid);
  
  vtkIntArray *syntheticNodesIDArray = NULL;

  if (nodesIDArray == NULL)
  {
    albaLogMessage("nodesID informations not found in vtk unstructured grid!\
                  Temporary nodes id array will be created in order to export the data.");

    int numPoints = inputUGrid->GetNumberOfPoints();
    syntheticNodesIDArray = vtkIntArray::New();

    int offset = 1 ;
    for (int i = 0; i < numPoints; i++) 
    {
      syntheticNodesIDArray->InsertNextValue(i + offset);
    }

    nodesIDArray = syntheticNodesIDArray;
  }

  // create vtkPointIdAnsysPointId map
  vcl_map<int, int> vtkPointIdAnsysPointsIdMap;
  int nodesIdNumber = nodesIDArray->GetNumberOfTuples();
  
  assert(nodesIdNumber == inputUGrid->GetNumberOfPoints());

  for (int i = 0; i < nodesIdNumber; i++)
  {
      int ansysNodeID = nodesIDArray->GetValue(i);
      vtkPointIdAnsysPointsIdMap[i] = ansysNodeID;
  }
  
  // get the MATERIAL array
	vtkIdType *materialArray = m_MatIdArray;
  vtkIdType *syntheticMaterialArray = NULL;

  if (materialArray == NULL)
  {
    albaLogMessage("material informations not found in vtk unstructured grid!\
    Temporary materials id array will be created in order to export the data.");

    int numCells = inputUGrid->GetNumberOfCells();
		syntheticMaterialArray = new vtkIdType[numCells];
    
    int defaultMaterialID = 1;

    for (int i = 0; i < numCells; i++) 
    {
      syntheticMaterialArray[i]=defaultMaterialID;
    }

    materialArray = syntheticMaterialArray;
  }

	FILE *file=fopen(outputFileName,"w");
		
  for (int rowID = 0 ; rowID < rowsNumber ; rowID++)
  {		
    fprintf(file, "%d ", elementIdArray->GetValue(rowID));

		if (m_WriteMode == WITH_MAT_MODE)
			fprintf(file, "%d ", materialArray[rowID]);

    // get the i-th cell from input mesh
    vtkCell *currentCell = inputUGrid->GetCell(rowID);

    // get the old id list
    vtkIdList *idList = currentCell->GetPointIds();

    for (int currentID = 0; currentID < numberOfPointsPerCell;currentID++)
			fprintf(file, "%d ",  vtkPointIdAnsysPointsIdMap[idList->GetId(currentID)]);

		fprintf(file,"\n");
  }

	fclose(file);

  vtkDEL(syntheticNodesIDArray);
  nodesIDArray = NULL;
  
  vtkDEL(syntheticElementsIDArray);
  elementIdArray = NULL;

  delete[] syntheticMaterialArray;
  materialArray = NULL;
  
  return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporter::WriteMaterialsFile(vtkUnstructuredGrid *inputUGrid, const char *outputFileName)
{
	// check this is a valid materials field data: I am assuming that vtk field data
	// contains only material attributes otherwise this code will not work
	vtkDataArray *materialsIDArray;
	vtkDataArray *exArray; 
	vtkDataArray *nuxyArray; 
	vtkDataArray *densArray; 


	if (m_MaterialData)
	{
		// try field data
		materialsIDArray = m_MaterialData->GetArray("material_id");
		exArray = m_MaterialData->GetArray("EX");
		nuxyArray = m_MaterialData->GetArray("NUXY");
		densArray = m_MaterialData->GetArray("DENS");

		if (materialsIDArray == NULL)
		{
			albaLogMessage("Material array not found");
			return;
		}
		if (exArray == NULL)
		{
			albaLogMessage("Ex array not found");
			return;
		}
		if (nuxyArray == NULL)
		{
			albaLogMessage("NUxy array not found");
			return;
		}
		if (densArray == NULL)
		{
			albaLogMessage("Density array not found");
			return;
		}
	}
	else
	{
		albaLogMessage("Material Data not found");
		return;
	}

	vcl_ofstream outputf;
	outputf.open(outputFileName);
	
	// get the number of materials
	int numberOfMaterials = materialsIDArray->GetNumberOfTuples();
	
	outputf << "MAT_N\tEx\tNUxy\tDens" << std::endl;
	
	// write each material
	for (int i = 0; i < numberOfMaterials; i++)
	{
		int materialID = materialsIDArray->GetTuple1(i);

		outputf << materialID << "\t";
		outputf << exArray->GetTuple1(i) << "\t";
		outputf << nuxyArray->GetTuple1(i) << "\t";
		outputf << densArray->GetTuple1(i) << std::endl;
	}

	outputf.close();
}


