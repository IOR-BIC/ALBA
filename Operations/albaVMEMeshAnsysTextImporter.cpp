/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEMeshAnsysTextImporter
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// debug facility
// #define DEBUG_MODE

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaVMEMeshAnsysTextImporter.h"

#include "vtkALBASmartPointer.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkFieldData.h"

#include "albaVMEMesh.h"
#include "albaTagArray.h"
#include "albaGui.h"
#include "albaGUIDialog.h"


// vcl includes
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>


//----------------------------------------------------------------------------
// constants

const int CHAR_BUF_SIZE = 1000;

albaVMEMeshAnsysTextImporter::albaVMEMeshAnsysTextImporter()
{
  m_NodesFileName = "";
  m_ElementsFileName = "";
  m_MaterialsFileName = "";
  m_ElementType = UNSUPPORTED_ELEMENT;
  m_NodesPerElement = -1;
  m_MeshType = UNKNOWN;
  m_ReaderMode = WITH_MAT_MODE;

  m_Output = NULL;
}
//----------------------------------------------------------------------------
albaVMEMeshAnsysTextImporter::~albaVMEMeshAnsysTextImporter()
{
  albaDEL(m_Output);
}

//----------------------------------------------------------------------------
int albaVMEMeshAnsysTextImporter::Read()
{	
  vtkALBASmartPointer<vtkUnstructuredGrid> grid;
  grid->Initialize();

  if (ParseNodesFile(grid) == -1) return ALBA_ERROR ; 
  if (ParseElementsFile(grid) == -1) return ALBA_ERROR;

	if (m_ReaderMode == WITH_MAT_MODE)
		if (ParseMaterialsFile(grid, m_MaterialsFileName) == -1) return ALBA_ERROR;

  // allocate the output if not yet allocated...
  if (m_Output == NULL)
  {
    albaNEW(m_Output);

    albaTagItem tag_Nature;
    tag_Nature.SetName("VME_NATURE");
    tag_Nature.SetValue("NATURAL");
    m_Output->GetTagArray()->SetTag(tag_Nature);
    m_Output->SetName("imported mesh");

  }

  // TO BE PORTED
  // albaVMEMeshParabolicMeshToLinearMesh *p2l = albaVMEMeshParabolicMeshToLinearMesh::New();

  // material file is present?

  vtkALBASmartPointer<vtkUnstructuredGrid> gridToLinearize;
  
  if (m_ReaderMode == WITH_MAT_MODE)
  {
    albaLogMessage("Materials file found. Building mesh attribute data from materials info...");

    // convert field data from materials to cell data       
    FEMDataToCellData(grid.GetPointer(), gridToLinearize.GetPointer());
  }
  else
  {
    albaLogMessage("Materials file not found! Not building attribute data from materials info...");
    gridToLinearize = grid;
  }
   
  m_Output->SetDataByDetaching(gridToLinearize,0);

  return ALBA_OK;
}

//----------------------------------------------------------------------------
int albaVMEMeshAnsysTextImporter::ParseNodesFile(vtkUnstructuredGrid *grid)
//----------------------------------------------------------------------------
{
  if (strcmp(m_NodesFileName, "") == 0)
  {
    albaLogMessage("Nodes filename not specified!");
    return -1;
  }

  int node_id_col = 0;
  int x_col = 1;
  int y_col = x_col + 1;
  int z_col = x_col + 2;

  
  vnl_matrix<double> M;

  if (ReadMatrix(M,this->m_NodesFileName))
  {
    albaErrorMacro("Wrong Node file! File:" << m_NodesFileName << endl);
	  return -1;
  }

  // create points structure

  vtkPoints* points = vtkPoints::New();
  points->SetNumberOfPoints(M.rows());

  for (int i = 0; i < M.rows(); i++)    
    {
      double xCoord = M(i, x_col);
      double yCoord = M(i, y_col);
      double zCoord = M(i, z_col);
      points->SetPoint(i, xCoord, yCoord, zCoord);
    }

  // fill the POINT section of the Mesh
  grid->SetPoints(points);

  vtkDEL(points);


  // create a map with id_val <-> node_id association
  for (int i = 0; i < M.rows(); i++)
  {
    m_NodeIdNodeNumberMap[M(i, node_id_col)] = i;  
  }
  
  /*
  // store info about node_id <-> node_index association 
  // example:
  
  NLIST  
  25  x y z => node_id = 25 && n_index = 0;
  27  x y z => node_id = 27 && n_index = 1;
  32  x y z => node_id = 32 && n_index = 2;
  45  x y z ...
                        
  ELIST
  245 25 27 32
  248 45 32 27
  */
  vtkIntArray *node_id_array = vtkIntArray::New();
  node_id_array->SetName("Id");
  node_id_array->SetNumberOfTuples(M.rows());

  for (int i = 0; i < M.rows(); i++)
  {
    // fill the MaterialsArray
    node_id_array->SetValue(i, M(i, node_id_col));
  }

  // create the materials section 
  grid->GetPointData()->SetScalars(node_id_array);

  vtkDEL(node_id_array);

  return 0;
}

void albaVMEMeshAnsysTextImporter::AddIntArrayToUnstructuredGridCellData( vtkUnstructuredGrid *grid, vnl_matrix<double> &elementsFileMatrix, int column, albaString outputArrayName , bool activeScalar)
{
  // store info about cell_id <-> material_id association
  vtkIntArray *array = vtkIntArray::New();
  array->SetName(outputArrayName.GetCStr());
  array->SetNumberOfTuples(elementsFileMatrix.rows());

  for (int i = 0; i < elementsFileMatrix.rows(); i++)
  {
    // fill the array
    array->SetValue(i, elementsFileMatrix(i, column));
  }

  if (activeScalar == true)
  {
    grid->GetCellData()->SetScalars(array);
  } 
  else
  {
    grid->GetCellData()->AddArray(array);
  }
  
  vtkDEL(array);
}
//----------------------------------------------------------------------------
int albaVMEMeshAnsysTextImporter::ParseElementsFile(vtkUnstructuredGrid *grid)
//----------------------------------------------------------------------------
{
  if (strcmp(m_ElementsFileName, "") == 0)
  {
    albaLogMessage("Elements filename not specified!");
    return -1;
  }
  
  vnl_matrix<double> ElementsFileMatrix;


  int cell_id_col = 0;

  //  column:          1          2         2
  //  from ANSYS file: TYPE, 2  $ MAT, 2  $ REAL, 3

  // elId  mat  pointId...
  // 37411 440  119324    6996    6994    4809   70910   70925   70920   84679 84682   84683

  int ansysELEMENTIDColumn = 0; 
  int ansysMATERIALColumn = 1; 

  m_FirstConnectivityColumn = 2;

  if(m_ReaderMode == WITHOUT_MAT_MODE)
  {
    // elId  pointId...
    // 37411 119324    6996    6994    4809   70910   70925   70920   84679 84682   84683
    m_FirstConnectivityColumn = 1;
  }

  int ret = GetElementType();
  if (ret == -1 || ret == UNSUPPORTED_ELEMENT )  
  {
		if (m_FirstConnectivityColumn == 1)
		{
			//Try to skip mat column
			m_FirstConnectivityColumn = 2;
			ret = GetElementType();
			if (ret == -1 || ret == UNSUPPORTED_ELEMENT)
			{
				return -1;
			}
		}
		else
			return -1;
  }

  // id list for connectivity
  vtkIdList *id_list = vtkIdList::New();

  if (ReadMatrix(ElementsFileMatrix,this->m_ElementsFileName))
  {
    albaErrorMacro("Wrong Elements file! File:" << m_ElementsFileName << endl);
	  return -1;
  }

  id_list->SetNumberOfIds(m_NodesPerElement);
    
  grid->Allocate(ElementsFileMatrix.rows(),1);


  // create the connectivity list for each cell from each row
  for (int i = 0; i < ElementsFileMatrix.rows(); i++)
  {
      int id_index = 0;
      for (int j = m_FirstConnectivityColumn; j < ElementsFileMatrix.columns(); j++ )
      {
        /*
        // store info about node_id <-> node_index association 
        // example:

        -------------------------

        NLIST  
        25  x y z => node_id = 25 && n_index = 0;
        27  x y z => node_id = 27 && n_index = 1;
        32  x y z => node_id = 32 && n_index = 2;
        45  x y z ...
                
        ELIST
        245 25 27 32
        248 45 32 27

        -------------------------

        node_id  node_number
        25          0
        27          1
        
        M(i,j) = node_id
        NodeIdNodeNumberMap[M(i, j)] = node_number
        */

        id_list->SetId(id_index, m_NodeIdNodeNumberMap[ElementsFileMatrix(i, j)]);
        int mval = ElementsFileMatrix(i,j);
        int val = m_NodeIdNodeNumberMap[ElementsFileMatrix(i, j)];
        id_index++;
      }
      grid->InsertNextCell(m_VtkCellType, id_list);
  }

  AddIntArrayToUnstructuredGridCellData(grid, ElementsFileMatrix, ansysELEMENTIDColumn, "Id");

  if(m_ReaderMode == WITH_MAT_MODE)
  {
    AddIntArrayToUnstructuredGridCellData(grid, ElementsFileMatrix, ansysMATERIALColumn, "Material",true);
  }

 
  
  vtkDEL(id_list);

  return 0;
}

//----------------------------------------------------------------------------
int albaVMEMeshAnsysTextImporter::ParseMaterialsFile(vtkUnstructuredGrid *grid, const char *matfilename)
{
	if (strcmp(matfilename, "") == 0)
	{
		albaLogMessage("Materials filename not specified!");
		return -1;
	}

	vnl_matrix<double> matMtr;

	if (ReadMatrix(matMtr, matfilename))
	{
		albaErrorMacro("Wrong Materials file!\nCannot read matrix, File:" << matfilename << endl);
		return -1;
	}


	int nCols = matMtr.cols();
	int nRows = matMtr.rows();

	if (nCols != 4)
	{
		albaErrorMacro("Wrong Materials file!\nWrong column number, File:" << matfilename << endl);
		return -1;
	}
	if (nRows < 1)
	{
		albaErrorMacro("Wrong Materials file!\nNo entries found, File:" << matfilename << endl);
		return -1;
	}

	/*

	Example Material text field:

	MAT_N	Ex	NUxy	Dens
	1	4001.62	0.3	0.418279
	2	3903.85	0.3	0.411391
	3	3584.65	0.3	0.388501
	4	3418.35	0.3	0.376311

	*/

	char *array_names[] = { "material_id","EX","NUXY","DENS" };

	vtkFieldData *fdata = vtkFieldData::New();
	

	for (int i = 0; i < 4; i++)
	{
		vtkDoubleArray *darr = vtkDoubleArray::New();
		darr->SetName(array_names[i]);
		darr->SetNumberOfValues(nRows);

		for (int j = 0; j < nRows; j++)
		{
			// fill ith data array with jth value 
			darr->SetTuple1(j, matMtr[j][i]);
		}

		double *range = darr->GetRange();
		
		// add the ith data array to the field data if contains non zero values
		if(range[0]!=0 || range[1]!=0)
			fdata->AddArray(darr);

		//clean up
		darr->Delete();
	}

	grid->SetFieldData(fdata);
	fdata->Delete();
}

//----------------------------------------------------------------------------
int albaVMEMeshAnsysTextImporter::GetElementType()
//----------------------------------------------------------------------------
{
  // read the first line of the connectivity file    
  vcl_ifstream connectivityStream;
  connectivityStream.open(m_ElementsFileName, ios::out);

  vcl_vector<int> connectivityVector;

  if (connectivityStream.is_open())
  {
    char buf[CHAR_BUF_SIZE];
    int tmpInt = -1;
    int numConnectivityPoints = -1;

    // read the first line
    connectivityStream.getline(buf, CHAR_BUF_SIZE, '\n');
    
    // associate an istringstream with full line
    vcl_istringstream connectivityStrStream(buf);

    // fill the connectivityVector
    while (connectivityStrStream >> tmpInt) 
    {
      connectivityVector.push_back(tmpInt);
    }

    numConnectivityPoints = connectivityVector.size() - m_FirstConnectivityColumn;
  
    if (numConnectivityPoints == 4)
    {
      // element is 4 nodes tetra
      m_ElementType = TETRA4; 
	    m_VtkCellType = VTK_TETRA;
      m_NodesPerElement = 4; 
      m_MeshType = LINEAR;
    }
	else if (numConnectivityPoints == 6)
	{
		// element is 4 nodes tetra
		m_ElementType = WEDGE6; 
		m_VtkCellType = VTK_WEDGE;
		m_NodesPerElement = 6; 
		m_MeshType = LINEAR;
	}
    else if (numConnectivityPoints == 8)
    {
      // element is 8 nodes hexa
      m_ElementType = HEXA8;
      m_VtkCellType = VTK_HEXAHEDRON;
      m_NodesPerElement = 8;
      m_MeshType = LINEAR;
    }
    else if (numConnectivityPoints == 10)
    {
      // element is 10 nodes tetra 
      m_ElementType = TETRA10;
      m_VtkCellType = VTK_QUADRATIC_TETRA;
      m_NodesPerElement = 10;
      m_MeshType = PARABOLIC;
    }
    else if (numConnectivityPoints == 20)
    {
      // element is 20 nodes hexa
      m_ElementType = HEXA20;
      m_VtkCellType = VTK_QUADRATIC_HEXAHEDRON;
      m_NodesPerElement = 20;
      m_MeshType = PARABOLIC;
    }
    else
    {
      albaErrorMacro("Element type with " << numConnectivityPoints << " not supported!" << endl);
      m_ElementType = UNSUPPORTED_ELEMENT;	   
      m_VtkCellType = -1;
      m_MeshType = UNKNOWN;
    }

  }
  else
  {
      albaErrorMacro("File: " << m_ElementsFileName << " does not exist" << endl);
      // since i found no valid element i set ElementType to unsupported
      m_ElementType = UNSUPPORTED_ELEMENT;
      m_VtkCellType = -1;
      m_MeshType = UNKNOWN;
	    return -1;
  }

  // close the stream
  connectivityStream.close();
  return m_ElementType;
  
}

//----------------------------------------------------------------------------
int albaVMEMeshAnsysTextImporter::ReadMatrix(vnl_matrix<double> &M, const char *fname)
{
  vcl_ifstream matrix_stream(fname, std::ios::in);

  if(matrix_stream.is_open() != 0)
  {	
    if(M.read_ascii(matrix_stream))
			return 0;
		else
		{
			matrix_stream.clear();
			matrix_stream.seekg(0, ios::beg);
			char buffer[1024];
			matrix_stream.getline(buffer,1024);
			if (M.read_ascii(matrix_stream))
				return 0;
		}
  }

  return 1;
}

//----------------------------------------------------------------------------
void albaVMEMeshAnsysTextImporter::FEMDataToCellData( vtkUnstructuredGrid *input, vtkUnstructuredGrid *output  )
//----------------------------------------------------------------------------
{

  if ( input == NULL || output == NULL )
  {
    return;
  }

  // materials id per cell
  vtkDoubleArray *materialIDArrayFD = NULL;
  int numCells = -1, numMatProp = -1, numMat = -1;
  albaString matStr("Material");
  albaString matIdStr("material_id");
  vtkIntArray *materialArrayCD = NULL;
  vtkFieldData *inFD = NULL;
  vtkCellData *outCD = NULL;

  numCells = input->GetNumberOfCells();
  materialArrayCD = vtkIntArray::SafeDownCast(input->GetCellData()->GetArray(matStr.GetCStr()));

  // check for field data existence
  if (materialArrayCD == NULL)
  {
    albaErrorMacro(<<"material cell data not found!");
    return;
  }

  inFD = input->GetFieldData();

  // check for field data existence
  if (inFD == NULL)
  {
    albaErrorMacro(<<"field data not found!");
    return;
  }

  numMatProp = inFD->GetNumberOfArrays();

  // get the materials id array
  materialIDArrayFD = vtkDoubleArray::SafeDownCast(inFD->GetArray(matIdStr.GetCStr()));

  // check for materials id section existence
  if (materialIDArrayFD == NULL)
  {
    albaErrorMacro(<<"materials_id field data data not found!");
    return;
  }

  int materialsNumber = materialIDArrayFD->GetNumberOfTuples();

  // create a vector for searching material ID:
  vcl_map<int, int> materialIdMaterialColumnMap;
  for (int i = 0; i < materialsNumber; i++)
  {
    int materialID = (int)(materialIDArrayFD->GetValue(i));
    materialIdMaterialColumnMap[materialID] = i;

    #ifdef DEBUG_MODE
    {
      cout << "material id: " << materialID << std::endl;
      cout << "material col: " << materialIdMaterialColumnMap[materialID] << std::endl;
    }
    #endif
  }

  // copying the input in the output
  output->DeepCopy(input);
  outCD = output->GetCellData();

  // for every fields
  for (int fieldNumber = 0; fieldNumber < numMatProp; fieldNumber++)
  {
    if (strcmp(inFD->GetArrayName(fieldNumber), matIdStr.GetCStr())) 
    {
      // source array
      vtkDoubleArray *materialPropertyRow = vtkDoubleArray::SafeDownCast(inFD->GetArray(fieldNumber));

      // create a double array with current field name
      // target_array is the current field array
      vtkDoubleArray *targetArray = vtkDoubleArray::New();
      const char* propName = inFD->GetArrayName(fieldNumber);
      targetArray->SetName(propName); 
      targetArray->SetNumberOfTuples(input->GetNumberOfCells());

      // for each cell
      for (int cellId = 0; cellId < input->GetNumberOfCells(); cellId++)
      {
        // get the current cell material
        int materialId = materialArrayCD->GetValue(cellId);

        int currentMaterialColumn = materialIdMaterialColumnMap[materialId];

        // set the material prop value in target array
        // <TODO!!!!!>  problems here!!!
        // double propValue = sourceArray->GetValue(currentCellMaterialId - firstMaterialId);

        double propertyValue = materialPropertyRow->GetValue(currentMaterialColumn);
        targetArray->SetValue(cellId, propertyValue);

        #ifdef DEBUG_MODE 
        {
          cout << "Prop name: " << propName << std::endl;
          cout << "cellId: " << cellId << std::endl << "propVal: " << propertyValue << std::endl;
          cout << "matId: " << materialId << std::endl;
        }
        #endif
      }

      // add the array to the output cell data 
      outCD->AddArray(targetArray);

      // clean up
      targetArray->Delete();
    }
  }  
}

//----------------------------------------------------------------------------
int albaVMEMeshAnsysTextImporter::GetMeshType()
//----------------------------------------------------------------------------
{
  GetElementType();
  return m_MeshType;

}