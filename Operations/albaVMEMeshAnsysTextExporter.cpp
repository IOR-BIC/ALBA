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
/*

DATA ARRAY NAMING

NODES:
--------------------------------
id array name: "Id"    type:  vtkIntArray


CELLS:  
--------------------------------
material array name: "Material"   type: vtkIntArray

*/
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

  WriteMaterialsFile(m_Input, m_OutputMaterialsFileName);
  
  return ALBA_OK;
}

//----------------------------------------------------------------------------
int albaVMEMeshAnsysTextExporter::WriteNodesFile( vtkUnstructuredGrid *inputUGrid, const char *outputFileName )
{
  // check this is a valid nodes id field data: 
  vtkIntArray *nodesIDArray = vtkIntArray::SafeDownCast(inputUGrid->GetPointData()->GetArray("Id"));
  if(nodesIDArray)
  {
    nodesIDArray = vtkIntArray::SafeDownCast(inputUGrid->GetPointData()->GetArray("id"));
  }

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

    transformFilter->SetInput(inUGDeepCopy);
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
  int offset = 6;  
  int columnsNumber = offset + numberOfPointsPerCell;

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

  // get the TYPE array
  vtkIntArray *typeArray = albaVMEMesh::GetElementsTypeArray(inputUGrid);

  vtkIntArray *syntheticTypeArray = NULL;

  if (typeArray == NULL)
  {
    albaLogMessage("Ansys TYPE informations not found in vtk unstructured grid!\
                  Temporary TYPE array will be created in order to export the data.");

    int numCells = inputUGrid->GetNumberOfCells();
    syntheticTypeArray = vtkIntArray::New();

    int defaultTypeValue = 1;

    for (int i = 0; i < numCells; i++) 
    {
      syntheticTypeArray->InsertNextValue(defaultTypeValue);
    }

    typeArray = syntheticTypeArray;
  }

  // get the REAL array
  vtkIntArray *realArray = albaVMEMesh::GetElementsRealArray(inputUGrid);

  vtkIntArray *syntheticRealArray = NULL;

  if (realArray == NULL)
  {
    albaLogMessage("Real informations not found in vtk unstructured grid!\
                  Temporary Real array will be created in order to export the data.");

    int numCells = inputUGrid->GetNumberOfCells();
    syntheticRealArray = vtkIntArray::New();

    int defaultRealValue = 1;

    for (int i = 0; i < numCells; i++) 
    {
      syntheticRealArray->InsertNextValue(defaultRealValue);
    }

    realArray = syntheticRealArray;
  }

	FILE *file=fopen(outputFileName,"w");
		
  for (int rowID = 0 ; rowID < rowsNumber ; rowID++)
  {
		int esys = 0;// <TODO!!!!!> this seems to be always 0... to be supported in the future anyway
		int comp = 1;// <TODO!!!!!> this seems to be always 1... to be supported in the future anyway

    fprintf(file, "%d ", elementIdArray->GetValue(rowID));
		fprintf(file, "%d ", materialArray[rowID]);
		fprintf(file, "%d ", typeArray->GetValue(rowID));
		fprintf(file, "%d ", realArray->GetValue(rowID));
		fprintf(file, "%d ", esys);
		fprintf(file, "%d ", comp);

    // get the ith cell from input mesh
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
  
  vtkDEL(syntheticTypeArray);
  typeArray = NULL;
  
  vtkDEL(syntheticRealArray);
  realArray = NULL;

  return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaVMEMeshAnsysTextExporter::WriteMaterialsFile(vtkUnstructuredGrid *inputUGrid, const char *outputFileName)
{
	// check this is a valid materials field data: I am assuming that vtk field data
	// contains only material attributes otherwise this code will not work
	vcl_string materialIDArrayName = "material_id";

	vtkDataArray *materialsIDArray = NULL;

	if (m_MaterialData)
	{
		// try field data
		materialsIDArray = m_MaterialData->GetArray("material_id");

		if (materialsIDArray != NULL)
		{
			albaLogMessage("Found material array in field data");
		}
		else
		{
			// try scalars 
			materialsIDArray = inputUGrid->GetCellData()->GetScalars(materialIDArrayName.c_str());
			if (materialsIDArray != NULL)
			{
				albaLogMessage("Found material array as active scalar");
			}
		}
	}

	if (materialsIDArray == NULL)
	{
		albaLogMessage("material informations not found in vtk unstructured grid!\
    A fake temporary material with ID = 1 will be created in order to export the data.");

		// write fake material to disk...
		vcl_ostringstream output;

		int fakeMaterialID = 1;
		double fakeMaterialTemperature = 0.0000;

		output << "MATERIAL NUMBER = " << fakeMaterialID << " EVALUATED AT TEMPERATURE OF " << \
			fakeMaterialTemperature << std::endl;
		
		vcl_string fakeMaterialPropertyName = "FAKEMATERIALUSEDFOREXPORTTOWARDANSYS";
		int fakeMaterialPropertyValue = 1;

		output << fakeMaterialPropertyName << " = " << fakeMaterialPropertyValue << std::endl;
		output << std::endl;

		cout << output.str();

		vcl_ofstream outputf;
		vcl_string fileName = outputFileName;

		outputf.open(fileName.c_str());
		outputf << output.str();
		outputf.close();
	}

	else
	{
		// get the number of materials
		int numberOfMaterials = materialsIDArray->GetNumberOfTuples();

		// get the number of materials properties
		int numberOfMaterialProperties = m_MaterialData->GetNumberOfArrays() - 1; // 1 is the materialsIDArray
		
		// gather material properties array names
		vcl_vector<vcl_string> materialProperties;
		for (int arrayID = 0; arrayID < m_MaterialData->GetNumberOfArrays(); arrayID++)
		{
			vcl_string arrayName = m_MaterialData->GetArray(arrayID)->GetName();
			if (arrayName != materialIDArrayName.c_str())
			{
				materialProperties.push_back(arrayName);
			}
		}

		//MATERIAL NUMBER =      3 EVALUATED AT TEMPERATURE OF   0.0000    
		//  EX   =  0.20000E+06
		//  NUXY =  0.33000
		//  DENS =   1.0700  

		// write each material
		//
		// write elements matrix to disk...
		vcl_ostringstream output;

		// for each material
		for (int i = 0; i < numberOfMaterials; i++)
		{
			int materialID = materialsIDArray->GetTuple(i)[0];
			double materialTemperature = 0.0000;  // not supported for the moment  

			output << "MATERIAL NUMBER = " << materialID << " EVALUATED AT TEMPERATURE OF " \
				<< materialTemperature << std::endl;

			// for each property
			for (int j = 0; j < numberOfMaterialProperties; j++)
			{
				vcl_string arrayName = materialProperties[j];
				vtkDataArray *array = m_MaterialData->GetArray(arrayName.c_str());
				output << arrayName << " = " << array->GetTuple(i)[0] << std::endl;
			}

			output << std::endl;
		}

		cout << output.str();

		vcl_ofstream outputf;
		vcl_string fileName = outputFileName;
		outputf.open(fileName.c_str());
		outputf << output.str();
		outputf.close();
	}
}

