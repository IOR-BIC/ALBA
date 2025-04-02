/*========================================================================= 
Program: Multimod Application Framework RELOADED 
Module: $RCSfile: vtkALBAPolyDataDeformation_M1.cpp,v $ 
Language: C++ 
Date: $Date: 2010-11-18 14:09:50 $ 
Version: $Revision: 1.1.2.5 $ 
Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
========================================================================== 
Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
See the COPYINGS file for license details 
=========================================================================
*/

#include "vtkALBAPolyDataDeformation_M1.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkMath.h"
#include "vtkCellArray.h"
#include "vtkCellLocator.h"
#include "vtkGenericCell.h"

vtkStandardNewMacro(vtkALBAPolyDataDeformation_M1);

#include "albaMemDbg.h"
#include "albaDbg.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"

#pragma region //Nested Classes
//Returns length of the edge. 
//If the edge does not have both vertices defined, it returns 0.0
double vtkALBAPolyDataDeformation_M1::CSkeletonEdge::GetLength()
{
  if (this->Verts[0] == NULL || this->Verts[1] == NULL)
    return 0.0;
  else
    return sqrt(vtkMath::Distance2BetweenPoints(
    this->Verts[0]->m_Coords, this->Verts[1]->m_Coords));
}

vtkALBAPolyDataDeformation_M1::CSkeleton::~CSkeleton()
{
  int nCount = (int)this->Vertices.size();
  for (int i = 0; i < nCount; i++)
  {
    delete this->Vertices[i];
  }

  nCount = (int)this->Edges.size();
  for (int i = 0; i < nCount; i++)
  {
    delete this->Edges[i];
  }

  this->Vertices.clear();
  this->Edges.clear();
}

//------------------------------------------------------------------------
//Computes topology weights for vertices
void vtkALBAPolyDataDeformation_M1::CSkeleton::ComputeTW()
//------------------------------------------------------------------------
{     
  int nVertices = (int)this->Vertices.size();
  int nEdges = (int)this->Edges.size();

  //process every vertex 
  for (int i = 0; i < nVertices; i++)
  {
    CSkeletonVertex* pVertex = this->Vertices[i];    
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
        int nWeight = ComputeEdgeWeight(pEdge, (pEdge->Verts[0] == pVertex ? 1 : 0));
        pVertex->m_WT += ((double)nWeight) / nEdges;
      }        
    }
  }
}

//------------------------------------------------------------------------
//Computes the weight for the given edge.
//The computation is recursive in the given direction.
int vtkALBAPolyDataDeformation_M1::CSkeleton::ComputeEdgeWeight(CSkeletonEdge* pEdge, int iDir)
//------------------------------------------------------------------------
{
  CSkeletonVertex* pVertex = pEdge->Verts[iDir];
  int nCount = pVertex->GetDegree();
  if (nCount == 1)  //end point
    return 1;

  int nRetValue = 0;
  for (int i = 0; i < nCount; i++)
  {
    CSkeletonEdge* pNbEdge = pVertex->m_OneRingEdges[i];
    if (pNbEdge == pEdge)
      continue;

    int nWeight = ComputeEdgeWeight(pNbEdge, (pNbEdge->Verts[0] == pVertex ? 1 : 0));    
    if (nRetValue < nWeight)
      nRetValue = nWeight;
  }

  //if (nCount != 2)
  //  nRetValue++;    //simple joined edges do not count
  return ++nRetValue;
}


//------------------------------------------------------------------------
//Computes the bounding box
void vtkALBAPolyDataDeformation_M1::CSkeleton::GetBoundingBox(double bnds[6])
//------------------------------------------------------------------------
{  
  bnds[0] = bnds[2] = bnds[4] = 0xFFFFFFFF;
  bnds[1] = bnds[3] = bnds[5] = -bnds[0];

  int nCount = (int)this->Vertices.size();

  for (int i = 0; i < nCount; i++)
  {
    double* pcoods = this->Vertices[i]->m_Coords;

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
vtkALBAPolyDataDeformation_M1::CMatrix<T>::CMatrix(int rows, int columns) 
{  
  // alloc arrays
  this->Matrix = new T*[rows]; // rows
  for ( int i = 0 ; i < rows ; i++ )
  {
    this->Matrix[i] = new T[columns]; // columns
    memset(this->Matrix[i], 0, sizeof(T)*columns);
  }

  this->NumOfRows = rows;
  this->NumOfCols = columns;
}

template <class T>
vtkALBAPolyDataDeformation_M1::CMatrix<T>::~CMatrix() 
{
  if ( this->Matrix != NULL ) 
  {
    // free arrays
    for ( int i = 0 ; i < this->NumOfRows ; i++ )
      delete [] this->Matrix[i];

    delete [] this->Matrix;
  }
  this->Matrix = NULL;
}

#pragma region CMunkres algorithm
#define Z_NORMAL 0
#define Z_STAR 1
#define Z_PRIME 2

int vtkALBAPolyDataDeformation_M1::CMunkres::Step1(void) 
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

int vtkALBAPolyDataDeformation_M1::CMunkres::Step2(void) 
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

int vtkALBAPolyDataDeformation_M1::CMunkres::Step3(void) 
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

int vtkALBAPolyDataDeformation_M1::CMunkres::Step4(void) 
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

int vtkALBAPolyDataDeformation_M1::CMunkres::Step5(void) {
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

void vtkALBAPolyDataDeformation_M1::CMunkres::Solve( CMatrix< double >* m, CMatrix< int >* matches )
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


vtkALBAPolyDataDeformation_M1::vtkALBAPolyDataDeformation_M1()
{
  m_Skeletons = NULL;
  m_SuperSkeleton = NULL;

  m_NumberOfSkeletons = 0;

  MatchGeometryWeight = 0.5;
  MatchTopologyWeight = 1;     //topology is more important
  MatchTolerance = 0.1;
  
  DivideSkeletonEdges = 0;
  PreserveVolume = 1;

#ifdef DEBUG_vtkALBAPolyDataDeformation_M1
  m_MATCHED_CC = NULL; m_MATCHED_FULLCC = NULL;
  m_MATCHED_POLYS[0] = NULL; m_MATCHED_POLYS[1] = NULL;
#endif
}

vtkALBAPolyDataDeformation_M1::~vtkALBAPolyDataDeformation_M1()
{
  //destroy skeletons
  SetNumberOfSkeletons(0);  

  //superskeleton should be destroyed by now
  _ASSERT(m_SuperSkeleton == NULL);
  DestroySuperSkeleton();
  
#ifdef DEBUG_vtkALBAPolyDataDeformation_M1
  DestroyMATCHEDData();  
#endif  
}

//----------------------------------------------------------------------------
void vtkALBAPolyDataDeformation_M1::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  this->Superclass::PrintSelf(os,indent);
}

//------------------------------------------------------------------------
//Sets the number of control skeletons.  
//Old skeletons are copied (and preserved)
/*virtual*/ void vtkALBAPolyDataDeformation_M1::SetNumberOfSkeletons( int nCount )
//------------------------------------------------------------------------
{
  _VERIFY_RET(nCount >= 0);

  if (nCount == m_NumberOfSkeletons)
    return; //nothing to be changed

  //if the number of curves is going to be decreased, we
  //need to delete some curves
  while (m_NumberOfSkeletons > nCount)
  {
    --m_NumberOfSkeletons;
    if (m_Skeletons[m_NumberOfSkeletons].pPolyLines[0] != NULL)
      m_Skeletons[m_NumberOfSkeletons].pPolyLines[0]->Delete();

    if (m_Skeletons[m_NumberOfSkeletons].pPolyLines[1] != NULL)
      m_Skeletons[m_NumberOfSkeletons].pPolyLines[1]->Delete();

    if (m_Skeletons[m_NumberOfSkeletons].pCCList != NULL)
      m_Skeletons[m_NumberOfSkeletons].pCCList->Delete();
  }

  CONTROL_SKELETON* pNewArr = NULL;
  if (nCount > 0)
  {    
    pNewArr = new CONTROL_SKELETON[nCount];
    memset(pNewArr, 0, sizeof(CONTROL_SKELETON)*nCount);

    //copy existing curves
    for (int i = 0; i < m_NumberOfSkeletons; i++) {
      pNewArr[i] = m_Skeletons[i];      
    }

    m_NumberOfSkeletons = nCount;    
  }

  delete[] m_Skeletons;
  m_Skeletons = pNewArr;

  this->Modified();
}

//------------------------------------------------------------------------
//Specifies the n-th control skeleton.
/*virtual*/ void vtkALBAPolyDataDeformation_M1
::SetNthSkeleton( int idx, vtkPolyData* original, 
                 vtkPolyData* modified, vtkIdList* correspondence )
                 //------------------------------------------------------------------------
{
  _VERIFY_RET(idx >= 0);

  if (idx >= GetNumberOfSkeletons())
    SetNumberOfSkeletons(idx + 1);

  if (m_Skeletons[idx].pPolyLines[0] != original)
  {
    if (NULL != m_Skeletons[idx].pPolyLines[0])
      m_Skeletons[idx].pPolyLines[0]->Delete();

    if (NULL != (m_Skeletons[idx].pPolyLines[0] = original))
      m_Skeletons[idx].pPolyLines[0]->Register(this);

    this->Modified();
  }

  if (m_Skeletons[idx].pPolyLines[1] != modified)
  {
    if (NULL != m_Skeletons[idx].pPolyLines[1])
      m_Skeletons[idx].pPolyLines[1]->Delete();

    if (NULL != (m_Skeletons[idx].pPolyLines[1] = modified))
      m_Skeletons[idx].pPolyLines[1]->Register(this);

    this->Modified();
  }

  if (m_Skeletons[idx].pCCList != correspondence)
  {
    if (NULL != m_Skeletons[idx].pCCList)
      m_Skeletons[idx].pCCList->Delete();

    if (NULL != (m_Skeletons[idx].pCCList = correspondence))
      m_Skeletons[idx].pCCList->Register(this);

    this->Modified();
  }
}

//------------------------------------------------------------------------
//Return this object's modified time.
/*virtual*/ vtkMTimeType vtkALBAPolyDataDeformation_M1::GetMTime()
//------------------------------------------------------------------------
{
	vtkMTimeType mtime = Superclass::GetMTime();
  for (int i = 0; i < m_NumberOfSkeletons; i++)
  {
		vtkMTimeType t1;
    if (m_Skeletons[i].pPolyLines[0] != NULL)
    {
      t1 = m_Skeletons[i].pPolyLines[0]->GetMTime();
      if (t1 > mtime)
        mtime = t1;
    }

    if (m_Skeletons[i].pPolyLines[1] != NULL)
    {
      t1 = m_Skeletons[i].pPolyLines[1]->GetMTime();
      if (t1 > mtime)
        mtime = t1;
    }

    if (m_Skeletons[i].pCCList != NULL)
    {
      t1 = m_Skeletons[i].pCCList->GetMTime();
      if (t1 > mtime)
        mtime = t1;
    }
  }

  return mtime;
}

//------------------------------------------------------------------------
//By default, UpdateInformation calls this method to copy information
//unmodified from the input to the output.
/*virtual*/int vtkALBAPolyDataDeformation_M1::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//------------------------------------------------------------------------
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (input == NULL)
  {
    vtkErrorMacro(<< "Invalid input for vtkALBAPolyDataDeformation_M1.");
    return 1;   //we have no input
  }

  //check output
  vtkPolyData* output = GetOutput();
  if (output == NULL)
    SetOutput(vtkPolyData::New());

  //copy input to output
  return Superclass::RequestInformation(request,inputVector,outputVector);  
}

#ifdef DEBUG_vtkALBAPolyDataDeformation_M1
#include "vtkCharArray.h"
#include "vtkPointData.h"
#endif

//------------------------------------------------------------------------
//This method is the one that should be used by subclasses, right now the 
//default implementation is to call the backwards compatibility method
/*virtual*/void vtkALBAPolyDataDeformation_M1::ExecuteData(vtkDataObject *output)
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
    vtkWarningMacro(<< "Invalid output for vtkALBAPolyDataDeformation_M1.");
    return;   //we have no valid output
  }  

  //process every single skeleton and construct 
  //super skeleton where everything is matched
  if (!CreateSuperSkeleton())
  {
    vtkWarningMacro(<< "Missing control skeleton for vtkALBAPolyDataDeformation_M1.");
    return;
  }

  //OK, we have super skeleton, let us build cells and neighbors (if they do not exist)
  //for the input mesh as we will need then to quickly traverse through the mesh
  m_MeshVertices.resize(input->GetNumberOfPoints());  
  input->BuildCells(); input->BuildLinks();
  
  //for every curve, we need to compute its ROI, i.e., vertices that are mapped for this curve 
  int nCount = (int)m_SuperSkeleton->m_pOC_Skel->Vertices.size();
  for (int iStartPos = 0; iStartPos < nCount; )
  {
    CSkeletonVertex* pOC_Curve = m_SuperSkeleton->m_pOC_Skel->Vertices[iStartPos];
    iStartPos += GetNumberOfCurveVertices(pOC_Curve);

    //compute local frames for both curves, original and deformed one
    ComputeLFS(pOC_Curve);
  }

  //compute the ROIs for all curve, i.e., detects vertices that 
  //might be influenced by those curves
  //this will need cell locator
  //octree based locator for cells of the input mesh should be enough
  vtkCellLocator* cellLocator = vtkCellLocator::New();    
  cellLocator->SetDataSet(vtkDataSet::SafeDownCast(GetInput()));
  cellLocator->Update();

  nCount = (int)m_SuperSkeleton->m_pOC_Skel->Edges.size();
  do 
  {
    for (int i = 0; i < nCount; i++) {
      ComputeROI(m_SuperSkeleton->m_pOC_Skel->Edges[i] );
    }

    //refine ROIs so every vertex is influenced just by one curve
    RefineCurveROIs(cellLocator);

    //check whether there is a risk of self-intersection
    //if some ROIs overlap, LFS will be modified to prevent self-intersection
    //=> we will need to recalculate ROIs
  } while(!ValidateCurveROIs());

  cellLocator->Delete();  //no longer needed

  //let us parametrize the mesh
  ComputeMeshParametrization();

  //and finally, deform the output mesh
  DeformMesh(pPoly);

#ifdef DEBUG_vtkALBAPolyDataDeformation_M1
  vtkCharArray* scalar = vtkCharArray::New();  
  nCount = input->GetNumberOfPoints();
  scalar->SetNumberOfTuples(nCount);
  scalar->SetNumberOfComponents(1);

  bool bWarnShown = false;
  int nUnmapped = 0;

  for (int i = 0; i < nCount; i++)
  {
    CMeshVertex* pVert = &m_MeshVertices[i];

    int iMin = 0;
    int nPars = (int)pVert->m_Parametrization.size();
    if (nPars == 0)
    {
      if (!bWarnShown) {
        _RPT0(_CRT_WARN, "UNMAPPED VERTICES DETECTED:\n====================\n");
        bWarnShown = true;
      }
      
      nUnmapped++;
      _RPT2(_CRT_WARN, "%d,%s", i, ((nUnmapped % 5) == 0 ? "\n" : " "));
      scalar->SetTuple1(i, -1);
    }
    else //if (nPars == 1)
      scalar->SetTuple1(i, pVert->m_Parametrization[0].pEdge->m_Id);
  //  else    
  //  {
  //    scalar->SetTuple1(i, -2);
  //    /*for (int j = 1; j < nPars; j++) {
  //      if (pVert->m_Parametrization[iMin].dblWeight > 
  //        pVert->m_Parametrization[j].dblWeight)
  //        iMin = j;
  //    }

  //    scalar->SetTuple1(i, 1 + pVert->m_Parametrization[iMin].pEdge->m_Id);*/
  //  }    
  }

  if (nUnmapped > 0)
    _RPT1(_CRT_WARN, "\n-------------------\nTotal unmapped: %d\n\n", nUnmapped);

  input->GetPointData()->SetScalars(scalar);
  pPoly->GetPointData()->SetScalars(scalar);
  scalar->Delete();

  CreatePolyDataFromSuperskeleton();
#endif
  DestroySuperSkeleton();
}  


//------------------------------------------------------------------------
//Creates a single skeleton that describes the deformation
//Returns false, if it cannot be created
bool vtkALBAPolyDataDeformation_M1::CreateSuperSkeleton()
//------------------------------------------------------------------------
{ 
  double dblEdgeFactor = DivideSkeletonEdges ?
    ComputeInputMeshAvgEdgeLength() : 0.0;
  dblEdgeFactor *= dblEdgeFactor*4;  //segment is k times larger than average edge length

  DestroySuperSkeleton();

  //combine every control skeleton to create match
  for (int i = 0; i < m_NumberOfSkeletons; i++)
  {
    CreateSuperSkeleton(m_Skeletons[i].pPolyLines[0], 
      m_Skeletons[i].pPolyLines[1], m_Skeletons->pCCList, dblEdgeFactor);
  } 

  if (m_SuperSkeleton == NULL)
    return false;

  int nCount = (int)m_SuperSkeleton->m_pOC_Skel->Vertices.size();
  if (nCount == 0)
  {
    DestroySuperSkeleton();
    return false;
  }
  
#ifdef DEBUG_vtkALBAPolyDataDeformation_M1  
  DestroyMATCHEDData();    
  CreatePolyDataFromSuperskeleton();
#endif

  //now we will extend the superskeleton by adding infinite
  //edges connected to end-points of the skeleton
  int nEdgeId = (int)m_SuperSkeleton->m_pOC_Skel->Edges.size();
  for (int i = 0; i < nCount; i++)
  {
    CSkeletonVertex* pVertex = m_SuperSkeleton->m_pOC_Skel->Vertices[i];
    if (pVertex->m_WT != 1)//== 0)
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

      pNewEdge->Id = nEdgeId;
      pDNewEdge->Id = nEdgeId++;

      pNewEdge->m_pMatch = pDNewEdge;
      pDNewEdge->m_pMatch = pNewEdge;

      m_SuperSkeleton->m_pOC_Skel->Edges.push_back(pNewEdge);
      m_SuperSkeleton->m_pDC_Skel->Edges.push_back(pDNewEdge);      
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
void vtkALBAPolyDataDeformation_M1::CreateSuperSkeleton(
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
  if (m_SuperSkeleton == NULL)
    m_SuperSkeleton = new CSuperSkeleton;

  //reset mark flag for vertices of both curves
  int nOCVerts = (int)pOC_Skel->Vertices.size();  
  for (int i = 0; i < nOCVerts; i++) {
    pOC_Skel->Vertices[i]->m_nMark = -1;    
  }

  int nDCVerts = (int)pDC_Skel->Vertices.size();
  for (int i = 0; i < nDCVerts; i++) {
    pDC_Skel->Vertices[i]->m_nMark = -1;    
  }

  //set match marks
  int nCount = pFullCC->GetNumberOfIds();
  vtkIdType* pIds = pFullCC->GetPointer(0);
  for (int i = 0; i < nCount; i += 2)
  {
    pOC_Skel->Vertices[pIds[i]]->m_nMark = pIds[i + 1];
    pDC_Skel->Vertices[pIds[i + 1]]->m_nMark = pIds[i];
  }

  //marks for every edge should be zeros as they are not used in MatchSkeletons
  _ASSERT(pOC_Skel->Edges[0]->m_nMark == 0);  
   
  //detect every matched curve  
  CSkeletonVertex** pOC_Curve = new CSkeletonVertex*[nOCVerts + nDCVerts];
  CSkeletonVertex** pDC_Curve = new CSkeletonVertex*[nOCVerts + nDCVerts];  

  for (int i = 0; i < nOCVerts; i++)
  {
    CSkeletonVertex* pVert1 = pOC_Skel->Vertices[i];
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
      CSkeletonVertex* pDVert1 = pDC_Skel->Vertices[pVert1->m_nMark];
      CSkeletonVertex* pDVert2 = pDC_Skel->Vertices[pVert2->m_nMark];
                  
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
      AddCurveToSuperSkeleton(pOC_Curve[0], true);
      AddCurveToSuperSkeleton(pDC_Curve[0], false);
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
vtkIdList* vtkALBAPolyDataDeformation_M1::MatchSkeletons(CSkeleton* pOC, 
                                                     CSkeleton* pDC, vtkIdList* pCC)
//------------------------------------------------------------------------
{  
  //first, determine, which points are going to be matched 
  int nOCVerts = (int)pOC->Vertices.size(); 
  for (int i = 0; i < nOCVerts; i++)
  {
    //mark the point, if it is not simple joined
    CSkeletonVertex* pVertex = pOC->Vertices[i];
    if (pVertex->GetDegree() == 2)
      pVertex->m_nMark = -1;
    else
      pVertex->m_nMark = 0;
  }

  int nDCVerts = (int)pDC->Vertices.size();
  for (int i = 0; i < nDCVerts; i++)
  {
    //mark the point, if it is not simple joined
    CSkeletonVertex* pVertex = pDC->Vertices[i];
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
        pOC->Vertices[pIds[i - 1]]->m_nMark = 1;
        pDC->Vertices[pIds[i]]->m_nMark = 1;
      }
    }
  }

  //reduce the set and get only vertices that are not marked
  //i.e., these vertices are not already matched and they must be matched
  CSkeletonVertex** pOCVerts = new CSkeletonVertex*[nOCVerts];
  int nCount = nOCVerts; nOCVerts = 0;
  for (int i = 0; i < nCount; i++)
  {    
    pOCVerts[nOCVerts] = pOC->Vertices[i];
    if (pOCVerts[nOCVerts]->m_nMark == 0)
      nOCVerts++; //this is end-point or junction
  }

  CSkeletonVertex** pDCVerts = new CSkeletonVertex*[nDCVerts];
  nCount = nDCVerts; nDCVerts = 0;
  for (int i = 0; i < nCount; i++)
  {    
    pDCVerts[nDCVerts] = pDC->Vertices[i];
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
int vtkALBAPolyDataDeformation_M1::TraceSkeletonCurve(
                        CSkeletonVertex* pStartVertex, int iDir, CSkeletonVertex** pOutBuf)
//------------------------------------------------------------------------
{ 
  int nRetCount = 0;
  do 
  {
    pOutBuf[nRetCount] = pStartVertex;
    nRetCount++;

    CSkeletonEdge* pEdge = pStartVertex->m_OneRingEdges[iDir];
    if (pEdge->Verts[0] == pStartVertex)    
      pStartVertex = pEdge->Verts[1];    
    else
      pStartVertex = pEdge->Verts[0];

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
void vtkALBAPolyDataDeformation_M1::MarkCurveEdges(CSkeletonVertex** pCurve, int nCount)
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
        (pEdge->Verts[0] == pCurve[0] && pEdge->Verts[1] == pCurve[1]) ||
        (pEdge->Verts[0] == pCurve[1] && pEdge->Verts[1] == pCurve[0])
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
typedef struct vtkALBAPolyDataDeformation_M1::CURVE_VERTEX
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
int vtkALBAPolyDataDeformation_M1::MatchCurves(CSkeletonVertex** pOC, int nOCVerts, 
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
        else if (pVertB->pVertex->m_pMatch == NULL && fabs(pVertB->t - pVert->t) <= dblLimit)
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
void vtkALBAPolyDataDeformation_M1::CreateCurveEdges(CSkeletonVertex** pVerts, int nVerts)
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
int vtkALBAPolyDataDeformation_M1::GetNumberOfCurveVertices(CSkeletonVertex* pCurve)
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
void vtkALBAPolyDataDeformation_M1::RefineCurve(CSkeletonVertex* pCurve, double dblEdgeFactor)
//------------------------------------------------------------------------
{
  CSkeletonEdge* pOCEdge = pCurve->m_OneRingEdges[0];
  while (pOCEdge != NULL)
  {    
    CSkeletonEdge* pDCEdge = pOCEdge->m_pMatch;

    double dblLen1 = vtkMath::Distance2BetweenPoints(
      pOCEdge->Verts[0]->m_Coords, pOCEdge->Verts[1]->m_Coords);
    double dblLen2 = vtkMath::Distance2BetweenPoints(
      pDCEdge->Verts[0]->m_Coords, pDCEdge->Verts[1]->m_Coords);

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
      pNewOCVert->m_Coords[j] = (pOCEdge->Verts[0]->m_Coords[j] + 
        pOCEdge->Verts[1]->m_Coords[j]) / 2;
      pNewDCVert->m_Coords[j] = (pDCEdge->Verts[0]->m_Coords[j] + 
        pDCEdge->Verts[1]->m_Coords[j]) / 2;
    }
    
    pNewOCVert->m_WT = pNewDCVert->m_WT = 1;
    pNewOCVert->m_pMatch = pNewDCVert;
    pNewDCVert->m_pMatch = pNewOCVert;

    CSkeletonEdge* pNewOCEdge = new CSkeletonEdge(pNewOCVert, pOCEdge->Verts[1]);
    CSkeletonEdge* pNewDCEdge = new CSkeletonEdge(pNewDCVert, pDCEdge->Verts[1]);    
    pNewOCEdge->m_pMatch = pNewDCEdge;
    pNewDCEdge->m_pMatch = pNewOCEdge;

    pNewOCVert->m_OneRingEdges.push_back(pNewOCEdge);
    pNewOCVert->m_OneRingEdges.push_back(pOCEdge);
    pNewDCVert->m_OneRingEdges.push_back(pNewDCEdge);
    pNewDCVert->m_OneRingEdges.push_back(pDCEdge);

    pOCEdge->Verts[1]->m_OneRingEdges.pop_back();
    pOCEdge->Verts[1]->m_OneRingEdges.push_back(pNewOCEdge);
    pDCEdge->Verts[1]->m_OneRingEdges.pop_back();
    pDCEdge->Verts[1]->m_OneRingEdges.push_back(pNewDCEdge);

    pOCEdge->Verts[1] = pNewOCVert;
    pDCEdge->Verts[1] = pNewDCVert;    
  }
}

//------------------------------------------------------------------------
//Stores vertices and edges from the given curve into superskeleton
//N.B. curve must be compatible with curves constructed by CreateCurveEdges.
void vtkALBAPolyDataDeformation_M1::AddCurveToSuperSkeleton(CSkeletonVertex* pCurve, bool bOriginal)
//------------------------------------------------------------------------
{
  CSkeleton* pSkel = bOriginal ? m_SuperSkeleton->m_pOC_Skel :
    m_SuperSkeleton->m_pDC_Skel;

  int nNextId = (int)pSkel->Vertices.size();
  int nNextEdgeId = (int)pSkel->Edges.size();

  while (pCurve != NULL)
  {
    if (pCurve->m_Id < 0)
    {
      pCurve->m_Id = nNextId++;
      pSkel->Vertices.push_back(pCurve);
    }

    CSkeletonEdge* pEdge = pCurve->m_OneRingEdges[0];
    if (pEdge->Id < 0)
    {
      pEdge->Id = nNextEdgeId++;
      pSkel->Edges.push_back(pEdge);
    }

    pCurve = GetNextCurveVertex(pCurve);
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
void vtkALBAPolyDataDeformation_M1::ComputeLFS(CSkeletonVertex* pOC)
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
//Computes the region of interest (ROI) for the given edge of curve.
//It detects every mesh vertex in the zone of influence of the given edge. 
//Note: one of end-points of the edge may be NULL to specify half-space
void vtkALBAPolyDataDeformation_M1::ComputeROI(CSkeletonEdge* pEdge)
//------------------------------------------------------------------------
{

	vtkDataSet* input = vtkDataSet::SafeDownCast(GetInput());
  int nPoints = input->GetNumberOfPoints();
  
  pEdge->m_ROI.clear();           //clear previous data (if present)
  pEdge->m_ROI.reserve(nPoints);  //reserve memory for   
  for (int ptId = 0; ptId < nPoints; ptId++)
  {
    //compute the location of the current point
    double* pcoords = input->GetPoint(ptId);
    bool bPlane1 = pEdge->Verts[0] == NULL ? true :
      pEdge->Verts[0]->IsInPositiveHalfspace(pcoords);

    bool bPlane2 = pEdge->Verts[1] == NULL ? true :
      !pEdge->Verts[1]->IsInPositiveHalfspace(pcoords);

    //if the point is delimited by both planes, we have found it
    if (bPlane1 && bPlane2)       
      pEdge->m_ROI.push_back(ptId);
  }

  pEdge->m_ROI.resize(pEdge->m_ROI.size());  
}
  
//------------------------------------------------------------------------
//Refines ROIs for curves.
//Refines the mapping of vertices of the input mesh to edges in such a manner 
//that every vertex is present only in one edge ROI. 
//The routine requires locator for cells, initialized with the input mesh
void vtkALBAPolyDataDeformation_M1::RefineCurveROIs(vtkCellLocator* locator)
//------------------------------------------------------------------------
{
  vtkPoints* input = vtkPolyData::SafeDownCast(GetInput())->GetPoints();
  int nPoints = input->GetNumberOfPoints(); 

  //number of edges to which points are mapped at present
  int* pPtMapCounts = new int[nPoints];
  memset(pPtMapCounts, 0, nPoints*sizeof(int));  

  int nEdges = (int)m_SuperSkeleton->m_pOC_Skel->Edges.size();
  for (int i = 0; i < nEdges; i++)
  {
    CSkeletonEdge* pEdge = m_SuperSkeleton->m_pOC_Skel->Edges[i];
    int nCount = (int)pEdge->m_ROI.size();

    for (int j = 0; j < nCount; j++) {
      pPtMapCounts[pEdge->m_ROI[j]]++;
    }

    ////edges with both end-points have always higher priority
    ////than infinite edges - we need to count them independently
    //if (pEdge->m_Verts[0] != NULL && pEdge->m_Verts[1] != NULL)
    //{
    //  for (int j = 0; j < nCount; j++) {
    //    pPtMapCounts[pEdge->m_ROI[j]]++;          //LOWORD
    //  }
    //}
    //else
    //{
    //  //edges with both end-points have always higher priority
    //  for (int j = 0; j < nCount; j++) {
    //    pPtMapCounts[pEdge->m_ROI[j]] += 0x10000; //HIWORD
    //  }
    //}
  }


  //the best mapping for points
  CSkeletonEdge** pBestEdges = new CSkeletonEdge*[nPoints];
  double* pBestDistances = new double[nPoints];

  memset(pBestEdges, 0, nPoints*sizeof(CSkeletonEdge*));
  memset(pBestDistances, 0, nPoints*sizeof(double));

  //compute bounding box and penalization factor
  double bnds[6];
  input->GetBounds(bnds);
  double A[3] = { bnds[1] - bnds[0], bnds[3] - bnds[2], bnds[5] - bnds[4] };  
  double dblMaxDiag = vtkMath::Norm(A);
  dblMaxDiag *= dblMaxDiag; //square    

  //this is just to speed-up the search for intersection
  vtkGenericCell* cellLoc = vtkGenericCell::New();

  //compute best distance map for every point   
  for (int i = 0; i < nEdges; i++)
  {
    CSkeletonEdge* pEdge = m_SuperSkeleton->m_pOC_Skel->Edges[i];
    int nCount = (int)pEdge->m_ROI.size();
    
    CSkeletonVertex* pSkelVert1 = pEdge->Verts[0]; 
    CSkeletonVertex* pSkelVert2 = pEdge->Verts[1];
    CSkeletonVertex* pSkelVert = pSkelVert1 != NULL ? pSkelVert1 : pSkelVert2;

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
      double ptStart[3], ptEnd[3];
      double* pcoords = input->GetPoint(nPtId);
      
      if (pSkelVert1 == NULL || pSkelVert2 == NULL)
      {
        //an infinite edge => weight is the distance of the current point P
        //to the existing end point
        for (int k = 0; k < 3; k++) 
        {        
          ptStart[k] = pcoords[k];
          ptEnd[k] = pSkelVert->m_Coords[k];
        }
      }
      else
      {
        //edge has both end-points => weight is the distance of 
        //the current point P to the current edge; the closest point on the edge 
        //can be computed as: Pk + (u*(P - Pk))*u, where u is normalized
        for (int k = 0; k < 3; k++) 
        {        
          ptStart[k] = pcoords[k];
          ptEnd[k] = pcoords[k] - pSkelVert->m_Coords[k];
        }

        double w = vtkMath::Dot(pSkelVert->m_LF.u, ptEnd);
        for (int k = 0; k < 3; k++) {
          ptEnd[k] = pSkelVert->m_Coords[k] + w*pSkelVert->m_LF.u[k];
        }
      }

      double dblWeight = vtkMath::Distance2BetweenPoints(ptStart, ptEnd);      
 /*     //compute line that is perpendicular to the current edge
      //and goes through the current point P      
      //and find all intersections with the mesh
      //IntersectWithLine is not well documented, I will try it
      int nCellId, nSubId;
      double t, x[3], par_x[3];

      //move towards ptEnd to avoid immediate detection
      double dblStep = 0.01;
      for (int k = 0; k < 3; k++) {
        ptStart[k] += dblStep*(ptEnd[k] - ptStart[k]);
      }

      while (0 != locator->IntersectWithLine(
        ptStart, ptEnd, 0.0,  //IN: end-points of line segment and tolerance
        t, x, par_x,          //OUT: time of intersection and position of intersection in
                              //global coordinates and parametric coordinates
        nSubId, nCellId,      //id of intersected cell and its face
        cellLoc)
        )
      {
        t += dblStep;  //step inside
        for (int k = 0; k < 3; k++) {
          ptStart[k] += t*(ptEnd[k] - ptStart[k]);
        }

        //distance penalization
        dblWeight += dblMaxDiag;
      }
*/      
      if (pBestEdges[nPtId] == NULL || dblWeight < pBestDistances[nPtId])
      {
        pBestEdges[nPtId] = pEdge;
        pBestDistances[nPtId] = dblWeight;
      }      
    } //end for j
  } //end for i
 
  cellLoc->Delete();

  delete[] pBestDistances;
  delete[] pPtMapCounts;    


  //last step is to update ROIs of edges
  for (int i = 0; i < nEdges; i++)
  {
    CSkeletonEdge* pEdge = m_SuperSkeleton->m_pOC_Skel->Edges[i];
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
}

//------------------------------------------------------------------------
//Validates ROIs of curves.
//It checks whether the current LFs&ROIs configuration is safe enough not
//to lead into self-intersection of the deformed mesh (see Blanco paper).
//If the outcome of this test is negative, LFs of the skeleton vertex where 
//the violation was detected is rotated and the routine returns false.
//The caller typically recomputes ROIs (as one of LFs has changed).
//The routine returns true, if there is no problem with the current configuration.
bool vtkALBAPolyDataDeformation_M1::ValidateCurveROIs()
//------------------------------------------------------------------------
{
  int nCount = (int)m_SuperSkeleton->m_pOC_Skel->Vertices.size();
  for (int iStartPos = 0; iStartPos < nCount; )
  {
    CSkeletonVertex* pCurve = m_SuperSkeleton->m_pOC_Skel->Vertices[iStartPos];
    iStartPos += GetNumberOfCurveVertices(pCurve);

    CSkeletonEdge* pPrevEdge = NULL;  

    //there might be an edge before the current vertex    
    CSkeletonEdge* pCurEdge = pCurve->m_OneRingEdges[pCurve->GetDegree() - 1];      
    do
    {   
      CSkeletonEdge* pNextEdge = GetNextCurveEdge(pCurEdge);
      if (pPrevEdge != NULL)
      {
        //to avoid self intersection during the deformation
        //we need to check the configuration of points and local frames
        //of pPrevEdge and pCurEdge. If the configuration is wrong, the
        //local frame at the point shared by these two edges  
        //must be modified (u no longer will be tangent line) and the construction
        //of ROIs for previous and the current zone restarted
        if (!CheckSelfIntersection(pPrevEdge, pCurEdge))
          return false;
      }

      pPrevEdge = pCurEdge; pCurEdge = pNextEdge;        
    } while (pCurEdge != NULL);
  }

  return true;
}

//------------------------------------------------------------------------
//Checks whether the part of mesh associated with given (adjacent) edges will 
//not self intersect after the deformation.
//If the outcome of the test is positive, the LFs at the point shared by those
//edges is modified to avoid such intersection and the routine returns false. 
//It returns true, if everything is OK. The algorithm is based on Blanco's paper.
bool vtkALBAPolyDataDeformation_M1::CheckSelfIntersection(CSkeletonEdge* pEdge1, CSkeletonEdge* pEdge2)
//------------------------------------------------------------------------
{
  if (pEdge1->Verts[0] == NULL || pEdge2->Verts[1] == NULL)
    return true;  //zones before or after the curve

  //first, compute the intersection of planes defined by LFs at both
  //points of the first edge, if the intersection does not exist,
  //i.e., if both planes are parallel, we are ready  
  CSkeletonVertex::LOCAL_FRAME* lf[2] = { &pEdge1->Verts[0]->m_LF, 
    &pEdge1->Verts[1]->m_LF};
  
  double ldir[3], lpos[3];  //direction and point of the line
  vtkMath::Cross(lf[0]->u, lf[1]->u, ldir);
  if (vtkMath::Normalize(ldir) < 1e-8)
    return true;  //parallel planes

  //According to Schneider: Geometric Tools for Computer Graphics, pg. 530
  //the intersection of planes P1*n1 + d1 = 0 and P2*n2 + d2 = 0 is
  //the line L = a*n1 + b*n2, where a = (d2*n1*n2 - d1*||n2||^2) /
  //((n1*n2)^2 - ||n1||^2*||n2||^2) and b = (d1*n1*n2 - d2*||n1||^2) /
  //((n1*n2)^2 - ||n1||^2*||n2||^2) => if both normals are normalized
  //a = (d2*n1*n2 - d1)/((n1*n2)^2 - 1) and b = (d1*n1*n2 - d2)/((n1*n2)^2 - 1)  
  double d[2];
  for (int i = 0; i < 2; i++) {
    d[i] = -vtkMath::Dot(lf[i]->u, pEdge1->Verts[i]->m_Coords);    
  }

  double n1n2 = vtkMath::Dot(lf[0]->u, lf[1]->u);
  double a = (d[1]*n1n2 - d[0]) / (n1n2*n1n2 - 1.0);
  double b = (d[1]*n1n2 - d[1]) / (n1n2*n1n2 - 1.0);
  for (int i = 0; i < 3; i++) {
    lpos[i] = a*lf[0]->u[i] + b*lf[1]->u[i];
  }

  //next, compute the squared distance of both LFS points from the line
  //According to Schneider: Geometric Tools for Computer Graphics, pg. 365
  //the distance of point Q from the line L(t) = P + t*d can be computed
  //as ||Q - Q'|| = ||Q - (P + t0*d)||, t0 = d*(Q-P) =>
  //||Q - (P + d*(Q-P)*d)|| assuming that d is normalized
  double d_lpk[2], lpk[2][3];
  for (int i = 0; i < 2; i++)
  {    
    for (int j = 0; j < 3; j++) { //Q-P
      lpk[i][j] = pEdge1->Verts[i]->m_Coords[j] - lpos[j];
    }

    d_lpk[i] = 0.0;

    double w = vtkMath::Dot(lpk[i], ldir); //t0 = d*(Q-P)
    for (int j = 0; j < 3; j++) 
    {
      lpk[i][j] = lpos[j] + w*ldir[j];      //Q' - point on the line
      double tmp = pEdge1->Verts[i]->m_Coords[j] - lpk[i][j];
      d_lpk[i] += tmp*tmp;
    }    
  }


  //now, project points in the pEdge1 zone onto the plane defined by LFS 
  //at the first vertex of pEdge1 and points in the pEdge2 zone onto
  //the plane defined by LFS at the first vertex of pEdge2 
  //and get smallest circles with centers at first vertices of edges
  //that contain all projected points
  double rk[2] = {0.0, 0.0};

  vtkPoints* input = vtkPolyData::SafeDownCast(GetInput())->GetPoints();
  CSkeletonEdge* pCurEdge = pEdge1;
  for (int i = 0; i < 2; i++)
  {
    int nCount = (int)pCurEdge->m_ROI.size();
    for (int j = 0; j < nCount; j++)
    {
      double coords[3];
      input->GetPoint(pCurEdge->m_ROI[j], coords);
     
      //According to Schneider: Geometric Tools for Computer Graphics, pg. 663
      //the projection of point Q onto the plane P*n + d = 0 can be computed as
      //Q-(Q*n+d)*n assuming that |n| = 1 => in our case n is LF.u, and as
      //the plane goes through the point p, then d = -p*LF.u, which is already computed
      double w = vtkMath::Dot(coords, lf[i]->u) + d[i];
      for (int k = 0; k < 3; k++) {
        coords[k] -= lf[i]->u[k]*w;
      }

      //update the radius of the circle
      double r = vtkMath::Distance2BetweenPoints(pCurEdge->Verts[0]->m_Coords, coords);
      if (r > rk[i])
      {
        rk[i] = r;

        //if the first radius is already computed then we can check self-intersection
        //we check it here to avoid projection of points when it is already
        //clear than the configuration of LFS is invalid
        if (i == 1)
        {                    
          //if dist(l,p0) < rk0 && dist(l,p1) < rk1 then there is self-intersection
          if (d_lpk[0] < rk[0] && d_lpk[1] < rk[1])
          {
            //we have to modify the LFS at the shared point by rotating it around 
            //vector ldir by angle between vectors v1 and v2 where v1 is a vector
            //between the center of the second circle and point on the intersection of
            //the first circle and line ll going through its center and perpendicular
            //to the line L, and v2 is a vector from this intersection to the 
            //intersection of both lines

            double v1[3], v2[3];
            for (int k = 0; k < 3; k++) {
              v1[k] = lpk[0][k] - pEdge1->Verts[0]->m_Coords[k];
            }

            vtkMath::Normalize(v1);
            for (int k = 0; k < 3; k++) {
              v2[k] = pEdge1->Verts[1]->m_Coords[k] - (
                pEdge1->Verts[0]->m_Coords[k] + rk[0]*v1[k]
              );
            }

            //we may rotate it now
            double M[3][3];
            BuildGeneralRotationMatrix(ldir, vtkMath::Dot(v1, v2), M);

            CSkeletonVertex::LOCAL_FRAME dlf; //new deformed LF
            CSkeletonVertex::LOCAL_FRAME& olf = pEdge1->Verts[1]->m_LF;
            for (int k = 0; k < 3; k++)
            {
              dlf.w[k] = dlf.v[k] = dlf.u[k] = 0.0;
              for (int l = 0; l < 3; l++)
              {
                dlf.u[k] += olf.u[l]*M[l][k];
                dlf.v[k] += olf.v[l]*M[l][k];
                dlf.w[k] += olf.w[l]*M[l][k];
              }
            }

            olf = dlf;  //store the result

            //frame is modified, however we will need to modify the position of
            //next node as well, so the new tangent vector is still parallel
            //to the edge, otherwise sudden jumps in mesh may appear
            //naturally this will cause recalculation of the LFs in that point as well
            if (pEdge2->Verts[1] != NULL)
            {
              CSkeletonVertex* pPrevVertex = pEdge2->Verts[0];
              CSkeletonVertex* pCurVertex = pEdge2->Verts[1];

              double dblDist = sqrt(vtkMath::Distance2BetweenPoints(
                pPrevVertex->m_Coords, pCurVertex->m_Coords));
              for (int k = 0; k < 3; k++) {
                pCurVertex->m_Coords[k] = pPrevVertex->m_Coords[k] + dblDist*olf.u[k];
              }

              CSkeletonVertex* pNextVertex = GetNextCurveVertex(pCurVertex);
              if (pNextVertex == NULL)  //if the current vertex is the last valid
                pCurVertex->m_LF = dlf; //it will have the same LFS as previous vertex
              else
              {                              
                //see ComputeLFS to see comments
                CSkeletonVertex::LOCAL_FRAME& nlf = pCurVertex->m_LF;                    
                for (int j = 0; j < 3; j++)  { 
                  nlf.u[j] = pNextVertex->m_Coords[j] - pCurVertex->m_Coords[j];
                }

                vtkMath::Normalize(nlf.u);

                double dblVal = vtkMath::Dot(olf.v, nlf.u);
                for (int j = 0; j < 3; j++) {
                  nlf.v[j] = olf.v[j] - dblVal*nlf.u[j];
                }

                vtkMath::Normalize(nlf.v);

                //compute w = u x v
                vtkMath::Cross(nlf.u, nlf.v, nlf.w);
                vtkMath::Normalize(nlf.w);

                //if the angle between w(i-1) and w(i) is greater than 90 degrees
                //change the sign of w(i)
                if (vtkMath::Dot(nlf.w, olf.w) < 0)
                {
                  for (int j = 0; j < 3; j++) {
                    nlf.w[j] = -nlf.w[j];
                  }
                }
              } //end else pNextVertex == NULL
            } //end if pEdge2->m_Verts[1] != NULL          

            //frame is modified, return back
            return false;
          } //end if d_lpk ...
        } //end if i == 1
      } //end if r > rk
    } //end for j
    
    pCurEdge = pEdge2; //move to the next edge
  } //end for i

  return true;
}


//------------------------------------------------------------------------
//Constructs the matrix for rotation of vectors around the vector r by the
//angle theta. The angle is given indirectly as cos(theta).
//N.B. vector r must be normalized. 
void vtkALBAPolyDataDeformation_M1
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
//Parametrize the given points of the input mesh.
//Points are given as indices. Parametrization is done for the given edge.
//if edge is infinite (only one end-point) or iWSkelEdge is -1,
//the parametrization of every vertex has the same weight (it is 1.0),
//otherwise it is computed as a function of the distance between the 
//mesh vertex and skeleton vertex pEdge->m_Verts[iWSkelEdge] and sigma2.
//This function is (see Blanco's paper): e^-(dist2(x,skelvertex)/sigma2).
//sigma2 should be 2*dist2(skelvertex,centroid(skelvertex))
void vtkALBAPolyDataDeformation_M1
    ::ComputeParametrization(std::vector< vtkIdType >& points, 
                            CSkeletonEdge* pEdge, int iWSkelEdge, double sigma2)
    //::ComputeParametrization(CSkeletonEdge* pNextEdge, 
    //  CSkeletonEdge* pEdge, int iWSkelEdge)
//------------------------------------------------------------------------
{ 
  /*double uu1[3], uu2[3];
  std::vector< vtkIdType >& points = pNextEdge->m_ROI;
  if (iWSkelEdge >= 0 && pEdge->m_Verts[0] != NULL &&
    pEdge->m_Verts[1] != NULL && pNextEdge->m_Verts[0] != NULL &&
    pNextEdge->m_Verts[1] != NULL)
  {
    int iNWSkelEdge = 0;
    if (pEdge->m_Verts[iWSkelEdge]->m_Coords[0] != pNextEdge->m_Verts[0]->m_Coords[0] ||
      pEdge->m_Verts[iWSkelEdge]->m_Coords[1] != pNextEdge->m_Verts[0]->m_Coords[1] ||
      pEdge->m_Verts[iWSkelEdge]->m_Coords[2] != pNextEdge->m_Verts[0]->m_Coords[2]
    )
      iNWSkelEdge = 1;

    for (int k = 0; k < 3; k++)
    {
      uu1[k] = pEdge->m_Verts[iWSkelEdge]->m_Coords[k] - 
        pEdge->m_Verts[1 - iWSkelEdge]->m_Coords[k];
      uu2[k] = pNextEdge->m_Verts[iNWSkelEdge]->m_Coords[k] - 
        pNextEdge->m_Verts[1 - iNWSkelEdge]->m_Coords[k];
    }

    vtkMath::Normalize(uu1);
    vtkMath::Normalize(uu2);
  }*/



  vtkPolyData* input = vtkPolyData::SafeDownCast(GetInput());

  CSkeletonVertex* pSkelVert1 = pEdge->Verts[0]; 
  CSkeletonVertex* pSkelVert2 = pEdge->Verts[1];

  //detect the edge configuration  
  CSkeletonVertex* pSkelVert = pSkelVert1 != NULL ? pSkelVert1 : pSkelVert2;
  bool bInfEdge = pSkelVert1 == NULL || pSkelVert2 == NULL;

  CMeshVertex::VERTEX_PARAM ssParam;
  memset(&ssParam, 0, sizeof(ssParam));

  ssParam.pEdge = pEdge;

  int nPoints = (int)points.size();
  for (int j = 0; j < nPoints; j++)
  {
    int nPtId = points[j];
    double* pcoords = input->GetPoint(nPtId);

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

    ssParam.pcoords[2] = M[2][3] / M[2][2];
    ssParam.pcoords[1] = (M[1][3] - ssParam.pcoords[2]*M[1][2]) / M[1][1];
    ssParam.pcoords[0] = (M[0][3] - ssParam.pcoords[2]*M[0][2] - 
      ssParam.pcoords[1]*M[0][1]) / M[0][0];

#ifdef DEBUG_vtkALBAPolyDataDeformation_M1
    double backprj[3];
    for (int k = 0; k < 3; k++)
    {
      //pSkelVert->coords + a*LF.u + b*LF.v + c*LF.w
      backprj[k] = pSkelVert->m_Coords[k] + 
        ssParam.pcoords[0]*pSkelVert->m_LF.u[k] + 
        ssParam.pcoords[1]*pSkelVert->m_LF.v[k] + 
        ssParam.pcoords[2]*pSkelVert->m_LF.w[k];

      _ASSERT(fabs(backprj[k] - pcoords[k]) <= 1e-5);
    }
#endif // _DEBUG


    //now, we need to incorporate the elongation factor
    if (bInfEdge)
    {
      ssParam.dblRm = 0.0;
      ssParam.dblWeight = 1.0;
    }
    else
    {
      //actually we need to find intersection of line going through the current
      //point (P) in the direction of ssParam.pEdge->m_Verts[0].m_LF.u (u1)
      //with both planes (defined by LFs at end-points of the given edge), i.e.,
      //with planes u1*X + d1 = 0 and u2*X + d2 = 0.
      //as the first plane goes through the point Pk1 and the second though
      //the point Pk2, d1 = -u1*Pk1 and d2 = -u2*Pk2
      //
      //intersection of line: X = P + u*t and plane: n*X + d = 0 
      //can be computed as P + u*ti and ti is computed:
      //n*X + d = 0 => n*(P + u*ti) + d = 0 => n*P + n*u*ti + d = 0 =>
      //ti = -(n*P + d)/n*u
      //
      //for the first plane, therefore, ti = (u1*Pk1 - u1*P)/(u1*u1);
      //as u1 is normalized u1*u1 = 1 => ti = u1*Pk1 - u1*P 
      //for the second plane ti =(u2*Pk2 - u2*P)/(u1*u2)        
      //N.B. we assume that planes are not orthogonal => u1*u2 != 0
      double vmp[3], vmn[3], t[2];
      t[0] = vtkMath::Dot(pSkelVert1->m_LF.u, pSkelVert1->m_Coords) - 
        vtkMath::Dot(pSkelVert1->m_LF.u, pcoords);
      t[1] = (vtkMath::Dot(pSkelVert2->m_LF.u, pSkelVert2->m_Coords) - 
        vtkMath::Dot(pSkelVert2->m_LF.u, pcoords)) / 
        vtkMath::Dot(pSkelVert1->m_LF.u, pSkelVert2->m_LF.u);

      for (int k = 0; k < 3; k++)
      {
        vmp[k] = pcoords[k] + t[0]*pSkelVert1->m_LF.u[k];
        vmn[k] = pcoords[k] + t[1]*pSkelVert1->m_LF.u[k];
      }

      ssParam.dblRm = sqrt(vtkMath::Distance2BetweenPoints(pcoords, vmp) / 
        vtkMath::Distance2BetweenPoints(vmp, vmn));

      if (iWSkelEdge < 0)
        ssParam.dblWeight = 1.0;  //no special weight
      else
      {
        //now compute the weight of this parametrization as Blanco's
        //function of the distance between the current point and the
        //iWSkelEdge point of edge        
        ssParam.dblWeight = exp(-vtkMath::Distance2BetweenPoints(pcoords,
          pEdge->Verts[iWSkelEdge]->m_Coords) / sigma2);
      
        //double up[3];
        //for (int k = 0; k < 3; k++) {
        //  up[k] = pcoords[k] - pEdge->m_Verts[iWSkelEdge]->m_Coords[k];
        //}
        //vtkMath::Normalize(up);

        ////the idea is that the larger angle, the smaller influence
        ////but the following implementation is INCORRECT!!!
        //double cos_a2 = vtkMath::Dot(up, uu1);
        //double cos_b2 = vtkMath::Dot(up, uu2);

        //cos_a2 = 1 - cos_a2*cos_a2;
        //cos_b2 = 1 - cos_b2*cos_b2;
        //ssParam.dblWeight = pow(cos_b2 / (cos_a2 + cos_b2), 8.0);
      } //else iWSkelEdge < 0
    } //end else pSkelVert1 == NULL || pSkelVert2 == NULL

    m_MeshVertices[nPtId].m_Parametrization.push_back(ssParam);
  } //end for j
}

//this struct is used in ComputeMeshParametrization
typedef struct vtkALBAPolyDataDeformation_M1::JOIN_VERTEX
{    
  std::vector< CSkeletonVertex* > vertices;

  double centroid[3]; //centroid for this join vertex
} JOIN_VERTEX; 

//------------------------------------------------------------------------
//Parametrize the input mesh using the super-skeleton.
//N.B. edges ROI must be build and refined before this routine may be called.
void vtkALBAPolyDataDeformation_M1::ComputeMeshParametrization()
//------------------------------------------------------------------------
{ 
  vtkPolyData* input = vtkPolyData::SafeDownCast(GetInput());

  //points that belong to an edge of one curve that si connected to 
  //another edge of another curve must be influenced by both edges
  //this is because configurations of those two edges may be incorrect
  //and lead to self-intersection of the deformed mesh 
  //we cannot, however, prevent this using the same technique as for
  //edges of one curve because it could deform the skeleton totally

  //so first, we need to find connection points for all curves
  std::vector< JOIN_VERTEX* > joints;

  int nSkelPoints = (int)m_SuperSkeleton->m_pDC_Skel->Vertices.size();
  for (int i = 0; i < nSkelPoints; i++)
  {
    CSkeletonVertex* pSkelVertex = m_SuperSkeleton->m_pOC_Skel->Vertices[i];
    if (pSkelVertex->m_WT <= 1)
      pSkelVertex->m_nMark = -1;    //mark vertex to be without list
    else
    {       
      //detect every vertex identical to the current one      
      JOIN_VERTEX* pJoint;
      int j, nJoints = (int)joints.size();
      for (j = 0; j < nJoints; j++)
      {
        pJoint = joints[j];
        CSkeletonVertex* pTstSkelVertex = pJoint->vertices[0];
        if (pSkelVertex->m_Coords[0] == pTstSkelVertex->m_Coords[0] &&
          pSkelVertex->m_Coords[1] == pTstSkelVertex->m_Coords[1] &&
          pSkelVertex->m_Coords[2] == pTstSkelVertex->m_Coords[2]) 
        {
          //the joint for the current vertex already exists        
          break;
        }
      }

      if (j == nJoints)
      {
        pJoint = new JOIN_VERTEX;
        for (int k = 0; k < 3; k++) {
          pJoint->centroid[k] = 0.0;
          //pJoint->centroid[k] = pSkelVertex->m_Coords[k];
        }        
        joints.push_back(pJoint);
      }

      //store index of the joint
      pSkelVertex->m_nMark = j;
      pJoint->vertices.push_back(pSkelVertex);

      //select edge with both valid vertices
      CSkeletonEdge* pEdge = pSkelVertex->m_OneRingEdges[0];
      if (pEdge->Verts[0] == NULL || pEdge->Verts[1] == NULL)
        pEdge = pSkelVertex->m_OneRingEdges[1];

      pSkelVertex = pEdge->Verts[0] != pSkelVertex ? 
        pEdge->Verts[0] : pEdge->Verts[1];

      for (int k = 0; k < 3; k++) {        
        pJoint->centroid[k] += pSkelVertex->m_Coords[k];
      }
    } //end else
  } //end for i

  int nJoints = (int)joints.size();
  for (int i = 0; i < nJoints; i++)
  {
    JOIN_VERTEX* pJoint = joints[i];
    int nDegree = (int)pJoint->vertices.size();
    
    for (int k = 0; k < 3; k++) {        
      pJoint->centroid[k] /= nDegree;
    }
  }

  //now compute parametrization for every edge
  int nCount = (int)m_SuperSkeleton->m_pOC_Skel->Edges.size();
  for (int i = 0; i < nCount; i++)
  {
    CSkeletonEdge* pEdge = m_SuperSkeleton->m_pOC_Skel->Edges[i];

    //parametrization for the current edge
    ComputeParametrization(pEdge->m_ROI, pEdge, -1, 0.0);
//    ComputeParametrization(pEdge, pEdge, -1);
  }

  //and include blending edges
  for (int i = 0; i < nCount; i++)
  {
    CSkeletonEdge* pEdge = m_SuperSkeleton->m_pOC_Skel->Edges[i];
    if (pEdge->Verts[0] == NULL || pEdge->Verts[1] == NULL)
      continue; //cannot be a joint, continue

    for (int j = 0; j < 2; j++)
    {
      int nJntId = pEdge->Verts[j]->m_nMark;
      if (nJntId < 0)
        continue; //not a joint, continue

      JOIN_VERTEX* pJoint = joints[nJntId];
      int nDegree = (int)pJoint->vertices.size();
      for (int k = 0; k < nDegree; k++)
      {
        CSkeletonVertex* pSkelVertex = pJoint->vertices[k];
        if (pSkelVertex == pEdge->Verts[j])
          continue; //this edge was already parametrized        

        CSkeletonEdge* pNbEdge = pSkelVertex->m_OneRingEdges[0];        
        if (pNbEdge->Verts[0] == NULL || pNbEdge->Verts[1] == NULL)
          pNbEdge = pSkelVertex->m_OneRingEdges[1]; //must exist

        //we want to parametrize points associated with pNbEdge
        //using the current edge pEdge        
        double sigma2 = 2*vtkMath::Distance2BetweenPoints(
          pEdge->Verts[1 - j]->m_Coords, pJoint->centroid);

        ComputeParametrization(pNbEdge->m_ROI, pEdge, 1 - j, sigma2);
        //ComputeParametrization(pNbEdge, pEdge, 1 - j);
      } //end for k
    } //end for j
  } //end for i

  
  //cleanup
  for (int i = 0; i < nJoints; i++) {
    delete joints[i];
  }
}


//------------------------------------------------------------------------
//Computes new position of vertices in the output mesh.
//In this last step, positions of vertices are modified according to their
//parametrization in respect to the superskeleton. 
//
//N.B. the given output polydata must be compatible with the input polydata
void vtkALBAPolyDataDeformation_M1::DeformMesh(vtkPolyData* output)
//------------------------------------------------------------------------
{  
  vtkPoints* points = output->GetPoints();
  int nPoints = output->GetNumberOfPoints();
  int nCount = (int)m_SuperSkeleton->m_pDC_Skel->Edges.size();
  
  double* EdgeLengths = new double[nCount];
  double* EdgeElongations = new double[nCount];
  for (int i = 0; i < nCount; i++)
  {
    EdgeLengths[i] = m_SuperSkeleton->m_pDC_Skel->Edges[i]->GetLength();
    if (this->PreserveVolume)
    {
      EdgeElongations[i] = m_SuperSkeleton->m_pOC_Skel->Edges[i]->GetLength();
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
    CMeshVertex* pMeshVertex = &m_MeshVertices[i];
    int nParams = (int)pMeshVertex->m_Parametrization.size();
    
//TODO: remove
//nParams = 1;

    const double* oldCoords = points->GetPoint(i); 
    double newCoords[3];
    
    for (int j = 0; j < nParams; j++)
    {
      CMeshVertex::VERTEX_PARAM& ssParam = pMeshVertex->m_Parametrization[j];
      
      CSkeletonVertex* pSkelVert1 = ssParam.pEdge->m_pMatch->Verts[0]; 
      CSkeletonVertex* pSkelVert2 = ssParam.pEdge->m_pMatch->Verts[1];

      if (pSkelVert1 == NULL || pSkelVert2 == NULL)
      {
        //half-space skeleton edge
        //compute simple Cartesian coordinates
        CSkeletonVertex* pSkelVert = pSkelVert1 != NULL ? pSkelVert1 : pSkelVert2;
        for (int k = 0; k < 3; k++) 
        {
          newCoords[k] = pSkelVert->m_Coords[k] + 
            ssParam.pcoords[0]*pSkelVert->m_LF.u[k] + 
            ssParam.pcoords[1]*pSkelVert->m_LF.v[k] + 
            ssParam.pcoords[2]*pSkelVert->m_LF.w[k];
        }
      }      
      else
      {
        //the control edge is well defined
        //we need to incorporate elongation of edge
        //compute points vmp and vmn on both planes
        double vmp[3], vmn[3];
        for (int k = 0; k < 3; k++)
        {
          //vmp = Pk1 + b*v1 + c*w1
          vmp[k] = pSkelVert1->m_Coords[k] + 
            ssParam.pcoords[1]*pSkelVert1->m_LF.v[k] + 
            ssParam.pcoords[2]*pSkelVert1->m_LF.w[k];
        }

        //vmn can be calculated as the intersection of line going through vmp
        //in u1 direction with the plane that goes through in Pk2 and has normal u2
        //intersection of line: X = P + u*t and plane: n*X + d = 0 
        //can be computed as P + u*ti and ti is computed:
        //n*X + d = 0 => n*(P + u*ti) + d = 0 => n*P + n*u*ti + d = 0 =>
        //ti = -(n*P + d)/n*u
        //        
        //for our plane ti =(u2*Pk2 - u2*vpm)/(u1*u2)        
        //N.B. we assume that planes are not orthogonal => u1*u2 != 0          
        double ti = (vtkMath::Dot(pSkelVert2->m_LF.u, pSkelVert2->m_Coords) - 
          vtkMath::Dot(pSkelVert2->m_LF.u, vmp)) / 
          vtkMath::Dot(pSkelVert1->m_LF.u, pSkelVert2->m_LF.u);

        for (int k = 0; k < 3; k++) {
          vmn[k] = vmp[k] + ti*pSkelVert1->m_LF.u[k];
        }

        //set the new position of point according to the elongation (shrinkage)
        //of the current edge - just linear equation 
        double dblNewDist = sqrt(vtkMath::Distance2BetweenPoints(vmp, vmn));          
        double w = ssParam.dblRm*dblNewDist;

        if (j == 0)
        {
          //the first parametrization is always the main one
          //and should have weight == 1.0
          for (int k = 0; k < 3; k++) {
            newCoords[k] = vmp[k] + w*pSkelVert1->m_LF.u[k];
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
              ptEnd[k] = newCoords[k] - pSkelVert1->m_Coords[k];
            }

            double w = vtkMath::Dot(pSkelVert1->m_LF.u, ptEnd);
            for (int k = 0; k < 3; k++) {
              ptEnd[k] = pSkelVert1->m_Coords[k] + w*pSkelVert1->m_LF.u[k];
            }

            for (int k = 0; k < 3; k++) {
              newCoords[k] = ptEnd[k] + (newCoords[k] - ptEnd[k])*
                EdgeElongations[ssParam.pEdge->Id];
            }
          }
        }
        else
        {
          //further parametrization - those are less important and
          //will slightly modify the position of already modified coordinates
          for (int k = 0; k < 3; k++) 
          {
            double dblTmp = vmp[k] + w*pSkelVert1->m_LF.u[k];
            newCoords[k] = (1.0 - ssParam.dblWeight)*newCoords[k] +
              ssParam.dblWeight*dblTmp;
          }
        }
      } //end else both vertices exist      
    } //for j
        
    //for (int j = 0; j < 3; j++) {
    //  newCoords[j] /= nParams;
    //}

    points->SetPoint(i, newCoords);   
  } //for i

  delete[] EdgeLengths;
  delete[] EdgeElongations;
}

//------------------------------------------------------------------------
//Creates the internal skeleton data structure for the given polydata
vtkALBAPolyDataDeformation_M1::CSkeleton* 
vtkALBAPolyDataDeformation_M1::CreateSkeleton(vtkPolyData* pPoly)
//------------------------------------------------------------------------
{  
  int nVerts = pPoly->GetNumberOfPoints();  

  CSkeleton* pSkel = new CSkeleton();
  pSkel->Vertices.reserve(nVerts);

  //load points
  for (int i = 0; i < nVerts; i++)
  {
    CSkeletonVertex* pVertex = new CSkeletonVertex();

    pPoly->GetPoint(pVertex->m_Id = i, pVertex->m_Coords);
    pSkel->Vertices.push_back(pVertex);
  }

  //ensure that we have cells
  pPoly->BuildCells();
  int nCells = pPoly->GetNumberOfCells();
  pSkel->Edges.reserve(nVerts - 1);

  int nEdgeId = 0;
  for (int i = 0; i < nCells; i++)
  {
    vtkNew<vtkIdList> pIds;
    pPoly->GetCellPoints(i, pIds);
    vtkIdType nCount = pIds->GetNumberOfIds();

    //create edges
    for (int j = 1; j < nCount; j++)
    {
      //get the next edge
      CSkeletonEdge* pEdge = new CSkeletonEdge();
      pEdge->Id = nEdgeId++;
      pEdge->Verts[0] = pSkel->Vertices[pIds->GetId(j - 1)];
      pEdge->Verts[1] = pSkel->Vertices[pIds->GetId(j)];

      pEdge->Verts[0]->m_OneRingEdges.push_back(pEdge);
      pEdge->Verts[1]->m_OneRingEdges.push_back(pEdge);
      pSkel->Edges.push_back(pEdge);
    }
  }

  return pSkel;
}

//------------------------------------------------------------------------
//Computes the average length of edges of the input mesh
double vtkALBAPolyDataDeformation_M1::ComputeInputMeshAvgEdgeLength()
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
    vtkNew<vtkIdList> ptIds;
    input->GetCellPoints(i, ptIds);
    vtkIdType nPoints = ptIds->GetNumberOfIds();

    for (int j = 0; j < nPoints; j++)
    {
      double coords1[3], coords2[3];
      input->GetPoint(ptIds->GetId(j), coords1);
      input->GetPoint(ptIds->GetId((j + 1) % nPoints), coords2);

      dblEdgeLen += sqrt(vtkMath::Distance2BetweenPoints(coords1, coords2));
    }

    nEdges += nPoints;
  }

  dblEdgeLen = dblEdgeLen / nEdges;
  return dblEdgeLen;
}

#ifdef DEBUG_vtkALBAPolyDataDeformation_M1
//------------------------------------------------------------------------
//Debug routine that creates polydata from superskeleton
void vtkALBAPolyDataDeformation_M1::CreatePolyDataFromSuperskeleton()
//------------------------------------------------------------------------
{
  m_MATCHED_FULLCC = vtkIdList::New(); 
  m_MATCHED_POLYS[0] = vtkPolyData::New();
  m_MATCHED_POLYS[1] = vtkPolyData::New();
  m_MATCHED_POLYS[0]->SetPoints(vtkPoints::New());
  m_MATCHED_POLYS[1]->SetPoints(vtkPoints::New());
  m_MATCHED_POLYS[0]->SetLines(vtkCellArray::New());
  m_MATCHED_POLYS[1]->SetLines(vtkCellArray::New());
  int nCount = (int)m_SuperSkeleton->m_pOC_Skel->m_Vertices.size();
  for (int i = 0; i < nCount; i++)
  {
    CSkeletonVertex* pVertex = m_SuperSkeleton->m_pOC_Skel->m_Vertices[i];
    m_MATCHED_POLYS[0]->GetPoints()->InsertNextPoint(pVertex->m_Coords);
    m_MATCHED_POLYS[1]->GetPoints()->InsertNextPoint(pVertex->m_pMatch->m_Coords);
    m_MATCHED_FULLCC->InsertNextId(i); m_MATCHED_FULLCC->InsertNextId(i);
  }

  vtkIdList* pList = vtkIdList::New();
  vtkIdType* pIds = pList->WritePointer(0, 2);

  nCount = (int)m_SuperSkeleton->m_pOC_Skel->m_Edges.size();
  for (int i = 0; i < nCount; i++)
  {
    CSkeletonEdge* pEdge = m_SuperSkeleton->m_pOC_Skel->m_Edges[i];
    if (pEdge->m_Verts[0] != NULL && pEdge->m_Verts[1] != NULL)
    {
      pIds[0] = pEdge->m_Verts[0]->m_Id;
      pIds[1] = pEdge->m_Verts[1]->m_Id;
      m_MATCHED_POLYS[0]->InsertNextCell(VTK_LINE, pList);
      m_MATCHED_POLYS[1]->InsertNextCell(VTK_LINE, pList);
    }
  }

  pList->Delete();

  m_MATCHED_POLYS[0]->GetPoints()->Delete();
  m_MATCHED_POLYS[1]->GetPoints()->Delete();
  m_MATCHED_POLYS[0]->GetLines()->Delete();
  m_MATCHED_POLYS[1]->GetLines()->Delete();
}

//------------------------------------------------------------------------
//Destroys the debug data
void vtkALBAPolyDataDeformation_M1::DestroyMATCHEDData()
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