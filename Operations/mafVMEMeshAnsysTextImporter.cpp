/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEMeshAnsysTextImporter.cpp,v $
Language:  C++
Date:      $Date: 2007-05-17 08:31:02 $
Version:   $Revision: 1.7 $
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
//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------

const int CHAR_BUF_SIZE = 1000;
const int FIRST_CONNECTIVITY_COLUMN = 6;

mafVMEMeshAnsysTextImporter::mafVMEMeshAnsysTextImporter()
{
  m_NodesFileName = "";
  m_ElementsFileName = "";
  m_MaterialsFileName = "";
  m_ElementType = UNSUPPORTED_ELEMENT;
  m_NodesPerElement = -1;
  m_MeshType = UNKNOWN;
  
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

int mafVMEMeshAnsysTextImporter::ParseNodesFile(vtkUnstructuredGrid *grid)
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
    points->SetPoint(i, M(i, x_col), M(i, y_col), M(i, z_col));
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
  node_id_array->SetName("id");
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

int mafVMEMeshAnsysTextImporter::ParseElementsFile(vtkUnstructuredGrid *grid)
{
  if (strcmp(m_ElementsFileName, "") == 0)
  {
    mafLogMessage("Elements filename not specified!");
    return -1;
  }

  int cell_id_col = 0;
  int materials_col = 1;
  int FIRST_CONNECTIVITY_COLUMN = 6;

  // name of the materials array
  mafString mat_string("material");
  
  vnl_matrix<double> M;
  
  int ret = GetElementType();
  if (ret == -1 || ret == UNSUPPORTED_ELEMENT )
  {
    return -1;
  }

  // id list for connectivity
  vtkIdList *id_list = vtkIdList::New();

  if (ReadMatrix(M,this->m_ElementsFileName))
  {
    mafErrorMacro("Elements file not found! File:" << m_ElementsFileName << "does not exist!" << endl);
	  return -1;
  }

  id_list->SetNumberOfIds(m_NodesPerElement);
    
  grid->Allocate(M.rows(),1);


  // create the connectivity list for each cell from each row
  for (int i = 0; i < M.rows(); i++)
  {
      int id_index = 0;
      for (int j = FIRST_CONNECTIVITY_COLUMN; j < M.columns(); j++ )
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

        id_list->SetId(id_index, m_NodeIdNodeNumberMap[M(i, j)]);
        int mval = M(i,j);
        int val = m_NodeIdNodeNumberMap[M(i, j)];
        id_index++;
      }
      grid->InsertNextCell(m_VtkCellType, id_list);
  }

  // store info about cell_id <-> material_id association 
  vtkIntArray *mat_array = vtkIntArray::New();
  mat_array->SetName(mat_string.GetCStr());
  mat_array->SetNumberOfTuples(M.rows());

  for (int i = 0; i < M.rows(); i++)
  {
    // fill the MaterialsArray
    mat_array->SetValue(i, M(i, materials_col));
  }

  // create the materials section 
  grid->GetCellData()->SetScalars(mat_array);
  
  vtkDEL(mat_array);  
  vtkDEL(id_list);

  return 0;
}

//----------------------------------------------------------------------------
int mafVMEMeshAnsysTextImporter::ParseMaterialsFile(vtkUnstructuredGrid *grid, const char *matfilename)
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
// used by loops;
int i = 0;

vcl_string tmpstr;
// material prop numerical value
double value;
// first read values holder
vcl_vector<double> tmp_vec;

// holds material properties name; first element is "material_id"
vcl_vector<vcl_string> mat_name_vec;
mat_name_vec.push_back("material_id");

// number of material properties
int num_prop = 0;
// number of materials
int num_mat = 0;

// materials properties vector [num_prop x num_mat];
typedef vcl_vector<double> row_vector;
vcl_vector<row_vector> mat_prop_vec;

/*

  mat_prop_vec[0] .... mat_prop_vec[n-1]
      
        matID_0               EX_0
        matID_1               EX_1
         ...                 ...
        matID_n-1             EX_n-1

*/

input.open(matfilename, ios::out);

if (input.is_open())
{
  
  // skip first line characters until "=" is encountered
  input.ignore(INT_MAX, '=');
  // save the material id
  int material_id;
  input >> material_id;
  tmp_vec.push_back(material_id);
  // skip the rest of the line
  input.ignore(INT_MAX, '\n');

  // the first string encountered should be the first material prop name...
  // read until an empty line is found

  //tmpstr should be EX the first step
  while (input >> tmpstr)
  {
    // if we found the second material break...
    if (tmpstr == "MATERIAL")
    {
      break;
    }
    // while tmpstr is not "MATERIAL
    const char *name = tmpstr.c_str();
    // prop names in mat_name_vec;
    mat_name_vec.push_back(tmpstr);
  
    //         =       value
    input >> tmpstr >> value;
    // keeping track of numerical value
    tmp_vec.push_back(value);

    // skip tokens after first string until the first newline
    input.ignore(INT_MAX, '\n');
  }

  num_prop = mat_name_vec.size();

  int i;
  for (i = 0; i < num_prop; i++)
  {
  vcl_vector<double> tmp;
  tmp.push_back(tmp_vec[i]);
  mat_prop_vec.push_back(tmp);
  }

  // this should be field 2 of materials file
  while (input >> tmpstr)
  {
  
    // skip first line characters until "=" is encountered
    input.ignore(INT_MAX, '=');
    // save the material id
    int material_id;
    input >> material_id;
    mat_prop_vec[0].push_back(material_id);
    // skip the rest of the line
    input.ignore(INT_MAX, '\n');
  
    for (i = 1; i < num_prop; i++)
    {
      //         DENS       =        5
      input >> tmpstr >> tmpstr >> value;
      mat_prop_vec[i].push_back(value);
      input.ignore(INT_MAX, '\n');
    }

    // next line should be the materials line, we ignore it;
    input.ignore(INT_MAX, '\n');
  }

  // all the materials vector should have the same length
  // equals to the number of materials
  num_mat  = mat_prop_vec[0].size();

  vtkFieldData *fdata = vtkFieldData::New();
  
  // TO BE PORTED... THIS SHOULD BE NOT NEEDED BY vtk 4.4
  //fdata->SetNumberOfArrays(num_prop);

  // create field data data array
  for (i = 0; i < num_prop; i++)
  {
    // create the ith data array
    vtkDoubleArray *darr = vtkDoubleArray::New();
    darr->SetName(mat_name_vec[i].c_str());
    darr->SetNumberOfValues(num_mat);
    
    for (int j = 0; j < num_mat; j++)
    {
      // fill ith data array with jth value 
      darr->InsertValue(j, mat_prop_vec[i][j]);
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

    numConnectivityPoints = connectivityVector.size() - FIRST_CONNECTIVITY_COLUMN;
  
    if (numConnectivityPoints == 4)
    {
      // element is 4 nodes tetra
      m_ElementType = TETRA4; 
      m_VtkCellType = 10;
      m_NodesPerElement = 4; 
      m_MeshType = LINEAR;
    }
    else if (numConnectivityPoints == 8)
    {
      // element is 8 nodes hexa
      m_ElementType = HEXA8;
      m_VtkCellType = 12;
      m_NodesPerElement = 8;
      m_MeshType = LINEAR;
    }
    else if (numConnectivityPoints == 10)
    {
      // element is 10 nodes tetra 
      m_ElementType = TETRA10;
      m_VtkCellType = 24;
      m_NodesPerElement = 10;
      m_MeshType = PARABOLIC;
    }
    else if (numConnectivityPoints == 20)
    {
      // element is 20 nodes hexa
      m_ElementType = HEXA20;
      m_VtkCellType = 25;
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

int mafVMEMeshAnsysTextImporter::ReadMatrix(vnl_matrix<double> &M, const char *fname)
{
  vcl_ifstream v_raw_matrix(fname, std::ios::in);


  if(v_raw_matrix.is_open() != 0)
  {	
    M.read_ascii(v_raw_matrix);
    return 0;
  }

  return 1;
}

void mafVMEMeshAnsysTextImporter::FEMDataToCellData( vtkUnstructuredGrid *input, vtkUnstructuredGrid *output  )
{

  if ( input == NULL || output == NULL )
  {
    return;
  }

  // materials id per cell
  vtkDoubleArray *materialIDArrayFD = NULL;
  int numCells = -1, numMatProp = -1, numMat = -1;
  mafString matStr("material");
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
  int firstMaterialId = (int) materialIDArrayFD->GetValue(0);

  // check for materials id section existence
  if (materialIDArrayFD == NULL)
  {
    mafErrorMacro(<<"materials_id field data data not found!");
    return;
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
      vtkDoubleArray *sourceArray = vtkDoubleArray::SafeDownCast(inFD->GetArray(fieldNumber));

      // create a double array with current field name
      // target_array is the current field array
      vtkDoubleArray *targetArray = vtkDoubleArray::New();
      targetArray->SetName(inFD->GetArrayName(fieldNumber)); 
      targetArray->SetNumberOfTuples(input->GetNumberOfCells());

      // for each cell
      for (int cellId = 0; cellId < input->GetNumberOfCells(); cellId++)
      {
        // get the current cell material
        int currentCellMaterialId = materialArrayCD->GetValue(cellId);
        // set the material prop value in target array
        targetArray->SetValue(cellId, sourceArray->GetValue(currentCellMaterialId - firstMaterialId));
      }

      // add the array to the output cell data 
      outCD->AddArray(targetArray);

      // clean up
      targetArray->Delete();
    }
  }  
}

int mafVMEMeshAnsysTextImporter::GetMeshType()
{
	GetElementType();
  return m_MeshType;

}