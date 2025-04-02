/*========================================================================= 
Program: Multimod Application Framework RELOADED 
Module: $RCSfile: vtkALBAPolyDataDeformation.cpp,v $ 
Language: C++ 
Date: $Date: 2010-06-08 08:23:57 $ 
Version: $Revision: 1.1.2.6 $ 
Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
========================================================================== 
Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
See the COPYINGS file for license details 
=========================================================================
*/

#include "vtkALBAPolyDataDeformation.h"
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

#ifdef DEBUG_vtkALBAPolyDataDeformation
#include "vtkCharArray.h"
#endif



vtkStandardNewMacro(vtkALBAPolyDataDeformation);

#include "albaMemDbg.h"
#include "albaDbg.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

#pragma region //Nested Classes

//Returns length of the edge. 
//If the edge does not have both vertices defined, it returns 0.0
double vtkALBAPolyDataDeformation::CSkeletonEdge::GetLength()
{
  if (Verts[0] == NULL || Verts[1] == NULL)
    return 0.0;
  else
    return sqrt(vtkMath::Distance2BetweenPoints(
    Verts[0]->Coords, Verts[1]->Coords));
}

vtkALBAPolyDataDeformation::CSkeleton::~CSkeleton()
{
  int nCount = (int)Vertices.size();
  for (int i = 0; i < nCount; i++)
  {
    delete Vertices[i];
  }

  nCount = (int)Edges.size();
  for (int i = 0; i < nCount; i++)
  {
    delete Edges[i];
  }

  Vertices.clear();
  Edges.clear();
}

//------------------------------------------------------------------------
//Computes topology weights for vertices
void vtkALBAPolyDataDeformation::CSkeleton::ComputeTW()
//------------------------------------------------------------------------
{     
  int nVertices = (int)Vertices.size();
  int nEdges = (int)Edges.size();

  //process every vertex 
  for (int i = 0; i < nVertices; i++)
  {
    CSkeletonVertex* pVertex = Vertices[i];    
    int nVertEdges = pVertex->GetDegree();    
    if (nVertEdges == 2)
      pVertex->WT = 0;    //invalid point, to be reduced
    else    
    {
      pVertex->WT = 0;    //nVertEdges;

      //this is a junction, compute weight for every
      //edge connected to the current vertex and get average weight 
      //int nMaxWeight = 0;
      for (int i = 0; i < nVertEdges; i++)
      {
        CSkeletonEdge* pEdge = pVertex->OneRingEdges[i];  
        int nWeight = ComputeEdgeWeight(pEdge, (pEdge->Verts[0] == pVertex ? 1 : 0));
        pVertex->WT += ((double)nWeight) / nEdges;
      }        
    }
  }
}

//------------------------------------------------------------------------
//Computes the weight for the given edge.
//The computation is recursive in the given direction.
int vtkALBAPolyDataDeformation::CSkeleton::ComputeEdgeWeight(CSkeletonEdge* pEdge, int iDir)
//------------------------------------------------------------------------
{
  CSkeletonVertex* pVertex = pEdge->Verts[iDir];
  int nCount = pVertex->GetDegree();
  if (nCount == 1)  //end point
    return 1;

  int nRetValue = 0;
  for (int i = 0; i < nCount; i++)
  {
    CSkeletonEdge* pNbEdge = pVertex->OneRingEdges[i];
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
void vtkALBAPolyDataDeformation::CSkeleton::GetBoundingBox(double bnds[6])
//------------------------------------------------------------------------
{  
  bnds[0] = bnds[2] = bnds[4] = 0xFFFFFFFF;
  bnds[1] = bnds[3] = bnds[5] = -bnds[0];

  int nCount = (int)Vertices.size();

  for (int i = 0; i < nCount; i++)
  {
    double* pcoods = Vertices[i]->Coords;

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
vtkALBAPolyDataDeformation::CMatrix<T>::CMatrix(int rows, int columns) 
{  
  // alloc arrays
  Matrix = new T*[rows]; // rows
  for ( int i = 0 ; i < rows ; i++ )
  {
    Matrix[i] = new T[columns]; // columns
    memset(Matrix[i], 0, sizeof(T)*columns);
  }

  NumOfRows = rows;
  NNumOfCols = columns;
}

template <class T>
vtkALBAPolyDataDeformation::CMatrix<T>::~CMatrix() 
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

int vtkALBAPolyDataDeformation::CMunkres::Step1(void) 
{
  for ( int row = 0 ; row < (*Matrix).GetNumberOfRows() ; row++ )
    for ( int col = 0 ; col < (*Matrix).GetNumberOfColumns() ; col++ )
      if ( (*Matrix)(row,col) == 0 ) {
        bool isstarred = false;
        for ( int nrow = 0 ; nrow < (*Matrix).GetNumberOfRows() ; nrow++ )
          if ( (*MaskMatrix)(nrow,col) == Z_STAR )
            isstarred = true;

        if ( !isstarred ) {
          for ( int ncol = 0 ; ncol < (*Matrix).GetNumberOfColumns() ; ncol++ )
            if ( (*MaskMatrix)(row,ncol) == Z_STAR )
              isstarred = true;
        }

        if ( !isstarred ) {
          (*MaskMatrix)(row,col) = Z_STAR;
        }
      }

      return 2;
}

int vtkALBAPolyDataDeformation::CMunkres::Step2(void) 
{
  int covercount = 0;
  for ( int row = 0 ; row < (*Matrix).GetNumberOfRows() ; row++ )
    for ( int col = 0 ; col < (*Matrix).GetNumberOfColumns() ; col++ )
      if ( (*MaskMatrix)(row,col) == Z_STAR ) {
        ColMask[col] = true;
        covercount++;
      }

      int k = (*Matrix).GetMinSize();

      if ( covercount >= k )
        return 0;      

      return 3;
}

int vtkALBAPolyDataDeformation::CMunkres::Step3(void) 
{
  /*
  Main Zero Search

  1. Find an uncovered Z in the distance (*matrix) and prime it. If no such zero exists, go to Step 5
  2. If No Z* exists in the row of the Z', go to Step 4.
  3. If a Z* exists, cover this row and uncover the column of the Z*. Return to Step 3.1 to find a new Z
  */
  if ( FindUncoveredInMatrix(0, Saverow, Savecol) ) {
    (*MaskMatrix)(Saverow,Savecol) = Z_PRIME; // prime it.
  } else {
    return 5;
  }

  for ( int ncol = 0 ; ncol < (*Matrix).GetNumberOfColumns() ; ncol++ )
    if ( (*MaskMatrix)(Saverow,ncol) == Z_STAR ) {
      RowMask[Saverow] = true; //cover this row and
      ColMask[ncol] = false; // uncover the column containing the starred zero
      return 3; // repeat
    }

    return 4; // no starred zero in the row containing this primed zero
}

int vtkALBAPolyDataDeformation::CMunkres::Step4(void) 
{
  std::list<std::pair<int,int> > seq;
  // use saverow, savecol from step 3.
  std::pair<int,int> z0(Saverow, Savecol);
  std::pair<int,int> z1(-1,-1);
  std::pair<int,int> z2n(-1,-1);
  seq.insert(seq.end(), z0);
  int row, col = Savecol;
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
    for ( row = 0 ; row < (*Matrix).GetNumberOfRows() ; row++ )
      if ( (*MaskMatrix)(row,col) == Z_STAR ) {
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

      for ( col = 0 ; col < (*Matrix).GetNumberOfColumns() ; col++ )
        if ( (*MaskMatrix)(row,col) == Z_PRIME ) {
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
      if ( (*MaskMatrix)(i->first,i->second) == Z_STAR )
        (*MaskMatrix)(i->first,i->second) = Z_NORMAL;

      // 3. Star each primed zero of the sequence,
      // thus increasing the number of starred zeros by one.
      if ( (*MaskMatrix)(i->first,i->second) == Z_PRIME )
        (*MaskMatrix)(i->first,i->second) = Z_STAR;
  }

  // 4. Erase all primes, uncover all columns and rows, 
  for ( int row = 0 ; row < (*MaskMatrix).GetNumberOfRows() ; row++ )
    for ( int col = 0 ; col < (*MaskMatrix).GetNumberOfColumns() ; col++ )
      if ( (*MaskMatrix)(row,col) == Z_PRIME )
        (*MaskMatrix)(row,col) = Z_NORMAL;

  for ( int i = 0 ; i < (*Matrix).GetNumberOfRows() ; i++ ) {
    RowMask[i] = false;
  }

  for ( int i = 0 ; i < (*Matrix).GetNumberOfColumns() ; i++ ) {
    ColMask[i] = false;
  }

  // and return to Step 2. 
  return 2;
}

int vtkALBAPolyDataDeformation::CMunkres::Step5(void) {
  /*
  New Zero Manufactures

  1. Let h be the smallest uncovered entry in the (modified) distance (*matrix).
  2. Add h to all covered rows.
  3. Subtract h from all uncovered columns
  4. Return to Step 3, without altering stars, primes, or covers. 
  */
  double h = 0;
  for ( int row = 0 ; row < (*Matrix).GetNumberOfRows() ; row++ ) {
    if ( !RowMask[row] ) {
      for ( int col = 0 ; col < (*Matrix).GetNumberOfColumns() ; col++ ) {
        if ( !ColMask[col] ) {
          if ( (h > (*Matrix)(row,col) && (*Matrix)(row,col) != 0) || h == 0 ) {
            h = (*Matrix)(row,col);
          }
        }
      }
    }
  }

  for ( int row = 0 ; row < (*Matrix).GetNumberOfRows() ; row++ )
    for ( int col = 0 ; col < (*Matrix).GetNumberOfColumns() ; col++ ) {
      if ( RowMask[row] )
        (*Matrix)(row,col) += h;

      if ( !ColMask[col] )
        (*Matrix)(row,col) -= h;
    }

    return 3;
}

void vtkALBAPolyDataDeformation::CMunkres::Solve( CMatrix< double >* m, CMatrix< int >* matches )
{
  // Linear assignment problem solution
  // [modifies matrix in-place.]
  // matrix(row,col): row major format assumed.

  // Assignments are remaining 0 values
  // (extra 0 values are replaced with -1)

  bool notdone = true;
  int step = 1;

  this->Matrix = m;
  // Z_STAR == 1 == starred, Z_PRIME == 2 == primed
  this->MaskMatrix = matches;

  RowMask = new bool[Matrix->GetNumberOfRows()];
  ColMask = new bool[Matrix->GetNumberOfColumns()];
  for ( int i = 0 ; i < Matrix->GetNumberOfRows() ; i++ ) {
    RowMask[i] = false;
  }

  for ( int i = 0 ; i < Matrix->GetNumberOfColumns(); i++ ) {
    ColMask[i] = false;
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
  for ( int row = 0 ; row < Matrix->GetNumberOfRows() ; row++ )
  {
    for ( int col = 0 ; col < Matrix->GetNumberOfColumns() ; col++ )
    {
      if ( (*MaskMatrix)(row,col) == Z_STAR )
        (*MaskMatrix)(row,col) = 1;
      else
        (*MaskMatrix)(row,col) = 0;
    }
  }  

  delete [] RowMask;
  delete [] ColMask;
}
#pragma endregion //CMunkres algorithm
#pragma endregion //Nested Classes

vtkALBAPolyDataDeformation::vtkALBAPolyDataDeformation()
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

#ifdef DEBUG_vtkALBAPolyDataDeformation
  m_MATCHED_CC = NULL; m_MATCHED_FULLCC = NULL;
  m_MATCHED_POLYS[0] = NULL; m_MATCHED_POLYS[1] = NULL;
#endif
}

vtkALBAPolyDataDeformation::~vtkALBAPolyDataDeformation()
{
  //destroy skeletons
  SetNumberOfSkeletons(0);  

  //superskeleton should be destroyed by now
  _ASSERT(SuperSkeleton == NULL);
  DestroySuperSkeleton();
  
#ifdef DEBUG_vtkALBAPolyDataDeformation
  DestroyMATCHEDData();  
#endif  

  //this should be released already
  _ASSERT(MeshVertices == NULL);
  delete[] MeshVertices;
  MeshVertices = NULL;
}

//------------------------------------------------------------------------
//Sets the number of control skeletons.  
//Old skeletons are copied (and preserved)
/*virtual*/ void vtkALBAPolyDataDeformation::SetNumberOfSkeletons( int nCount )
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
//If RSO points are specified, they are used during the computation of LFs
//of curves of both skeletons. A local fame is defined by its origin point 
//and three vectors u, v and w. Vector u is the tangent vector (it goes in
//the direction of polyline) and vectors v,w are perpendicular to this vector.
//As there is infinite number of u,v,w configurations, the algorithm uses the
//given RSO point to get a unique one (v lies in the plane defined by u and RSO). 
//If RSO is not specified, v is chosen to lie in the plane closest to the u vector.
//When RSO points are not specified (or they are specified incorrectly), 
//the deformed object might be unrealistically rotated against other objects 
//in the scene, if the skeleton of object to deform tends to rotate (simple edge, 
//or only one skeleton for object). 
/*virtual*/ void vtkALBAPolyDataDeformation::SetNthSkeleton( int idx, 
            vtkPolyData* original, vtkPolyData* modified, vtkIdList* correspondence,
                                         double* original_rso, double* modified_rso)
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

  if (Skeletons[idx].RSOValid[0] != (original_rso != NULL))
  {
    if (Skeletons[idx].RSOValid[0] = (original_rso != NULL)) 
    {
      Skeletons[idx].RSO[0][0] = original_rso[0];
      Skeletons[idx].RSO[0][1] = original_rso[1];
      Skeletons[idx].RSO[0][2] = original_rso[2];
    }

    this->Modified();
  }

  if (Skeletons[idx].RSOValid[1] != (modified_rso != NULL))
  {
    if (Skeletons[idx].RSOValid[1] = (modified_rso != NULL)) 
    {
      Skeletons[idx].RSO[1][0] = modified_rso[0];
      Skeletons[idx].RSO[1][1] = modified_rso[1];
      Skeletons[idx].RSO[1][2] = modified_rso[2];
    }

    this->Modified();
  }
}

//------------------------------------------------------------------------
//Return this object's modified time.
/*virtual*/ vtkMTimeType vtkALBAPolyDataDeformation::GetMTime()
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
/*virtual*/int vtkALBAPolyDataDeformation::RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//------------------------------------------------------------------------
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	
	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (input == NULL)
  {
    vtkErrorMacro(<< "Invalid input for vtkALBAPolyDataDeformation.");
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
/*virtual*/void vtkALBAPolyDataDeformation::ExecuteData(vtkDataObject *output)
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
    vtkWarningMacro(<< "Invalid output for vtkALBAPolyDataDeformation.");
    return;   //we have no valid output
  }  

  //process every single skeleton and construct 
  //super skeleton where everything is matched
  if (!CreateSuperSkeleton())
  {
    vtkWarningMacro(<< "Missing control skeleton for vtkALBAPolyDataDeformation.");
    return;
  }

  //OK, we have super skeleton, let us build cells and neighbors (if they do not exist)
  //for the input mesh as we will need then to quickly traverse through the mesh
  MeshVertices = new CMeshVertex[input->GetNumberOfPoints()];
  input->BuildCells(); input->BuildLinks();
  
  //for every curve, we need to compute its ROI, i.e., vertices that are mapped for this curve 
  int iCurSkel = 0;
  int nCount = (int)SuperSkeleton->POCSkel->Vertices.size();
  for (int iStartPos = 0; iStartPos < nCount; )
  {
    CSkeletonVertex* pOC_Curve = SuperSkeleton->POCSkel->Vertices[iStartPos];
    iStartPos += GetNumberOfCurveVertices(pOC_Curve);

    while (SuperSkeleton->PSkelPositions[iCurSkel] < iStartPos) {
      iCurSkel++; //advance to the next skeleton
    }

    //compute local frames for both curves, original and deformed one
    ComputeLFS(pOC_Curve, 
      (Skeletons[iCurSkel].RSOValid[0] ? Skeletons[iCurSkel].RSO[0] : NULL),
      (Skeletons[iCurSkel].RSOValid[1] ? Skeletons[iCurSkel].RSO[1] : NULL)
      );
  }  

  //let us parametrize the mesh
  ComputeMeshParametrization();

  //and finally, deform the output mesh
  DeformMesh(pPoly);

#ifdef DEBUG_vtkALBAPolyDataDeformation
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
  CSkeletonEdge* pEdge = SuperSkeleton->m_pOC_Skel->m_Edges[iDisp];
  nCount = pEdge->m_ROI.size();
  for (int i = 0; i < nCount; i++)
  {
    scalar->SetTuple1(pEdge->m_ROI[i], 0);
  }
*/

  for (int i = 0; i < nCount; i++)
  {
    CMeshVertex& pVert = MeshVertices[i];    
    if (pVert.size() == 0)
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
      scalar->SetTuple1(i, pVert[0].m_pEdge->m_Id);  
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
bool vtkALBAPolyDataDeformation::CreateSuperSkeleton()
//------------------------------------------------------------------------
{ 
  double dblEdgeFactor = DivideSkeletonEdges ?
    ComputeInputMeshAvgEdgeLength() : 0.0;
  dblEdgeFactor *= dblEdgeFactor*4;  //segment is k times larger than average edge length

  DestroySuperSkeleton();

  //combine every control skeleton to create match
  int* pSkelPoints = new int[NumberOfSkeletons];
  for (int i = 0; i < NumberOfSkeletons; i++)
  {
    CreateSuperSkeleton(Skeletons[i].pPolyLines[0], 
      Skeletons[i].pPolyLines[1], Skeletons->pCCList, dblEdgeFactor);

    pSkelPoints[i] = (SuperSkeleton != NULL) ? (int)SuperSkeleton->POCSkel->Vertices.size() : 0;      
  } 

  if (SuperSkeleton != NULL)
    SuperSkeleton->PSkelPositions = pSkelPoints;
  else
  {
    delete[] pSkelPoints;
    return false;    
  }

  int nCount = (int)SuperSkeleton->POCSkel->Vertices.size();
  if (nCount == 0)
  {
    DestroySuperSkeleton();
    return false;
  }
  
#ifdef DEBUG_vtkALBAPolyDataDeformation  
  DestroyMATCHEDData();    
  CreatePolyDataFromSuperskeleton();
#endif

  //now we will extend the superskeleton by adding infinite
  //edges connected to end-points of the skeleton
  int nEdgeId = (int)SuperSkeleton->POCSkel->Edges.size();
  for (int i = 0; i < nCount; i++)
  {
    CSkeletonVertex* pVertex = SuperSkeleton->POCSkel->Vertices[i];
    if (pVertex->WT == 0) //terminal nodes
    {
      CSkeletonEdge *pNewEdge, *pDNewEdge;

      //this is either the first or the last vertex of curve
      if (GetNextCurveVertex(pVertex) != NULL)
      {
        //OK, it is first vertex
        pNewEdge = new CSkeletonEdge(NULL, pVertex);
        pVertex->OneRingEdges.push_back(pNewEdge);            

        pDNewEdge = new CSkeletonEdge(NULL, pVertex->PMatch);
        pVertex->PMatch->OneRingEdges.push_back(pDNewEdge);
      }
      else
      {
        //the last one => it will be slightly more complex
        pNewEdge = new CSkeletonEdge(pVertex, NULL);
        pVertex->OneRingEdges.insert(pVertex->OneRingEdges.begin(), pNewEdge);            

        pDNewEdge = new CSkeletonEdge(pVertex->PMatch, NULL);
        pVertex->PMatch->OneRingEdges.insert(pVertex->PMatch->
          OneRingEdges.begin(), pDNewEdge);
      }      

      pNewEdge->Id = nEdgeId;
      pDNewEdge->Id = nEdgeId++;

      pNewEdge->PMatch = pDNewEdge;
      pDNewEdge->PMatch = pNewEdge;

      SuperSkeleton->POCSkel->Edges.push_back(pNewEdge);
      SuperSkeleton->PDCSkel->Edges.push_back(pDNewEdge);      
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
void vtkALBAPolyDataDeformation::CreateSuperSkeleton(
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
  int nOCVerts = (int)pOC_Skel->Vertices.size();  
  for (int i = 0; i < nOCVerts; i++) {
    pOC_Skel->Vertices[i]->NMark = -1;    
  }

  int nDCVerts = (int)pDC_Skel->Vertices.size();
  for (int i = 0; i < nDCVerts; i++) {
    pDC_Skel->Vertices[i]->NMark = -1;    
  }

  //set match marks
  int nCount = pFullCC->GetNumberOfIds();
  vtkIdType* pIds = pFullCC->GetPointer(0);
  for (int i = 0; i < nCount; i += 2)
  {
    pOC_Skel->Vertices[pIds[i]]->NMark = pIds[i + 1];
    pDC_Skel->Vertices[pIds[i + 1]]->NMark = pIds[i];
  }

  //marks for every edge should be zeros as they are not used in MatchSkeletons
  _ASSERT(pOC_Skel->Edges[0]->NMark == 0);  
   
  //detect every matched curve  
  CSkeletonVertex** pOC_Curve = new CSkeletonVertex*[nOCVerts + nDCVerts];
  CSkeletonVertex** pDC_Curve = new CSkeletonVertex*[nOCVerts + nDCVerts];  

  for (int i = 0; i < nOCVerts; i++)
  {
    CSkeletonVertex* pVert1 = pOC_Skel->Vertices[i];
    if (pVert1->NMark < 0)
      continue; //this is inner vertex

    int nEdges = (int)pVert1->OneRingEdges.size();
    for (int j = 0; j < nEdges; j++)
    {
      if (pVert1->OneRingEdges[j]->NMark != 0)
        continue; //this curve was already traced

      int nOCCurveVerts = TraceSkeletonCurve(pVert1, j, pOC_Curve);
      CSkeletonVertex* pVert2 = pOC_Curve[nOCCurveVerts - 1];
      if (pVert2->NMark < 0)
        continue; //this curve is invalid and will be ignored

      //now find the matching curve in the deformed      
      CSkeletonVertex* pDVert1 = pDC_Skel->Vertices[pVert1->NMark];
      CSkeletonVertex* pDVert2 = pDC_Skel->Vertices[pVert2->NMark];
                  
      int nDCCurveVerts;
      int nDEdges = (int)pDVert1->OneRingEdges.size();
      for (int k = 0; k < nDEdges; k++)
      {
        if (pDVert1->OneRingEdges[k]->NMark == 0)
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
vtkIdList* vtkALBAPolyDataDeformation::MatchSkeletons(CSkeleton* pOC, 
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
      pVertex->NMark = -1;
    else
      pVertex->NMark = 0;
  }

  int nDCVerts = (int)pDC->Vertices.size();
  for (int i = 0; i < nDCVerts; i++)
  {
    //mark the point, if it is not simple joined
    CSkeletonVertex* pVertex = pDC->Vertices[i];
    if (pVertex->GetDegree() == 2)
      pVertex->NMark = -1;
    else
      pVertex->NMark = 0;
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
        pOC->Vertices[pIds[i - 1]]->NMark = 1;
        pDC->Vertices[pIds[i]]->NMark = 1;
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
    if (pOCVerts[nOCVerts]->NMark == 0)
      nOCVerts++; //this is end-point or junction
  }

  CSkeletonVertex** pDCVerts = new CSkeletonVertex*[nDCVerts];
  nCount = nDCVerts; nDCVerts = 0;
  for (int i = 0; i < nCount; i++)
  {    
    pDCVerts[nDCVerts] = pDC->Vertices[i];
    if (pDCVerts[nDCVerts]->NMark == 0)
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
      double ri = sqrt(vtkMath::Distance2BetweenPoints(pVert1->Coords, 
        pVert2->Coords)) / dblMaxDiag;

      //N.B. ti is always from interval <0,1)
      double ti = (pVert1->WT - pVert2->WT)*(pVert1->WT - pVert2->WT) /
        (pVert1->WT + pVert2->WT);

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
        pIds[nPairs++] = pOCVerts[i]->Id;
        pIds[nPairs++] = pDCVerts[j]->Id;
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
int vtkALBAPolyDataDeformation::TraceSkeletonCurve(
                        CSkeletonVertex* pStartVertex, int iDir, CSkeletonVertex** pOutBuf)
//------------------------------------------------------------------------
{ 
  int nRetCount = 0;
  do 
  {
    pOutBuf[nRetCount] = pStartVertex;
    nRetCount++;

    CSkeletonEdge* pEdge = pStartVertex->OneRingEdges[iDir];
    if (pEdge->Verts[0] == pStartVertex)    
      pStartVertex = pEdge->Verts[1];    
    else
      pStartVertex = pEdge->Verts[0];

    //get the adjacent edge
    iDir = (pStartVertex->OneRingEdges[0] == pEdge) ? 1 : 0; 
  }
  while(pStartVertex->NMark < 0);

  //store the last vertex
  pOutBuf[nRetCount] = pStartVertex;
  return ++nRetCount;
}

//------------------------------------------------------------------------
//Marks every edge of the given skeleton curve
void vtkALBAPolyDataDeformation::MarkCurveEdges(CSkeletonVertex** pCurve, int nCount)
//------------------------------------------------------------------------
{
  //first and last vertex may have more than one or more edges as they may
  //be junction nodes, the rest have just two edges
  for (int i = 1; i < nCount - 1; i++)
  {
    pCurve[i]->OneRingEdges[0]->NMark = 1;
    pCurve[i]->OneRingEdges[1]->NMark = 1;
  }

  //if there is at least one inner vertex than we are ready
  if (nCount == 2)
  {
    //otherwise, we need to find the proper edge
    int nEdges = (int)pCurve[0]->OneRingEdges.size();
    for (int i = 0; i < nEdges; i++)
    {
      CSkeletonEdge* pEdge = pCurve[0]->OneRingEdges[i];
      if (
        (pEdge->Verts[0] == pCurve[0] && pEdge->Verts[1] == pCurve[1]) ||
        (pEdge->Verts[0] == pCurve[1] && pEdge->Verts[1] == pCurve[0])
        )      
      {
        //this is the correct edge
        pEdge->NMark = 1; 
        break;
      }
    } //end for
  }
}


//this structure is used in MatchCurves
typedef struct vtkALBAPolyDataDeformation::CURVE_VERTEX
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
int vtkALBAPolyDataDeformation::MatchCurves(CSkeletonVertex** pOC, int nOCVerts, 
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
    pVertexPool[nNextVertex].pVertex = new CSkeletonVertex(pOC_DC[i][0]->Coords);    
    pVertexPool[nNextVertex].pVertex->WT = pOC_DC[i][0]->GetDegree() - 1;

    pVertexPool[nNextVertex].pLast = NULL;
    pVertexPool[nNextVertex].pNext = NULL;    
    nNextVertex++;

    int nOldNextVertex = nNextVertex;
    for (int j = 1; j < nOC_DCVerts[i]; j++)
    {
      pVertexPool[nNextVertex].t = pVertexPool[nNextVertex - 1].t + 
        sqrt(vtkMath::Distance2BetweenPoints(pOC_DC[i][j - 1]->Coords, 
        pOC_DC[i][j]->Coords));

      pVertexPool[nNextVertex].pVertex = new CSkeletonVertex(pOC_DC[i][j]->Coords);
      pVertexPool[nNextVertex].pVertex->WT = pOC_DC[i][j]->GetDegree() - 1;
      
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
  pCurves[0]->pVertex->PMatch = pCurves[1]->pVertex;
  pCurves[1]->pVertex->PMatch = pCurves[0]->pVertex;
  (pCurves[1] - 1)->pVertex->PMatch = pVertexPool[nNextVertex - 1].pVertex;
  pVertexPool[nNextVertex - 1].pVertex->PMatch = (pCurves[1] - 1)->pVertex;
  
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
      if (pVert->pVertex->PMatch == NULL)
      {
        //find the edge in the other curve, where this point belong
        while (pVertB->t < pVert->t) {
          pVertB = pVertB->pNext;
        }

        //pVertB and pVertB->pLast denote the edge, 
        //where the current vertex pVert should be mapped
        CURVE_VERTEX* pVertA = pVertB->pLast;

        //first check, whether pVert is not mapped into pVertA or pVertB        
        if (pVertA->pVertex->PMatch == NULL && fabs(pVertA->t - pVert->t) <= dblLimit)
        {
          //A is the valid closest point
          pVertA->pVertex->PMatch = pVert->pVertex;
          pVert->pVertex->PMatch = pVertA->pVertex;
        }
        else if (pVertB->pVertex->PMatch == NULL && fabs(pVertB->t - pVert->t) <= dblLimit)
        {
          //B is the valid closest point
          pVertB->pVertex->PMatch = pVert->pVertex;
          pVert->pVertex->PMatch = pVertB->pVertex;
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
            coords[j] = pVertA->pVertex->Coords[j]*dblA +  
              pVertB->pVertex->Coords[j]*dblB;
          }

          pNewVert->pVertex = new CSkeletonVertex(coords);
          pNewVert->pVertex->WT = 1; //it is an inner node => it must have two edges

          pNewVert->pVertex->PMatch = pVert->pVertex;
          pVert->pVertex->PMatch = pNewVert->pVertex;
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
    pOC_DC[0][i]->OneRingEdges[0]->PMatch = pOC_DC[1][i]->OneRingEdges[0];
    pOC_DC[1][i]->OneRingEdges[0]->PMatch = pOC_DC[0][i]->OneRingEdges[0];
  }

  //destroy the vertex pool
  delete[] pVertexPool;
  return nRetCount;
}

//------------------------------------------------------------------------
//Create edges for the given array of vertices.
void vtkALBAPolyDataDeformation::CreateCurveEdges(CSkeletonVertex** pVerts, int nVerts)
//------------------------------------------------------------------------
{
  for (int i = 0; i < nVerts - 1; i++) {  //create left to right edges
    pVerts[i]->OneRingEdges.push_back(new CSkeletonEdge(pVerts[i], pVerts[i + 1]));
  }

  for (int i = 1; i < nVerts; i++) {    //create backward edges
    pVerts[i]->OneRingEdges.push_back(pVerts[i - 1]->OneRingEdges[0]);    
  }
}

//------------------------------------------------------------------------
//Gets the number of vertices belonging to the given curve.
//N.B. curve must be compatible with curves constructed by CreateCurveEdges.
int vtkALBAPolyDataDeformation::GetNumberOfCurveVertices(CSkeletonVertex* pCurve)
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
void vtkALBAPolyDataDeformation::RefineCurve(CSkeletonVertex* pCurve, double dblEdgeFactor)
//------------------------------------------------------------------------
{
  CSkeletonEdge* pOCEdge = pCurve->OneRingEdges[0];
  while (pOCEdge != NULL)
  {    
    CSkeletonEdge* pDCEdge = pOCEdge->PMatch;

    double dblLen1 = vtkMath::Distance2BetweenPoints(
      pOCEdge->Verts[0]->Coords, pOCEdge->Verts[1]->Coords);
    double dblLen2 = vtkMath::Distance2BetweenPoints(
      pDCEdge->Verts[0]->Coords, pDCEdge->Verts[1]->Coords);

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
      pNewOCVert->Coords[j] = (pOCEdge->Verts[0]->Coords[j] + 
        pOCEdge->Verts[1]->Coords[j]) / 2;
      pNewDCVert->Coords[j] = (pDCEdge->Verts[0]->Coords[j] + 
        pDCEdge->Verts[1]->Coords[j]) / 2;
    }
    
    pNewOCVert->WT = pNewDCVert->WT = 1;
    pNewOCVert->PMatch = pNewDCVert;
    pNewDCVert->PMatch = pNewOCVert;

    CSkeletonEdge* pNewOCEdge = new CSkeletonEdge(pNewOCVert, pOCEdge->Verts[1]);
    CSkeletonEdge* pNewDCEdge = new CSkeletonEdge(pNewDCVert, pDCEdge->Verts[1]);    
    pNewOCEdge->PMatch = pNewDCEdge;
    pNewDCEdge->PMatch = pNewOCEdge;

    pNewOCVert->OneRingEdges.push_back(pNewOCEdge);
    pNewOCVert->OneRingEdges.push_back(pOCEdge);
    pNewDCVert->OneRingEdges.push_back(pNewDCEdge);
    pNewDCVert->OneRingEdges.push_back(pDCEdge);

    pOCEdge->Verts[1]->OneRingEdges.pop_back();
    pOCEdge->Verts[1]->OneRingEdges.push_back(pNewOCEdge);
    pDCEdge->Verts[1]->OneRingEdges.pop_back();
    pDCEdge->Verts[1]->OneRingEdges.push_back(pNewDCEdge);

    pOCEdge->Verts[1] = pNewOCVert;
    pDCEdge->Verts[1] = pNewDCVert;    
  }
}

//------------------------------------------------------------------------
//Stores vertices and edges from the given curve and the matched one into the superskeleton 
//The routine also constructs automatically joints for end-points of curves.
//N.B. both curves must be compatible with curves constructed by CreateCurveEdges.
void vtkALBAPolyDataDeformation::AddCurveToSuperSkeleton(CSkeletonVertex* pOCCurve)
//------------------------------------------------------------------------
{
  int nPoints = (int)SuperSkeleton->POCSkel->Vertices.size();  
  int nNextEdgeId = (int)SuperSkeleton->POCSkel->Edges.size();
  int nNextPtId = nPoints;

  CSkeletonVertex* pEndPoints[2];
  pEndPoints[0] = pOCCurve;

  while (pOCCurve != NULL)
  {
    //insert vertex
    CSkeletonVertex* pDCCurve = pOCCurve->PMatch;
    pDCCurve->Id = pOCCurve->Id = nNextPtId;
    SuperSkeleton->POCSkel->Vertices.push_back(pOCCurve);    
    SuperSkeleton->PDCSkel->Vertices.push_back(pDCCurve);
    nNextPtId++;

    //insert the next edge; if we are in the last vertex, 
    //m_OneRingEdges[0] refers to the previous edge already inserted.
    CSkeletonEdge* pEdge = pOCCurve->OneRingEdges[0];
    if (pEdge->Id < 0)
    {
      pEdge->Id = nNextEdgeId;
      SuperSkeleton->POCSkel->Edges.push_back(pEdge);

      pEdge = pEdge->PMatch;
      pEdge->Id = nNextEdgeId++;
      SuperSkeleton->PDCSkel->Edges.push_back(pEdge);
    }

    pEndPoints[1] = pOCCurve;
    pOCCurve = GetNextCurveVertex(pOCCurve);
  }

  //pEndPoints now stores end-points of the original curve  
  int iVPos = 0;
  while (iVPos < nPoints)
  {
    CSkeletonVertex* pCurEndPoints[2];
    pCurEndPoints[0] = SuperSkeleton->POCSkel->Vertices[iVPos];
    iVPos += GetNumberOfCurveVertices(pCurEndPoints[0]);
    pCurEndPoints[1] = SuperSkeleton->POCSkel->Vertices[iVPos - 1];
 
    for (int i = 0; i < 2; i++)
    {
      if (pEndPoints[i]->WT <= 1.0)
        continue; //this vertex cannot have joints

      for (int j = 0; j < 2; j++)
      {
        if (pCurEndPoints[j]->WT <= 1.0)
          continue; //this vertex cannot have joints

        if (
          pEndPoints[i]->Coords[0] == pCurEndPoints[j]->Coords[0] &&
          pEndPoints[i]->Coords[1] == pCurEndPoints[j]->Coords[1] &&
          pEndPoints[i]->Coords[2] == pCurEndPoints[j]->Coords[2]
          )
        {
          //those points match => establish links between them
          pEndPoints[i]->JoinedVertices.push_back(pCurEndPoints[j]);
          pCurEndPoints[j]->JoinedVertices.push_back(pEndPoints[i]);

          pEndPoints[i]->PMatch->JoinedVertices.push_back(pCurEndPoints[j]->PMatch);
          pCurEndPoints[j]->PMatch->JoinedVertices.push_back(pEndPoints[i]->PMatch);
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
//
//ROS_OC and ROS_DC defines the plane to compute the first LF - see SetNthSkeleton
void vtkALBAPolyDataDeformation::ComputeLFS(CSkeletonVertex* pOC, double* ROS_OC, double* ROS_DC)
//------------------------------------------------------------------------
{
  CSkeletonVertex* pDC = pOC->PMatch;

  //compute the LFS for the first point
  //u is the tangent vector
  CSkeletonVertex::LOCAL_FRAME& lf = pOC->LF;  
  CSkeletonVertex* pNextVertex = GetNextCurveVertex(pOC);
  for (int i = 0; i < 3; i++) {  
    lf.u[i] = pNextVertex->Coords[i] - pOC->Coords[i];    
  }

  if (ROS_OC != NULL)
  {
    //Ref.Sys. point for OC is specified => together with u, it defines
    //the optimal plane for the vector v
    for (int i = 0; i < 3; i++){
      lf.v[i] = ROS_OC[i] - pOC->Coords[i]; 
    }

    //w is perpendicular to both vectors
    vtkMath::Cross(lf.u, lf.v, lf.w);
    vtkMath::Cross(lf.w, lf.u, lf.v);
  }
  else
  {
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
  } 

  vtkMath::Normalize(lf.u);
  vtkMath::Normalize(lf.v);
  vtkMath::Normalize(lf.w);

  //compute the local frame system for the first point
  //of the deformed curve; it will be a bit different
  CSkeletonVertex::LOCAL_FRAME& dlf = pDC->LF;
  pNextVertex = GetNextCurveVertex(pDC);
  for (int i = 0; i < 3; i++) {  
    dlf.u[i] = pNextVertex->Coords[i] - pDC->Coords[i];  
  }

  vtkMath::Normalize(dlf.u);  
  if (ROS_DC != NULL)
  {
    //Ref.Sys. point for OC is specified => together with u, it defines
    //the optimal plane for the vector v
    for (int i = 0; i < 3; i++){
      dlf.v[i] = ROS_DC[i] - pDC->Coords[i]; 
    }

    //w is perpendicular to both vectors
    vtkMath::Cross(dlf.u, dlf.v, dlf.w);
    vtkMath::Cross(dlf.w, dlf.u, dlf.v);
    vtkMath::Normalize(dlf.v);
    vtkMath::Normalize(dlf.w);
  }
  else
  {
    //Ref. Sys. point not specified => get the DLF from LF by minimizing rotation
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
      pDC->LF = pOC->LF;
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
      const CSkeletonVertex::LOCAL_FRAME& prev_lf = pPrevCurve->LF;
      CSkeletonVertex::LOCAL_FRAME& lf = pCurve->LF;      

      if (pNextVertex == NULL)
      {
        for (int j = 0; j < 3; j++)  { 
          lf.u[j] = pCurve->Coords[j] - pPrevCurve->Coords[j];
        }
      }
      else
      {
        for (int j = 0; j < 3; j++)  { 
          lf.u[j] = pNextVertex->Coords[j] - pCurve->Coords[j];
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
double vtkALBAPolyDataDeformation::GetDistance( vtkIdType nPtStartId, 
          double ptEnd[3], vtkCellLocator* cellLocator, double dblMaxDist )
//------------------------------------------------------------------------
{
  const static double dblStep = 0.01;   //constant sampling
  vtkPolyData* input = vtkPolyData::SafeDownCast(GetInput());

  double ptStart[3];
  input->GetPoint(nPtStartId, ptStart);

  //this is just to speed-up the search for intersection
  vtkGenericCell* cellLoc = vtkGenericCell::New();

	vtkIdType nCellId;
	int nSubId;
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
      vtkIdType nPtIds;
      vtkSmartPointer<vtkIdList> pPtIds = vtkSmartPointer<vtkIdList>::New();
      input->GetCellPoints(nCellId, pPtIds);

      nPtIds = pPtIds->GetNumberOfIds();

      int iMinPos = 0;
      double dblMinDist = vtkMath::Distance2BetweenPoints(x, input->GetPoint(pPtIds->GetId(0)));
      for (int i = 1; i < nPtIds; i++)
      {
        double dblDist = vtkMath::Distance2BetweenPoints(x, input->GetPoint(pPtIds->GetId(i)));
        if (dblDist < dblMinDist) {
          iMinPos = i; dblMinDist = dblDist;
        }
      }

      //now we have the end point
      vtkIdType nNextPtId = pPtIds->GetId(iMinPos);
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
typedef struct vtkALBAPolyDataDeformation::DIJKSTRA_ITEM
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
double vtkALBAPolyDataDeformation::GetPathLength(vtkIdType nPtFrom, 
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
    vtkSmartPointer<vtkIdList> pCellsIds = vtkSmartPointer<vtkIdList>::New();
    input->GetPointCells(nCurItemId, pCellsIds);
    unsigned short nCellsIds = pCellsIds->GetNumberOfIds();

    for (int i = 0; i < nCellsIds; i++)
    {
      vtkSmartPointer<vtkIdList> pPtsIds = vtkSmartPointer<vtkIdList>::New();
      input->GetCellPoints(pCellsIds->GetId(i), pPtsIds);
      vtkIdType nPtsIds = pPtsIds->GetNumberOfIds();

      for (int j = 0; j < nPtsIds; j++)
      {
        if (pPtsIds->GetId(j) == nCurItemId)
        {
          //cells fill the area around the vertex fully, so we need
          //to process only one edge per cell as the other will be
          //processed in the next iteration
          vtkIdType nNbPtId = pPtsIds->GetId((j + 1) % nPtsIds);
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
        } //end if (pPtsIds->GetId(j) == pCurItem->nId)
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
void vtkALBAPolyDataDeformation
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
void vtkALBAPolyDataDeformation::ComputeMeshParametrization()
//------------------------------------------------------------------------
{ 
  vtkPolyData* input = vtkPolyData::SafeDownCast(GetInput());    
  int nPoints = input->GetNumberOfPoints();
  int nCount = (int)SuperSkeleton->POCSkel->Edges.size();

  double dblDiag = input->GetLength(); dblDiag *= dblDiag;
  double* EdgeLengths = new double[nCount];
  for (int i = 0; i < nCount; i++)
  {
    CSkeletonEdge* pEdge = SuperSkeleton->POCSkel->Edges[i];
    
    if (pEdge->Verts[0] == NULL || pEdge->Verts[1] == NULL)
      EdgeLengths[i] = 0.0; //this is special edge
    else
      EdgeLengths[i] = sqrt(vtkMath::Distance2BetweenPoints(
        pEdge->Verts[0]->Coords, pEdge->Verts[1]->Coords));    
  }

  for (int nPtId = 0; nPtId < nPoints; nPtId++)
  {
    double* pcoords = input->GetPoint(nPtId);
    MeshVertices[nPtId].resize(nCount);   //every edge parametrize it

    double dblWSum = 0.0;
    for (int i = 0; i < nCount; i++)
    {
      CSkeletonEdge* pEdge = SuperSkeleton->POCSkel->Edges[i];        
      CMeshVertexParametrization* pParams = &(MeshVertices[nPtId][i]);
      
      pParams->PEdge = pEdge;
      pParams->NOriginPos = 0;

      //detect the edge configuration        
      bool bInfEdge = true;
      CSkeletonVertex* pSkelVerts[2];
      if (pEdge->Verts[1] == NULL)
        pSkelVerts[0] = pSkelVerts[1] = pEdge->Verts[0];
      else if (pEdge->Verts[0] == NULL)
      {
        pSkelVerts[0] = pSkelVerts[1] = pEdge->Verts[1];
        pParams->NOriginPos = 1;
      }
      else
      {      
        pSkelVerts[0] = pEdge->Verts[0];
        pSkelVerts[1] = pEdge->Verts[1];
        bInfEdge = false;
      }           

      CSkeletonVertex* pSkelVert = pSkelVerts[pParams->NOriginPos];

      //compute a, b, c parametrization for pVertex such that
      //pcoords = pSkelVert->coords + a*LF.u + b*LF.v + c*LF.w
      double M[3][4];      
      for (int k = 0; k < 3; k++) 
      {
        M[k][0] = pSkelVert->LF.u[k];
        M[k][1] = pSkelVert->LF.v[k];
        M[k][2] = pSkelVert->LF.w[k];
        M[k][3] = pcoords[k] - pSkelVert->Coords[k]; 
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

      pParams->PCoords[2] = M[2][3] / M[2][2];
      pParams->PCoords[1] = (M[1][3] - pParams->PCoords[2]*M[1][2]) / M[1][1];
      pParams->PCoords[0] = (M[0][3] - pParams->PCoords[2]*M[0][2] - 
        pParams->PCoords[1]*M[0][1]) / M[0][0];

#ifdef DEBUG_vtkALBAPolyDataDeformation
      double backprj[3];
      for (int k = 0; k < 3; k++)
      {
        //pSkelVert->coords + a*LF.u + b*LF.v + c*LF.w
        backprj[k] = pSkelVert->m_Coords[k] + 
          pParams->PCoords[0]*pSkelVert->m_LF.u[k] + 
          pParams->PCoords[1]*pSkelVert->m_LF.v[k] + 
          pParams->PCoords[2]*pSkelVert->m_LF.w[k];

        _ASSERT(fabs(backprj[k] - pcoords[k]) <= 1e-5);
      }
#endif // _DEBUG

      //compute the point closest to the current one that lies on the line
      //supported by the current edge and then compute the distance      

      //the closest point on the edge can be computed as: 
      //Pk + (u*(P - Pk))*u, where u is normalized
      double ptEnd[3];
      for (int k = 0; k < 3; k++) {                  
        ptEnd[k] = pcoords[k] - pSkelVert->Coords[k];
      }

      double w = vtkMath::Dot(pSkelVert->LF.u, ptEnd);
      for (int k = 0; k < 3; k++) {
        ptEnd[k] = pSkelVert->Coords[k] + w*pSkelVert->LF.u[k];
      }
      
      double lw, rw = vtkMath::Distance2BetweenPoints(ptEnd, pcoords);      

      //ptEnd might be outside the edge
      if (w <= 0.0)  
      { //we are before the edge => the first node is the closest one
        lw = /*dblDiag +*/ vtkMath::Distance2BetweenPoints(ptEnd, 
          pSkelVerts[pParams->NOriginPos]->Coords);
        pParams->DblRm = 0.0;          
      }
      else if (w >= EdgeLengths[i]) 
      {
        //we are after the edge => the other node is the closest one
        lw = /*dblDiag +*/ vtkMath::Distance2BetweenPoints(ptEnd, 
          pSkelVerts[1 - pParams->NOriginPos]->Coords);
        
        pParams->DblRm = 1.0;          
      }
      else
      {
        //closest point lies on the edge
        lw = 0.0;
        pParams->DblRm = w / EdgeLengths[i];
      }        
          
      //weight decreases with the square (typical physical behavior)
      pParams->DblWeight = 1.0 / (0.0001 + lw + rw/*((1.0 + lw)*rw)*/);  //0.0001 is here if any point lies on the edge
      dblWSum += pParams->DblWeight;
    } //end for i (edges)

    for (int i = 0; i < nCount; i++)
    {
      CMeshVertexParametrization* pParams = &(MeshVertices[nPtId][i]);
      pParams->DblWeight /= dblWSum;
    }
  } //end for (points)

  delete[] EdgeLengths;
}


//------------------------------------------------------------------------
//Computes new position of vertices in the output mesh.
//In this last step, positions of vertices are modified according to their
//parametrization in respect to the superskeleton. 
//
//N.B. the given output polydata must be compatible with the input polydata
void vtkALBAPolyDataDeformation::DeformMesh(vtkPolyData* output)
//------------------------------------------------------------------------
{  
  vtkPoints* points = output->GetPoints();
  int nPoints = output->GetNumberOfPoints();
  int nCount = (int)SuperSkeleton->PDCSkel->Edges.size();
  
  double* EdgeLengths = new double[nCount];
  double* EdgeElongations = new double[nCount];
  for (int i = 0; i < nCount; i++)
  {
    EdgeLengths[i] = SuperSkeleton->PDCSkel->Edges[i]->GetLength();
    if (this->PreserveVolume)
    {
      EdgeElongations[i] = SuperSkeleton->POCSkel->Edges[i]->GetLength();
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
    const double* oldCoords = points->GetPoint(i); 
    double newCoords[3] = {0.0, 0.0, 0.0};

    for (int j = 0; j < nCount; j++)
    {
      CMeshVertexParametrization* pParam = &(MeshVertices[i][j]);    
      CSkeletonVertex* pSkelVert = pParam->PEdge->PMatch->
        Verts[pParam->NOriginPos];

      double thisCoords[3];
      if (pParam->DblRm <= 0.0 || 
        pParam->PEdge->PMatch->Verts[0] == NULL ||
        pParam->PEdge->PMatch->Verts[1] == NULL
        //pParam->m_dblRm >= 1.0)
        )
      {
        //no elongation factor to be applied (it is either an infinite edge
        //or point is outside the edge domain
        for (int k = 0; k < 3; k++) 
        {
          thisCoords[k] = (pSkelVert->Coords[k] + 
            pParam->PCoords[0]*pSkelVert->LF.u[k] + 
            pParam->PCoords[1]*pSkelVert->LF.v[k] + 
            pParam->PCoords[2]*pSkelVert->LF.w[k]);            
        }
      }
      else
      {     
        //the control edge is well defined => we need to incorporate elongation of edge                    
        double w = pParam->DblRm * EdgeLengths[j];
        for (int k = 0; k < 3; k++)
        {
          //vmo = Pk1 + b*v1 + c*w1
          thisCoords[k] = (pSkelVert->Coords[k] + 
            pSkelVert->LF.u[k]*w + 
            pParam->PCoords[1]*pSkelVert->LF.v[k] + 
            pParam->PCoords[2]*pSkelVert->LF.w[k]);            
        }

        if (this->PreserveVolume)
        {
          //if Volume is to be preserved than the position of point will
          //be scaled so the point is farer (or closer) to the skeleton
          //NB. may not work well, if skeleton edge is not lie in the center of mesh
          //compute the point closest to the current one that lies on the line
          //supported by the current edge and then compute the distance      
          //The algorithm is based on the paper: 
          //Aubel A., Thalmann D. Efficient Muscle Shape Deformation
          //http://vrlab.epfl.ch/Publications/pdf/Aubel_Thalmann_Derformable_Avatars_01.pdf

          //the closest point on the edge can be computed as: 
          //Pk + (u*(P - Pk))*u, where u is normalized
          double ptEnd[3];
          for (int k = 0; k < 3; k++) {                  
            ptEnd[k] = thisCoords[k] - pSkelVert->Coords[k];
          }

          double w = vtkMath::Dot(pSkelVert->LF.u, ptEnd);
          for (int k = 0; k < 3; k++) {
            ptEnd[k] = pSkelVert->Coords[k] + w*pSkelVert->LF.u[k];
          }

          for (int k = 0; k < 3; k++) {
            thisCoords[k] = ptEnd[k] + (thisCoords[k] - ptEnd[k])*EdgeElongations[j];
          }
        }
      } //end else both vertices exist

      for (int k = 0; k < 3; k++){
        newCoords[k] += thisCoords[k]*pParam->DblWeight;
      }
    }

    points->SetPoint(i, newCoords);   
  } //for i

  delete[] EdgeLengths;
  delete[] EdgeElongations;
}

//------------------------------------------------------------------------
//Creates the internal skeleton data structure for the given polydata
vtkALBAPolyDataDeformation::CSkeleton* 
vtkALBAPolyDataDeformation::CreateSkeleton(vtkPolyData* pPoly)
//------------------------------------------------------------------------
{  
  int nVerts = pPoly->GetNumberOfPoints();  

  CSkeleton* pSkel = new CSkeleton();
  pSkel->Vertices.reserve(nVerts);

  //load points
  for (int i = 0; i < nVerts; i++)
  {
    CSkeletonVertex* pVertex = new CSkeletonVertex();

    pPoly->GetPoint(pVertex->Id = i, pVertex->Coords);
    pSkel->Vertices.push_back(pVertex);
  }

  //ensure that we have cells
  pPoly->BuildCells();
  int nCells = pPoly->GetNumberOfCells();
  pSkel->Edges.reserve(nVerts - 1);

  int nEdgeId = 0;
  for (int i = 0; i < nCells; i++)
  {
    vtkSmartPointer<vtkIdList> pIds = vtkSmartPointer<vtkIdList>::New();
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

      pEdge->Verts[0]->OneRingEdges.push_back(pEdge);
      pEdge->Verts[1]->OneRingEdges.push_back(pEdge);
      pSkel->Edges.push_back(pEdge);
    }
  }

  return pSkel;
}

//------------------------------------------------------------------------
//Creates polydata from the given skeleton.
void vtkALBAPolyDataDeformation::CreatePolyDataFromSkeleton(
                                    CSkeleton* pSkel, vtkPolyData* output)
//------------------------------------------------------------------------
{ 
  int nEdges = (int)pSkel->Edges.size();
  int nPoints = (int)pSkel->Vertices.size();

  int* pVertMap = new int[nPoints];
  memset(pVertMap, -1, nPoints*sizeof(int));  
    
  vtkPoints* points = vtkPoints::New();         //points
  vtkCellArray* cells = vtkCellArray::New();    //cells    
  
  for (int i = 0; i < nEdges; i++)
  {
    CSkeletonEdge* pEdge = pSkel->Edges[i];
    if (pEdge->Verts[0] == NULL || pEdge->Verts[1] == NULL)
      continue; //this is an infinite edge    

    vtkIdType cellIds[2];  //cells (lines)  
    for (int j = 0; j < 2; j++)
    {
      CSkeletonVertex* pVert = pEdge->Verts[j];
      if (pVertMap[pVert->Id] < 0)
      {
        //this vertex has not be stored yet
        pVertMap[pVert->Id] = points->InsertNextPoint(pVert->Coords);
        
        //joined vertices must be stored only once
        int nJoints = (int)pVert->JoinedVertices.size();
        for (int k = 0; k < nJoints; k++) { 
          pVertMap[pVert->JoinedVertices[k]->Id] = pVertMap[pVert->Id];
        }
      } //end if

      cellIds[j] = pVertMap[pVert->Id];
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
double vtkALBAPolyDataDeformation::ComputeInputMeshAvgEdgeLength()
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
    vtkSmartPointer<vtkIdList> ptIds = vtkSmartPointer<vtkIdList>::New();
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

#ifdef DEBUG_vtkALBAPolyDataDeformation
//------------------------------------------------------------------------
//Debug routine that creates polydata from superskeleton
void vtkALBAPolyDataDeformation::CreatePolyDataFromSuperskeleton()
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
void vtkALBAPolyDataDeformation::DestroyMATCHEDData()
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
