/*=========================================================================

 Program: MAF2
 Module: mafVMEMeshAnsysTextImporter
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// debug facility
// #define DEBUG_MODE

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEMeshAnsysTextImporter.h"

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
#include "mafGui.h"
#include "mafGUIDialog.h"


// vcl includes
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>


#define MIN( x, y ) ( (x) < (y) ? (x) : (y) )
#define MAX( x, y ) ( (x) > (y) ? (x) : (y) )

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
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------

const int CHAR_BUF_SIZE = 1000;

mafVMEMeshAnsysTextImporter::mafVMEMeshAnsysTextImporter()
{
  m_NodesFileName = "";
  m_ElementsFileName = "";
  m_MaterialsFileName = "";
  m_ElementType = UNSUPPORTED_ELEMENT;
  m_NodesPerElement = -1;
  m_MeshType = UNKNOWN;
  m_ReaderMode = ANSYS_MODE;

  m_Output = NULL;
}
//----------------------------------------------------------------------------
mafVMEMeshAnsysTextImporter::~mafVMEMeshAnsysTextImporter()
{
  mafDEL(m_Output);
}

//----------------------------------------------------------------------------
int mafVMEMeshAnsysTextImporter::Read()
{	
  vtkMAFSmartPointer<vtkUnstructuredGrid> grid;
  grid->Initialize();

  if (this->ParseNodesFile(grid) == -1) return MAF_ERROR ; 
  if (this->ParseElementsFile(grid) == -1) return MAF_ERROR;

  int ret = this->ParseMaterialsFile(grid, m_MaterialsFileName);

  // allocate the output if not yet allocated...
  if (m_Output == NULL)
  {
    mafNEW(m_Output);

    mafTagItem tag_Nature;
    tag_Nature.SetName("VME_NATURE");
    tag_Nature.SetValue("NATURAL");
    m_Output->GetTagArray()->SetTag(tag_Nature);
    m_Output->SetName("imported mesh");

  }

  // TO BE PORTED
  // mafVMEMeshParabolicMeshToLinearMesh *p2l = mafVMEMeshParabolicMeshToLinearMesh::New();

  // material file is present?

  vtkMAFSmartPointer<vtkUnstructuredGrid> gridToLinearize;
  
  if (ret == 0)
  {
    mafLogMessage("Materials file found; building mesh attribute data from materials info...");

    // convert field data from materials to cell data       
    FEMDataToCellData(grid.GetPointer(), gridToLinearize.GetPointer());
  }
  else
  {
    mafLogMessage("Materials file not found! Not building attribute data from materials info...");
    gridToLinearize = grid;
  }
  
  // NOT HANDLING LINEARIZATION...
  //// do we have to linearize?

  //if (MeshType == PARABOLIC)
  //{
  //  // linearize input mesh 
  //  
  //  p2l->SetInput(gridToLinearize);
  //  
  //  // Importer->SetInput(p2l->GetOutput());
  //}
  //else
  //{
  //  // Importer->SetInput(gridToLinearize);
  //}

  
  m_Output->SetDataByDetaching(gridToLinearize,0);

  return MAF_OK;
}

//----------------------------------------------------------------------------
int mafVMEMeshAnsysTextImporter::ParseNodesFile(vtkUnstructuredGrid *grid)
//----------------------------------------------------------------------------
{
  if (strcmp(m_NodesFileName, "") == 0)
  {
    mafLogMessage("Nodes filename not specified!");
    return -1;
  }

  int node_id_col = 0;
  int x_col = 1;
  int y_col = x_col + 1;
  int z_col = x_col + 2;

  
  vnl_matrix<double> M;

  if (ReadMatrix(M,this->m_NodesFileName))
  {
    mafErrorMacro("Node files not found! File:" << m_NodesFileName << " does not exist!" << endl);
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

void mafVMEMeshAnsysTextImporter::AddIntArrayToUnstructuredGridCellData( vtkUnstructuredGrid *grid, vnl_matrix<double> &elementsFileMatrix, int column, mafString outputArrayName , bool activeScalar)
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
int mafVMEMeshAnsysTextImporter::ParseElementsFile(vtkUnstructuredGrid *grid)
//----------------------------------------------------------------------------
{
  if (strcmp(m_ElementsFileName, "") == 0)
  {
    mafLogMessage("Elements filename not specified!");
    return -1;
  }
  
  vnl_matrix<double> ElementsFileMatrix;


  int cell_id_col = 0;

  //  column:          1          2         2
  //  from ANSYS file: TYPE, 2  $ MAT, 2  $ REAL, 3

  // elId  mat  type real ?   ?   pointId...
  // 37411 440  50   2    0   1   119324    6996    6994    4809   70910   70925   70920   84679 84682   84683

  int ansysELEMENTIDColumn = 0; 
  int ansysMATERIALColumn = 1; 
  int ansysTYPEColumn = 2;
  int ansysREALColumn = 3;

  m_FirstConnectivityColumn = 6;

  if(m_ReaderMode == GENERIC_MODE)
  {
    // elId  pointId...
    // 37411 119324    6996    6994    4809   70910   70925   70920   84679 84682   84683
    m_FirstConnectivityColumn = 1;
  }

  int ret = GetElementType();
  if (ret == -1 || ret == UNSUPPORTED_ELEMENT )  
  {
    return -1;
  }

  // id list for connectivity
  vtkIdList *id_list = vtkIdList::New();

  if (ReadMatrix(ElementsFileMatrix,this->m_ElementsFileName))
  {
    mafErrorMacro("Elements file not found! File:" << m_ElementsFileName << "does not exist!" << endl);
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

  if(m_ReaderMode == ANSYS_MODE)
  {
    AddIntArrayToUnstructuredGridCellData(grid, ElementsFileMatrix, ansysTYPEColumn, "Type");
    AddIntArrayToUnstructuredGridCellData(grid, ElementsFileMatrix, ansysMATERIALColumn, "Material",true);
    AddIntArrayToUnstructuredGridCellData(grid, ElementsFileMatrix, ansysREALColumn, "Real"); 
  }

 
  
  vtkDEL(id_list);

  return 0;
}

//----------------------------------------------------------------------------
int mafVMEMeshAnsysTextImporter::ParseMaterialsFile(vtkUnstructuredGrid *grid, const char *matfilename)
//----------------------------------------------------------------------------
{
  if (strcmp(matfilename, "") == 0)
  {
    mafLogMessage("Materials filename not specified!");
    return -1;
  }

	// read materials from file
	vcl_ifstream input;

	/*

	 Example Material text field

	 MATERIAL NUMBER =      1 EVALUATED AT TEMPERATURE OF   0.0000    
	 EX   =   18304.    
	 DENS =   1.2747    
	 PRXY =  0.30000  

	 material: (index, map(string, double))

	*/

	vcl_string startStr;
	vcl_string tmpStr;

	// material prop numerical value
	double value;

	// materials properties vector [loc_num_prop x num_mat];
	typedef vcl_vector<double> row_vector;
	vcl_vector<row_vector> mat_prop_values_vec;
	vcl_vector<row_vector> mat_prop_ids_vec;
	row_vector mat_prop_values_min_vec;
	row_vector mat_prop_values_max_vec;


	// holds material properties name; first element is "material_id"
	vcl_vector<vcl_string> mat_prop_name_vec;
	mat_prop_name_vec.push_back("material_id");
	//id props does not need min max, pushing back for other props align
	mat_prop_values_min_vec.push_back(0);
	mat_prop_values_max_vec.push_back(0);


	// number of material properties
	int num_prop = 1;
	// number of materials
	int num_mat = 0;
	

	/*
		mat_name_vec: ["material_id", "EX", "NUX", "DENS"]
		mat_prop_values_min_vec[min(matID), min(EX), min(NUX), min(DENS)]
		mat_prop_values_max_vec[max(matID), max(EX), max(NUX), max(DENS)]

		mat_prop_value_vec[0] .... mat_prop_value_vec[n-1]
					matID												matID
					EX													EX
					NUX													DENS

		mat_prop_value_vec[0] .... mat_prop_value_vec[n-1]
					0														0
					1														1
					2														3
	*/

	input.open(matfilename, ios::out);

	if (input.is_open())
	{
		int propPos;

		//reading materials
		while (input >> startStr)
		{
			if (startStr == "MATERIAL")
			{
				//found new material
				int material_id;

				// skip first line characters until "=" is encountered
				input.ignore(INT_MAX, '=');
				input >> material_id;
				row_vector matValueVector;
				row_vector matPropIdVector;
				matValueVector.push_back(material_id);
				matPropIdVector.push_back(0);
				mat_prop_values_vec.push_back(matValueVector);
				mat_prop_ids_vec.push_back(matPropIdVector);
				num_mat++;
			}
			else 
			{
				//         =        5
				input >> tmpStr >> value;

				//Search Property name
				for(propPos=num_prop-1;propPos>=0;propPos--)
				{
					if (startStr == mat_prop_name_vec[propPos])
						break;
				}

				//Property name not found add new prop
				if (propPos < 0)
				{
					mat_prop_name_vec.push_back(startStr);
					mat_prop_values_min_vec.push_back(value);
					mat_prop_values_max_vec.push_back(value);
					num_prop++;
					propPos=num_prop-1;
				}

				mat_prop_values_min_vec[propPos]=MIN(value,mat_prop_values_min_vec[propPos]);
				mat_prop_values_max_vec[propPos]=MAX(value,mat_prop_values_max_vec[propPos]);
				mat_prop_values_vec[num_mat-1].push_back(value);
				mat_prop_ids_vec[num_mat-1].push_back(propPos);				
			}

			// next line should be the materials line, we ignore it;
			input.ignore(INT_MAX, '\n');
		}
				
		vtkFieldData *fdata = vtkFieldData::New();
  		
		// create field data data array
		for (int propIndex = 0; propIndex < num_prop; propIndex++)
		{
			int assignToAll=false;
			double assignValue=0;


			// create the ith data array
			vtkDoubleArray *darr = vtkDoubleArray::New();
			darr->SetName(mat_prop_name_vec[propIndex].c_str());
			darr->SetNumberOfValues(num_mat);
    
			for (int j = 0; j < num_mat; j++)
			{
				//Search Property id
				for(propPos=mat_prop_ids_vec[j].size()-1;propPos>=0;propPos--)
				{
					if (mat_prop_ids_vec[j][propPos]==propIndex)
					{
						//assign prop value
						value=mat_prop_values_vec[j][propPos];
						break;
					}
				}

				//Property id not found assign default value ask value to the user
				if (propPos < 0)
				{
					//If user does not have already select "Apply to all" for this property we ask him the value for current material
					if (!assignToAll)
					{
						//setting default value to the min of the range
						assignValue=MIN(value,mat_prop_values_min_vec[propIndex]);
						
						//Create strings for GUI
						mafString matLabel;
						mafString rangeLabel;
						mafString applyLabel;
						matLabel.Printf(" Material N. %d has no property %s defined", (int)mat_prop_values_vec[j][0], mat_prop_name_vec[propIndex].c_str());
						rangeLabel.Printf(" %s current Range is (%.4f,%.4f)",mat_prop_name_vec[propIndex].c_str(), mat_prop_values_min_vec[propIndex], mat_prop_values_max_vec[propIndex]);
						applyLabel.Printf("Apply to all %s",mat_prop_name_vec[propIndex].c_str());
															
						//Create GUI
						mafGUI *dialogGui;
						dialogGui = new mafGUI(NULL);
						dialogGui->Label(matLabel,"",true);
						dialogGui->Label("");
						dialogGui->Label(rangeLabel,"");
						dialogGui->Label("");
						dialogGui->Label("Please Insert property value");
						dialogGui->Double(-1,_("Value: "), &assignValue);
						dialogGui->Bool(-1,applyLabel, &assignToAll,true);
						dialogGui->FitGui();

						//Create and show dialog
						mafGUIDialog *dialog;
						dialog = new mafGUIDialog("Unkonwn Property Value", mafRESIZABLE | mafOK);
						dialog->Add(dialogGui);
						dialog->SetMinSize(wxSize(320,100));
						dialog->Fit();
						dialog->ShowModal();

						//Updating current property range
						mat_prop_values_min_vec[propIndex]=MIN(assignValue,mat_prop_values_min_vec[propIndex]);
						mat_prop_values_max_vec[propIndex]=MAX(assignValue,mat_prop_values_max_vec[propIndex]);
					}
					//updating property value
					value=assignValue;
				}

				// fill ith data array with jth value 
				darr->InsertValue(j, value);
			}
			// add the ith data array to the field data
			fdata->AddArray(darr);

			//clean up
			darr->Delete();
		}

		grid->SetFieldData(fdata);
		fdata->Delete();

		return 0;
	}
	else
	{
		 mafErrorMacro("File:" << matfilename << "does not exist" << endl << "Not building attribute data from materials id." << endl);
		 return -1;
	}

}

//----------------------------------------------------------------------------
int mafVMEMeshAnsysTextImporter::GetElementType()
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
      mafErrorMacro("Element type with " << numConnectivityPoints << " not supported!" << endl);
      m_ElementType = UNSUPPORTED_ELEMENT;	   
      m_VtkCellType = -1;
      m_MeshType = UNKNOWN;
    }

  }
  else
  {
      mafErrorMacro("File: " << m_ElementsFileName << " does not exist" << endl);
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
int mafVMEMeshAnsysTextImporter::ReadMatrix(vnl_matrix<double> &M, const char *fname)
//----------------------------------------------------------------------------
{
  vcl_ifstream v_raw_matrix(fname, std::ios::in);


  if(v_raw_matrix.is_open() != 0)
  {	
    M.read_ascii(v_raw_matrix);
    return 0;
  }

  return 1;
}

//----------------------------------------------------------------------------
void mafVMEMeshAnsysTextImporter::FEMDataToCellData( vtkUnstructuredGrid *input, vtkUnstructuredGrid *output  )
//----------------------------------------------------------------------------
{

  if ( input == NULL || output == NULL )
  {
    return;
  }

  // materials id per cell
  vtkDoubleArray *materialIDArrayFD = NULL;
  int numCells = -1, numMatProp = -1, numMat = -1;
  mafString matStr("Material");
  mafString matIdStr("material_id");
  vtkIntArray *materialArrayCD = NULL;
  vtkFieldData *inFD = NULL;
  vtkCellData *outCD = NULL;

  numCells = input->GetNumberOfCells();
  materialArrayCD = vtkIntArray::SafeDownCast(input->GetCellData()->GetArray(matStr.GetCStr()));

  // check for field data existence
  if (materialArrayCD == NULL)
  {
    mafErrorMacro(<<"material cell data not found!");
    return;
  }

  inFD = input->GetFieldData();

  // check for field data existence
  if (inFD == NULL)
  {
    mafErrorMacro(<<"field data not found!");
    return;
  }

  numMatProp = inFD->GetNumberOfArrays();

  // get the materials id array
  materialIDArrayFD = vtkDoubleArray::SafeDownCast(inFD->GetArray(matIdStr.GetCStr()));

  // check for materials id section existence
  if (materialIDArrayFD == NULL)
  {
    mafErrorMacro(<<"materials_id field data data not found!");
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
int mafVMEMeshAnsysTextImporter::GetMeshType()
//----------------------------------------------------------------------------
{
  GetElementType();
  return m_MeshType;

}