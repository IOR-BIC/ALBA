/*=========================================================================

 Program: MAF2
 Module: mafVMEMeshAnsysTextExporter
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

#include "mafVMEMeshAnsysTextExporter.h"

#include "vtkMAFSmartPointer.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkFieldData.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"

#include "mafVMEMesh.h"
#include "mafTagArray.h"


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
id array name: "id"    type:  vtkIntArray


CELLS:  
--------------------------------
material array name: "material"   type: vtkIntArray

*/
mafVMEMeshAnsysTextExporter::mafVMEMeshAnsysTextExporter()
{
  m_Input = NULL;
  m_OutputNodesFileName = "NLIST.txt";
  m_OutputElementsFileName = "ELIST.txt";
  m_OutputMaterialsFileName = "MPLIST.txt";
  m_ApplyMatrixFlag = 0;
  m_MatrixToBeAppliedToGeometry = NULL;
}
//----------------------------------------------------------------------------
mafVMEMeshAnsysTextExporter::~mafVMEMeshAnsysTextExporter()
{
  
}

//----------------------------------------------------------------------------
int mafVMEMeshAnsysTextExporter::Write()
{
  int writeResult = MAF_ERROR;

  writeResult = WriteNodesFile(m_Input, m_OutputNodesFileName);

  if (writeResult == MAF_ERROR)
  {
    return MAF_ERROR;
  }

  writeResult = WriteElementsFile(m_Input, m_OutputElementsFileName);
  if (writeResult == MAF_ERROR)
  {
    return MAF_ERROR;
  }

  WriteMaterialsFile(m_Input, m_OutputMaterialsFileName);
  
  return MAF_OK;
}

int mafVMEMeshAnsysTextExporter::WriteNodesFile( vtkUnstructuredGrid *inputUGrid, const char *outputFileName )
{
  // check this is a valid nodes id field data: 
  vcl_string nodesIDArrayName = "id";

  vtkIntArray *nodesIDArray = vtkIntArray::SafeDownCast(inputUGrid->GetPointData()->GetArray(nodesIDArrayName.c_str()));
  
  vtkIntArray *syntheticNodesIDArray = NULL;

  if (nodesIDArray == NULL)
  {
    mafLogMessage("nodesID informations not found in vtk unstructured grid!\
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

  return MAF_OK;

}

int mafVMEMeshAnsysTextExporter::WriteElementsFile( vtkUnstructuredGrid *inputUGrid, const char *outputFileName )
{
  // create elements matrix 
  int cellsNumber = inputUGrid->GetNumberOfCells();

  // since we do not support hybrid meshes I use the first cell as a probe
  int numberOfPointsPerCell = inputUGrid->GetCell(0)->GetNumberOfPoints();

  int rowsNumber = cellsNumber;

  // tetra 10 example
  //  offset                  |                     pointsToBeExported ID
  //  1   3   3   3   0   1      5     2     4     3    11    10    14    13    9    12
  int offset = 6;  
  int columnsNumber = offset + numberOfPointsPerCell;

  // read all the elements with their attribute data in memory (vnl_matrix)

  mafString ansysNODESIDArrayName("id");
  mafString ansysELEMENTIDArrayName("ANSYS_ELEMENT_ID");
  mafString ansysTYPEIntArrayName("ANSYS_ELEMENT_TYPE");
  mafString ansysMATERIALIntArrayName("material"); 
  mafString ansysREALIntArrayName("ANSYS_ELEMENT_REAL");

  vtkCellData *cellData = inputUGrid->GetCellData();
  vtkPointData *pointData = inputUGrid->GetPointData();

  // get the ELEMENT_ID array
  vtkIntArray *elementIdArray = vtkIntArray::SafeDownCast(cellData->GetArray(ansysELEMENTIDArrayName.GetCStr()));
  
  vtkIntArray *syntheticElementsIDArray = NULL;

  if (elementIdArray == NULL)
  {
    mafLogMessage("ANSYS_ELEMENT_ID information not found in vtk unstructured grid!\
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
  vtkIntArray *nodesIDArray = vtkIntArray::SafeDownCast(pointData->GetArray(ansysNODESIDArrayName.GetCStr()));  
  
  vtkIntArray *syntheticNodesIDArray = NULL;

  if (nodesIDArray == NULL)
  {
    mafLogMessage("nodesID informations not found in vtk unstructured grid!\
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
  vtkIntArray *materialArray = vtkIntArray::SafeDownCast(cellData->GetArray(ansysMATERIALIntArrayName.GetCStr()));
  
  vtkIntArray *syntheticMaterialArray = NULL;

  if (materialArray == NULL)
  {
    mafLogMessage("material informations not found in vtk unstructured grid!\
    Temporary materials id array will be created in order to export the data.");

    int numCells = inputUGrid->GetNumberOfCells();
    syntheticMaterialArray = vtkIntArray::New();
    
    int defaultMaterialID = 1;

    for (int i = 0; i < numCells; i++) 
    {
      syntheticMaterialArray->InsertNextValue(defaultMaterialID);
    }

    materialArray = syntheticMaterialArray;
  }

  // get the TYPE array
  vtkIntArray *typeArray = vtkIntArray::SafeDownCast(cellData->GetArray(ansysTYPEIntArrayName.GetCStr()));
  
  vtkIntArray *syntheticTypeArray = NULL;

  if (typeArray == NULL)
  {
    mafLogMessage("Ansys TYPE informations not found in vtk unstructured grid!\
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
  vtkIntArray *realArray = vtkIntArray::SafeDownCast(cellData->GetArray(ansysREALIntArrayName.GetCStr()));
  
  vtkIntArray *syntheticRealArray = NULL;

  if (realArray == NULL)
  {
    mafLogMessage("Ansys REAL informations not found in vtk unstructured grid!\
                  Temporary REAL array will be created in order to export the data.");

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
		fprintf(file, "%d ", materialArray->GetValue(rowID));
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

  vtkDEL(syntheticMaterialArray);
  materialArray = NULL;
  
  vtkDEL(syntheticTypeArray);
  typeArray = NULL;
  
  vtkDEL(syntheticRealArray);
  realArray = NULL;

  return MAF_OK;
}

void mafVMEMeshAnsysTextExporter::WriteMaterialsFile( vtkUnstructuredGrid *inputUGrid, const char *outputFileName )
{
  // check this is a valid materials field data: I am assuming that vtk field data
  // contains only material attributes otherwise this code will not work
  vcl_string materialIDArrayName = "material_id";

  vtkDataArray *materialsIDArray = NULL;
  
  // try field data
  materialsIDArray = inputUGrid->GetFieldData()->GetArray(materialIDArrayName.c_str());

  if (materialsIDArray != NULL)
  {
    mafLogMessage("Found material array in field data");
  }
  else
  {
    // try scalars 
    materialsIDArray = inputUGrid->GetCellData()->GetScalars(materialIDArrayName.c_str());  
    if (materialsIDArray != NULL)
    {
      mafLogMessage("Found material array as active scalar");
    }
  }

  if (materialsIDArray == NULL)
  {
    mafLogMessage("material informations not found in vtk unstructured grid!\
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
    output   << std::endl;

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
    int numberOfMaterialProperties = inputUGrid->GetFieldData()->GetNumberOfArrays() - 1; // 1 is the materialsIDArray

    vtkFieldData *fieldData = inputUGrid->GetFieldData();

    // gather material properties array names
    vcl_vector<vcl_string> materialProperties;
    for (int arrayID = 0; arrayID < fieldData->GetNumberOfArrays(); arrayID++)
    {
      vcl_string arrayName = fieldData->GetArray(arrayID)->GetName();
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
        vtkDataArray *array = fieldData->GetArray(arrayName.c_str());
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


