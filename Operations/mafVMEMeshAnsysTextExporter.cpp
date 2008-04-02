/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEMeshAnsysTextExporter.cpp,v $
Language:  C++
Date:      $Date: 2008-04-02 15:10:13 $
Version:   $Revision: 1.3 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
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

}
//----------------------------------------------------------------------------
mafVMEMeshAnsysTextExporter::~mafVMEMeshAnsysTextExporter()
{
  
}

//----------------------------------------------------------------------------
void mafVMEMeshAnsysTextExporter::Write()
{	 
  WriteNodesFile(m_Input, m_OutputNodesFileName);
  WriteElementsFile(m_Input, m_OutputElementsFileName);
  WriteMaterialsFile(m_Input, m_OutputMaterialsFileName);

}

void mafVMEMeshAnsysTextExporter::WriteNodesFile( vtkUnstructuredGrid *inputUGrid, const char *outputFileName )
{
  // check this is a valid nodes id field data: 
  vcl_string nodesIDArrayName = "id";

  vtkIntArray *nodesIDArray = vtkIntArray::SafeDownCast(inputUGrid->GetPointData()->GetArray(nodesIDArrayName.c_str()));

  if (nodesIDArray == NULL)
  {
    mafLogMessage("nodesID informations not found in vtk unstructured grid!\
    nodes file will not be written");
    return;
  }

  assert(nodesIDArray != NULL);

  // get the points
  int columnsNumber;
  int rowsNumber = inputUGrid->GetNumberOfPoints();
  columnsNumber = 4; // point ID + point coordinates

  // create a matrix
  vnl_matrix<double>  PointsMatrix;
  PointsMatrix.set_size(rowsNumber,columnsNumber);

  // read all the points in memory (vnl_matrix)
  vtkPoints *points = inputUGrid->GetPoints();

  int pointIDColumn = 0;

  double pointCoordinates[3] = {-9999, -9999, -9999};

  for (int rowID = 0 ; rowID < rowsNumber ; rowID++)
  {
    points->GetPoint(rowID, pointCoordinates);

    enum {x = 1, y, z};

    int pointID = rowID + 1; // this is +1 fortran offset
    PointsMatrix(rowID, pointIDColumn) = nodesIDArray->GetValue(rowID);
    PointsMatrix(rowID, x) = pointCoordinates[0];
    PointsMatrix(rowID, y) = pointCoordinates[1];
    PointsMatrix(rowID, z) = pointCoordinates[2];
  }

  cout << PointsMatrix;

  assert(PointsMatrix.rows() == rowsNumber);
  assert(PointsMatrix.columns() == columnsNumber);

  // write nodes matrix to disk...
  vcl_ofstream output;

  vcl_string fileName = outputFileName;

  output.open(fileName.c_str());
  output << PointsMatrix;
  output.close();
}

void mafVMEMeshAnsysTextExporter::WriteElementsFile( vtkUnstructuredGrid *inputUGrid, const char *outputFileName )
{
  // create elements matrix 
  int cellsNumber = inputUGrid->GetNumberOfCells();

  // since we do not support hybrid meshes I use the first cell as a probe
  int numberOfPointsPerCell = inputUGrid->GetCell(0)->GetNumberOfPoints();

  int rowsNumber = cellsNumber;

  // tetra 10 example
  //  offset                  |                     points ID
  //  1   3   3   3   0   1      5     2     4     3    11    10    14    13    9    12
  int offset = 6;  
  int columnsNumber = offset + numberOfPointsPerCell;

  // create a matrix
  vnl_matrix<double>  ElementsMatrix;
  ElementsMatrix.set_size(rowsNumber,columnsNumber);

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
  assert(elementIdArray);

  // get the ELEMENT_ID array
   vtkIntArray *nodesIDArray = vtkIntArray::SafeDownCast(pointData->GetArray(ansysNODESIDArrayName.GetCStr()));
   assert(nodesIDArray);

  // create vtkPointIdAnsysPointId map
  vcl_map<int, int> vtkPointIdAnsysPointsIdMap;
  int nodesIdNumber = nodesIDArray->GetNumberOfTuples();
  
  assert(nodesIdNumber == inputUGrid->GetNumberOfPoints());

  for (int i = 0; i < nodesIdNumber; i++)
  {
      int ansysNodeID = nodesIDArray->GetValue(i);
      vtkPointIdAnsysPointsIdMap[i] = ansysNodeID;
  }

  // get the TYPE array
  vtkIntArray *typeArray = vtkIntArray::SafeDownCast(cellData->GetArray(ansysTYPEIntArrayName.GetCStr()));
  assert(typeArray);

  // get the MATERIAL array
  vtkIntArray *materialArray = vtkIntArray::SafeDownCast(cellData->GetArray(ansysMATERIALIntArrayName.GetCStr()));
  assert(materialArray);

  // get the REAL array
  vtkIntArray *realArray = vtkIntArray::SafeDownCast(cellData->GetArray(ansysREALIntArrayName.GetCStr()));
  assert(realArray);

  // fill the matrix
  int cellIDColumn = 0;
  int MATERIALColumn = 1;
  int TYPEColumn = 2;
  int REALColumn = 3;
  int ESYSColumn = 4;
  int COMPColumn = 5;
  int firstPointIDCol = 6;
  int fortranOffset = 1;

  for (int rowID = 0 ; rowID < rowsNumber ; rowID++)
  {
    // write element ID into matrix
    int id = elementIdArray->GetValue(rowID);
    ElementsMatrix(rowID, cellIDColumn) = id;

    // type
    int type = typeArray->GetValue(rowID);
    ElementsMatrix(rowID, TYPEColumn) = type;

    // material
    int material = materialArray->GetValue(rowID);
    ElementsMatrix(rowID, MATERIALColumn) = material;

    // real
    int real = realArray->GetValue(rowID);
    ElementsMatrix(rowID, REALColumn) = real;

    // esys
    int esys = 0;// <TODO!!!!!> this seems to be always 0... to be supported in the future anyway
    ElementsMatrix(rowID, ESYSColumn) = esys;

    // comp
    int comp = 1;// <TODO!!!!!> this seems to be always 1... to be supported in the future anyway
    ElementsMatrix(rowID, COMPColumn) = comp;

    // get the ith cell from input mesh
    vtkCell *currentCell = inputUGrid->GetCell(rowID);

    // get the old id list
    vtkIdList *idList = currentCell->GetPointIds();
    int currentID = 0;

    for (int column = firstPointIDCol; column < firstPointIDCol + numberOfPointsPerCell;column++)
    {
      ElementsMatrix(rowID,column) = vtkPointIdAnsysPointsIdMap[idList->GetId(currentID)];//  + fortranOffset; // this is +1 fortran offset;to be eventually replaced with pointID from fieldData...
      currentID++;
    }  
  }

  cout << ElementsMatrix;

  // write elements matrix to disk...
  vcl_ofstream output;

  vcl_string fileName = outputFileName;

  output.open(fileName.c_str());
  output << ElementsMatrix;
  output.close();

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

  if (materialsIDArray  == NULL)
  {
    mafLogMessage("materials informations not found in vtk unstructured grid!\
    materials file will not be written");
    return;
  }
   
  assert(materialsIDArray != NULL);

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
  vcl_ofstream output;

  vcl_string fileName = outputFileName;

  output.open(fileName.c_str());


  // for each material
  for (int i = 0; i < numberOfMaterials; i++)
  {
    int materialID = materialsIDArray->GetTuple(i)[0];
    double materialTemperature = 0.0000;  // not supported for the moment  

    output << "MATERIAL NUMBER = " << materialID << " EVALUATED AT TEMPERATURE OF " << materialTemperature << std::endl;

    // for each property
    for (int j = 0; j < numberOfMaterialProperties; j++)
    {
      vcl_string arrayName = materialProperties[j];
      vtkDataArray *array = fieldData->GetArray(arrayName.c_str());
      output << arrayName << " = " << array->GetTuple(i)[0] << std::endl;
    }

    output << std::endl;
  }  

  cout << output;
  output.close();

}


