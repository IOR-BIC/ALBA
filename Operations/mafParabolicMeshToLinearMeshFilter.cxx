/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafParabolicMeshToLinearMeshFilter.cxx,v $
Language:  C++
Date:      $Date: 2007-05-30 09:33:20 $
Version:   $Revision: 1.2 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafParabolicMeshToLinearMeshFilter.h"

#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkIdList.h"
#include "vtkIntArray.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkUnstructuredGrid.h"

#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_map.h>
#include <vcl_list.h>
#include <vnl\vnl_matrix.h>

#include "mafString.h"

vtkCxxRevisionMacro(mafParabolicMeshToLinearMeshFilter, "$Revision: 1.2 $");
vtkStandardNewMacro(mafParabolicMeshToLinearMeshFilter);

mafParabolicMeshToLinearMeshFilter::mafParabolicMeshToLinearMeshFilter()
{
  
}

mafParabolicMeshToLinearMeshFilter::~mafParabolicMeshToLinearMeshFilter()
{

}

void mafParabolicMeshToLinearMeshFilter::Execute()
{
  
  int nCells;
  int numPointsNew;
  int oldCellType, newCellType;
  int numPointsPerCellNew;
  
  // 
  vtkUnstructuredGrid *input = this->GetInput();
  vtkUnstructuredGrid *output = this->GetOutput();

  vcl_list<int> connectivityVectorWithRepeatedValues;
  vcl_list<int> connectivityVectorWithUniqueValues;

  nCells = input->GetNumberOfCells();
  
  // since the mesh is not hybrid I can ask for the first element to determine the element type
  oldCellType = input->GetCell(0)->GetCellType();

  switch(oldCellType) 
  {
    case (24):
    {
      // quadratic tetra
      numPointsPerCellNew = 4;
      newCellType = 10;
    }
    break;
    case (25):
    {
      // quadratic hexaedron
      numPointsPerCellNew = 8;
      newCellType = 12;
    }
    break;
    {

    }
    default:
    {
      output->DeepCopy(input);

      mafLogMessage("Mesh is already linear or made of unsupported type elements!\
      Bypassing the filter");
      return;
    }
  }

  vnl_matrix<double> connectivityMatrix(nCells, numPointsPerCellNew);
  
  // load the geometry
      
  // load the connectivity 
  for (int i = 0; i < nCells; i++)
  {
    for (int j = 0; j < numPointsPerCellNew; j++)
    {     
      // create vector with id from submatrix
      // 0 1 2 3 3 1 2 10
      connectivityVectorWithRepeatedValues.push_back(input->GetCell(i)->GetPointId(j));
    }
  }

  // order ids
  // 0 1 1 2 3 3 10
  connectivityVectorWithRepeatedValues.sort();

  // eliminate repeating ids
  // 0 1 2 3 10 => linear mesh points
  std::unique_copy(connectivityVectorWithRepeatedValues.begin(), connectivityVectorWithRepeatedValues.end(),  std::back_inserter(connectivityVectorWithUniqueValues));

  numPointsNew = connectivityVectorWithUniqueValues.size();

  /*
    map(10) = 4;
  */

  //-----------------------
  // create output geometry
  //-----------------------

  // create new geometry from connectivityVectorWithUniqueValues
  vtkPoints *points = vtkPoints::New();
  

  typedef vcl_list<int>::iterator listIter;
  listIter iter;
  int pointId = 0;

  for (iter = connectivityVectorWithUniqueValues.begin(); iter != connectivityVectorWithUniqueValues.end(); iter++)    
  {
    double point[3];
    input->GetPoints()->GetPoint((*iter), point);
    points->InsertNextPoint(point[0], point[1], point[2]);
    pointId++;
  }

  // fill the POINT section of the Mesh
  output->SetPoints(points);
  points->Delete();

  //-----------------------
  // create output topology
  //-----------------------

  int numCells = input->GetNumberOfCells(); 

  // create new topology
  output->Allocate(numCells, 1);

  vcl_map<int, int> oldIdToNewIdMap;
  
  int newPointId = 0;

  // map with (key = oldPointId, value = newPointId)
  for (iter = connectivityVectorWithUniqueValues.begin(); iter != connectivityVectorWithUniqueValues.end(); iter++)    
  {
    oldIdToNewIdMap.insert(vcl_map<int,int>::value_type(*iter, newPointId));
    newPointId++;
  }

  vtkIdList *newIdList = vtkIdList::New();
  
  // create the connectivity list for each cell
  for (int cellId = 0; cellId < numCells; cellId++)
  {   
      newIdList->SetNumberOfIds(numPointsPerCellNew);

      // get the ith cell from input mesh
      vtkCell *currentCell = input->GetCell(cellId);
         
      // get the old id list
      vtkIdList *oldIdList = currentCell->GetPointIds();

      for (int idListNewIndex = 0; idListNewIndex < numPointsPerCellNew; idListNewIndex++)
      {
        int oldId = oldIdList->GetId(idListNewIndex);

        newIdList->SetId(idListNewIndex, oldIdToNewIdMap[oldId]);//map id old to id new)
      }

      output->InsertNextCell(newCellType, newIdList);    
  }

  newIdList->Delete();

  // data arrays
  // cell data is ok since cell number is not changed
  vtkCellData *inCD = input->GetCellData();
  vtkCellData *outCD = output->GetCellData();
  outCD->DeepCopy(inCD);
  
    
  // point data:
    vtkPointData *inPD = input->GetPointData();
    vtkPointData *outPD = output->GetPointData();
   
    vtkDataArray *activeArray = input->GetPointData()->GetScalars();
    

    for (int idArray = 0; idArray < inPD->GetNumberOfArrays(); idArray++)
    {
      int numPoint = 0;
 
      vtkDataArray *currentArray = inPD->GetArray(idArray);
      
      vtkDataArray *newArray = currentArray->NewInstance();
      
      newArray->SetName(currentArray->GetName());
      newArray->SetNumberOfComponents(currentArray->GetNumberOfComponents());
      newArray->SetNumberOfTuples(numPointsNew);      

      for (iter = connectivityVectorWithUniqueValues.begin(); iter != connectivityVectorWithUniqueValues.end(); iter++)    
      {
        newArray->SetTuple(numPoint, currentArray->GetTuple(*iter));
        numPoint++;
      }
  
      outPD->AddArray(newArray);

      //modified by Daniele
      if(strcmp(activeArray->GetName(), newArray->GetName()) == 0) outPD->SetScalars(newArray);

      newArray->Delete();
    }
 
    // pass field data to output; this contains material information
    // and does not need to be changed.
    vtkFieldData *inFD = input->GetFieldData();
    output->GetFieldData()->DeepCopy(inFD);
  
    output->Squeeze();  
}

void mafParabolicMeshToLinearMeshFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  //os << indent << "PrintMe! : " << this->Ivar2Print << "\n";
}
