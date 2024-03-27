/*========================================================================= 
Program: Multimod Application Framework RELOADED 
Module: $RCSfile: vtkALBAPolyDataDeformation_M2.cpp,v $ 
Language: C++ 
Date: $Date: 2012-02-15 11:14:48 $ 
Version: $Revision: 1.1.2.4 $ 
Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
========================================================================== 
Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
See the COPYINGS file for license details 
=========================================================================
*/

#include "vtkALBAPolyDataDeformation_M2.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkMath.h"
#include "vtkCellArray.h"
#include "vtkCellLocator.h"
#include "vtkGenericCell.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkIntArray.h"
#include "vtkCell3D.h"
#include "vtkExtractEdges.h"
#include "vtkConvexPointSet.h"
#include <float.h>

#ifdef DEBUG_vtkALBAPolyDataDeformation_M2
#include "vtkCharArray.h"
#endif



vtkStandardNewMacro(vtkALBAPolyDataDeformation_M2);

#include "albaMemDbg.h"
#include "albaDbg.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"

#pragma region //Nested Classes

//Returns length of the edge. 
//If the edge does not have both vertices defined, it returns 0.0
double vtkALBAPolyDataDeformation_M2::CSkeletonEdge::GetLength()
{
  if (m_Verts[0] == NULL || m_Verts[1] == NULL)
    return 0.0;
  else
    return sqrt(vtkMath::Distance2BetweenPoints(
    m_Verts[0]->m_Coords, m_Verts[1]->m_Coords));
}

vtkALBAPolyDataDeformation_M2::CSkeleton::~CSkeleton()
{
  int nCount = (int)m_Vertices.size();
  for (int i = 0; i < nCount; i++)
  {
    delete m_Vertices[i];
  }

  nCount = (int)m_Edges.size();
  for (int i = 0; i < nCount; i++)
  {
    delete m_Edges[i];
  }

  m_Vertices.clear();
  m_Edges.clear();
}

//------------------------------------------------------------------------
//Computes topology weights for vertices
void vtkALBAPolyDataDeformation_M2::CSkeleton::ComputeTW()
//------------------------------------------------------------------------
{     
  int nVertices = (int)m_Vertices.size();
  int nEdges = (int)m_Edges.size();

  //process every vertex 
  for (int i = 0; i < nVertices; i++)
  {
    CSkeletonVertex* pVertex = m_Vertices[i];    
    int nVertEdges = pVertex->GetDegree();    
    if (nVertEdges == 2)
      pVertex->m_WT = 0;    //invalid point, to be reduced
    else    
    {
      pVertex->m_WT = 0;    //nVertEdges;

      //this is a junction, compute weight for every
      //edge connected to the current vertex and get average weight 
      //int nMaxWeight = 0;
      for (int i = 0; i < nVertEdges; i++)
      {
        CSkeletonEdge* pEdge = pVertex->m_OneRingEdges[i];  
        int nWeight = ComputeEdgeWeight(pEdge, (pEdge->m_Verts[0] == pVertex ? 1 : 0));
        pVertex->m_WT += ((double)nWeight) / nEdges;
      }        
    }
  }
}

//------------------------------------------------------------------------
//Computes the weight for the given edge.
//The computation is recursive in the given direction.
int vtkALBAPolyDataDeformation_M2::CSkeleton::ComputeEdgeWeight(CSkeletonEdge* pEdge, int iDir)
//------------------------------------------------------------------------
{
  CSkeletonVertex* pVertex = pEdge->m_Verts[iDir];
  int nCount = pVertex->GetDegree();
  if (nCount == 1)  //end point
    return 1;

  int nRetValue = 0;
  for (int i = 0; i < nCount; i++)
  {
    CSkeletonEdge* pNbEdge = pVertex->m_OneRingEdges[i];
    if (pNbEdge == pEdge)
      continue;

    int nWeight = ComputeEdgeWeight(pNbEdge, (pNbEdge->m_Verts[0] == pVertex ? 1 : 0));    
    if (nRetValue < nWeight)
      nRetValue = nWeight;
  }

  //if (nCount != 2)
  //  nRetValue++;    //simple joined edges do not count
  return ++nRetValue;
}


//------------------------------------------------------------------------
//Computes the bounding box
void vtkALBAPolyDataDeformation_M2::CSkeleton::GetBoundingBox(double bnds[6])
//------------------------------------------------------------------------
{  
  bnds[0] = bnds[2] = bnds[4] = 0xFFFFFFFF;
  bnds[1] = bnds[3] = bnds[5] = -bnds[0];

  int nCount = (int)m_Vertices.size();

  for (int i = 0; i < nCount; i++)
  {
    double* pcoods = m_Vertices[i]->m_Coords;

    if (pcoods[0] < bnds[0])
      bnds[0] = pcoods[0];

    if (pcoods[0] > bnds[1])
      bnds[1] = pcoods[0];

    if (pcoods[1] < bnds[2])
      bnds[2] = pcoods[1];

    if (pcoods[1] > bnds[3])
      bnds[3] = pcoods[1];

    if (pcoods[2] < bnds[4])
      bnds[4] = pcoods[2];

    if (pcoods[2] > bnds[5])
      bnds[5] = pcoods[2];
  }
}

template <class T>
vtkALBAPolyDataDeformation_M2::CMatrix<T>::CMatrix(int rows, int columns) 
{  
  // alloc arrays
  Matrix = new T*[rows]; // rows
  for ( int i = 0 ; i < rows ; i++ )
  {
    Matrix[i] = new T[columns]; // columns
    memset(Matrix[i], 0, sizeof(T)*columns);
  }

  NumOfRows = rows;
  NumOfCols = columns;
}

template <class T>
vtkALBAPolyDataDeformation_M2::CMatrix<T>::~CMatrix() 
{
  if ( Matrix != NULL ) 
  {
    // free arrays
    for ( int i = 0 ; i < NumOfRows ; i++ )
      delete [] Matrix[i];

    delete [] Matrix;
  }
  Matrix = NULL;
}

#pragma region CMunkres algorithm
#define Z_NORMAL 0
#define Z_STAR 1
#define Z_PRIME 2

int vtkALBAPolyDataDeformation_M2::CMunkres::Step1(void) 
{
  for ( int row = 0 ; row < (*matrix).GetNumberOfRows() ; row++ )
    for ( int col = 0 ; col < (*matrix).GetNumberOfColumns() ; col++ )
      if ( (*matrix)(row,col) == 0 ) {
        bool isstarred = false;
        for ( int nrow = 0 ; nrow < (*matrix).GetNumberOfRows() ; nrow++ )
          if ( (*mask_matrix)(nrow,col) == Z_STAR )
            isstarred = true;

        if ( !isstarred ) {
          for ( int ncol = 0 ; ncol < (*matrix).GetNumberOfColumns() ; ncol++ )
            if ( (*mask_matrix)(row,ncol) == Z_STAR )
              isstarred = true;
        }

        if ( !isstarred ) {
          (*mask_matrix)(row,col) = Z_STAR;
        }
      }

      return 2;
}

int vtkALBAPolyDataDeformation_M2::CMunkres::Step2(void) 
{
  int covercount = 0;
  for ( int row = 0 ; row < (*matrix).GetNumberOfRows() ; row++ )
    for ( int col = 0 ; col < (*matrix).GetNumberOfColumns() ; col++ )
      if ( (*mask_matrix)(row,col) == Z_STAR ) {
        col_mask[col] = true;
        covercount++;
      }

      int k = (*matrix).GetMinSize();

      if ( covercount >= k )
        return 0;      

      return 3;
}

int vtkALBAPolyDataDeformation_M2::CMunkres::Step3(void) 
{
  /*
  Main Zero Search

  1. Find an uncovered Z in the distance (*matrix) and prime it. If no such zero exists, go to Step 5
  2. If No Z* exists in the row of the Z', go to Step 4.
  3. If a Z* exists, cover this row and uncover the column of the Z*. Return to Step 3.1 to find a new Z
  */
  if ( FindUncoveredInMatrix(0, saverow, savecol) ) {
    (*mask_matrix)(saverow,savecol) = Z_PRIME; // prime it.
  } else {
    return 5;
  }

  for ( int ncol = 0 ; ncol < (*matrix).GetNumberOfColumns() ; ncol++ )
    if ( (*mask_matrix)(saverow,ncol) == Z_STAR ) {
      row_mask[saverow] = true; //cover this row and
      col_mask[ncol] = false; // uncover the column containing the starred zero
      return 3; // repeat
    }

    return 4; // no starred zero in the row containing this primed zero
}

int vtkALBAPolyDataDeformation_M2::CMunkres::Step4(void) 
{
  std::list<std::pair<int,int> > seq;
  // use saverow, savecol from step 3.
  std::pair<int,int> z0(saverow, savecol);
  std::pair<int,int> z1(-1,-1);
  std::pair<int,int> z2n(-1,-1);
  seq.insert(seq.end(), z0);
  int row, col = savecol;
  /*
  Increment Set of Starred Zeros

  1. Construct the ``alternating sequence'' of primed and starred zeros:

  Z0 : Unpaired Z' from Step 4.2 
  Z1 : The Z* in the column of Z0
  Z[2N] : The Z' in the row of Z[2N-1], if such a zero exists 
  Z[2N+1] : The Z* in the column of Z[2N]

  The sequence eventually terminates with an unpaired Z' = Z[2N] for some N.
  */
  bool madepair;
  do {
    madepair = false;
    for ( row = 0 ; row < (*matrix).GetNumberOfRows() ; row++ )
      if ( (*mask_matrix)(row,col) == Z_STAR ) {
        z1.first = row;
        z1.second = col;
        if ( PairInList(z1, seq) )
          continue;

        madepair = true;
        seq.insert(seq.end(), z1);
        break;
      }

      if ( !madepair )
        break;

      madepair = false;

      for ( col = 0 ; col < (*matrix).GetNumberOfColumns() ; col++ )
        if ( (*mask_matrix)(row,col) == Z_PRIME ) {
          z2n.first = row;
          z2n.second = col;
          if ( PairInList(z2n, seq) )
            continue;
          madepair = true;
          seq.insert(seq.end(), z2n);
          break;
        }
  } while ( madepair );

  for ( std::list<std::pair<int,int> >::iterator i = seq.begin() ;
    i != seq.end() ;
    i++ ) {
      // 2. Unstar each starred zero of the sequence.
      if ( (*mask_matrix)(i->first,i->second) == Z_STAR )
        (*mask_matrix)(i->first,i->second) = Z_NORMAL;

      // 3. Star each primed zero of the sequence,
      // thus increasing the number of starred zeros by one.
      if ( (*mask_matrix)(i->first,i->second) == Z_PRIME )
        (*mask_matrix)(i->first,i->second) = Z_STAR;
  }

  // 4. Erase all primes, uncover all columns and rows, 
  for ( int row = 0 ; row < (*mask_matrix).GetNumberOfRows() ; row++ )
    for ( int col = 0 ; col < (*mask_matrix).GetNumberOfColumns() ; col++ )
      if ( (*mask_matrix)(row,col) == Z_PRIME )
        (*mask_matrix)(row,col) = Z_NORMAL;

  for ( int i = 0 ; i < (*matrix).GetNumberOfRows() ; i++ ) {
    row_mask[i] = false;
  }

  for ( int i = 0 ; i < (*matrix).GetNumberOfColumns() ; i++ ) {
    col_mask[i] = false;
  }

  // and return to Step 2. 
  return 2;
}

int vtkALBAPolyDataDeformation_M2::CMunkres::Step5(void) {
  /*
  New Zero Manufactures

  1. Let h be the smallest uncovered entry in the (modified) distance (*matrix).
  2. Add h to all covered rows.
  3. Subtract h from all uncovered columns
  4. Return to Step 3, without altering stars, primes, or covers. 
  */
  double h = 0;
  for ( int row = 0 ; row < (*matrix).GetNumberOfRows() ; row++ ) {
    if ( !row_mask[row] ) {
      for ( int col = 0 ; col < (*matrix).GetNumberOfColumns() ; col++ ) {
        if ( !col_mask[col] ) {
          if ( (h > (*matrix)(row,col) && (*matrix)(row,col) != 0) || h == 0 ) {
            h = (*matrix)(row,col);
          }
        }
      }
    }
  }

  for ( int row = 0 ; row < (*matrix).GetNumberOfRows() ; row++ )
    for ( int col = 0 ; col < (*matrix).GetNumberOfColumns() ; col++ ) {
      if ( row_mask[row] )
        (*matrix)(row,col) += h;

      if ( !col_mask[col] )
        (*matrix)(row,col) -= h;
    }

    return 3;
}

void vtkALBAPolyDataDeformation_M2::CMunkres::Solve( CMatrix< double >* m, CMatrix< int >* matches )
{
  // Linear assignment problem solution
  // [modifies matrix in-place.]
  // matrix(row,col): row major format assumed.

  // Assignments are remaining 0 values
  // (extra 0 values are replaced with -1)

  bool notdone = true;
  int step = 1;

  this->matrix = m;
  // Z_STAR == 1 == starred, Z_PRIME == 2 == primed
  this->mask_matrix = matches;

  row_mask = new bool[matrix->GetNumberOfRows()];
  col_mask = new bool[matrix->GetNumberOfColumns()];
  for ( int i = 0 ; i < matrix->GetNumberOfRows() ; i++ ) {
    row_mask[i] = false;
  }

  for ( int i = 0 ; i < matrix->GetNumberOfColumns(); i++ ) {
    col_mask[i] = false;
  }

  while ( notdone ) {
    switch ( step ) {
      case 0:
        notdone = false;
        break;
      case 1:
        step = Step1();
        break;
      case 2:
        step = Step2();
        break;
      case 3:
        step = Step3();
        break;
      case 4:
        step = Step4();
        break;
      case 5:
        step = Step5();
        break;
    }
  }

  // Store results
  for ( int row = 0 ; row < matrix->GetNumberOfRows() ; row++ )
  {
    for ( int col = 0 ; col < matrix->GetNumberOfColumns() ; col++ )
    {
      if ( (*mask_matrix)(row,col) == Z_STAR )
        (*mask_matrix)(row,col) = 1;
      else
        (*mask_matrix)(row,col) = 0;
    }
  }  

  delete [] row_mask;
  delete [] col_mask;
}
#pragma endregion //CMunkres algorithm
#pragma endregion //Nested Classes

vtkALBAPolyDataDeformation_M2::vtkALBAPolyDataDeformation_M2()
{
  Skeletons = NULL;
  SuperSkeleton = NULL;
  MeshVertices = NULL;

  NumberOfSkeletons = 0;

  MatchGeometryWeight = 0.5;
  MatchTopologyWeight = 1;     //topology is more important
  MatchTolerance = 0.1;
  
  DivideSkeletonEdges = 0;
  PreserveVolume = 1;

#ifdef DEBUG_vtkALBAPolyDataDeformation_M2
  m_MATCHED_CC = NULL; m_MATCHED_FULLCC = NULL;
  m_MATCHED_POLYS[0] = NULL; m_MATCHED_POLYS[1] = NULL;
#endif
}

vtkALBAPolyDataDeformation_M2::~vtkALBAPolyDataDeformation_M2()
{
  //destroy skeletons
  SetNumberOfSkeletons(0);  

  //superskeleton should be destroyed by now
  _ASSERT(SuperSkeleton == NULL);
  DestroySuperSkeleton();
  
#ifdef DEBUG_vtkALBAPolyDataDeformation_M2
  DestroyMATCHEDData();  
#endif  

  //this should be released already
  _ASSERT(MeshVertices == NULL);
  delete[] MeshVertices;
  MeshVertices = NULL;
}

//----------------------------------------------------------------------------
void vtkALBAPolyDataDeformation_M2::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}

//------------------------------------------------------------------------
//Sets the number of control skeletons.  
//Old skeletons are copied (and preserved)
/*virtual*/ void vtkALBAPolyDataDeformation_M2::SetNumberOfSkeletons( int nCount )
//------------------------------------------------------------------------
{
  _VERIFY_RET(nCount >= 0);

  if (nCount == NumberOfSkeletons)
    return; //nothing to be changed

  //if the number of curves is going to be decreased, we
  //need to delete some curves
  while (NumberOfSkeletons > nCount)
  {
    --NumberOfSkeletons;
    if (Skeletons[NumberOfSkeletons].pPolyLines[0] != NULL)
      Skeletons[NumberOfSkeletons].pPolyLines[0]->Delete();

    if (Skeletons[NumberOfSkeletons].pPolyLines[1] != NULL)
      Skeletons[NumberOfSkeletons].pPolyLines[1]->Delete();

    if (Skeletons[NumberOfSkeletons].pCCList != NULL)
      Skeletons[NumberOfSkeletons].pCCList->Delete();
  }

  CONTROL_SKELETON* pNewArr = NULL;
  if (nCount > 0)
  {    
    pNewArr = new CONTROL_SKELETON[nCount];
    memset(pNewArr, 0, sizeof(CONTROL_SKELETON)*nCount);

    //copy existing curves
    for (int i = 0; i < NumberOfSkeletons; i++) {
      pNewArr[i] = Skeletons[i];      
    }

    NumberOfSkeletons = nCount;    
  }

  delete[] Skeletons;
  Skeletons = pNewArr;

  this->Modified();
}

//------------------------------------------------------------------------
//Specifies the n-th control skeleton.
/*virtual*/ void vtkALBAPolyDataDeformation_M2
::SetNthSkeleton( int idx, vtkPolyData* original, 
                 vtkPolyData* modified, vtkIdList* correspondence )
//------------------------------------------------------------------------
{
  _VERIFY_RET(idx >= 0);

  if (idx >= GetNumberOfSkeletons())
    SetNumberOfSkeletons(idx + 1);

  if (Skeletons[idx].pPolyLines[0] != original)
  {
    if (NULL != Skeletons[idx].pPolyLines[0])
      Skeletons[idx].pPolyLines[0]->Delete();

    if (NULL != (Skeletons[idx].pPolyLines[0] = original))
      Skeletons[idx].pPolyLines[0]->Register(this);

    this->Modified();
  }

  if (Skeletons[idx].pPolyLines[1] != modified)
  {
    if (NULL != Skeletons[idx].pPolyLines[1])
      Skeletons[idx].pPolyLines[1]->Delete();

    if (NULL != (Skeletons[idx].pPolyLines[1] = modified))
      Skeletons[idx].pPolyLines[1]->Register(this);

    this->Modified();
  }

  if (Skeletons[idx].pCCList != correspondence)
  {
    if (NULL != Skeletons[idx].pCCList)
      Skeletons[idx].pCCList->Delete();

    if (NULL != (Skeletons[idx].pCCList = correspondence))
      Skeletons[idx].pCCList->Register(this);

    this->Modified();
  }
}

//------------------------------------------------------------------------
//Return this object's modified time.
/*virtual*/ vtkMTimeType vtkALBAPolyDataDeformation_M2::GetMTime()
//------------------------------------------------------------------------
{
	vtkMTimeType mtime = Superclass::GetMTime();
  for (int i = 0; i < NumberOfSkeletons; i++)
  {
		vtkMTimeType t1;
    if (Skeletons[i].pPolyLines[0] != NULL)
    {
      t1 = Skeletons[i].pPolyLines[0]->GetMTime();
      if (t1 > mtime)
        mtime = t1;
    }

    if (Skeletons[i].pPolyLines[1] != NULL)
    {
      t1 = Skeletons[i].pPolyLines[1]->GetMTime();
      if (t1 > mtime)
        mtime = t1;
    }

    if (Skeletons[i].pCCList != NULL)
    {
      t1 = Skeletons[i].pCCList->GetMTime();
      if (t1 > mtime)
        mtime = t1;
    }
  }

  return mtime;
}

//------------------------------------------------------------------------
//By default, UpdateInformation calls this method to copy information
//unmodified from the input to the output.
/*virtual*/int vtkALBAPolyDataDeformation_M2::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//------------------------------------------------------------------------
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (input == NULL)
  {
    vtkErrorMacro(<< "Invalid input for vtkALBAPolyDataDeformation_M2.");
    return 1;   //we have no input
  }

  //check output
  vtkPolyData* output = GetOutput();
  if (output == NULL)
    SetOutput(vtkPolyData::New());

  //copy input to output
  return Superclass::RequestInformation(request,inputVector,outputVector);  
}

//------------------------------------------------------------------------
//This method is the one that should be used by subclasses, right now the 
//default implementation is to call the backwards compatibility method
/*virtual*/void vtkALBAPolyDataDeformation_M2::ExecuteData(vtkDataObject *output)
{
  //check whether output is valid
  vtkPolyData* input = vtkPolyData::SafeDownCast(GetInput());
  if (input == NULL)
    return;

  vtkPolyData* pPoly = vtkPolyData::SafeDownCast(output);
  if (pPoly != NULL)
    pPoly->DeepCopy(input);

  if (pPoly == NULL || pPoly->GetPoints() == NULL || pPoly->GetPoints()->GetNumberOfPoints() == 0)
  {
    vtkWarningMacro(<< "Invalid output for vtkALBAPolyDataDeformation_M2.");
    return;   //we have no valid output
  }  

  //process every single skeleton and construct 
  //super skeleton where everything is matched
  if (!CreateSuperSkeleton())
  {
    vtkWarningMacro(<< "Missing control skeleton for vtkALBAPolyDataDeformation_M2.");
    return;
  }

  //OK, we have super skeleton, let us build cells and neighbors (if they do not exist)
  //for the input mesh as we will need then to quickly traverse through the mesh
  MeshVertices = new CMeshVertex[input->GetNumberOfPoints()];
  input->BuildCells(); input->BuildLinks();
  
  //for every curve, we need to compute its ROI, i.e., vertices that are mapped for this curve 
  int nCount = (int)SuperSkeleton->m_pOC_Skel->m_Vertices.size();
  for (int iStartPos = 0; iStartPos < nCount; )
  {
    CSkeletonVertex* pOC_Curve = SuperSkeleton->m_pOC_Skel->m_Vertices[iStartPos];
    iStartPos += GetNumberOfCurveVertices(pOC_Curve);

    //compute local frames for both curves, original and deformed one
    ComputeLFS(pOC_Curve);

    //compute separating planes for both curves
    ComputeSeparatingPlanes(pOC_Curve);    
  }

  
  //compute the ROIs for all curve, i.e., detects vertices that 
  //might be influenced by those curves
  nCount = (int)SuperSkeleton->m_pOC_Skel->m_Edges.size();
  for (int i = 0; i < nCount; i++) {
    ComputeROI(SuperSkeleton->m_pOC_Skel->m_Edges[i] );
  }

  //some points might be influenced by more edges, we need to detect which edge
  //is the best one => after that every vertex is influenced just by one curve
  RefineCurveROIs();

  //let us parametrize the mesh
  ComputeMeshParametrization();

  //and finally, deform the output mesh
  DeformMesh(pPoly);

#ifdef DEBUG_vtkALBAPolyDataDeformation_M2
  vtkCharArray* scalar = vtkCharArray::New();  
  nCount = input->GetNumberOfPoints();
  scalar->SetNumberOfTuples(nCount);
  scalar->SetNumberOfComponents(1);

  bool bWarnShown = false;
  int nUnmapped = 0;
/*
  for (int i = 0; i < nCount; i++)
  {
    scalar->SetTuple1(i, -1);
  }

  int iDisp = 0;
  CSkeletonEdge* pEdge = m_SuperSkeleton->m_pOC_Skel->m_Edges[iDisp];
  nCount = pEdge->m_ROI.size();
  for (int i = 0; i < nCount; i++)
  {
    scalar->SetTuple1(pEdge->m_ROI[i], 0);
  }
*/

  for (int i = 0; i < nCount; i++)
  {
    CMeshVertex* pVert = &MeshVertices[i];    
    if (pVert->m_pEdge == NULL)
    {
      if (!bWarnShown) {
        _RPT0(_CRT_WARN, "UNMAPPED VERTICES DETECTED:\n====================\n");
        bWarnShown = true;
      }
      
      nUnmapped++;
      _RPT2(_CRT_WARN, "%d,%s", i, ((nUnmapped % 5) == 0 ? "\n" : " "));
      scalar->SetTuple1(i, -1);
    }
    else 
      scalar->SetTuple1(i, pVert->m_pEdge->m_Id);  
    //else
    //  scalar->SetTuple1(i, 1);
  }

  if (nUnmapped > 0)
    _RPT1(_CRT_WARN, "\n-------------------\nTotal unmapped: %d\n\n", nUnmapped);


  input->GetPointData()->SetScalars(scalar);
  pPoly->GetPointData()->SetScalars(scalar);
  scalar->Delete();

  CreatePolyDataFromSuperskeleton();
#endif
  DestroySuperSkeleton();

  delete[] MeshVertices;
  MeshVertices = NULL;
}  


//------------------------------------------------------------------------
//Creates a single skeleton that describes the deformation
//Returns false, if it cannot be created
bool vtkALBAPolyDataDeformation_M2::CreateSuperSkeleton()
//------------------------------------------------------------------------
{ 
  double dblEdgeFactor = DivideSkeletonEdges ?
    ComputeInputMeshAvgEdgeLength() : 0.0;
  dblEdgeFactor *= dblEdgeFactor*4;  //segment is k times larger than average edge length

  DestroySuperSkeleton();

  //combine every control skeleton to create match
  for (int i = 0; i < NumberOfSkeletons; i++)
  {
    CreateSuperSkeleton(Skeletons[i].pPolyLines[0], 
      Skeletons[i].pPolyLines[1], Skeletons->pCCList, dblEdgeFactor);
  } 

  if (SuperSkeleton == NULL)
    return false;

  int nCount = (int)SuperSkeleton->m_pOC_Skel->m_Vertices.size();
  if (nCount == 0)
  {
    DestroySuperSkeleton();
    return false;
  }
  
#ifdef DEBUG_vtkALBAPolyDataDeformation_M2  
  DestroyMATCHEDData();    
  CreatePolyDataFromSuperskeleton();
#endif

  //now we will extend the superskeleton by adding infinite
  //edges connected to end-points of the skeleton
  int nEdgeId = (int)SuperSkeleton->m_pOC_Skel->m_Edges.size();
  for (int i = 0; i < nCount; i++)
  {
    CSkeletonVertex* pVertex = SuperSkeleton->m_pOC_Skel->m_Vertices[i];
    if (pVertex->m_WT == 0) //terminal nodes
    {
      CSkeletonEdge *pNewEdge, *pDNewEdge;

      //this is either the first or the last vertex of curve
      if (GetNextCurveVertex(pVertex) != NULL)
      {
        //OK, it is first vertex
        pNewEdge = new CSkeletonEdge(NULL, pVertex);
        pVertex->m_OneRingEdges.push_back(pNewEdge);            

        pDNewEdge = new CSkeletonEdge(NULL, pVertex->m_pMatch);
        pVertex->m_pMatch->m_OneRingEdges.push_back(pDNewEdge);
      }
      else
      {
        //the last one => it will be slightly more complex
        pNewEdge = new CSkeletonEdge(pVertex, NULL);
        pVertex->m_OneRingEdges.insert(pVertex->m_OneRingEdges.begin(), pNewEdge);            

        pDNewEdge = new CSkeletonEdge(pVertex->m_pMatch, NULL);
        pVertex->m_pMatch->m_OneRingEdges.insert(pVertex->m_pMatch->
          m_OneRingEdges.begin(), pDNewEdge);
      }      

      pNewEdge->m_Id = nEdgeId;
      pDNewEdge->m_Id = nEdgeId++;

      pNewEdge->m_pMatch = pDNewEdge;
      pDNewEdge->m_pMatch = pNewEdge;

      SuperSkeleton->m_pOC_Skel->m_Edges.push_back(pNewEdge);
      SuperSkeleton->m_pDC_Skel->m_Edges.push_back(pDNewEdge);      
    } //end if (pVertex->m_WT == 0)
  }

  return true;
}

//------------------------------------------------------------------------
//Creates a single super skeleton for the given control skeleton.
//It combines both skeletons together, matching their vertices and
//creating new vertices as needed. It also computes local frames.
//The combined information is then appended into super skeleton.
//If the super skeleton does not exist, it is created. 
void vtkALBAPolyDataDeformation_M2::CreateSuperSkeleton(
  vtkPolyData* pOC, vtkPolyData* pDC, vtkIdList* pCC,
  double dblEdgeFactor)
//------------------------------------------------------------------------
{  
  if (pOC == NULL || pDC == NULL)
    return;   //insufficient information

  //convert both polydata into skeletons
  CSkeleton* pOC_Skel = CreateSkeleton(pOC);
  CSkeleton* pDC_Skel = CreateSkeleton(pDC);

  //match these skeletons
  vtkIdList* pFullCC = MatchSkeletons(pOC_Skel, pDC_Skel, pCC);

  //now, we have (in an ideal case), correspondence for every end-point and joint,
  //which means that both skeletons can be decomposed into set of matching curves
  if (SuperSkeleton == NULL)
    SuperSkeleton = new CSuperSkeleton;

  //reset mark flag for vertices of both curves
  int nOCVerts = (int)pOC_Skel->m_Vertices.size();  
  for (int i = 0; i < nOCVerts; i++) {
    pOC_Skel->m_Vertices[i]->m_nMark = -1;    
  }

  int nDCVerts = (int)pDC_Skel->m_Vertices.size();
  for (int i = 0; i < nDCVerts; i++) {
    pDC_Skel->m_Vertices[i]->m_nMark = -1;    
  }

  //set match marks
  int nCount = pFullCC->GetNumberOfIds();
  vtkIdType* pIds = pFullCC->GetPointer(0);
  for (int i = 0; i < nCount; i += 2)
  {
    pOC_Skel->m_Vertices[pIds[i]]->m_nMark = pIds[i + 1];
    pDC_Skel->m_Vertices[pIds[i + 1]]->m_nMark = pIds[i];
  }

  //marks for every edge should be zeros as they are not used in MatchSkeletons
  _ASSERT(pOC_Skel->m_Edges[0]->m_nMark == 0);  
   
  //detect every matched curve  
  CSkeletonVertex** pOC_Curve = new CSkeletonVertex*[nOCVerts + nDCVerts];
  CSkeletonVertex** pDC_Curve = new CSkeletonVertex*[nOCVerts + nDCVerts];  

  for (int i = 0; i < nOCVerts; i++)
  {
    CSkeletonVertex* pVert1 = pOC_Skel->m_Vertices[i];
    if (pVert1->m_nMark < 0)
      continue; //this is inner vertex

    int nEdges = (int)pVert1->m_OneRingEdges.size();
    for (int j = 0; j < nEdges; j++)
    {
      if (pVert1->m_OneRingEdges[j]->m_nMark != 0)
        continue; //this curve was already traced

      int nOCCurveVerts = TraceSkeletonCurve(pVert1, j, pOC_Curve);
      CSkeletonVertex* pVert2 = pOC_Curve[nOCCurveVerts - 1];
      if (pVert2->m_nMark < 0)
        continue; //this curve is invalid and will be ignored

      //now find the matching curve in the deformed      
      CSkeletonVertex* pDVert1 = pDC_Skel->m_Vertices[pVert1->m_nMark];
      CSkeletonVertex* pDVert2 = pDC_Skel->m_Vertices[pVert2->m_nMark];
                  
      int nDCCurveVerts;
      int nDEdges = (int)pDVert1->m_OneRingEdges.size();
      for (int k = 0; k < nDEdges; k++)
      {
        if (pDVert1->m_OneRingEdges[k]->m_nMark == 0)
        {
          //this curve was not yet traced
          nDCCurveVerts = TraceSkeletonCurve(pDVert1, k, pDC_Curve);
          if (pDC_Curve[nDCCurveVerts - 1] == pDVert2)
            break;  //we have found it          
        }

        nDCCurveVerts = 0;  //reset number of vertices
      }

      if (nDCCurveVerts == 0)
        continue; //there is no matching curve (N.B. this can really happen)

      //OK, now we have two curves in pOC_Curve and pDC_Curve
      //first, mark their edges to prevent retracing of them
      MarkCurveEdges(pOC_Curve, nOCCurveVerts);
      MarkCurveEdges(pDC_Curve, nDCCurveVerts);
      
      //and now match both curve; it will create a set of new vertices as well
      //as edges between them, everything is matched (vertices and edges)
      MatchCurves(pOC_Curve, nOCCurveVerts, pDC_Curve, nDCCurveVerts);                        

      //BES 22.9.2008 - split large edges of super-skeleton
      if (DivideSkeletonEdges)
        RefineCurve(pOC_Curve[0], dblEdgeFactor);

      //save both curves into the superskeleton
      AddCurveToSuperSkeleton(pOC_Curve[0]);      
    }
  }
  
  delete[] pOC_Curve;
  delete[] pDC_Curve;

  //destroys skeletons
  DestroySkeleton(pOC_Skel);
  DestroySkeleton(pDC_Skel);
  pFullCC->Delete();  
}

//------------------------------------------------------------------------
//Compute the best match for skeleton vertices.
//Only junctions and terminal nodes are matched. The caller can optionally 
//specify some (or all) correspondences. The routine returns the list containing
//pairs of corresponding vertices.
vtkIdList* vtkALBAPolyDataDeformation_M2::MatchSkeletons(CSkeleton* pOC, 
                                                     CSkeleton* pDC, vtkIdList* pCC)
//------------------------------------------------------------------------
{  
  //first, determine, which points are going to be matched 
  int nOCVerts = (int)pOC->m_Vertices.size(); 
  for (int i = 0; i < nOCVerts; i++)
  {
    //mark the point, if it is not simple joined
    CSkeletonVertex* pVertex = pOC->m_Vertices[i];
    if (pVertex->GetDegree() == 2)
      pVertex->m_nMark = -1;
    else
      pVertex->m_nMark = 0;
  }

  int nDCVerts = (int)pDC->m_Vertices.size();
  for (int i = 0; i < nDCVerts; i++)
  {
    //mark the point, if it is not simple joined
    CSkeletonVertex* pVertex = pDC->m_Vertices[i];
    if (pVertex->GetDegree() == 2)
      pVertex->m_nMark = -1;
    else
      pVertex->m_nMark = 0;
  }

  //incorporate explicitly given correspondence
  if (pCC != NULL)
  {
    int nCount = pCC->GetNumberOfIds();
    vtkIdType* pIds = pCC->GetPointer(0);
    for (int i = 1; i < nCount; i += 2)
    {
      if (pIds[i - 1] < nOCVerts && pIds[i] < nDCVerts)
      {
        pOC->m_Vertices[pIds[i - 1]]->m_nMark = 1;
        pDC->m_Vertices[pIds[i]]->m_nMark = 1;
      }
    }
  }

  //reduce the set and get only vertices that are not marked
  //i.e., these vertices are not already matched and they must be matched
  CSkeletonVertex** pOCVerts = new CSkeletonVertex*[nOCVerts];
  int nCount = nOCVerts; nOCVerts = 0;
  for (int i = 0; i < nCount; i++)
  {    
    pOCVerts[nOCVerts] = pOC->m_Vertices[i];
    if (pOCVerts[nOCVerts]->m_nMark == 0)
      nOCVerts++; //this is end-point or junction
  }

  CSkeletonVertex** pDCVerts = new CSkeletonVertex*[nDCVerts];
  nCount = nDCVerts; nDCVerts = 0;
  for (int i = 0; i < nCount; i++)
  {    
    pDCVerts[nDCVerts] = pDC->m_Vertices[i];
    if (pDCVerts[nDCVerts]->m_nMark == 0)
      nDCVerts++; //this is end-point or junction
  }  

  //and now, we can start the matching of vertices
  //for that purpose, we need some weights 
  //so let us recompute topology weights
  pOC->ComputeTW(); pDC->ComputeTW();

  //get the common bounding box
  double bnds[6], bnds2[6];
  pOC->GetBoundingBox(bnds);
  pDC->GetBoundingBox(bnds2);

  for (int i = 0; i < 6; i++)
  {
    if (bnds2[i] < bnds[i])
      bnds[i] = bnds2[i];

    i++;

    if (bnds2[i] > bnds[i])
      bnds[i] = bnds2[i];
  }

  //compute the diagonal (all diagonals are the same)
  double A[3] = { bnds[1] - bnds[0], bnds[3] - bnds[2], bnds[5] - bnds[4] };  
  double dblMaxDiag = vtkMath::Norm(A);

  //and finally, construct cost matrix
  CMatrix< double > Matrix(nOCVerts, nDCVerts);
  for (int i = 0; i < nOCVerts; i++)
  {
    CSkeletonVertex* pVert1 = pOCVerts[i];
    for (int j = 0; j < nDCVerts; j++)
    {
      CSkeletonVertex* pVert2 = pDCVerts[j];
      
      //compute the cost of matching those two vertices
      //the cost computation is inspired by the paper:
      //Xiang Bai and Longin Jan Latecki. Path Similarity Skeleton Graph Matching.
      //In: IEEE Transactions on Pattern Analysis and Machine Inteligence,
      //July 2008, 30(7):1282- 1292
      double ri = sqrt(vtkMath::Distance2BetweenPoints(pVert1->m_Coords, 
        pVert2->m_Coords)) / dblMaxDiag;

      //N.B. ti is always from interval <0,1)
      double ti = (pVert1->m_WT - pVert2->m_WT)*(pVert1->m_WT - pVert2->m_WT) /
        (pVert1->m_WT + pVert2->m_WT);

      //the constant is there to have values quite different from perfect matches
      Matrix(i, j) = MatchGeometryWeight*ri + MatchTopologyWeight*ti;      
    }
  }

  //run Hungarian algorithm to get matches
  CMunkres ha;
  CMatrix< int > Matches(nOCVerts, nDCVerts);
  ha.Solve(&Matrix, &Matches);

  //get correspondences  
  vtkIdList* pCCRet = vtkIdList::New();  
  if (pCC == NULL)
    nCount = 0;
  else
  {
    //copy ids
    nCount = pCC->GetNumberOfIds();
    memcpy(pCCRet->WritePointer(0, nCount), 
      pCC->GetPointer(0), nCount*sizeof(vtkIdType));
  }

  vtkIdType* pIds = pCCRet->WritePointer(nCount, 2*nOCVerts);  //at most every vertex has match

  int nPairs = 0;
  for (int i = 0; i < nOCVerts; i++)
  {    
    for (int j = 0; j < nDCVerts; j++)
    {
      if (Matches(i, j) == 1)
      {
        pIds[nPairs++] = pOCVerts[i]->m_Id;
        pIds[nPairs++] = pDCVerts[j]->m_Id;
      }
    }
  }

  pCCRet->SetNumberOfIds(nCount + nPairs);

  delete[] pOCVerts;
  delete[] pDCVerts;

  return pCCRet;
}

//------------------------------------------------------------------------
//Traces the curve in the given direction starting from the given vertex.
//Starting from the given vertex, the routine moves in the given direction
//(which actually defines the edge which to go) storing the visited vertices
//of the graph into the output buffer. The buffer must be capable to hold 
//all vertices of the graph. The traversal process ends when a "junction" 
//or end-point is reached (those vertices have mark >= 0). The routine 
//returns the number of vertices stored in the buffer. 
//N.B. this routine is supposed to be called from CreateSuperSkeleton
int vtkALBAPolyDataDeformation_M2::TraceSkeletonCurve(
                        CSkeletonVertex* pStartVertex, int iDir, CSkeletonVertex** pOutBuf)
//------------------------------------------------------------------------
{ 
  int nRetCount = 0;
  do 
  {
    pOutBuf[nRetCount] = pStartVertex;
    nRetCount++;

    CSkeletonEdge* pEdge = pStartVertex->m_OneRingEdges[iDir];
    if (pEdge->m_Verts[0] == pStartVertex)    
      pStartVertex = pEdge->m_Verts[1];    
    else
      pStartVertex = pEdge->m_Verts[0];

    //get the adjacent edge
    iDir = (pStartVertex->m_OneRingEdges[0] == pEdge) ? 1 : 0; 
  }
  while(pStartVertex->m_nMark < 0);

  //store the last vertex
  pOutBuf[nRetCount] = pStartVertex;
  return ++nRetCount;
}

//------------------------------------------------------------------------
//Marks every edge of the given skeleton curve
void vtkALBAPolyDataDeformation_M2::MarkCurveEdges(CSkeletonVertex** pCurve, int nCount)
//------------------------------------------------------------------------
{
  //first and last vertex may have more than one or more edges as they may
  //be junction nodes, the rest have just two edges
  for (int i = 1; i < nCount - 1; i++)
  {
    pCurve[i]->m_OneRingEdges[0]->m_nMark = 1;
    pCurve[i]->m_OneRingEdges[1]->m_nMark = 1;
  }

  //if there is at least one inner vertex than we are ready
  if (nCount == 2)
  {
    //otherwise, we need to find the proper edge
    int nEdges = (int)pCurve[0]->m_OneRingEdges.size();
    for (int i = 0; i < nEdges; i++)
    {
      CSkeletonEdge* pEdge = pCurve[0]->m_OneRingEdges[i];
      if (
        (pEdge->m_Verts[0] == pCurve[0] && pEdge->m_Verts[1] == pCurve[1]) ||
        (pEdge->m_Verts[0] == pCurve[1] && pEdge->m_Verts[1] == pCurve[0])
        )      
      {
        //this is the correct edge
        pEdge->m_nMark = 1; 
        break;
      }
    } //end for
  }
}


//this structure is used in MatchCurves
typedef struct vtkALBAPolyDataDeformation_M2::CURVE_VERTEX
{
  double t;             //<time parameter
  CSkeletonVertex* pVertex;     //<skeleton vertex

  CURVE_VERTEX* pLast;  //<last vertex on the curve
  CURVE_VERTEX* pNext;  //<next vertex on the curve    
} CURVE_VERTEX;

//------------------------------------------------------------------------
//Matches two curves defined by two arrays of vertices.
//It creates at least max(nOCVerts, nDCVerts) new vertices and those 
//vertices are stored in pOC and pDC buffers. Note: both array buffers 
//must be capable enough to hold nOCVerts + nDCVerts vertices.
//Returns the number of vertices in matched curves. 
//It also creates edges between vertices and establishes links between
//both vertices and edges. The caller is responsible for deletion of all 
//objects when they are no longer needed.
//N.B. the original vertices are not destroyed, the routine damages buffers only
int vtkALBAPolyDataDeformation_M2::MatchCurves(CSkeletonVertex** pOC, int nOCVerts, 
                                           CSkeletonVertex** pDC, int nDCVerts)
//------------------------------------------------------------------------
{
  CSkeletonVertex** pOC_DC[2] = { pOC, pDC };
  int nOC_DCVerts[2] = { nOCVerts, nDCVerts };
  
  CURVE_VERTEX* pVertexPool = new CURVE_VERTEX[2*(nOCVerts + nDCVerts)];
  int nNextVertex = 0;

  //create internal structures and parametrize both curves
  for (int i = 0; i < 2; i++)
  {    
    pVertexPool[nNextVertex].t = 0.0;
    pVertexPool[nNextVertex].pVertex = new CSkeletonVertex(pOC_DC[i][0]->m_Coords);    
    pVertexPool[nNextVertex].pVertex->m_WT = pOC_DC[i][0]->GetDegree() - 1;

    pVertexPool[nNextVertex].pLast = NULL;
    pVertexPool[nNextVertex].pNext = NULL;    
    nNextVertex++;

    int nOldNextVertex = nNextVertex;
    for (int j = 1; j < nOC_DCVerts[i]; j++)
    {
      pVertexPool[nNextVertex].t = pVertexPool[nNextVertex - 1].t + 
        sqrt(vtkMath::Distance2BetweenPoints(pOC_DC[i][j - 1]->m_Coords, 
        pOC_DC[i][j]->m_Coords));

      pVertexPool[nNextVertex].pVertex = new CSkeletonVertex(pOC_DC[i][j]->m_Coords);
      pVertexPool[nNextVertex].pVertex->m_WT = pOC_DC[i][j]->GetDegree() - 1;
      
      pVertexPool[nNextVertex].pLast = &pVertexPool[nNextVertex - 1];
      pVertexPool[nNextVertex - 1].pNext = &pVertexPool[nNextVertex];
      pVertexPool[nNextVertex].pNext = NULL;
      
      nNextVertex++;
    }

    double dblTotalLength = pVertexPool[nNextVertex - 1].t;
    for (int j = 1; j < nOC_DCVerts[i] - 1; j++) 
    {
      pVertexPool[nOldNextVertex].t /= dblTotalLength;
      nOldNextVertex++;
    }

    pVertexPool[nNextVertex - 1].t = 1.0;
  }

  //match end-points of both curves
  CURVE_VERTEX* pCurves[2];
  pCurves[0] = &pVertexPool[0];
  pCurves[1] = &pVertexPool[nOCVerts];
  pCurves[0]->pVertex->m_pMatch = pCurves[1]->pVertex;
  pCurves[1]->pVertex->m_pMatch = pCurves[0]->pVertex;
  (pCurves[1] - 1)->pVertex->m_pMatch = pVertexPool[nNextVertex - 1].pVertex;
  pVertexPool[nNextVertex - 1].pVertex->m_pMatch = (pCurves[1] - 1)->pVertex;
  
  //compute the matching limit
  double dblLimit = 1.0;
  for (int i = 0; i < 2; i++)
  {
    CURVE_VERTEX* pVertA = pCurves[i];
    CURVE_VERTEX* pVertB = pVertA->pNext;
    while (pVertB != NULL)
    {
      double t = pVertB->t - pVertA->t;
      if (t < dblLimit)
        dblLimit = t;

      pVertA = pVertB;
      pVertB = pVertB->pNext;
    }
  }

  dblLimit *= MatchTolerance;
  
  //now match also inner vertices of curves
  for (int i = 0; i < 2; i++)
  {
    CURVE_VERTEX* pVert = pCurves[i];
    CURVE_VERTEX* pVertB = pCurves[1 - i];
    while (pVert != NULL)
    {
      if (pVert->pVertex->m_pMatch == NULL)
      {
        //find the edge in the other curve, where this point belong
        while (pVertB->t < pVert->t) {
          pVertB = pVertB->pNext;
        }

        //pVertB and pVertB->pLast denote the edge, 
        //where the current vertex pVert should be mapped
        CURVE_VERTEX* pVertA = pVertB->pLast;

        //first check, whether pVert is not mapped into pVertA or pVertB        
        if (pVertA->pVertex->m_pMatch == NULL && fabs(pVertA->t - pVert->t) <= dblLimit)
        {
          //A is the valid closest point
          pVertA->pVertex->m_pMatch = pVert->pVertex;
          pVert->pVertex->m_pMatch = pVertA->pVertex;
        }
        else if (pVertA->pVertex->m_pMatch == NULL && fabs(pVertB->t - pVert->t) <= dblLimit)
        {
          //B is the valid closest point
          pVertB->pVertex->m_pMatch = pVert->pVertex;
          pVert->pVertex->m_pMatch = pVertB->pVertex;
        }
        else
        {
          //we need to create a new vertex
          CURVE_VERTEX* pNewVert = &pVertexPool[nNextVertex++];
          pNewVert->pLast = pVertA;
          pVertA->pNext = pNewVert;
          pNewVert->pNext = pVertB;
          pVertB->pLast = pNewVert;
          
          pNewVert->t = pVert->t;                    
          
          double dblB = (pNewVert->t - pVertA->t) / (pVertB->t - pVertA->t);
          double dblA = 1.0 - dblB;
          double coords[3];

          for (int j = 0; j < 3; j++) {            
            coords[j] = pVertA->pVertex->m_Coords[j]*dblA +  
              pVertB->pVertex->m_Coords[j]*dblB;
          }

          pNewVert->pVertex = new CSkeletonVertex(coords);
          pNewVert->pVertex->m_WT = 1; //it is an inner node => it must have two edges

          pNewVert->pVertex->m_pMatch = pVert->pVertex;
          pVert->pVertex->m_pMatch = pNewVert->pVertex;
        }
      }

      pVert = pVert->pNext;
    } //end while
  } //end for

  //now, every vertex is matched => store the data
  int nRetCount;  
  for (int i = 0; i < 2; i++)
  {
    CURVE_VERTEX* pVert = pCurves[i];    
    nRetCount = 0;

    while (pVert != NULL)
    {      
      pOC_DC[i][nRetCount] = pVert->pVertex;
      
      pVert = pVert->pNext;
      nRetCount++;
    }

    //build edges for the current curve
    CreateCurveEdges(pOC_DC[i], nRetCount);
  }

  //match edges now
  for (int i = 0; i < nRetCount - 1; i++) 
  {
    pOC_DC[0][i]->m_OneRingEdges[0]->m_pMatch = pOC_DC[1][i]->m_OneRingEdges[0];
    pOC_DC[1][i]->m_OneRingEdges[0]->m_pMatch = pOC_DC[0][i]->m_OneRingEdges[0];
  }

  //destroy the vertex pool
  delete[] pVertexPool;
  return nRetCount;
}

//------------------------------------------------------------------------
//Create edges for the given array of vertices.
void vtkALBAPolyDataDeformation_M2::CreateCurveEdges(CSkeletonVertex** pVerts, int nVerts)
//------------------------------------------------------------------------
{
  for (int i = 0; i < nVerts - 1; i++) {  //create left to right edges
    pVerts[i]->m_OneRingEdges.push_back(new CSkeletonEdge(pVerts[i], pVerts[i + 1]));
  }

  for (int i = 1; i < nVerts; i++) {    //create backward edges
    pVerts[i]->m_OneRingEdges.push_back(pVerts[i - 1]->m_OneRingEdges[0]);    
  }
}

//------------------------------------------------------------------------
//Gets the number of vertices belonging to the given curve.
//N.B. curve must be compatible with curves constructed by CreateCurveEdges.
int vtkALBAPolyDataDeformation_M2::GetNumberOfCurveVertices(CSkeletonVertex* pCurve)
//------------------------------------------------------------------------
{
  int nCount = 0;  
  while(pCurve != NULL)
  {
    pCurve = GetNextCurveVertex(pCurve);
    nCount++;
  }  

  return nCount;
}

//------------------------------------------------------------------------
//Refines the given curve (and its corresponding one) by adding more vertices.
//Every skeleton edge larger than sqrt(dblEdgeFactor) is split recursively into 
//two shorter edges. The matching curve is split appropriately.
//N.B. the routine is intended to be called after CreateCurveEdges
void vtkALBAPolyDataDeformation_M2::RefineCurve(CSkeletonVertex* pCurve, double dblEdgeFactor)
//------------------------------------------------------------------------
{
  CSkeletonEdge* pOCEdge = pCurve->m_OneRingEdges[0];
  while (pOCEdge != NULL)
  {    
    CSkeletonEdge* pDCEdge = pOCEdge->m_pMatch;

    double dblLen1 = vtkMath::Distance2BetweenPoints(
      pOCEdge->m_Verts[0]->m_Coords, pOCEdge->m_Verts[1]->m_Coords);
    double dblLen2 = vtkMath::Distance2BetweenPoints(
      pDCEdge->m_Verts[0]->m_Coords, pDCEdge->m_Verts[1]->m_Coords);

    if (dblLen1 <= dblEdgeFactor && dblLen2 <= dblEdgeFactor)
    {
      //the edge is small enough, so we can continue
      pOCEdge = GetNextCurveEdge(pOCEdge);
      continue;
    }

    //the edge must be split into two parts => create a new vertex
    CSkeletonVertex* pNewOCVert = new CSkeletonVertex();
    CSkeletonVertex* pNewDCVert = new CSkeletonVertex();
    for (int j = 0; j < 3; j++)
    {
      pNewOCVert->m_Coords[j] = (pOCEdge->m_Verts[0]->m_Coords[j] + 
        pOCEdge->m_Verts[1]->m_Coords[j]) / 2;
      pNewDCVert->m_Coords[j] = (pDCEdge->m_Verts[0]->m_Coords[j] + 
        pDCEdge->m_Verts[1]->m_Coords[j]) / 2;
    }
    
    pNewOCVert->m_WT = pNewDCVert->m_WT = 1;
    pNewOCVert->m_pMatch = pNewDCVert;
    pNewDCVert->m_pMatch = pNewOCVert;

    CSkeletonEdge* pNewOCEdge = new CSkeletonEdge(pNewOCVert, pOCEdge->m_Verts[1]);
    CSkeletonEdge* pNewDCEdge = new CSkeletonEdge(pNewDCVert, pDCEdge->m_Verts[1]);    
    pNewOCEdge->m_pMatch = pNewDCEdge;
    pNewDCEdge->m_pMatch = pNewOCEdge;

    pNewOCVert->m_OneRingEdges.push_back(pNewOCEdge);
    pNewOCVert->m_OneRingEdges.push_back(pOCEdge);
    pNewDCVert->m_OneRingEdges.push_back(pNewDCEdge);
    pNewDCVert->m_OneRingEdges.push_back(pDCEdge);

    pOCEdge->m_Verts[1]->m_OneRingEdges.pop_back();
    pOCEdge->m_Verts[1]->m_OneRingEdges.push_back(pNewOCEdge);
    pDCEdge->m_Verts[1]->m_OneRingEdges.pop_back();
    pDCEdge->m_Verts[1]->m_OneRingEdges.push_back(pNewDCEdge);

    pOCEdge->m_Verts[1] = pNewOCVert;
    pDCEdge->m_Verts[1] = pNewDCVert;    
  }
}

//------------------------------------------------------------------------
//Stores vertices and edges from the given curve and the matched one into the superskeleton 
//The routine also constructs automatically joints for end-points of curves.
//N.B. both curves must be compatible with curves constructed by CreateCurveEdges.
void vtkALBAPolyDataDeformation_M2::AddCurveToSuperSkeleton(CSkeletonVertex* pOCCurve)
//------------------------------------------------------------------------
{
  int nPoints = (int)SuperSkeleton->m_pOC_Skel->m_Vertices.size();  
  int nNextEdgeId = (int)SuperSkeleton->m_pOC_Skel->m_Edges.size();
  int nNextPtId = nPoints;

  CSkeletonVertex* pEndPoints[2];
  pEndPoints[0] = pOCCurve;

  while (pOCCurve != NULL)
  {
    //insert vertex
    CSkeletonVertex* pDCCurve = pOCCurve->m_pMatch;
    pDCCurve->m_Id = pOCCurve->m_Id = nNextPtId;
    SuperSkeleton->m_pOC_Skel->m_Vertices.push_back(pOCCurve);    
    SuperSkeleton->m_pDC_Skel->m_Vertices.push_back(pDCCurve);    
    nNextPtId++;

    //insert the next edge; if we are in the last vertex, 
    //m_OneRingEdges[0] refers to the previous edge already inserted.
    CSkeletonEdge* pEdge = pOCCurve->m_OneRingEdges[0];
    if (pEdge->m_Id < 0)
    {
      pEdge->m_Id = nNextEdgeId;
      SuperSkeleton->m_pOC_Skel->m_Edges.push_back(pEdge);

      pEdge = pEdge->m_pMatch;
      pEdge->m_Id = nNextEdgeId++;
      SuperSkeleton->m_pDC_Skel->m_Edges.push_back(pEdge);
    }

    pEndPoints[1] = pOCCurve;
    pOCCurve = GetNextCurveVertex(pOCCurve);
  }

  //pEndPoints now stores end-points of the original curve  
  int iVPos = 0;
  while (iVPos < nPoints)
  {
    CSkeletonVertex* pCurEndPoints[2];
    pCurEndPoints[0] = SuperSkeleton->m_pOC_Skel->m_Vertices[iVPos];
    iVPos += GetNumberOfCurveVertices(pCurEndPoints[0]);
    pCurEndPoints[1] = SuperSkeleton->m_pOC_Skel->m_Vertices[iVPos - 1];
 
    for (int i = 0; i < 2; i++)
    {
      if (pEndPoints[i]->m_WT <= 1.0)
        continue; //this vertex cannot have joints

      for (int j = 0; j < 2; j++)
      {
        if (pCurEndPoints[j]->m_WT <= 1.0)
          continue; //this vertex cannot have joints

        if (
          pEndPoints[i]->m_Coords[0] == pCurEndPoints[j]->m_Coords[0] &&
          pEndPoints[i]->m_Coords[1] == pCurEndPoints[j]->m_Coords[1] &&
          pEndPoints[i]->m_Coords[2] == pCurEndPoints[j]->m_Coords[2]
          )
        {
          //those points match => establish links between them
          pEndPoints[i]->m_JoinedVertices.push_back(pCurEndPoints[j]);
          pCurEndPoints[j]->m_JoinedVertices.push_back(pEndPoints[i]);

          pEndPoints[i]->m_pMatch->m_JoinedVertices.push_back(pCurEndPoints[j]->m_pMatch);
          pCurEndPoints[j]->m_pMatch->m_JoinedVertices.push_back(pEndPoints[i]->m_pMatch);
        }
      }
    }
  }  
}

//------------------------------------------------------------------------
//Computes the local frame systems for both curves.
//N.B. both curves must be compatible with curves constructed by CreateCurveEdges
//and the links must be established between them
//
//The algorithm is based on the paper: Blanco FR, Oliveira MM: Instant mesh deformation.
//In: Proceedings of the 2008 symposium on Interactive 3D graphics and games,
//Redwood City, California, 2008, ptEnd. 71-78
void vtkALBAPolyDataDeformation_M2::ComputeLFS(CSkeletonVertex* pOC)
//------------------------------------------------------------------------
{
  CSkeletonVertex* pDC = pOC->m_pMatch;

  //compute the LFS for the first point
  //u is the tangent vector
  CSkeletonVertex::LOCAL_FRAME& lf = pOC->m_LF;  
  CSkeletonVertex* pNextVertex = GetNextCurveVertex(pOC);
  for (int i = 0; i < 3; i++) {  
    lf.u[i] = pNextVertex->m_Coords[i] - pOC->m_Coords[i];    
  }

  //v is a projection into one of of XZ, XY or YZ plane + 90 degrees rotation
  //the optimal plane is the one closest to the plane where u lies
  int iPlane = 0;
  for (int i = 1; i < 3; i++) {
    if (fabs(lf.u[i]) < fabs(lf.u[iPlane]))
      iPlane = i; //new minimum
  }
    
  int i1 = (iPlane + 1) % 3;
  int i2 = (iPlane + 2) % 3;
  lf.v[i1] = lf.u[i2];
  lf.v[i2] = -lf.u[i1];
  lf.v[iPlane] = 0.0;

  //w is perpendicular to both vectors
  lf.w[i1] = 0.0;
  lf.w[i2] = 0.0;
  lf.w[iPlane] = lf.u[i1]*lf.u[i1] + lf.u[i2]*lf.u[i2];

  vtkMath::Normalize(lf.u);
  vtkMath::Normalize(lf.v);
  vtkMath::Normalize(lf.w);

  //compute the local frame system for the first point
  //of the deformed curve; it will be a bit different
  CSkeletonVertex::LOCAL_FRAME& dlf = pDC->m_LF;
  pNextVertex = GetNextCurveVertex(pDC);
  for (int i = 0; i < 3; i++) {  
    dlf.u[i] = pNextVertex->m_Coords[i] - pDC->m_Coords[i];
  }
  vtkMath::Normalize(dlf.u);  
  double dblMaxR = 0.0;
  for (int i = 0; i < 3; i++) 
  {
    double dblR = fabs(dlf.u[i] - lf.u[i]);
    if (dblMaxR < dblR)
      dblMaxR = dblR;
  }


  if (dblMaxR < 1e-5)
  {
    //lf.u and dlf.u are colinear vectors => use the same system
    pDC->m_LF = pOC->m_LF;
  }
  else
  {  
    //vectors v' and w' of the deformed curve are obtained 
    //from v and w of the original curve by rotating them around 
    //the vector r = u x u' by angle between u and u'    
    double r[3], M[3][3];
    vtkMath::Cross(dlf.u, lf.u, r); 
    vtkMath::Normalize(r);
    BuildGeneralRotationMatrix(r, vtkMath::Dot(lf.u, dlf.u), M);    

    for (int i = 0; i < 3; i++) 
    {  
      dlf.w[i] = dlf.v[i] = 0.0;
      for (int j = 0; j < 3; j++)
      {
        dlf.v[i] += lf.v[j]*M[j][i];
        dlf.w[i] += lf.w[j]*M[j][i];
      }
    }

    vtkMath::Normalize(dlf.v);
    vtkMath::Normalize(dlf.w);
  }

  //and now compute LF for every other vertex of both curves
  for (int iCurve = 0; iCurve < 2; iCurve++)
  {
    CSkeletonVertex* pPrevCurve = iCurve == 0 ? pOC : pDC;    
    CSkeletonVertex* pCurve = GetNextCurveVertex(pPrevCurve);
    while (pCurve != NULL)    
    {
      pNextVertex = GetNextCurveVertex(pCurve);

      //compute the tangent vector at pCurve[i]
      //if this is the last vertex of curve, then the previous edge
      //is parallel with this vector
      //N.B. tangent vector must be parallel with the direction of edge pCurve, pNextVertex
      //otherwise we will have sudden jumps in the deformed mesh!
      const CSkeletonVertex::LOCAL_FRAME& prev_lf = pPrevCurve->m_LF;
      CSkeletonVertex::LOCAL_FRAME& lf = pCurve->m_LF;      

      if (pNextVertex == NULL)
      {
        for (int j = 0; j < 3; j++)  { 
          lf.u[j] = pCurve->m_Coords[j] - pPrevCurve->m_Coords[j];
        }
      }
      else
      {
        for (int j = 0; j < 3; j++)  { 
          lf.u[j] = pNextVertex->m_Coords[j] - pCurve->m_Coords[j];
        }
      }

      vtkMath::Normalize(lf.u);

      //compute v(i) by the projection of the previous v(i-1) onto the plane that is defined by
      //the normal u(i) and the current point pCurve(i)
      //according to P.Schneider: Geometric Tools for Computer Graphics, pg. 665
      //projection of vector v onto plane P*n + d = 0 can be computed as
      //v - (v*n)*n assuming that vector n is normalized
      double dblVal = vtkMath::Dot(prev_lf.v, lf.u);
      for (int j = 0; j < 3; j++) {
        lf.v[j] = prev_lf.v[j] - dblVal*lf.u[j];
      }

      vtkMath::Normalize(lf.v);

      //compute w = u x v
      vtkMath::Cross(lf.u, lf.v, lf.w);
      vtkMath::Normalize(lf.w);

      //if the angle between w(i-1) and w(i) is greater than 90 degrees
      //change the sign of w(i)
      if (vtkMath::Dot(lf.w, prev_lf.w) < 0)
      {
        for (int j = 0; j < 3; j++) {
          lf.w[j] = -lf.w[j];
        }
      }

      pPrevCurve = pCurve;
      pCurve = pNextVertex;
    } //end while
  } //end for
}

//------------------------------------------------------------------------
//Computes separating planes for every vertex on both curves, the original
//and the deformed ones. The original curve starts at the given vertex.
//This routine is supposed to be called together with ComputeLFS. A planes
//is computed for each pair of edges adjacent to every vertex on the curve.
//Joined edges are also taken into account. The plane is always
//perpendicular to both edges and the axis of both edges lies on the plane
void vtkALBAPolyDataDeformation_M2::ComputeSeparatingPlanes(CSkeletonVertex* pOC_Curve)
//------------------------------------------------------------------------
{ 
  for (int iCurve = 0; iCurve < 2; iCurve++)
  {
    CSkeletonVertex* pSkelVert = iCurve == 0 ? pOC_Curve : pOC_Curve->m_pMatch;
    CSkeletonVertex* pPrevVert = GetPreviousCurveVertex(pSkelVert);

    while (pSkelVert != NULL)
    {
      CSkeletonVertex* pNextVert = GetNextCurveVertex(pSkelVert);  

      int nJoints = pSkelVert->GetNumberOfJoinedVertices();
      pSkelVert->m_nPlanes = nJoints == 0 ? 1 : nJoints;  //1 if number is 0, otherwise the number

      _ASSERT(pSkelVert->m_pPlaneNormals == NULL);

      pSkelVert->m_pPlaneNormals = new CSkeletonVertex::VCoord[pSkelVert->m_nPlanes];

      if (pPrevVert != NULL && pNextVert != NULL) //inner vertex
        ComputeSeparatingPlane(pPrevVert, pSkelVert, pNextVert, pSkelVert->m_pPlaneNormals[0]);
      else if (nJoints == 0)
      {
        //the vertex is the first or last one and it is not a joint
        //=> plane is equivalent to LF.u
        for (int i = 0; i < 3; i++) {
          pSkelVert->m_pPlaneNormals[0][i] = pSkelVert->m_LF.u[i];
        }
      }
      else
      { 
        //it is a terminal vertex with joined vertices    
        for (int i = 0; i < nJoints; i++)
        {
          CSkeletonVertex* pVert = pSkelVert->m_JoinedVertices[i];
          CSkeletonEdge* pNbEdge = pVert->m_OneRingEdges[0];
          pVert = pNbEdge->m_Verts[0] == pVert ? 
            pNbEdge->m_Verts[1] : pNbEdge->m_Verts[0];

          if (pPrevVert == NULL)  //we are first
            ComputeSeparatingPlane(pVert, pSkelVert, pNextVert, pSkelVert->m_pPlaneNormals[i]);
          else
            ComputeSeparatingPlane(pPrevVert, pSkelVert, pVert, pSkelVert->m_pPlaneNormals[i]);
        }  
      } //end else (pPrevVert != NULL && pNextVert != NULL)
      pPrevVert = pSkelVert; 
      pSkelVert = pNextVert;
    }
  } //end for iCurve
}

//------------------------------------------------------------------------
//Computes normal of the separating plane at pCenterVert.
//Vertices pVert1, pCenterVert2 and pVert2 form a polyline composed of two 
//edges. The plane is perpendicular to both edges and the axis of both 
//edges lies on the plane. The angle between the computed normal and the 
//vector pVert1 - pCenterVert2 is not obtuse (i.e., the normal directs
//the same way as the other vector). The returned normal is normalized.
void vtkALBAPolyDataDeformation_M2::ComputeSeparatingPlane(CSkeletonVertex* pVert1, 
  CSkeletonVertex* pCenterVert, CSkeletonVertex* pVert2, double* out_normal)
//------------------------------------------------------------------------
{
  double u1[3], u2[3], v[3], w[3];
  for (int i = 0; i < 3; i++)
  {
    u1[i] = pVert1->m_Coords[i] - pCenterVert->m_Coords[i];
    u2[i] = pVert2->m_Coords[i] - pCenterVert->m_Coords[i];
  }

  vtkMath::Normalize(u1);
  vtkMath::Normalize(u2);
  vtkMath::Cross(u1, u2, v);  //perpendicular to both and also to the normal
  if (vtkMath::Norm(v) < 1e-8)
  {
    //u1 and u2 are collinear, i.e., all three vectors lie on the common line
    for (int i = 0; i < 3; i++){
      out_normal[i] = -u1[i];
    }
  }
  else
  {
    //compute axis between edges
    for (int i = 0; i < 3; i++){
      w[i] = (u1[i] + u2[i]) / 2;
    }

    vtkMath::Cross(v, w, out_normal);

    //normal must direct in the same way as the edge vector -u1
    if (vtkMath::Dot(out_normal, u1) > 0)
    {
      for (int i = 0; i < 3; i++){
        out_normal[i] = -out_normal[i];
      }
    }  
  }

  vtkMath::Normalize(out_normal);

//
//_RPT3(_CRT_WARN, "V1 %g %g %g\n", pVert1->m_Coords[0], pVert1->m_Coords[1], pVert1->m_Coords[2]);
//_RPT3(_CRT_WARN, "VC %g %g %g\n", pCenterVert->m_Coords[0], pCenterVert->m_Coords[1], pCenterVert->m_Coords[2]);
//_RPT3(_CRT_WARN, "V2 %g %g %g\n", pVert2->m_Coords[0], pVert2->m_Coords[1], pVert2->m_Coords[2]);
//_RPT3(_CRT_WARN, "v %g %g %g\n", v[0], v[1], v[2]);
//_RPT3(_CRT_WARN, "w %g %g %g\n", w[0], w[1], w[2]);
//_RPT3(_CRT_WARN, "n %g %g %g\n", out_normal[0], out_normal[1], out_normal[2]);
}

//------------------------------------------------------------------------
//Computes the region of interest (ROI) for the given edge of curve.
//It detects every mesh vertex in the zone of influence of the given edge. 
//Note: one of end-points of the edge may be NULL to specify half-space
void vtkALBAPolyDataDeformation_M2::ComputeROI(CSkeletonEdge* pEdge)
//------------------------------------------------------------------------
{  
  vtkPoints* input = vtkPolyData::SafeDownCast(GetInput())->GetPoints();
  int nPoints = input->GetNumberOfPoints();
  
  pEdge->m_ROI.clear();           //clear previous data (if present)
  pEdge->m_ROI.reserve(nPoints);  //reserve memory for   
  for (int ptId = 0; ptId < nPoints; ptId++)
  {
    bool bPointValid = true;
    double dblCoef = 1.0;

    double* pcoords = input->GetPoint(ptId);     
    for (int i = 0; i < 2 && bPointValid; i++)
    {
      if (pEdge->m_Verts[i] != NULL)
      {
        //compute the location of the current point
        double u[3];
        for (int j = 0; j < 3; j++){
          u[j] = pcoords[j] - pEdge->m_Verts[i]->m_Coords[j];
        }

        int nPlanes = pEdge->m_Verts[i]->m_nPlanes;
        for (int j = 0; j < nPlanes; j++)
        {
          //compute whether the point lies in the side defined by normal
          double dblDot = vtkMath::Dot(u, pEdge->m_Verts[i]->m_pPlaneNormals[j]);
          if (dblCoef*dblDot < 0) { //the point is not inside all planes
            bPointValid = false; break;
          }
        }
      }

      //normals at the other point must be inverted
      dblCoef = -1.0;
    }         
    
    //if the point is delimited by all planes, we have found it
    if (bPointValid)       
      pEdge->m_ROI.push_back(ptId);
  }

  pEdge->m_ROI.resize(pEdge->m_ROI.size());
}
  
//------------------------------------------------------------------------
//Refines ROIs for curves.
//Refines the mapping of vertices of the input mesh to edges in such a manner 
//that every vertex is present only in one edge ROI. 
void vtkALBAPolyDataDeformation_M2::RefineCurveROIs()
//------------------------------------------------------------------------
{
  //this detection will need a cell locator  
  //octree based locator for cells of the input mesh should be enough
  vtkCellLocator* cellLocator = vtkCellLocator::New();    
  cellLocator->SetDataSet(vtkDataSet::SafeDownCast(GetInput()));
  cellLocator->Update();
  
  vtkPoints* input = vtkPolyData::SafeDownCast(GetInput())->GetPoints();
  int nPoints = input->GetNumberOfPoints(); 

  //number of edges to which points are mapped at present
  int* pPtMapCounts = new int[nPoints];
  memset(pPtMapCounts, 0, nPoints*sizeof(int));  

  int nEdges = (int)SuperSkeleton->m_pOC_Skel->m_Edges.size();
  for (int i = 0; i < nEdges; i++)
  {
    CSkeletonEdge* pEdge = SuperSkeleton->m_pOC_Skel->m_Edges[i];
    int nCount = (int)pEdge->m_ROI.size();

    for (int j = 0; j < nCount; j++) {
      pPtMapCounts[pEdge->m_ROI[j]]++;
    }
  }

  //the best mapping for points
  CSkeletonEdge** pBestEdges = new CSkeletonEdge*[nPoints];
  double* pBestDistances = new double[nPoints];  

  for (int i = 0; i < nPoints; i++) {
    pBestDistances[i] = DBL_MAX;
  }  
  
  //compute best distance map for every point   
  for (int i = 0; i < nEdges; i++)
  {
    CSkeletonEdge* pEdge = SuperSkeleton->m_pOC_Skel->m_Edges[i];
    int nCount = (int)pEdge->m_ROI.size();
    
    CSkeletonVertex* pSkelVert1 = pEdge->m_Verts[0]; 
    CSkeletonVertex* pSkelVert2 = pEdge->m_Verts[1];
    
    //get edge status
    bool bInfEdge;
    double dblEdgeLength;
    CSkeletonVertex* pSkelVert;
    if (pSkelVert1 == NULL)
    {
      bInfEdge = true;
      pSkelVert = pSkelVert2;
      dblEdgeLength = DBL_MAX;
    }
    else if (pSkelVert2 == NULL)
    {
      bInfEdge = true;
      pSkelVert = pSkelVert1;
      dblEdgeLength = DBL_MAX;
    }
    else
    {
      bInfEdge = false;
      pSkelVert = pSkelVert1;
      dblEdgeLength = sqrt(vtkMath::Distance2BetweenPoints(
        pSkelVert1->m_Coords, pSkelVert2->m_Coords));
    }

    for (int j = 0; j < nCount; j++)
    {
      int nPtId = pEdge->m_ROI[j];
      
      //if there is just one edge to which the current point is mapped
      //there is no need to measure distances
      if (pPtMapCounts[nPtId] == 1)
      {
        pBestEdges[nPtId] = pEdge;
        continue; //this is simple      
      }      

      //more than one valid edge => we need to compute the weight
      double ptEnd[3];
      double* pcoords = input->GetPoint(nPtId);
      
      if (bInfEdge)
      {
        //an infinite edge => weight is the distance of the current point P
        //to the existing end point
        for (int k = 0; k < 3; k++) {
          ptEnd[k] = pSkelVert->m_Coords[k];
        }
      }
      else
      {
        //edge has both end-points => weight is the distance of 
        //the current point P to the current edge; the closest point on the edge 
        //can be computed as: Pk + (u*(P - Pk))*u, where u is normalized
        for (int k = 0; k < 3; k++) {                  
          ptEnd[k] = pcoords[k] - pSkelVert->m_Coords[k];
        }

        double w = vtkMath::Dot(pSkelVert->m_LF.u, ptEnd);
        if (w < 0.0)  
        { //we are before the edge => the first node is the closest one
          for (int k = 0; k < 3; k++) {
            ptEnd[k] = pSkelVert1->m_Coords[k];
          }
        }          
        else if (w > dblEdgeLength) 
        {
          //we are after the edge => the other node is the closest one
          for (int k = 0; k < 3; k++) {
            ptEnd[k] = pSkelVert2->m_Coords[k];
          }
        }
        else
        {
          //closest point lies on the edge
          for (int k = 0; k < 3; k++) {
            ptEnd[k] = pSkelVert->m_Coords[k] + w*pSkelVert->m_LF.u[k];
          }
        }
      }
      
      //we need to compute distance between ptStart and ptEnd points
      //the Euclidian distance, however, is not the optimal measurement 
      //as it can lead into wrong assignment of points. For instance, if
      //the skeleton is not perfectly centered, points of one leg can be
      //mapped onto an edge belonging to the other leg.
      double dblWeight = GetDistance(nPtId, ptEnd, 
        cellLocator, pBestDistances[nPtId]);
  
      if (dblWeight < pBestDistances[nPtId])
      {
        pBestEdges[nPtId] = pEdge;
        pBestDistances[nPtId] = dblWeight;
      }      
    } //end for j
  } //end for i
 
  delete[] pBestDistances;
  delete[] pPtMapCounts;    


  //last step is to update ROIs of edges
  for (int i = 0; i < nEdges; i++)
  {
    CSkeletonEdge* pEdge = SuperSkeleton->m_pOC_Skel->m_Edges[i];
    int nCount = (int)pEdge->m_ROI.size();

    std::vector< vtkIdType > newROI;
    newROI.reserve(pEdge->m_ROI.size());

    for (int j = 0; j < nCount; j++)
    {      
      vtkIdType nPtId = pEdge->m_ROI[j];
      if (pEdge == pBestEdges[nPtId])
        newROI.push_back(nPtId);
    }

    pEdge->m_ROI.assign(newROI.begin(), newROI.end());    
    pEdge->m_ROI.resize(pEdge->m_ROI.size());
  }   

  delete[] pBestEdges;
  cellLocator->Delete();  //no longer needed
}

//------------------------------------------------------------------------
//Compute an approximate geodesic distance between two points.
//If the straight line between both points does not intersect the input mesh,
//the returned distance is the Euclidian distance between those two points;
//otherwise it is the sum of the length of the shortest (surface) path from 
//nPtStartId to the surface point closest to the intersection and the distance
//that returned by GetDistance with the first parameter to be the surface 
//point closest to the intersection. If the total distance during the 
//computation exceeds the given dblMaxDist, the algorithm stops (and returns
//the distance measured so far) - this is to speed up the process.
//N.B. cellLocator must be initialized with the input mesh.
double vtkALBAPolyDataDeformation_M2::GetDistance( vtkIdType nPtStartId, 
          double ptEnd[3], vtkCellLocator* cellLocator, double dblMaxDist )
//------------------------------------------------------------------------
{
  const static double dblStep = 0.01;   //constant sampling
  vtkPolyData* input = vtkPolyData::SafeDownCast(GetInput());

  double ptStart[3];
  input->GetPoint(nPtStartId, ptStart);

  //this is just to speed-up the search for intersection
  vtkGenericCell* cellLoc = vtkGenericCell::New();

  int nCellId, nSubId;
  double x[3], par_x[3];
  double dblRetDist = 0.0, t = 0.0;
  while (dblRetDist < dblMaxDist)     //search is limited
  {   
    //compute the straight distance
    double dblDist = sqrt(vtkMath::Distance2BetweenPoints(ptStart, ptEnd));

    //if even straight distance is bad, then it is pointless to search for
    //more precise distance
    if (dblRetDist + dblDist >= dblMaxDist) {  
      dblRetDist += dblDist; break;
    }
        
    //move towards ptEnd to avoid immediate detection  
    t += dblStep;  //step inside
    for (int k = 0; k < 3; k++) {
      ptStart[k] += t*(ptEnd[k] - ptStart[k]);
    }
 
    //checks the intersection of that line
    //IntersectWithLine is not well documented, I will try it
    if (0 == cellLocator->IntersectWithLine(
          ptStart, ptEnd, 0.0,  //IN: end-points of line segment and tolerance
          t, x, par_x,          //OUT: time of intersection and position 
          //of intersection in global coordinates and parametric coordinates
          nSubId, nCellId,      //id of intersected cell and its face
          cellLoc)
      )
    {
      //no intersection
      dblRetDist += dblDist;
      break;
    }
    else
    {
      //there is an intersection (at x) with the mesh
      //find the closest mesh point 
      vtkIdType nPtIds, *pPtIds;
      input->GetCellPoints(nCellId, nPtIds, pPtIds);

      int iMinPos = 0;
      double dblMinDist = vtkMath::Distance2BetweenPoints(x, input->GetPoint(pPtIds[0]));
      for (int i = 1; i < nPtIds; i++)
      {
        double dblDist = vtkMath::Distance2BetweenPoints(x, input->GetPoint(pPtIds[i]));
        if (dblDist < dblMinDist) {
          iMinPos = i; dblMinDist = dblDist;
        }
      }
      
      //now we have the end point
      vtkIdType nNextPtId = pPtIds[iMinPos];
      dblDist = GetPathLength(nPtStartId, nNextPtId, dblMaxDist);
      dblRetDist += dblDist;

      //advance to the next position
      nPtStartId = nNextPtId;
      for (int i = 0; i < 3; i++){
        ptStart[i] = x[i];
      }
    } //end else 0 == IntersectWithLine
  } //end while

  cellLoc->Delete();
  return dblRetDist;
}

//this structure is used in GetPathLength
typedef struct vtkALBAPolyDataDeformation_M2::DIJKSTRA_ITEM
{
  //vtkIdType nId;      //<pointId
  double dblWeight;   //<weight

  DIJKSTRA_ITEM* pPrev; //<double linked list ordered by dblWeight
  DIJKSTRA_ITEM* pNext;
} DIJKSTRA_ITEM;

//------------------------------------------------------------------------
//Computes the length of the path between nPtFrom to nPtTo vertices.
//The computation is not precise (because of speed). If there is no path, or the
//path would be too long, the routine returns dblMaxDist. 
//N.B. the found path may not be the shortest one!
double vtkALBAPolyDataDeformation_M2::GetPathLength(vtkIdType nPtFrom, 
                                      vtkIdType nPtTo, double dblMaxDist)
//------------------------------------------------------------------------
{
  vtkPolyData* input = vtkPolyData::SafeDownCast(GetInput());
  int nPoints = input->GetNumberOfPoints();

  //initialize path distance
  DIJKSTRA_ITEM* pDIPool = new DIJKSTRA_ITEM[nPoints];
  for (int i = 0; i < nPoints; i++)
  {
    //pDIPool[i].nId = i;
    pDIPool[i].dblWeight = dblMaxDist;
    pDIPool[i].pNext = &pDIPool[i + 1];
    pDIPool[i].pPrev = &pDIPool[i - 1];
  }

  pDIPool[nPoints - 1].pNext = NULL;       
  if (nPtTo != 0)
  {
    //move nPtFrom to the head position
    pDIPool[nPtFrom - 1].pNext = pDIPool[nPtFrom].pNext;
    if (pDIPool[nPtFrom].pNext != NULL)
      pDIPool[nPtFrom].pNext->pPrev = pDIPool[nPtFrom].pPrev;

    pDIPool[nPtFrom].pNext = &pDIPool[0];
    pDIPool[0].pPrev = &pDIPool[nPtFrom];
  }
  
  DIJKSTRA_ITEM* pHead = &pDIPool[nPtFrom];
  pHead->dblWeight = 0.0;
  pHead->pPrev = NULL;
    
  //while there is anything in the queue
  while (pHead != NULL)
  {
    //get the top item from the queue, say it is vertex u
    DIJKSTRA_ITEM* pCurItem = pHead;
    pHead = pHead->pNext;
    if (pHead != NULL)
      pHead->pPrev = NULL;

    vtkIdType nCurItemId = static_cast<vtkIdType>(pCurItem - pDIPool);

    double coords[3];
    input->GetPoint(nCurItemId, coords);

    //now get all neighbors for the vertex u
    vtkIdType *pCellsIds;
    unsigned short nCellsIds;
    input->GetPointCells(nCurItemId, nCellsIds, pCellsIds);
    for (int i = 0; i < nCellsIds; i++)
    {
      vtkIdType nPtsIds, *pPtsIds;
      input->GetCellPoints(pCellsIds[i], nPtsIds, pPtsIds);
      for (int j = 0; j < nPtsIds; j++)
      {
        if (pPtsIds[j] == nCurItemId)
        {
          //cells fill the area around the vertex fully, so we need
          //to process only one edge per cell as the other will be
          //processed in the next iteration
          vtkIdType nNbPtId = pPtsIds[(j + 1) % nPtsIds];
          double dblDist = sqrt(vtkMath::Distance2BetweenPoints(coords,            
            input->GetPoint(nNbPtId))) + pCurItem->dblWeight;

          DIJKSTRA_ITEM* pNbItem = &pDIPool[nNbPtId];
          if (pNbItem->dblWeight > dblDist)
          {
            //we have found shorter path => update it            
            if (nNbPtId == nPtTo)
            {
              //if we reached the destination, exit
              delete[] pDIPool;
              return dblDist;
            }

            pNbItem->dblWeight = dblDist;
            if (pNbItem != pHead)
            {              
              //if we are not already at head of the list, we will need to
              //move the item to its new ordered position
              DIJKSTRA_ITEM* pAfter = pNbItem->pPrev;

              while (pAfter != NULL && dblDist < pAfter->dblWeight){
                pAfter = pAfter->pPrev;
              }
            
              //pNbItem should be after pAfter, check if it needs change
              if (pAfter != pNbItem->pPrev)
              {
                //we need to remove pNbItem from the list and insert it
                //before pAfter item (pNbItem is not head)
                if (pNbItem->pNext != NULL)
                  pNbItem->pNext->pPrev = pNbItem->pPrev;
                pNbItem->pPrev->pNext = pNbItem->pNext;

                pNbItem->pPrev = pAfter;
                if (pAfter == NULL)
                {                  
                  pNbItem->pNext = pHead;
                  pHead->pPrev = pNbItem;
                  pHead = pNbItem;
                }
                else
                {                  
                  pNbItem->pNext = pAfter->pNext;
                  if (pAfter->pNext != NULL)
                    pAfter->pNext->pPrev = pNbItem;
                  pAfter->pNext = pNbItem;
                }
              }
            } //end if (pNbItem != pHead)
          } //end if (pNbItem->dblWeight > dblDist)

          break;  //continue with the next cell
        } //end if (pPtsIds[j] == pCurItem->nId)
      } //end for points of the current cell
    } //end for cells around the current vertex
  } //end while (pHead != NULL)

  double dblRet = pDIPool[nPtTo].dblWeight;    
  delete[] pDIPool;
  return dblRet;
}

//------------------------------------------------------------------------
//Constructs the matrix for rotation of vectors around the vector r by the
//angle theta. The angle is given indirectly as cos(theta).
//N.B. vector r must be normalized. 
void vtkALBAPolyDataDeformation_M2
::BuildGeneralRotationMatrix(double r[3], double cos_theta, double M[3][3])
//------------------------------------------------------------------------
{
  //the matrix for the rotation can be computed by procedure given in
  //Bloomenthal J. Calculation of reference frames along a
  //space curve. Graphics Gems I, 1990, 567–571.
  //http://www.unchainedgeometry.com/jbloom/pdf/ref-frames.pdf
  //or also by Schneider: Geometric Tools for Computer Games

  double sqx = r[0]*r[0];
  double sqy = r[1]*r[1];
  double sqz = r[2]*r[2];

  double sin_w = sqrt(1.0 - cos_theta*cos_theta);
  double cos_w1 = 1.0 - cos_theta;

  double xycos1 = r[0]*r[1]*cos_w1;
  double yzcos1 = r[1]*r[2]*cos_w1;
  double zxcos1 = r[2]*r[0]*cos_w1;

  double xsin = r[0]*sin_w;
  double ysin = r[1]*sin_w;
  double zsin = r[2]*sin_w;


  M[0][0] = sqx + (1.0 - sqx)*cos_theta;
  M[1][0] = xycos1 + zsin;
  M[2][0] = zxcos1 - ysin;

  M[0][1] = xycos1 - zsin;
  M[1][1] = sqy + (1.0 - sqy)*cos_theta;
  M[2][1] = yzcos1 + xsin;

  M[0][2] = zxcos1 + ysin;
  M[1][2] = yzcos1 - xsin;
  M[2][2] = sqz + (1 - sqz)*cos_theta;
}

//------------------------------------------------------------------------
//Parametrize the input mesh using the super-skeleton.
//N.B. edges ROI must be build and refined before this routine may be called.
void vtkALBAPolyDataDeformation_M2::ComputeMeshParametrization()
//------------------------------------------------------------------------
{ 
  vtkPolyData* input = vtkPolyData::SafeDownCast(GetInput());
  int nCount = (int)SuperSkeleton->m_pOC_Skel->m_Edges.size();
  for (int i = 0; i < nCount; i++)
  {
    CSkeletonEdge* pEdge = SuperSkeleton->m_pOC_Skel->m_Edges[i];        

    CSkeletonVertex* pSkelVert1 = pEdge->m_Verts[0]; 
    CSkeletonVertex* pSkelVert2 = pEdge->m_Verts[1];

    //detect the edge configuration  
    CSkeletonVertex* pSkelVert = pSkelVert1 != NULL ? pSkelVert1 : pSkelVert2;
    bool bInfEdge = pSkelVert1 == NULL || pSkelVert2 == NULL;    

    int nPoints = (int)pEdge->m_ROI.size();
    for (int j = 0; j < nPoints; j++)
    {
      int nPtId = pEdge->m_ROI[j];
      CMeshVertex* pParams = &MeshVertices[nPtId];
      double* pcoords = input->GetPoint(nPtId);
      pParams->m_pEdge = pEdge;

      //compute a, b, c parametrization for pVertex such that
      //pcoords = pSkelVert->coords + a*LF.u + b*LF.v + c*LF.w
      double M[3][4];      
      for (int k = 0; k < 3; k++) 
      {
        M[k][0] = pSkelVert->m_LF.u[k];
        M[k][1] = pSkelVert->m_LF.v[k];
        M[k][2] = pSkelVert->m_LF.w[k];
        M[k][3] = pcoords[k] - pSkelVert->m_Coords[k]; 
      }

      //Gauss-Seidel elimination method      
      for (int k = 0; k < 2; k++)
      {
        //find pivot, i.e., the line with the largest value in k-th column
        int iPivot = k;
        for (int m = k + 1; m < 3; m++) {
          if (fabs(M[m][k]) > fabs(M[iPivot][k]))
            iPivot = m;
        }

        //now swap iPivot line with k-th line
        if (iPivot != k)
        {          
          for (int m = 0; m < 4; m++) 
          {
            double dblTmp = M[iPivot][m];
            M[iPivot][m] = M[k][m];
            M[k][m] = dblTmp;
          }
        }

        //eliminate all values in the current column starting from k+1 line        
        for (int m = k + 1; m < 3; m++) 
        {
          if (fabs(M[m][k]) == 0.0)
            continue; //0.0 is safe as we know that all values are in range 0..1

          //all previous columns are zeros now
          for (int n = k + 1; n < 4; n++) {
            M[m][n] = M[m][k]*M[k][n] - M[k][k]*M[m][n];
          }

          M[m][k] = 0.0;
        }
      }

      pParams->m_PCoords[2] = M[2][3] / M[2][2];
      pParams->m_PCoords[1] = (M[1][3] - pParams->m_PCoords[2]*M[1][2]) / M[1][1];
      pParams->m_PCoords[0] = (M[0][3] - pParams->m_PCoords[2]*M[0][2] - 
        pParams->m_PCoords[1]*M[0][1]) / M[0][0];

#ifdef DEBUG_vtkALBAPolyDataDeformation_M2
      double backprj[3];
      for (int k = 0; k < 3; k++)
      {
        //pSkelVert->coords + a*LF.u + b*LF.v + c*LF.w
        backprj[k] = pSkelVert->m_Coords[k] + 
          pParams->m_PCoords[0]*pSkelVert->m_LF.u[k] + 
          pParams->m_PCoords[1]*pSkelVert->m_LF.v[k] + 
          pParams->m_PCoords[2]*pSkelVert->m_LF.w[k];

        _ASSERT(fabs(backprj[k] - pcoords[k]) <= 1e-5);
      }
#endif // _DEBUG

      //detect the separating planes that are the closest
      CSkeletonVertex::VCoord vmpn[2];      
      for (int k = 0; k < 2; k++)
      {
        if (pEdge->m_Verts[k] == NULL)
          pParams->m_nPlanes[k] = -1;
        else
        {
          double dblMinDist2 = DBL_MAX;

          //compute vector Pk-P
          double ppvec[3];
          for (int m = 0; m < 3; m++){
            ppvec[m] = pEdge->m_Verts[k]->m_Coords[m] - pcoords[m];
          }

          int nPlanes = pEdge->m_Verts[k]->m_nPlanes;
          CSkeletonVertex::VCoord* pPlNormals = pEdge->m_Verts[k]->m_pPlaneNormals;
          for (int m = 0; m < nPlanes; m++)
          {
            //actually we need to find intersection of line going through the 
            //current point (P) in the direction of pEdge->m_Verts[k].m_LF.u
            //with every plane and store the one with minimal distance.
            //
            //intersection of line: X = P + u*t and plane: n*X + d = 0 
            //can be computed as P + u*ti and ti is computed:
            //n*X + d = 0 => n*(P + u*ti) + d = 0 => n*P + n*u*ti + d = 0 =>
            //ti = -(n*P + d)/n*u; as plane goes through end-point of the edge
            //d = -n*Pk => ti = n*(Pk-P) / n*u   
            //N.B. we assume that u does not lie on the plane => u*n != 0
            double ti = vtkMath::Dot(pPlNormals[m], ppvec) /
                        vtkMath::Dot(pPlNormals[m], pEdge->m_Verts[k]->m_LF.u);

            double tmp[3];
            for (int n = 0; n < 3; n++) {
              tmp[n] = pcoords[n] + ti*pEdge->m_Verts[k]->m_LF.u[n];
            }
          
            double dblDist = vtkMath::Distance2BetweenPoints(tmp, pcoords);
            if (dblDist < dblMinDist2)
            {
              //this plane is closer to the current point => update it
              vmpn[k][0] = tmp[0]; vmpn[k][1] = tmp[1]; vmpn[k][2] = tmp[2];
              pParams->m_nPlanes[k] = m;
              dblMinDist2 = dblDist;              
            }
          } //end for m (planes)
        } //end else
      } //end for k

      //now, we need to incorporate the elongation factor
      if (bInfEdge)      
        pParams->m_dblRm = 0.0;      
      else      
        pParams->m_dblRm = sqrt(vtkMath::Distance2BetweenPoints(pcoords, vmpn[0]) / 
          vtkMath::Distance2BetweenPoints(vmpn[0], vmpn[1]));            
    } //end for j (points in edge)
  } //end for i (edges)
}


//------------------------------------------------------------------------
//Computes new position of vertices in the output mesh.
//In this last step, positions of vertices are modified according to their
//parametrization in respect to the superskeleton. 
//
//N.B. the given output polydata must be compatible with the input polydata
void vtkALBAPolyDataDeformation_M2::DeformMesh(vtkPolyData* output)
//------------------------------------------------------------------------
{  
  vtkPoints* points = output->GetPoints();
  int nPoints = output->GetNumberOfPoints();
  int nCount = (int)SuperSkeleton->m_pDC_Skel->m_Edges.size();
  
  double* EdgeLengths = new double[nCount];
  double* EdgeElongations = new double[nCount];
  for (int i = 0; i < nCount; i++)
  {
    EdgeLengths[i] = SuperSkeleton->m_pDC_Skel->m_Edges[i]->GetLength();
    if (this->PreserveVolume)
    {
      EdgeElongations[i] = SuperSkeleton->m_pOC_Skel->m_Edges[i]->GetLength();
      if (EdgeElongations[i] == 0.0)
        EdgeElongations[i] = 1.0;
      else
        EdgeElongations[i] = sqrt(EdgeElongations[i] / EdgeLengths[i]);
    }
  }
  
  for (int i = 0; i < nPoints; i++)
  {
    //compute deformed coordinates for every existing parametrization 
    //of the current vertex      
    CMeshVertex* pMeshVertex = &MeshVertices[i];
        
    const double* oldCoords = points->GetPoint(i); 
    double newCoords[3];

    CSkeletonVertex* pSkelVerts[2];    
    pSkelVerts[0] = pMeshVertex->m_pEdge->m_pMatch->m_Verts[0]; 
    pSkelVerts[1] = pMeshVertex->m_pEdge->m_pMatch->m_Verts[1];     

    if (pSkelVerts[0] == NULL || pSkelVerts[1] == NULL)
    {
      //half-space skeleton edge
      //compute simple Cartesian coordinates
      CSkeletonVertex* pSkelVert = pSkelVerts[0] != NULL ? pSkelVerts[0] : pSkelVerts[1];
      for (int k = 0; k < 3; k++) 
      {
        newCoords[k] = pSkelVert->m_Coords[k] + 
          pMeshVertex->m_PCoords[0]*pSkelVert->m_LF.u[k] + 
          pMeshVertex->m_PCoords[1]*pSkelVert->m_LF.v[k] + 
          pMeshVertex->m_PCoords[2]*pSkelVert->m_LF.w[k];
      }
    }      
    else
    {
      //the control edge is well defined => we need to incorporate elongation
      //of edge, i.e., we need to compute points vmp and vmn on both planes
      //first, compute point vmo that lies on the plane with normal u1 and
      //position at Pk1 and that lies on a line parallel to u1 on which
      //the transformed point, vmp and vmn will lie
      double vmo[3];
      CSkeletonVertex::VCoord vmpn[2];       
      for (int k = 0; k < 3; k++)
      {
        //vmo = Pk1 + b*v1 + c*w1
        vmo[k] = pSkelVerts[0]->m_Coords[k] + 
          pMeshVertex->m_PCoords[1]*pSkelVerts[0]->m_LF.v[k] + 
          pMeshVertex->m_PCoords[2]*pSkelVerts[0]->m_LF.w[k];
      }      

      //vmn and vmp can be calculated as the intersection of line going 
      //through vmo in u1 direction with separating planes in points Pk1 
      //and Pk2; intersection of line: X = vmo + u*t and plane: n*X + d = 0 
      //can be computed as vmo + u*ti and ti is computed:
      //n*X + d = 0 => n*(vmo + u*ti) + d = 0 => n*P + n*u*ti + d = 0 =>
      //ti = -(n*P + d)/n*u; as plane goes through end-point of the edge
      //d = -n*Pk => ti = n*(Pk-vmo) / n*u 
      for (int k = 0; k < 2; k++)
      {
        double* n = pSkelVerts[k]->m_pPlaneNormals[pMeshVertex->m_nPlanes[k]];
        double ti = (vtkMath::Dot(n, pSkelVerts[k]->m_Coords) - 
          vtkMath::Dot(n, vmo)) /vtkMath::Dot(n, pSkelVerts[0]->m_LF.u);            

        for (int m = 0; m < 3; m++) {
          vmpn[k][m] = vmo[m] + ti*pSkelVerts[0]->m_LF.u[m];
        }
      } //end for k
          
      //set the new position of point according to the elongation (shrinkage)
      //of the current edge - just linear equation 
      double dblNewDist = sqrt(vtkMath::Distance2BetweenPoints(vmpn[0], vmpn[1]));          
      double w = pMeshVertex->m_dblRm*dblNewDist;
      
      for (int k = 0; k < 3; k++) {
        newCoords[k] = vmpn[0][k] + w*pSkelVerts[0]->m_LF.u[k];
      }  

      if (this->PreserveVolume)
      {
        //if Volume is to be preserved than the position of point will
        //be scaled so the point is farer (or closer) to the skeleton
        //NB. may not work well, if skeleton edge is not lie in the center of mesh
        //compute the point closest to the current one that lies on the line
        //supported by the current edge and then compute the distance  
        //Aubel A., Thalmann D. Efficient Muscle Shape Deformation
        //http://vrlab.epfl.ch/Publications/pdf/Aubel_Thalmann_Derformable_Avatars_01.pdf

        //the closest point on the edge can be computed as: 
        //Pk + (u*(P - Pk))*u, where u is normalized
        double ptEnd[3];
        for (int k = 0; k < 3; k++) {                  
          ptEnd[k] = newCoords[k] - pSkelVerts[0]->m_Coords[k];
        }

        double w = vtkMath::Dot(pSkelVerts[0]->m_LF.u, ptEnd);
        for (int k = 0; k < 3; k++) {
          ptEnd[k] = pSkelVerts[0]->m_Coords[k] + w*pSkelVerts[0]->m_LF.u[k];
        }

        for (int k = 0; k < 3; k++) {
          newCoords[k] = ptEnd[k] + (newCoords[k] - ptEnd[k])*
            EdgeElongations[pMeshVertex->m_pEdge->m_Id];
        }
      }
    } //end else both vertices exist          

    points->SetPoint(i, newCoords);   
  } //for i

  delete[] EdgeLengths;
  delete[] EdgeElongations;
}

//------------------------------------------------------------------------
//Creates the internal skeleton data structure for the given polydata
vtkALBAPolyDataDeformation_M2::CSkeleton* 
vtkALBAPolyDataDeformation_M2::CreateSkeleton(vtkPolyData* pPoly)
//------------------------------------------------------------------------
{  
  int nVerts = pPoly->GetNumberOfPoints();  

  CSkeleton* pSkel = new CSkeleton();
  pSkel->m_Vertices.reserve(nVerts);

  //load points
  for (int i = 0; i < nVerts; i++)
  {
    CSkeletonVertex* pVertex = new CSkeletonVertex();

    pPoly->GetPoint(pVertex->m_Id = i, pVertex->m_Coords);
    pSkel->m_Vertices.push_back(pVertex);
  }

  //ensure that we have cells
  pPoly->BuildCells();
  int nCells = pPoly->GetNumberOfCells();
  pSkel->m_Edges.reserve(nVerts - 1);

  int nEdgeId = 0;
  for (int i = 0; i < nCells; i++)
  {
    vtkIdType* pIds;
    vtkIdType nCount;    
    pPoly->GetCellPoints(i, nCount, pIds);

    //create edges
    for (int j = 1; j < nCount; j++)
    {
      //get the next edge
      CSkeletonEdge* pEdge = new CSkeletonEdge();
      pEdge->m_Id = nEdgeId++;
      pEdge->m_Verts[0] = pSkel->m_Vertices[pIds[j - 1]];
      pEdge->m_Verts[1] = pSkel->m_Vertices[pIds[j]];

      pEdge->m_Verts[0]->m_OneRingEdges.push_back(pEdge);
      pEdge->m_Verts[1]->m_OneRingEdges.push_back(pEdge);
      pSkel->m_Edges.push_back(pEdge);
    }
  }

  return pSkel;
}

//------------------------------------------------------------------------
//Creates polydata from the given skeleton.
void vtkALBAPolyDataDeformation_M2::CreatePolyDataFromSkeleton(
                                    CSkeleton* pSkel, vtkPolyData* output)
//------------------------------------------------------------------------
{ 
  int nEdges = (int)pSkel->m_Edges.size();
  int nPoints = (int)pSkel->m_Vertices.size();

  int* pVertMap = new int[nPoints];
  memset(pVertMap, -1, nPoints*sizeof(int));  
    
  vtkPoints* points = vtkPoints::New();         //points
  vtkCellArray* cells = vtkCellArray::New();    //cells    
  
  for (int i = 0; i < nEdges; i++)
  {
    CSkeletonEdge* pEdge = pSkel->m_Edges[i];
    if (pEdge->m_Verts[0] == NULL || pEdge->m_Verts[1] == NULL)
      continue; //this is an infinite edge    

    vtkIdType cellIds[2];  //cells (lines)  
    for (int j = 0; j < 2; j++)
    {
      CSkeletonVertex* pVert = pEdge->m_Verts[j];
      if (pVertMap[pVert->m_Id] < 0)
      {
        //this vertex has not be stored yet
        pVertMap[pVert->m_Id] = points->InsertNextPoint(pVert->m_Coords);
        
        //joined vertices must be stored only once
        int nJoints = (int)pVert->m_JoinedVertices.size();
        for (int k = 0; k < nJoints; k++) { 
          pVertMap[pVert->m_JoinedVertices[k]->m_Id] = pVertMap[pVert->m_Id];
        }
      } //end if

      cellIds[j] = pVertMap[pVert->m_Id];
    }

    cells->InsertNextCell(2, cellIds);
  }

  output->SetPoints(points);
  output->SetLines(cells);

  points->Delete();
  cells->Delete();

  delete[] pVertMap;
}

//------------------------------------------------------------------------
//Computes the average length of edges of the input mesh
double vtkALBAPolyDataDeformation_M2::ComputeInputMeshAvgEdgeLength()
//------------------------------------------------------------------------
{
  //compute the average length of edges of the input mesh
  double dblEdgeLen = 0.0;
  int nEdges = 0;

  vtkPolyData* input = vtkPolyData::SafeDownCast(GetInput());
  input->BuildCells();  //just for sure

  int nCells = input->GetNumberOfCells();
  for (int i = 0; i < nCells; i++)
  {
    vtkIdType nPoints, *ptIds;
    input->GetCellPoints(i, nPoints, ptIds);
    for (int j = 0; j < nPoints; j++)
    {
      double coords1[3], coords2[3];
      input->GetPoint(ptIds[j], coords1);
      input->GetPoint(ptIds[(j + 1) % nPoints], coords2);

      dblEdgeLen += sqrt(vtkMath::Distance2BetweenPoints(coords1, coords2));
    }

    nEdges += nPoints;
  }

  dblEdgeLen = dblEdgeLen / nEdges;
  return dblEdgeLen;
}

#ifdef DEBUG_vtkALBAPolyDataDeformation_M2
//------------------------------------------------------------------------
//Debug routine that creates polydata from superskeleton
void vtkALBAPolyDataDeformation_M2::CreatePolyDataFromSuperskeleton()
//------------------------------------------------------------------------
{
  m_MATCHED_FULLCC = vtkIdList::New(); 
  m_MATCHED_POLYS[0] = vtkPolyData::New();
  m_MATCHED_POLYS[1] = vtkPolyData::New();

  CreatePolyDataFromSkeleton(SuperSkeleton->m_pOC_Skel, m_MATCHED_POLYS[0]);
  CreatePolyDataFromSkeleton(SuperSkeleton->m_pDC_Skel, m_MATCHED_POLYS[1]);
  
  int nCount = m_MATCHED_POLYS[0]->GetNumberOfPoints();
  for (int i = 0; i < nCount; i++)
  {    
    m_MATCHED_FULLCC->InsertNextId(i); 
    m_MATCHED_FULLCC->InsertNextId(i);
  }
}

//------------------------------------------------------------------------
//Destroys the debug data
void vtkALBAPolyDataDeformation_M2::DestroyMATCHEDData()
//------------------------------------------------------------------------
{
  if (m_MATCHED_CC != NULL)
  {
    m_MATCHED_CC->Delete(); 
    m_MATCHED_CC = NULL;
  }

  if (m_MATCHED_FULLCC != NULL)
  {
    m_MATCHED_FULLCC->Delete(); 
    m_MATCHED_FULLCC = NULL;
  }

  for (int i = 0; i < 2; i++)
  {
    if (m_MATCHED_POLYS[i] != NULL)
    {
      m_MATCHED_POLYS[i]->Delete(); 
      m_MATCHED_POLYS[i] = NULL;
    }
  }
}
#endif