/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAMeshCutter
 Authors: Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkALBAMeshCutter.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkIndent.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPlane.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkMatrix4x4.h"
#include <unordered_map>
#include <unordered_set>

#include <ostream>
#include "albaDefines.h"
#include "vtkALBAToLinearTransform.h"
#include "vtkTransform.h"


#if _MSC_VER >= 1400
#include <intrin.h>
#pragma intrinsic(memcpy, memset, fabs)
#endif

//------------------------------------------------------------------------------
// standard macros
vtkStandardNewMacro(vtkALBAMeshCutter);
//------------------------------------------------------------------------------
#include "albaMemDbg.h"
#include "vtkALBASmartPointer.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

//------------------------------------------------------------------------------
// Constructor
vtkALBAMeshCutter::vtkALBAMeshCutter()
//------------------------------------------------------------------------------
{
  CutFunction = NULL ;
  UnstructGrid = vtkUnstructuredGrid::New() ;

  PointsCoords = NULL;
  BReleasePointsCoords = false;

  LastInput = NULL;
  LastInputTimeStamp = 0;

  Idlist0 = vtkIdList::New() ;
  Idlist1 = vtkIdList::New() ;
  Ptlist = vtkIdList::New();

  PointsInCells = NULL;
}

//------------------------------------------------------------------------------
// Destructor
vtkALBAMeshCutter::~vtkALBAMeshCutter()
//------------------------------------------------------------------------------
{
  UnstructGrid->Delete() ;
  Idlist0->Delete();
  Idlist1->Delete();
  Ptlist->Delete();

  if (BReleasePointsCoords) {
    delete[] PointsCoords;
    PointsCoords = NULL;
  }

  delete[] PointsInCells; 
}

//------------------------------------------------------------------------------
// Overload standard modified time function. If cut function is modified,
// then this object is modified as well.
vtkMTimeType vtkALBAMeshCutter::GetMTime()
//------------------------------------------------------------------------------
{
	vtkMTimeType mTime = this->vtkAlgorithm::GetMTime();
	vtkMTimeType time;

  if (CutFunction != NULL )
  {
    time = CutFunction->GetMTime();
    mTime = ( time > mTime ? time : mTime );
  }

  return mTime;
}

//------------------------------------------------------------------------------
int vtkALBAMeshCutter::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//------------------------------------------------------------------------------
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkUnstructuredGrid  *input = vtkUnstructuredGrid::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (LastInput != input || LastInputTimeStamp != input->GetMTime())
  {
    // Make a copy of the input data and build links
    // Can't just set a pointer because BuildLinks() would change the input.
		UnstructGrid->Initialize() ;
		//UnstructGrid->DeepCopy(input) ;
		vtkUnstructuredGrid * cleanedMesh = RemoveUnusedPoints(input);
    UnstructGrid->DeepCopy(cleanedMesh);
		vtkDEL(cleanedMesh);
    UnstructGrid->BuildLinks();

    //and now get points (so we can avoid calling slow GetPoint later)
    if (BReleasePointsCoords) {
      delete[] PointsCoords;
      PointsCoords = NULL;
    }

    vtkDataArray* pda = UnstructGrid->GetPoints()->GetData();
    if (pda->GetDataType() == VTK_DOUBLE && pda->GetNumberOfComponents() == 3)
    {
      PointsCoords = (double*)pda->GetVoidPointer(0);
      BReleasePointsCoords = false;
    }
    else
    {
      //allocate buffer, NB. if the number of components is larger than 3, we need to allocate more
      //as GetTuple expect that the buffer is capable to hold NumberOfComponents elements
      vtkIdType nCount = pda->GetNumberOfTuples();
      PointsCoords = new double[(nCount - 1)* 3 + pda->GetNumberOfComponents()];
      memset(PointsCoords, 0, nCount*3*sizeof(double));
      BReleasePointsCoords = true;

      //get coordinates (in doubles)
      double* pCurPt = PointsCoords;
      for (vtkIdType i = 0; i < nCount; i++)
      {
        pda->GetTuple(i, pCurPt);
        pCurPt += 3;
      }
    }    

    LastInput = input;
    LastInputTimeStamp = input->GetMTime();
  }

  // Set pointer to output
  Polydata = output;

  // Make sure the cutter is cleared of previous data before you run it !
  Initialize() ;

  // make sure the output is empty
  Polydata->Initialize() ;
  
  // Run the cutter
  CreateSlice() ;  

	return 1;
}

//------------------------------------------------------------------------------
vtkUnstructuredGrid * vtkALBAMeshCutter::RemoveUnusedPoints(vtkUnstructuredGrid* input)
{
	// Set of used point IDs
	bool *usedPoints;

	vtkIdType numberOfPoints = input->GetNumberOfPoints();
	usedPoints = new bool[numberOfPoints];
	memset(usedPoints, 0, sizeof(bool)*numberOfPoints);

	vtkIdType numOfOutPoints = 0;

	// Step 1: collect used point IDs
	for (vtkIdType i = 0; i < input->GetNumberOfCells(); ++i)
	{
		vtkCell *cell = input->GetCell(i);
		for (vtkIdType j = 0; j < cell->GetNumberOfPoints(); ++j) 
		{
			vtkIdType pointId = cell->GetPointId(j);
			
			//add only one time for each point
			if (!usedPoints[pointId])
				numOfOutPoints++;
			usedPoints[pointId]=true;
		}
	}


	//when the outputs
	if (numberOfPoints == numOfOutPoints)
	{
		vtkUnstructuredGrid *output;
		vtkNEW(output);
		output->DeepCopy(input);
		delete[] usedPoints;
		return output;
	}

	// Step 2: map old point IDs to new ones
	vtkIdType *oldToNewIdMap, *newToOldIdMap;
	oldToNewIdMap = new vtkIdType[numberOfPoints];
	newToOldIdMap = new vtkIdType[numOfOutPoints];


	vtkALBASmartPointer<vtkPoints> newPoints;
	newPoints->Allocate(numOfOutPoints);
	for (vtkIdType oldId = 0, newId = 0; oldId <numberOfPoints; ++oldId) {
		if (usedPoints[oldId]) {
			double p[3];
			input->GetPoint(oldId, p);
			newPoints->InsertNextPoint(p);
			oldToNewIdMap[oldId] = newId;
			newToOldIdMap[newId] = oldId;
			newId++;
		}
	}

	// Step 3: create new unstructured grid
	vtkUnstructuredGrid *output;
	vtkNEW(output);
	output->SetPoints(newPoints);

	for (vtkIdType i = 0; i < input->GetNumberOfCells(); ++i) {
		vtkCell *cell = input->GetCell(i);
		vtkALBASmartPointer<vtkIdList> ids;
		for (vtkIdType j = 0; j < cell->GetNumberOfPoints(); ++j) {
			ids->InsertNextId(oldToNewIdMap[cell->GetPointId(j)]);
		}
		output->InsertNextCell(cell->GetCellType(), ids);
	}

	// Optional: copy point data and cell data
	output->GetPointData()->CopyAllocate(input->GetPointData());
	for (int i = 0; i < numOfOutPoints; i++) {
		output->GetPointData()->CopyData(input->GetPointData(), newToOldIdMap[i], i);
	}
	output->GetCellData()->ShallowCopy(input->GetCellData());

	delete[] usedPoints;
	delete[] oldToNewIdMap;
	delete[] newToOldIdMap;
	return output;
}
//------------------------------------------------------------------------------
// Set cutting plane
void vtkALBAMeshCutter::SetCutFunction(vtkPlane *P)
//------------------------------------------------------------------------------
{
  CutFunction = P ;
}

//------------------------------------------------------------------------------
// Set cutting plane
vtkPlane* vtkALBAMeshCutter::GetCutFunction()
//------------------------------------------------------------------------------
{
  return CutFunction ;
}

//------------------------------------------------------------------------------
// Find the output point idout corresponding to the input edge (id0,id1)
// This returns true if the the input edge is in the table
bool vtkALBAMeshCutter::GetOutputPointWhichCutsEdge(vtkIdType id0, vtkIdType id1, vtkIdType *idout, double *lambda) const
//------------------------------------------------------------------------------
{
  unsigned long long key = GetEdgeKey(id0, id1);
  InvEdgeMappingType::const_iterator it = InvEdgeMapping.find(key);
  if (it == InvEdgeMapping.end())
    return false; //not found

  const EdgeMappingType& edge = EdgeMapping[it->second];
  assert(edge.mtype == POINT_TO_EDGE);
  *idout = edge.idout;
  *lambda = edge.lambda;
  return true;
}

//------------------------------------------------------------------------------
// Find the output point idout corresponding to the single input point id0
// This returns true if the the input point is in the table
bool vtkALBAMeshCutter::GetOutputPointWhichCutsPoint(vtkIdType id0, vtkIdType *idout) const
//------------------------------------------------------------------------------
{    
  unsigned long long key = id0;
  InvEdgeMappingType::const_iterator it = InvEdgeMapping.find(key);
  if (it == InvEdgeMapping.end())
    return false; //not found

  const EdgeMappingType& edge = EdgeMapping[it->second];
  assert(edge.mtype == POINT_TO_POINT);
  *idout = edge.idout;  
  return true;
}

//------------------------------------------------------------------------------
// Get the output cell corresponding to the input cell
// returns true if the input id was found
bool vtkALBAMeshCutter::GetOutputCellWhichCutsInputCell(vtkIdType idin, vtkIdType *idout)
//------------------------------------------------------------------------------
{
  //BES: this function is not used - it is not optimized
  unsigned int i ;
  bool found ;

  for (i = 0, found = false ;  i < CellMapping.size() ;  i++){
    if (CellMapping[i] == idin){
      found = true ;
      *idout = i ;
    }
  }

  return found ;
}


//------------------------------------------------------------------------------
// Get intersection of line with plane
// Edge is defined by coords of endpoints (p0, p1).
// Plane is defined by origin and normal.
// Returns type of intersection.
// lambda returns fractional distance of interpolated point along edge (0 <= lambda <= 1).
int vtkALBAMeshCutter::GetIntersectionOfLineWithPlane(const double *p0, const double *p1, const double *origin, const double *norm, double *coords, double *lambda) const
//------------------------------------------------------------------------------
{
  int i ;

  // get coords of points relative to centre of plane
  double dp0[3], dp1[3] ;
  for (i = 0 ;  i < 3 ;  i++){
    dp0[i] = p0[i] - origin[i] ;
    dp1[i] = p1[i] - origin[i] ;
  }

  // get dot products with normal
  double dotprod0, dotprod1 ;
  for (i = 0, dotprod0 = 0.0, dotprod1 = 0.0 ;  i < 3 ;  i++){
    dotprod0 += dp0[i]*norm[i] ;
    dotprod1 += dp1[i]*norm[i] ;
  }

  // are both points above or below the plane
  if ((dotprod0 > 0.0 && dotprod1 > 0.0) || (dotprod0 < 0.0 && dotprod1 < 0.0))
    return NO_INTERSECTION ;

  // are both points in the plane
  // n.b. you will have to set the coords elsewhere in this case
  if ((dotprod0 == 0.0) && (dotprod1 == 0.0))
    return LINE_IN_PLANE ;

  // find the coords of the single crossing point
  *lambda = dotprod0 / (dotprod0 - dotprod1) ;
  coords[0] = (*lambda)*p1[0] + (1.0-(*lambda))*p0[0] ;
  coords[1] = (*lambda)*p1[1] + (1.0-(*lambda))*p0[1] ;
  coords[2] = (*lambda)*p1[2] + (1.0-(*lambda))*p0[2] ;

  if (*lambda == 0.0)
    return INTERSECTS_POINT0 ;
  else if (*lambda == 1.0)
    return INTERSECTS_POINT1 ;
  else
    return INTERSECTS_LINE ;
}


//-----------------------------------------------------------------------------
// Get list of cells which are adjoined to point
void vtkALBAMeshCutter::GetCellNeighboursOfPoint(vtkIdType idpt, vtkIdList *idlist) const 
//-----------------------------------------------------------------------------
{  // Get cell neighbours of point
  if (!UnstructGrid) 
    return;

  UnstructGrid->GetPointCells(idpt, idlist);
}

//-----------------------------------------------------------------------------
// Get list of cells which are adjoined to edge
void vtkALBAMeshCutter::GetCellNeighboursOfEdge(const Edge& edge, vtkIdList *idlist) const
//-----------------------------------------------------------------------------
{
  Idlist0->Reset();
  Idlist1->Reset();  
  
  // list cell neighbours of both endpoints of edge 
  GetCellNeighboursOfPoint(edge.id0, Idlist0) ;
  GetCellNeighboursOfPoint(edge.id1, Idlist1) ;
  
  // copy cell id's to idlist if they are in both lists
  vtkIdType* pList0 = Idlist0->GetPointer(0);
  vtkIdType* pList1 = Idlist1->GetPointer(0);
  int n0 = Idlist0->GetNumberOfIds();
  int n1 = Idlist1->GetNumberOfIds();

  vtkIdType* pListOut = idlist->WritePointer(0, n0 + n1);
  int nCount = 0; //number of items in output list

  int jStart = 0;
  for (int i = 0 ;  i < n0;  i++)
  {    
    for (int j = jStart;  j < n1;  j++)
    {
      if (pList1[j] == pList0[i])
      {
        //once we found the match, the remaining tests for this j will be always false
        //we will avoid this unnecessary testing, i.e., instead of O(N^2) in the expected case
        //we have O(NlogN) in the expected case, worst cases are, indeed, the same        
        pListOut[nCount++] = pList1[j];
        pList1[j] = pList1[jStart++];
        break;
      }
    }
  }  

  //set the correct number of ids
  idlist->SetNumberOfIds(nCount);   
}

//------------------------------------------------------------------------
//computes the intersection of the given plane with the mesh bounding box
//returns coordinates of the first intersection in pts, returns false, if the bounding box is not intersected
bool vtkALBAMeshCutter::GetIntersectionOfBoundsWithPlane(const double *origin, 
                                                            const double *norm, double* pts)
//------------------------------------------------------------------------
{
  double DataBounds[3][2];
  UnstructGrid->GetBounds(&DataBounds[0][0]);  

  //intersect the cutting plane ax + by + cz + d = 0, where (a,b,c) is normal GlobalPlaneAxisZ
  //and d can is computed so the plane goes through GlobalPlaneOrigin with the bounding box
  const double d = -(norm[0] * origin[0] + norm[1] * origin[1] + norm[2] * origin[2]);  

  //bounding box is symmetric => we will compute intersections
  //for every rotation of the coordinate system [i, j, k]
  //i.e., for [0,1,2], [1,2,0], [2,0,1], i.e., [x,y,z],[y,z,x],[z,x,y]
  for (int i = 0; i < 3; i++) 
  {
    //check if the i-axis is not parallel to the plane
    if (fabs(norm[i]) < 1.e-10)
      continue; //there is no intersection => continue

    //i-axis is the major direction where we want to compute intersections
    //the box has 4 edges parallel to this axis, 2 of them intersect j-axis and 2 k-axis,
    //thus we will compute P[i, 0, 0], P[i, 0, bbox corner on k-axis], 
    //P[i, bbox corner on j-axis, 0] and P[i, bbox corner on j-axis, bbox corner on k-axis]
    const int j = (i + 1) % 3, k = (i + 2) % 3;       
    for (int jj = 0; jj < 2; jj++) 
    {
      for (int kk = 0; kk < 2; kk++) 
      {
        //compute intersection of the bounding box edge denoted by i, j, k coordinate system and
        //the index (jj, kk) with the cutting plane        
        pts[j] = DataBounds[j][jj];
        pts[k] = DataBounds[k][kk];
        pts[i] = -(d + norm[j] * pts[j] + norm[k] * pts[k]) / norm[i];

        // check that p[i] is in inside the box
        if (pts[i] < DataBounds[i][0] || pts[i] > DataBounds[i][1])
          continue; //the supporting line intersects the plane but the edge does not

        return true;
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------
//gets the first cell intersected by the current plane
//the search starts with cells sharing the given point
vtkIdType vtkALBAMeshCutter::GetFirstIntersectedCell(vtkIdType ptId)
//------------------------------------------------------------------------
{
  double* origin = CutTranformedOrigin;
  double* normal = CutTranformedNormal;

  vtkIdList* cellIds = vtkIdList::New();
  cellIds->Allocate(8);  

  //we need to check every point in the worst case 
  //so prepare a stack capable to hold ids of every point
  //and bit array where nonzero bits mean that the point was already checked 
  int np = UnstructGrid->GetNumberOfPoints();
  vtkIdType* stckTocheck = new vtkIdType[np];
  int SP = 0; //stack to be used for points

  np = (np + 7) >> 3; //align number of cells to bytes
  unsigned char* pointStatus = new unsigned char[np];    //bit array
  memset(pointStatus, 0, np); 
  
  //set cellId bit to 1 -> set cellId % sizeof(byte) bit of cellId / sizeof(byte) byte to 1
  //as sizeof(byte) == 8, we can make the computation easier
  pointStatus[ptId >> 3] |= 1 << (ptId & 0x7);
  stckTocheck[SP++] = ptId;  

  while (SP != 0)
  {
    vtkIdType ptsCellId = stckTocheck[--SP];    //get the point to be checked    

    //get list of cells around the current point
    UnstructGrid->GetPointCells(ptsCellId, cellIds);
    int nc = cellIds->GetNumberOfIds();
    
    vtkIdType* pCellIdsPtr = cellIds->GetPointer(0);
    for (int i = 0; i < nc; i++)
    {
      vtkIdType cellId = pCellIdsPtr[i];
      vtkCell* cell = UnstructGrid->GetCell(cellId);

      //check every edge, if it is not intersected
      int ne = cell->GetNumberOfEdges() ;
      for (int j = 0; j < ne; j++)
      {
        vtkIdType ids[2];
        vtkCell* edgecell = cell->GetEdge(j);
        ids[0] = edgecell->GetPointId(0);
        ids[1] = edgecell->GetPointId(1); 

        // find if and where edge crosses plane
        double coords[3], lambda;    
        int itype = GetIntersectionOfLineWithPlane(&PointsCoords[3*ids[0]], 
          &PointsCoords[3*ids[1]], origin, normal, coords, &lambda);   

        if (itype != NO_INTERSECTION)
        {
          //great, we have found it
          delete[] stckTocheck;
          delete[] pointStatus;
          cellIds->Delete();
          return cellId;
        }

        //push points into stack
        for (int k = 0; k < 2; k++)
        {
          //push all yet not processed points into the stack
          //checking the appropriate bit in the bit array (using masking trick)          
          unsigned char mask = 1 << (ids[k] & 0x7);                        
          if ((pointStatus[ids[k] >> 3] & mask) == 0)
          {
            pointStatus[ids[k] >> 3] |= mask;
            stckTocheck[SP++] = ids[k];              
          }
        }
      }
    }
  }  

  delete[] stckTocheck;
  delete[] pointStatus;
  cellIds->Delete();
  return -1;
}

//------------------------------------------------------------------------------
// Find where the edges cross the plane and create polydata points
// It creates the table EdgeMapping which maps output points to the input mesh.
// It also creates IntersectedCells, which is a list of the input cells which were intersected.
void vtkALBAMeshCutter::FindPointsInPlane()
//------------------------------------------------------------------------------
{
  double* origin = CutTranformedOrigin;
  double* normal = CutTranformedNormal;

  //we need to check every cell in the worst case 
  //so prepare a stack capable to hold ids of every cell
  //and bit array where nonzero bits mean that the cell was already checked 
  int nc = UnstructGrid->GetNumberOfCells();
  vtkIdType* stckTocheck = new vtkIdType[nc];
  int SP = 0; //stack to be used for cells

  nc = (nc + 7) >> 3; //align number of cells to bytes
  unsigned char* cellStatus = new unsigned char[nc];    //bit array
  memset(cellStatus, 0, nc); 

  //allocate some structure we will need for our purpose
  vtkIdList* edgeIds = vtkIdList::New();  
  vtkIdType* edgesIdsPtr = edgeIds->WritePointer(0, 2);

  vtkIdList* cellsIds = vtkIdList::New();
  cellsIds->Allocate(8);

  //allocate the output points structure
  //if points are uniformly distributed, then there is N^(2/3) points in one slice
  //sqrt is, however, more easy to calculate
  vtkPoints *points = vtkPoints::New();  
  points->Allocate((int)sqrt((float)UnstructGrid->GetNumberOfPoints()));

  //check if the plane cuts the bounding box
  double ptsInter[3];
	if (GetIntersectionOfBoundsWithPlane(origin, normal, ptsInter))
	{
		//bounding box is intersected by the cutting plane and we have the coordinates of the
		//intersection of one of bounding edges with the plane
		//we can assume that at least one cell intersected by the plane will lie in the proximity of this point

		//TODO: BES: 13.5.2009 - The following algorithm does not work correctly if the plane separates the
		//input into several components. To fix it, we need to repeat GetFirstIntersectedCell as
		//long as it returns >= 0 (and we need to mark cells that were processed) 

		//get the closest mesh point and find the first intersected cell searching from that point
		vtkIdType ptsCellId = UnstructGrid->FindPoint(ptsInter);
		vtkIdType cellId = GetFirstIntersectedCell(ptsCellId >= 0 ? ptsCellId : 0);
		if (cellId >= 0)
		{
			//set cellId bit to 1 -> set cellId % sizeof(byte) bit of cellId / sizeof(byte) byte to 1
			//as sizeof(byte) == 8, we can make the computation easier
			cellStatus[cellId >> 3] |= 1 << (cellId & 0x7);
			stckTocheck[SP++] = cellId;

			while (SP != 0)
			{
				//get the cell to be checked and remove it from the stack
				cellId = stckTocheck[--SP];
				bool found = false;

				vtkCell* cell = UnstructGrid->GetCell(cellId);
				int ne = cell->GetNumberOfEdges();
				for (int j = 0; j < ne; j++)
				{
					vtkCell* edgecell = cell->GetEdge(j);
					vtkIdType id0 = edgecell->GetPointId(0);
					vtkIdType id1 = edgecell->GetPointId(1);

					// find if and where edge crosses plane
					double coords[3], lambda;
					vtkIdType idout, idtemp;
					double lamtemp;
					int itype = GetIntersectionOfLineWithPlane(&PointsCoords[3 * id0],
						&PointsCoords[3 * id1], origin, normal, coords, &lambda);

					if (itype != NO_INTERSECTION)
					{
						switch (itype) {
							case INTERSECTS_LINE:
								if (!GetOutputPointWhichCutsEdge(id0, id1, &idtemp, &lamtemp)) {
									// if edge has not been visited before, add point to the array and map it to the edge
									idout = points->InsertNextPoint(coords);

									Edge edge = { id0, id1 };
									AddMapping(idout, edge, lambda);
								}
								found = true;
								break;
							case INTERSECTS_POINT0:
								if (!GetOutputPointWhichCutsPoint(id0, &idtemp)) {
									// if input point has not been visited before, add point to the array and map it to the single point id0
									idout = points->InsertNextPoint(coords);
									AddMapping(idout, id0, 0.0);
								}
								found = true;
								break;
							case INTERSECTS_POINT1:
								if (!GetOutputPointWhichCutsPoint(id1, &idtemp)) {
									// if input point has not been visited before, add point to the array and map it to the single point id1
									idout = points->InsertNextPoint(coords);
									AddMapping(idout, id1, 0.0);
								}
								found = true;
								break;
							case LINE_IN_PLANE:
								// When the input edge {id0,id1} is exactly in the plane, we just list the endpoints as separate points,
								// without noting that they are joined by an edge.  (Otherwise things get very complicated).
								// This means that the edge only touches the plane at the endpoints.
								// Here we add both the endpoints to the array and map them to the ends of the edge.
								if (!GetOutputPointWhichCutsPoint(id0, &idtemp)) {
									idout = points->InsertNextPoint(&PointsCoords[3 * id0]);
									AddMapping(idout, id0, 1.0);
								}
								if (!GetOutputPointWhichCutsPoint(id1, &idtemp)) {
									idout = points->InsertNextPoint(&PointsCoords[3 * id1]);
									AddMapping(idout, id1, 1.0);
								}
								found = true;
								break;
						} //end switch

						//add also all adjacent cells sharing the current edge into the stack,
						//these cells are also intersected        
						edgesIdsPtr[0] = id0;
						edgesIdsPtr[1] = id1;

						UnstructGrid->GetCellNeighbors(cellId, edgeIds, cellsIds);
						int nac = cellsIds->GetNumberOfIds();
						vtkIdType* pCellIdsPtr = cellsIds->GetPointer(0);
            for (int k = 0; k < nac; k++)
						{
							//push all yet not processed cells into the stack
							//checking the appropriate bit in the bit array (using masking trick)
							vtkIdType acellId = pCellIdsPtr[k];
							unsigned char mask = 1 << (acellId & 0x7);
							if ((cellStatus[acellId >> 3] & mask) == 0)
							{
								cellStatus[acellId >> 3] |= mask;
								stckTocheck[SP++] = acellId;
							}
						} //end for
					} //end if (itype != NO_INTERSECTION)
				} //end for

				if (found) {
					// note that cell i has been intersected
					IntersectedCells.push_back(cellId);
				}
			} //end while
		}
	}

  points->Squeeze();
  Polydata->SetPoints(points) ;
  points->Delete();

  edgeIds->Delete();
  cellsIds->Delete();
  delete[] stckTocheck;
  delete[] cellStatus;
}

//------------------------------------------------------------------------------
// Get the list of input cells which are attached to the output point
void vtkALBAMeshCutter::GetInputCellsOnOutputPoint(vtkIdType idout, vtkIdList *cellids) const
//------------------------------------------------------------------------------
{
  vtkIdType id0, id1 ;
  double lamtemp ;

  if (GetInputEdgeCutByPoint(idout, &id0, &id1, &lamtemp)){   // look for input edge which point belongs to
    Edge edge = {id0,id1} ;
    GetCellNeighboursOfEdge(edge, cellids) ;
  }
  else if (GetInputPointCutByPoint(idout, &id0))              // look for input point which point belongs to
    GetCellNeighboursOfPoint(id0, cellids) ;
  else{
    // throw fatal error if point not found by either search
    std::cout << "GetInputCellsOnOutputPoint() can't find point " << idout << std::endl ;
    assert(false) ;
  }
}

//------------------------------------------------------------------------------
// Create and fill table of cells, PointsInCells.
// There is an entry for every input cell.
// Entry i is the list of output points in input cell i.
void vtkALBAMeshCutter::AssignPointsToCells()
//------------------------------------------------------------------------------
{
  int i, j ;
  vtkIdList *cellids = vtkIdList::New() ;

  // loop through all the points in the output polydata
  for (i = 0; i < Polydata->GetNumberOfPoints();  i++)
  {
    // get the input cells associated with this point
    cellids->Reset() ;    //Reset is faster than Initialize
    GetInputCellsOnOutputPoint(i, cellids) ;
    
    vtkIdType* cellidPtr = cellids->GetPointer(0);
    int nids = cellids->GetNumberOfIds();
    for (j = 0;  j < nids;  j++){
      // get the cell id and its list of points
      // add point i to the list of points
      std::vector<vtkIdType>& pointslistref = PointsInCells[cellidPtr[j]] ;
      pointslistref.push_back(i) ;
    }
  }

  cellids->Delete() ;
}

//------------------------------------------------------------------------------
// Are all the point ids members of this face
// Undefined id's are ignored, eg if the list is {1, 3, -1} and the face is {1, 3, 10, 11}, the result is true
bool vtkALBAMeshCutter::AllIdsInFace(vtkCell *face, vtkIdList *idlist)
//------------------------------------------------------------------------------
{
  int i, j ;
  bool found ;

  vtkIdList* fidlist = face->GetPointIds();
  vtkIdType* fidsPtr = fidlist->GetPointer(0);
  int nf = fidlist->GetNumberOfIds();

  vtkIdType* idsPtr = idlist->GetPointer(0);
  int nids = idlist->GetNumberOfIds();

  for (i = 0, found = true;  i < nids && found ;  i++)
  {
    if (idsPtr[i] >= 0)
    {
      for (j = 0, found = false;  j < nf && !found ;  j++)
        found = (idsPtr[i] == fidsPtr[j]) ;
    }
  }

  return found;
}

//------------------------------------------------------------------------------
// Find a face containing all id's in the list
// Undefined id's are ignored, ie the result can still be true if some of the id's are undefined
bool vtkALBAMeshCutter::FindFaceContainingAllIds(vtkCell *cell, vtkIdList *idlist, vtkIdType *faceid)
//------------------------------------------------------------------------------
{
  int nf = cell->GetNumberOfFaces();
  for (int i = 0; i < nf; i++)
  {
    vtkCell *face = cell->GetFace(i) ;
    if (AllIdsInFace(face, idlist)){      
      *faceid = i ;
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
// Construct the polygonal slice through the cell
// This involves putting the intersection points associated with this cell in the correct order to form a polygon.
// NB If the cell is cut by fewer than 3 points, no polygon is created.
bool vtkALBAMeshCutter::ConstructCellSlicePolygon(vtkIdType cellid, vtkIdList *polygon)
//------------------------------------------------------------------------------
{
  int i, j ;
  bool connected[8][8] ;
  vtkIdType edgepts[8][2] ;
  int maptype[8] ;
  vtkIdType f, ilast, inext, istart ;
  bool found, foundEdgeMapping,  sstop ;
  bool sameface = false, foundPtMapping = false;
  double lamtemp ;

  vtkCell* cell = UnstructGrid->GetCell(cellid);

  // clear list
  polygon->Reset() ;  //reset is faster than Initialize

  // create a convenient reference for the list of output points on this cell
  const std::vector<vtkIdType>& pointslistref = PointsInCells[cellid] ;
  int npts = pointslistref.size() ;

  if (npts == 0){
    // this cell is supposed to be intersected, so it can't be empty
    std::cout << "ConstructCellSlicePolygon(): cell " << cellid << " contains no intersection points" << std::endl ;
    assert(false) ;
  }
  else if (npts < 3){
    // Ignore cells with less than 3 points.
    // NB this means that not every cell in IntersectedCells corresponds to an output polygon !
    return false ;
  }
  else if (npts > 8){
    // there cannot be more than 8 intersection points
    std::cout << "ConstructCellSlicePolygon(): too many points " << pointslistref.size() << " in cell " << cellid << std::endl ;
    assert(false) ;
  }

  // construct a mini table, edgepts[][] and maptype[] to map output points to input edges and points
  vtkIdType id0, id1 ;
  for (i = 0 ;  i < npts ;  i++)
    maptype[i] = NO_MAPPING ;

  for (i = 0, foundEdgeMapping = false ;  i < npts ;  i++){
    vtkIdType ptId = pointslistref[i];
    if (GetInputEdgeCutByPoint(ptId, &id0, &id1, &lamtemp)){
      edgepts[i][0] = id0 ;
      edgepts[i][1] = id1 ;
      maptype[i] = POINT_TO_EDGE ;
      foundEdgeMapping = true ;
    }
    else if (GetInputPointCutByPoint(ptId, &id0)){
      edgepts[i][0] = id0 ;
      edgepts[i][1] = -1 ;
      maptype[i] = POINT_TO_POINT ;
      foundPtMapping = true ;
    }
  }

  if (foundPtMapping && !foundEdgeMapping){
    // Only point-to-point mappings were found, so we have to check for the special case
    // where all the points are in one face.

    // Construct a list of the intersected points
    Ptlist->Reset();
    vtkIdType* ptListPtr = Ptlist->WritePointer(0, npts);
    int nValidPts = 0;

    for (i = 0 ;  i < npts ;  i++){
      if (maptype[i] == POINT_TO_POINT)
        ptListPtr[nValidPts++] = (edgepts[i][0]) ;
    }
    
    Ptlist->SetNumberOfIds(nValidPts);

    // search for a face which contains all the points
    sameface = FindFaceContainingAllIds(cell, Ptlist, &f) ;
  } //end if (foundPtMapping && !foundEdgeMapping){

  if (sameface){
    //NB. this branch is not optimized
    // If the points are on the same face, we can just copy the face
    vtkCell *face = cell->GetFace(f) ;
    vtkIdType id_input, id_output ;
    for (i = 0 ;  i < face->GetNumberOfPoints() ;  i++){
      id_input = face->GetPointId(i) ;
      GetOutputPointWhichCutsPoint(id_input, &id_output) ;
      polygon->InsertNextId(id_output) ;
    } //end for
  }
  else{
    // find which pairs of points are connected (i.e. that share a face)
    for (i = 0 ;  i < npts ;  i++)
      connected[i][i] = false;    
    
    vtkIdType* ptListPtr = Ptlist->WritePointer(0, 4);
    for (i = 0 ;  i < npts-1 ;  i++)
    {
      // construct list of input point id's - first two entries
      ptListPtr[0] = (edgepts[i][0]) ;
      ptListPtr[1] = (edgepts[i][1]) ;

      for (j = i+1 ;  j < npts ;  j++)
      {
        // construct list of input point id's - other two entries
        ptListPtr[2] = (edgepts[j][0]) ;
        ptListPtr[3] = (edgepts[j][1]) ;

        // points are connected if they share a face
        connected[i][j] = FindFaceContainingAllIds(cell, Ptlist, &f);           
        connected[j][i] = connected[i][j];        
      }
    }    

    // If the points are on different faces, they must form a ring around the cell.
    // Pick a point and track it until it comes back to itself
    istart = 0 ;
    ilast = istart ;
    i = istart ;
    sstop = false ;
    do {
      // add point index to polygon
      polygon->InsertNextId(pointslistref[i]) ;

      // find next point connected to i
      for (j = 0, found = false ;  j <  npts;  j++){
        if ((j != i) && (j != ilast) && connected[i][j]){
          found = true ;
          inext = j ;
          break;
        }
      }

      if (found){
        if (inext == istart){
          // we have got back to the start point
          sstop = true ;
        }
        else{
          // carry on tracking
          ilast = i ;
          i = inext ;
        }
      }
      else{
        // this can't be right - we are in a dead end
        std::cout << "ConstructCellSlicePolygon(): can't finish tracking polygon in cell " << cellid << std::endl ;
        assert(false) ;
      }
    } while (!sstop);

    // check whether we used up all the points
    // this is either a fatal error or the slice formed a hole, which we can't handle yet
    if (polygon->GetNumberOfIds() != pointslistref.size()){
      std::cout << "ConstructCellSlicePolygon(): failed to connect all the points in cell " << cellid << std::endl ;
      assert(false) ;
    }
  }

  // find the normal and compare it with the normal of the cutting plane
  double normply[3];
  CalculatePolygonNormal(polygon, normply) ;
  double dotprod = normply[0]*CutTranformedNormal[0] + normply[1]*CutTranformedNormal[1] + normply[2]*CutTranformedNormal[2] ;

  if (dotprod > 0.0)
  {
    // polygon is winding the wrong way - need to reverse it
    vtkIdType* polyPtr = polygon->GetPointer(0);
    int nPolyPts = polygon->GetNumberOfIds();

    for (i = 0, j = nPolyPts - 1 ;  i < j ;  i++, j--)
    {
      vtkIdType tempi = polyPtr[i];      
      polyPtr[i] = polyPtr[j];
      polyPtr[j] = tempi;      
    }
  }
  return true ;
}

//------------------------------------------------------------------------------
// calculate the normal of the output polygon, given the list of point id's
void vtkALBAMeshCutter::CalculatePolygonNormal(vtkIdList *idlist, double *norm)
//------------------------------------------------------------------------------
{
  int i, j ;
  double x[8][3] ;
  double center[3] = {0.0, 0.0, 0.0} ;
  double cross[3] ;

  int npts = idlist->GetNumberOfIds() ;

  // get the x of the points
  vtkIdType* idsPtr = idlist->GetPointer(0);
  vtkPoints* pts = Polydata->GetPoints();
  for (i = 0 ;  i < npts ;  i++)
    pts->GetPoint(idsPtr[i], x[i]) ;

  // get the center
  for (i = 0 ;  i < npts ;  i++)
    for (j = 0 ;  j < 3 ;  j++)
      center[j] += x[i][j] ;
  for (j = 0 ;  j < 3 ;  j++)
    center[j] /= (double)npts ;

  // make the x relative to the center
  for (i = 0 ;  i < npts ;  i++)
    for (j = 0 ;  j < 3 ;  j++)
      x[i][j] -= center[j] ;


  // add the cross products round the polygon
  for (j = 0 ;  j < 3 ;  j++)
    norm[j] = 0.0 ;
  for (i = 0 ;  i < npts ;  i++){
    int inext = (i+1) % npts ;
    cross[0] =   x[i][1]*x[inext][2] - x[i][2]*x[inext][1] ;
    cross[1] = -(x[i][0]*x[inext][2] - x[i][2]*x[inext][0]) ;
    cross[2] =   x[i][0]*x[inext][1] - x[i][1]*x[inext][0] ;
    for (j = 0 ;  j < 3 ;  j++)
      norm[j] += cross[j] ;
  }

  // normalise the normal
  double nm = sqrt(norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2]) ;
  for (j = 0 ;  j < 3 ;  j++)
    norm[j] /= nm ;

}

//------------------------------------------------------------------------------
// Transfer the scalars by interpolation from input to output
void vtkALBAMeshCutter::TransferScalars()
//------------------------------------------------------------------------------
{
  int i, j ;
  double tuple0[100], tuple1[100], tuple_interp[100] ;
  int ncomp[100], dtype[100] ;

  // copy the structure of the point scalar arrays
  Polydata->GetPointData()->CopyStructure(UnstructGrid->GetPointData()) ;

  // allocate tuples for every point
  int npts = Polydata->GetPoints()->GetNumberOfPoints() ;
  Polydata->GetPointData()->SetNumberOfTuples(npts) ;

  // get no. of arrays, components and types
  int narrays = Polydata->GetPointData()->GetNumberOfArrays() ;
  for (i = 0 ;  i < narrays ;  i++){
    ncomp[i] = Polydata->GetPointData()->GetArray(i)->GetNumberOfComponents() ;
    dtype[i] = Polydata->GetPointData()->GetArray(i)->GetDataType() ;
  }

  // interpolate the scalars for every point
  for (i = 0 ;  i < (int)EdgeMapping.size() ;  i++){
    vtkIdType idout = EdgeMapping[i].idout ;
    vtkIdType id0 = EdgeMapping[i].id0 ;
    vtkIdType id1 = EdgeMapping[i].id1 ;
    double lambda = EdgeMapping[i].lambda ;

    // loop over all scalar arrays
    for (j = 0 ;  j < narrays ;  j++){
      // get the scalars for the input points
      if (EdgeMapping[i].mtype == POINT_TO_POINT){
        // the output point corresponds to only one input point - just copy the scalars
        UnstructGrid->GetPointData()->GetArray(j)->GetTuple(id0, tuple0) ;
        Polydata->GetPointData()->GetArray(j)->SetTuple(idout, tuple0) ;
      }
      else{
        // the output point corresponds to an input edge, so get scalars at both ends and interpolate
        UnstructGrid->GetPointData()->GetArray(j)->GetTuple(id0, tuple0) ;
        UnstructGrid->GetPointData()->GetArray(j)->GetTuple(id1, tuple1) ;
        InterpolateScalars(tuple0, tuple1, tuple_interp, lambda, ncomp[j], dtype[j]) ;
        Polydata->GetPointData()->GetArray(j)->SetTuple(idout, tuple_interp) ;
      }
    }
  }

  // copy the structure of the cell scalar arrays
  Polydata->GetCellData()->CopyStructure(UnstructGrid->GetCellData()) ;

  // allocate tuples for every cell
  int ncells = Polydata->GetNumberOfCells() ;
  Polydata->GetCellData()->SetNumberOfTuples(ncells) ;

  // get no. of arrays, components and types
  int ncellarrays = Polydata->GetCellData()->GetNumberOfArrays() ;

  // copy the scalars for every output cell
  for (i = 0 ;  i < (int)CellMapping.size() ;  i++){
    vtkIdType idin = GetInputCellCutByOutputCell(i) ;

    // loop over all scalar arrays
    for (j = 0 ;  j < ncellarrays ;  j++){
      UnstructGrid->GetCellData()->GetArray(j)->GetTuple(idin, tuple0) ;
      Polydata->GetCellData()->GetArray(j)->SetTuple(i, tuple0) ;
    }
  }

  if (UnstructGrid->GetPointData()->GetScalars())
  {
    const char* scalarName = UnstructGrid->GetPointData()->GetScalars()->GetName();
    Polydata->GetPointData()->SetActiveScalars(scalarName);
  }
  else if (UnstructGrid->GetCellData()->GetScalars())
  {
    const char* scalarName = UnstructGrid->GetCellData()->GetScalars()->GetName();
    Polydata->GetCellData()->SetActiveScalars(scalarName);
  }
}



//------------------------------------------------------------------------------
// Initialize the cutter
void vtkALBAMeshCutter::Initialize()
//------------------------------------------------------------------------------
{
  EdgeMapping.clear();
  InvEdgeMapping.clear();
  IntersectedCells.clear();
  //PointsInCells.clear();
  CellMapping.clear(); 

  // allocate the table with empty lists  
  delete[] PointsInCells;  
  PointsInCells = new std::vector<vtkIdType>[UnstructGrid->GetNumberOfCells()];  
}


//------------------------------------------------------------------------------
// Create the polydata slice
// There must have been a call to UnstructGrid->BuildLinks() for this to work
void vtkALBAMeshCutter::CreateSlice()
//------------------------------------------------------------------------------
{
  int i ;
  vtkIdList *polygon = vtkIdList::New() ;
  vtkCellArray *cells = vtkCellArray::New() ;

	//update local cut coordinates
	CalculateLocalCutCoord();

  FindPointsInPlane() ;           // this finds the points where the mesh intersects the plane

  AssignPointsToCells() ;         // this lists the intersection points for each mesh cell

  // construct the polygons for all the intersected cells
  int ncells = IntersectedCells.size() ;
  cells->Allocate(4*ncells) ;
  for (i = 0 ;  i < ncells ;  i++){
    if (ConstructCellSlicePolygon(IntersectedCells[i], polygon)){
      // if the polygon was created, insert it as a new output cell
      cells->InsertNextCell(polygon) ;

      // note mapping from new output cell to input cell
      CellMapping.push_back(IntersectedCells[i]) ;
    }
  }

  // squeeze the memory
  cells->Squeeze() ;

  // add to polydata
  Polydata->SetPolys(cells) ;

  // interpolate the scalars
  TransferScalars() ;

  // free memory
  cells->Delete()  ;
  polygon->Delete() ;
}

//------------------------------------------------------------------------------
// print self
void vtkALBAMeshCutter::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  int i, j, ni ;

  // print cutting plane
  os << indent << "cutting plane..." << std::endl ;
  double *po = CutTranformedOrigin ;
  double *pn = CutTranformedNormal ;
  os << indent << "origin: " << po[0] << " " << po[1] << " " << po[2] << std::endl ;
  os << indent << "normal: " << pn[0] << " " << pn[1] << " " << pn[2] << std::endl ;
  os << indent << std::endl ;

  // print table mapping output points to input mesh
  os << indent << "mapping output points to input mesh..." << std::endl ;
  os << indent << "no. of output points = " << EdgeMapping.size() << std::endl ;
  for (i = 0 ;  i < (int)EdgeMapping.size() ;  i++){
    EdgeMappingType em = EdgeMapping.at(i) ;
    if (em.mtype == POINT_TO_EDGE)
      os << indent << "mapping " << i << " point " << em.idout << " = input edge " << em.id0 << " " << em.id1 << " lambda " << em.lambda << std::endl ;
    else
      os << indent << "mapping " << i << " point " << em.idout << " = input point " << em.id0 << std::endl ;
  }
  os << indent << std::endl ;

  // print id's of intersected cells
  os <<  indent << "list of intersected cells..." << std::endl ;
  os << indent << "no. of intersected cells = " << IntersectedCells.size() << std::endl ;
  for (i = 0 ;  i < (int)IntersectedCells.size() ;  i++)
    os << indent <<"cut cell " << i << " cell id " << IntersectedCells.at(i) << std::endl ;
  os <<  indent <<std::endl ;

  // print list of output points in each input cell
  os << indent << "list of output points created in each input cell..." << std::endl ;
  for (i = 0, ni = 0 ;  i < (int)UnstructGrid->GetNumberOfCells()  ;  i++){
    // get the list for cell i
    std::vector<vtkIdType> &v = PointsInCells[i];

    if (v.size() > 0){
      // print the list of points if the cell is not empty
      os << indent << "cell id " << i << " point id's: " ;
      for (j = 0 ;  j < (int)v.size() ;  j++)
        os << v.at(j) << " " ;
      os << indent << std::endl ;

      // increment the count of non-empty cells
      ni++ ;
    }
  }
  os << indent << "counted " << ni << " cells containing points (should be same as no. of intersected cells)" << std::endl ;
  os << std::endl ;

  // print output polydata
  os << "polydata..." << std::endl ;
  Polydata->PrintSelf(os, indent) ;

}

void vtkALBAMeshCutter::ToRotationMatrix(vtkMatrix4x4 *matrix)
{
	//remove translation components
	matrix->SetElement(0,3,0);
	matrix->SetElement(1,3,0);
	matrix->SetElement(2,3,0);
}

void vtkALBAMeshCutter::CalculateLocalCutCoord()
{
	//Getting Mesh coordinates
	vtkLinearTransform *trans=vtkLinearTransform::SafeDownCast(CutFunction->GetTransform());
	if(trans)
	{
		double origin[4],normal[4];
		vtkMatrix4x4 *inverse;
		vtkNEW(inverse);
		//Getting slicing transform matrix
		trans->GetMatrix(inverse);
		//Inverting matrix to obtain slice to mesh transform matrix
		inverse->Invert();

		//vtkMatrix4x4 MultiplyPoint requires a 4d vector with last element set to 1
		origin[3]=normal[3]=1;
		CutFunction->GetNormal(normal);
		CutFunction->GetOrigin(origin);

		//Getting mesh local origin
		inverse->MultiplyPoint(origin,CutTranformedOrigin);

		//Isolate rotation matrix
		ToRotationMatrix(inverse);
		//Using rotation matrix to calculate mesh local normal
		inverse->MultiplyPoint(normal,CutTranformedNormal);
	}
	else
	{
		//No Transform found -> copy origin and normal
		CutFunction->GetNormal(CutTranformedNormal);
		CutFunction->GetOrigin(CutTranformedOrigin);
	}
}

//------------------------------------------------------------------------------
int vtkALBAMeshCutter::FillInputPortInformation(int, vtkInformation *info)
{
	info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkUnstructuredGrid");
	return 1;
}

