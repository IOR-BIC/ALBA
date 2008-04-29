/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafParabolicMeshToLinearMeshFilter.cxx,v $
Language:  C++
Date:      $Date: 2008-04-29 10:56:01 $
Version:   $Revision: 1.4 $
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
#include <vnl/vnl_matrix.h>

#include "mafString.h"

vtkCxxRevisionMacro(mafParabolicMeshToLinearMeshFilter, "$Revision: 1.4 $");
vtkStandardNewMacro(mafParabolicMeshToLinearMeshFilter);

mafParabolicMeshToLinearMeshFilter::mafParabolicMeshToLinearMeshFilter()
{
  
}

mafParabolicMeshToLinearMeshFilter::~mafParabolicMeshToLinearMeshFilter()
{

}

//#define __PROFILING__
//#include <atlbase.h>
//#include "G:/Programs/Libraries/BSGenLib/BSGenLib/Include/BSGenLib.h"

void mafParabolicMeshToLinearMeshFilter::Execute()
{
//  PROFILE_THIS_FUNCTION();

  int nCells;
  int numPointsNew;
  int oldCellType, newCellType;
  int numPointsPerCellNew;
  
  // 
  vtkUnstructuredGrid *input = this->GetInput();
  vtkUnstructuredGrid *output = this->GetOutput();

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

      mafLogMessage("Mesh is already linear or made of unsupported type elements! Bypassing the filter");
      return;
    }
  }  

  //-----------------------
  // create output geometry
  //-----------------------
    
  vtkPoints *points = vtkPoints::New();
  vcl_map<int, int> oldIdToNewIdMap;
  typedef vcl_map<int, int>::const_iterator  Iter;
  numPointsNew = 0;
    
  // load the connectivity and store the new geometry
  for (int i = 0; i < nCells; i++)
  {
    for (int j = 0; j < numPointsPerCellNew; j++)
    {      
      int ptId = input->GetCell(i)->GetPointId(j);
      if (oldIdToNewIdMap.find(ptId) == oldIdToNewIdMap.end())
      {
        oldIdToNewIdMap.insert(vcl_map<int,int>::value_type(ptId, numPointsNew));        
        numPointsNew++;

        //add another point to output
        double point[3];
        input->GetPoints()->GetPoint(ptId, point);
        points->InsertNextPoint(point[0], point[1], point[2]);
      }
    }
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

      for (Iter iter = oldIdToNewIdMap.begin(); iter != oldIdToNewIdMap.end(); iter++)    
      {
        newArray->SetTuple(numPoint, currentArray->GetTuple(iter->first));
        numPoint++;
      }
  
      outPD->AddArray(newArray);

      //modified by Daniele
      if(strcmp(activeArray->GetName(), newArray->GetName()) == 0) 
        outPD->SetScalars(newArray);

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

  //os << indent << "PrintMe! : " << this->Ivar2Print << "/n";
}
