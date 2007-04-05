/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEMeshAnsysTextImporter.cpp,v $
Language:  C++
Date:      $Date: 2007-04-05 08:23:08 $
Version:   $Revision: 1.2 $
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

// TO BE PORTED
// #include "mafVMEMeshDataToCellData.h"
// #include "mafVMEMeshParabolicMeshToLinearMesh.h"

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
  NodesFileName = "";
  ElementsFileName = "";
  MaterialsFileName = "";
  ElementType = UNSUPPORTED_ELEMENT;
  NodesPerElement = -1;
  MeshType = UNKNOWN;
  
  Output = NULL;
  mafNEW(Output);

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");
  Output->GetTagArray()->SetTag(tag_Nature);
  Output->SetName("imported mesh");

}
//----------------------------------------------------------------------------
mafVMEMeshAnsysTextImporter::~mafVMEMeshAnsysTextImporter()
{
  mafDEL(Output);
}

//----------------------------------------------------------------------------
int mafVMEMeshAnsysTextImporter::Read()
{	
  vtkMAFSmartPointer<vtkUnstructuredGrid> grid;
  grid->Initialize();

  if (this->ParseNodesFile(grid) == -1) return MAF_ERROR ; 
  if (this->ParseElementsFile(grid) == -1) return MAF_ERROR;
  
  int ret = this->ParseMaterialsFile(grid);
  
  // TO BE PORTED
  // mafVMEMeshDataToCellData *f2Cell = mafVMEMeshDataToCellData::New();
  // mafVMEMeshParabolicMeshToLinearMesh *p2l = mafVMEMeshParabolicMeshToLinearMesh::New();

  // material file is present?

  vtkUnstructuredGrid *gridToLinearize = NULL;

  // NOT HANDLING MATERIALS FILE...
  //if (ret == 0)
  //{
  //  mafErrorMacro("Materials file found; building mesh attribute data from materials info...")
  //  // convert field data from materials to cell data   
  //  f2Cell->SetInput(grid);
  //  f2Cell->Update();
  //  gridToLinearize = f2Cell->GetOutput();
  //}
  //else
  //{
    mafErrorMacro("Materials file not yet supported; not building attribute data from materials info...")
    gridToLinearize = grid;
  
    // }
  
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

  //Importer->SetModeToMove();
  //Importer->SetVMEName("mesh");
  //Importer->Import();

  //this->Output = Importer->GetOutput();
  
  // p2l->Delete();
  // f2Cell->Delete();
  
  Output->SetDataByDetaching(gridToLinearize,0);

  return MAF_OK;
}

int mafVMEMeshAnsysTextImporter::ParseNodesFile(vtkUnstructuredGrid *grid)
{
  int node_id_col = 0;
  int x_col = 1;
  int y_col = x_col + 1;
  int z_col = x_col + 2;

  
  vnl_matrix<double> M;

  if (ReadMatrix(M,this->NodesFileName))
  {
    mafErrorMacro("Node files not found! File:" << NodesFileName << " does not exist!" << endl);
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
    NodeIdNodeNumberMap[M(i, node_id_col)] = i;  
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

  if (ReadMatrix(M,this->ElementsFileName))
  {
    mafErrorMacro("Elements file not found! File:" << ElementsFileName << "does not exist!" << endl);
	  return -1;
  }

  id_list->SetNumberOfIds(NodesPerElement);
    
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

        id_list->SetId(id_index, NodeIdNodeNumberMap[M(i, j)]);
        int mval = M(i,j);
        int val = NodeIdNodeNumberMap[M(i, j)];
        id_index++;
      }
      grid->InsertNextCell(VtkCellType, id_list);
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
    mafErrorMacro("Materials filename not specified!" << endl);
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

  for (int i = 0; i < num_prop; i++)
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
  connectivityStream.open(ElementsFileName, ios::out);

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
      ElementType = TETRA4; 
      VtkCellType = 10;
      NodesPerElement = 4; 
      MeshType = LINEAR;
    }
    else if (numConnectivityPoints == 8)
    {
      // element is 8 nodes hexa
      ElementType = HEXA8;
      VtkCellType = 12;
      NodesPerElement = 8;
      MeshType = LINEAR;
    }
    else if (numConnectivityPoints == 10)
    {
      // element is 10 nodes tetra 
      ElementType = TETRA10;
      VtkCellType = 24;
      NodesPerElement = 10;
      MeshType = PARABOLIC;
    }
    else if (numConnectivityPoints == 20)
    {
      // element is 20 nodes hexa
      ElementType = HEXA20;
      VtkCellType = 25;
      NodesPerElement = 20;
      MeshType = PARABOLIC;
    }
    else
    {
      mafErrorMacro("Element type with " << numConnectivityPoints << " not supported!" << endl);
      ElementType = UNSUPPORTED_ELEMENT;	   
      VtkCellType = -1;
      MeshType = UNKNOWN;
    }

  }
  else
  {
      mafErrorMacro("File: " << ElementsFileName << " does not exist" << endl);
      // since i found no valid element i set ElementType to unsupported
      ElementType = UNSUPPORTED_ELEMENT;
      VtkCellType = -1;
      MeshType = UNKNOWN;
	    return -1;
  }

  // close the stream
  connectivityStream.close();
  return ElementType;
  
}

int mafVMEMeshAnsysTextImporter::ReadMatrix(vnl_matrix<double> &M, const char *fname)
{
  //Read raw motion data
  vcl_ifstream v_raw_matrix(fname, std::ios::in);


  if(v_raw_matrix.is_open() != 0)
  {	
    M.read_ascii(v_raw_matrix);
    return 0;
  }

  return 1;
}

