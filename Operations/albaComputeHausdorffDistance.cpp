/*=========================================================================
Program:   LHP
Module:    $RCSfile: albaComputeHausdorffDistance.cpp,v $
Language:  C++
Date:      $Date: 2011-09-08 15:41:49 $
Version:   $Revision: 1.1.2.6 $
Authors:   Eleonora Mambrini, Gianluigi Crimi
==========================================================================
Copyright (c) 2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#include "albaDefines.h"
#include "albaEvent.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the alba must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Includes :
#include "albaGUIBusyInfo.h"
#include "albaComputeHausdorffDistance.h"
#include "vtkCleanPolyData.h"
#include "vtkALBASmartPointer.h"
#include "vtkTriangleFilter.h"


#include "vtkCellData.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkGenericCell.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkTriangle.h"
#include "vtkUnstructuredGrid.h"

#include <math.h>
#include "albaProgressBarHelper.h"

#define FILTER_PERC 10
#define GRID_PERC 10

// The value of 1/sqrt(3) 
#define SQRT_1_3 0.5773502691896258

/* Margin factor from DBL_MIN to consider a triangle side length too small and
 * mark it as degenerate. */
#define DMARGIN 1e10

/* Maximum number of cells in the grid. */
#define GRID_CELLS_MAX 512000

// a few useful macros
#ifndef min
#  define min(__X, __Y) ((__X)<(__Y)?(__X):(__Y))
#endif
#ifndef max
#  define max(__X, __Y) ((__X)>(__Y)?(__X):(__Y))
#endif
#ifndef max3
#  define max3(__X,__Y,__Z) max((__X), max((__Y), (__Z)))
#endif
#ifndef min3
#  define min3(__X,__Y,__Z) min((__X), min((__Y),(__Z)))
#endif

//----------------------------------------------------------------------------
albaComputeHausdorffDistance::albaComputeHausdorffDistance()
{
  m_OverallDistance = 0;
  m_SamplingDensity = 1;

  m_Surf1 = NULL;
  m_Surf2 = NULL;
  m_CleanSurf1 = NULL;
  m_CleanSurf2 = NULL;
  m_OutputSurf = NULL;

  
  m_NumberOfTriangles1 = m_NumberOfTriangles2 = 0;

  m_CurrentNumberOfCells = 0;
  m_AverageTrianglesForNonEmptyCell = 0;

  m_MaxDistance =0;
  

  for(int i=0;i<3;i++)
  {
    m_BBoxMin[i] = m_BBoxMax[i]= 0.0;
    m_GridSize[i] = 0;
  }

  m_CellSize = 0.0;

   
  m_VertexErrorValues = NULL;
  
  m_OverallDistance = 0.0;

	m_TotalAreaSurf1= m_TotalAreaSurf2=0.0;

}

//----------------------------------------------------------------------------
albaComputeHausdorffDistance::~albaComputeHausdorffDistance()
{
  vtkDEL(m_OutputSurf);
  vtkDEL(m_CleanSurf1);
  vtkDEL(m_CleanSurf2);

  for(int i=0;i<m_ListOfTriangles2.size();i++)
    vtkDEL(m_ListOfTriangles2[i]);
  m_ListOfTriangles2.clear();

  for(int i=0;i<m_IntersectingTrianglesForCell.size();i++)
    m_IntersectingTrianglesForCell[i].clear();
  m_IntersectingTrianglesForCell.clear();

  m_EmptyCell.clear();

  vtkDEL(m_VertexErrorValues);
}

//----------------------------------------------------------------------------
void albaComputeHausdorffDistance::ComputeHausdorffDistance()
{
  //////////////////////////////////////////////////////////////////////////
  //initialize list of Triangles
  //////////////////////////////////////////////////////////////////////////
  int numberOfSurfCells = m_CleanSurf1->GetNumberOfCells();

  
 
  for(int i=0;i<numberOfSurfCells;i++)
  {
    if(m_CleanSurf1->GetCellType(i) == VTK_TRIANGLE)
    {
      vtkGenericCell *triangle = vtkGenericCell::New();
      m_CleanSurf1->GetCell(i, triangle);
      m_NumberOfTriangles1++;
    }
  }

  numberOfSurfCells = m_CleanSurf2->GetNumberOfCells();
  for(int i=0;i<numberOfSurfCells;i++)
  {
    if(m_CleanSurf2->GetCellType(i) == VTK_TRIANGLE)
    {
      vtkGenericCell *triangle= vtkGenericCell::New();
      m_CleanSurf2->GetCell(i, triangle);
      m_NumberOfTriangles2++;

      vtkPoints *points = triangle->GetPoints();
      double a[3],b[3],c[3];

      points->GetPoint(0, a);
      points->GetPoint(1, b);
      points->GetPoint(2, c);

      m_ListOfTriangles2.push_back(triangle);

      double area = vtkTriangle::TriangleArea(a,b,c);

      //area is used to estimate the cell size;
      m_TotalAreaSurf2 += area;
    }
  }

  //////////////////////////////////////////////////////////////////////////
	numberOfSurfCells = m_CleanSurf1->GetNumberOfCells();
	for (int i = 0; i < numberOfSurfCells; i++)
	{
		if (m_CleanSurf1->GetCellType(i) == VTK_TRIANGLE)
		{
			vtkGenericCell *triangle = vtkGenericCell::New();
			m_CleanSurf1->GetCell(i, triangle);

			vtkPoints *points = triangle->GetPoints();
			double a[3], b[3], c[3];

			points->GetPoint(0, a);
			points->GetPoint(1, b);
			points->GetPoint(2, c);

			double area = vtkTriangle::TriangleArea(a, b, c);
			
			//area is used to estimate the cell size;
			m_TotalAreaSurf1 += area;
		}
	}

  ComputeCellAndGridSize();

  m_IntersectingTrianglesForCell.resize(m_GridSize[0]*m_GridSize[1]*m_GridSize[2]);
  
  ComputeTrianglesInCell();

  long progress=FILTER_PERC+GRID_PERC;
	m_ProgBarHelper->UpdateProgressBar(progress);

  
  
  //For each triangle in Surf1, sample and compute error

  m_VertexErrorValues = vtkFloatArray::New();

  vtkPoints *points = m_CleanSurf1->GetPoints();
  int totVertex=points->GetNumberOfPoints();
 
  double dist = 0, prevDist = 0;
  double vertex[3],prevVertex[3];

	int step = MAX(1, totVertex / (100 - (FILTER_PERC + GRID_PERC)));
  prevVertex[0] = prevVertex[1] = prevVertex[2] = 0.0;
  for(int vertexID=0; vertexID<totVertex; vertexID++)
  {
    
    //////////////////////////////////////////////////////////////////////////
    if ((vertexID%step) == 0)
    {
      progress=FILTER_PERC + GRID_PERC + vertexID*(100-(FILTER_PERC + GRID_PERC))/totVertex;
			m_ProgBarHelper->UpdateProgressBar(progress);
    }
      
    int gridDimension = m_GridSize[0]*m_GridSize[1]*m_GridSize[2];

    points->GetPoint(vertexID,vertex);

    dist= ComputePointToSurfaceDistance(vertex, prevDist, prevVertex);
    m_VertexErrorValues->InsertNextTuple1(dist);
  }

}

//----------------------------------------------------------------------------
void albaComputeHausdorffDistance::ComputeTrianglesInCell()
{
  vtkPoints *sampleList = vtkPoints::New();

  std::vector<int> intersectingTrianglesForCell; /* Array with the number of intersecting
                               * triangles found so far for each cell */
  int cellId, prevCellId; /* linear (1D) cell indices */
  int cellStride_z;          /* spacement for Z index in 3D addressing of
                               * cell list */
  int i,j,h;             /* counters and loop limits */
  int m_a,n_a,o_a,m_b,n_b,o_b,m_c,n_c,o_c; /* 3D cell indices for vertices */
  int tmpi,maxCellDist;     /* maximum cell distance along any axis */
  int numberOfSamples;              /* number of samples to use for triangles */
  int m,n,o;                  /* 3D cell indices for samples */
  std::vector<int> c_buf;                 /* temp storage for cell list */

  /* Initialize */
  int gridDimension = m_GridSize[0]* m_GridSize[1]* m_GridSize[2];
  cellStride_z = m_GridSize[0]* m_GridSize[1];

  intersectingTrianglesForCell.assign(gridDimension, 0);
  m_EmptyCell.assign(gridDimension, 0);


  vtkGenericCell *triangle;
  /* Get intersecting cells for each triangle */
  for (i=0; i<m_ListOfTriangles2.size();i++) 
  {
    /* Get the cells in which the triangle vertices are. For non-negative
     * values, cast to int is equivalent to floor and probably faster (here
     * negative values can not happen since bounding box is obtained from the
     * vertices in m_ListOfTriangles2). */

    triangle= m_ListOfTriangles2[i];
    vtkPoints *points = triangle->GetPoints();
    double a[3],b[3],c[3];

    points->GetPoint(0, b);
    points->GetPoint(1, c);
    points->GetPoint(2, a);

    // cell indices for vertex A
    m_a = (int)((a[0]-m_BBoxMin[0])/m_CellSize);
    n_a = (int)((a[1]-m_BBoxMin[1])/m_CellSize);
    o_a = (int)((a[2]-m_BBoxMin[2])/m_CellSize);

    // cell indices for vertex B
    m_b = (int)((b[0]-m_BBoxMin[0])/m_CellSize);
    n_b = (int)((b[1]-m_BBoxMin[1])/m_CellSize);
    o_b = (int)((b[2]-m_BBoxMin[2])/m_CellSize);

    // cell indices for vertex C
    m_c = (int)((c[0]-m_BBoxMin[0])/m_CellSize);
    n_c = (int)((c[1]-m_BBoxMin[1])/m_CellSize);
    o_c = (int)((c[2]-m_BBoxMin[2])/m_CellSize);

    if (m_a == m_b && m_a == m_c && n_a == n_b && n_a == n_c && o_a == o_b && o_a == o_c) 
    {
      // The ABC triangle fits entirely into one cell => fast case
      cellId = m_a+n_a*m_GridSize[0]+o_a*cellStride_z;

      if(cellId <0 || cellId >= gridDimension)
        albaLogMessage("Error in ComputeTrianglesInCell. ");
      
      intersectingTrianglesForCell[cellId]++;
      m_IntersectingTrianglesForCell[cellId].push_back(i);
      continue;
    }

    // Triangle does not fit in one cell, how many cells does the triangle span?
    
    maxCellDist = abs(m_a-m_b);
    if ((tmpi = abs(m_a-m_c)) > maxCellDist) maxCellDist = tmpi;
    if ((tmpi = abs(m_b-m_c)) > maxCellDist) maxCellDist = tmpi;
    if ((tmpi = abs(n_a-n_b)) > maxCellDist) maxCellDist = tmpi;
    if ((tmpi = abs(n_a-n_c)) > maxCellDist) maxCellDist = tmpi;
    if ((tmpi = abs(n_b-n_c)) > maxCellDist) maxCellDist = tmpi;
    if ((tmpi = abs(o_a-o_b)) > maxCellDist) maxCellDist = tmpi;
    if ((tmpi = abs(o_a-o_c)) > maxCellDist) maxCellDist = tmpi;
    if ((tmpi = abs(o_b-o_c)) > maxCellDist) maxCellDist = tmpi;

    /* Sample the triangle so as to have twice the samples in any direction
     * than the number of cells spanned in that direction. */
    numberOfSamples = 2*(maxCellDist+1);
    SampleTriangle(triangle,numberOfSamples,sampleList);
    // Get the intersecting cells from the samples
    prevCellId = -1;

    c_buf.resize(0);

    for(j=0;j<sampleList->GetNumberOfPoints();j++)
    {
      // Get cell in which the sample is. Due to rounding in the triangle
      // sampling process we check the indices to be within bounds. 
      double sample[3];
      sampleList->GetPoint(j, sample);
      
      // m,n,o ---> cell coordinates 
      m = (int)((sample[0]-m_BBoxMin[0])/m_CellSize);
      if(m >= m_GridSize[0])
        m = m_GridSize[0] - 1;
      else if (m < 0) 
        m = 0;

      n=(int)((sample[1]-m_BBoxMin[1])/m_CellSize);
      if (n >= m_GridSize[1]) 
        n = m_GridSize[1] - 1;
      else if (n < 0) 
        n = 0;
      

      o = (int)((sample[2]-m_BBoxMin[2])/m_CellSize);
      if (o >= m_GridSize[2])
        o = m_GridSize[2] - 1;
      else if (o < 0)
        o = 0;

      /* Include cell index in list only if not the same as previous one
       * (avoid too many duplicates). */
      cellId = m + n*m_GridSize[0] + o*cellStride_z;

      if(cellId < 0 || cellId>= gridDimension)
      {
        albaLogMessage("Error in ComputeTrianglesInCell()");
        exit(1);
      }
      
      if (cellId != prevCellId) 
      {
        c_buf.push_back(cellId);
        prevCellId = cellId;
      }
    }

    /* Include triangle in intersecting cell lists, without duplicate. */
    for (j=0; j<c_buf.size(); j++)
    {
      cellId = c_buf[j];
      if(cellId < 0 || cellId >= gridDimension)
      {
        albaLogMessage("Error in ComputeTrianglesInCell()");
        exit(1);
      }


      if (intersectingTrianglesForCell[cellId] == 0 || m_IntersectingTrianglesForCell[cellId][intersectingTrianglesForCell[cellId]-1] != (int)i) 
      {
        intersectingTrianglesForCell[cellId]++;
        m_IntersectingTrianglesForCell[cellId].push_back((int)i);
      }
    }
  }

  /* Terminate lists with -1 and set empty cell bitmap */
  for(i=0, j=0, h=0; i<gridDimension; i++)
  {
    if (intersectingTrianglesForCell[i] == 0) 
    {
      /* mark empty cell in bitmap */
      m_EmptyCell[i] = 1;
    } 
    else 
    {
      j++;
      h += intersectingTrianglesForCell[i];
    }
  }

  int numberOfNonEmptyCells = j;
  m_AverageTrianglesForNonEmptyCell = (double)h/j;

  m_CurrentNumberOfCells = gridDimension;

}

//----------------------------------------------------------------------------
void albaComputeHausdorffDistance::SampleTriangle(vtkGenericCell *triangle, int sampleFrequency, vtkPoints *sampleList)
{
  if(sampleFrequency == 0)
    return;
  int numberOfSamples = sampleFrequency*(sampleFrequency+1)/2;

  sampleList->SetNumberOfPoints(numberOfSamples);

  double a[3], b[3], c[3];
  double u[3], v[3];
  double sample[3];
  vtkPoints *points = triangle->GetPoints();

  points->GetPoint(0, b);
  points->GetPoint(1, c);
  points->GetPoint(2, a);

  if(sampleFrequency != 1)
  {
    // get basis vectors
    for(int i=0;i<3;i++)
    {
      u[i] = (b[i]-a[i])/(double)(sampleFrequency-1);
      v[i] = (c[i]-a[i])/(double)(sampleFrequency-1);
    }

    // Sample triangle
    for (int k = 0, i = 0; i < sampleFrequency; i++) 
    {
      for (int j = 0, maxj = sampleFrequency-i;j < maxj; j++) 
      {
        sample[0] = a[0]+i*u[0]+j*v[0];
        sample[1] = a[1]+i*u[1]+j*v[1];
        sample[2] = a[2]+i*u[2]+j*v[2];

        sampleList->SetPoint(k, sample);
        k++;
      }
    }

  }
  else
  {
    /* special case, use triangle middle point */
    for(int j=0;j<3;j++)
      sample[j]= 1/3.0*(a[j]+b[j]+c[j]);

    sampleList->SetPoint(0, sample);
  }
}


//----------------------------------------------------------------------------
double albaComputeHausdorffDistance::ComputePointToSurfaceDistance(double point[3], double prevDist, double prevPoint[3])
{
  double distanceSqr = 0.0;
  //Get relative coordinates of point
  double relCoords[3];
  for(int i=0;i<3;i++)
    relCoords[i] = point[i]-m_BBoxMin[i];


  //////////////////////////////////////////////////////////////////////////
  // Get the cell coordinates of where point is. Since the bounding box bbox
  // is that of the model 2, the grid coordinates can be out of bounds (in
  // which case we limit them)
  //////////////////////////////////////////////////////////////////////////
  int gridCoords[3];

  for(int i=0;i<3;i++)
  {
    gridCoords[i] = (int) floor(relCoords[i]/m_CellSize);
    if (gridCoords[i] < 0) 
      gridCoords[i] = 0;
    else if (gridCoords[i] >= m_GridSize[i]) 
        gridCoords[i] = m_GridSize[i]-1;
  }

  //////////////////////////////////////////////////////////////////////////
  // Determine starting k, based on previous point (which is typically close
  // to current point) and its distance to closest triangle
  //////////////////////////////////////////////////////////////////////////
  double d0 = point[0]-prevPoint[0];
  double d1 = point[1]-prevPoint[1];
  double d2 = point[2]-prevPoint[2];

  double ddp = sqrt(d0*d0 + d1*d1 + d2*d2);

  double distMin = prevDist - ddp;
  int k = (int) floor(distMin*SQRT_1_3/m_CellSize)-2;
  if (k <0) 
    k = 0;

  //////////////////////////////////////////////////////////////////////////
  // Scan cells, at sequentially increasing index distance k
  //////////////////////////////////////////////////////////////////////////
  
  int kmax = max3(m_GridSize[0],m_GridSize[1],m_GridSize[2]);
  if (k >= kmax) 
    k = kmax-1;
  double distMinSqr = VTK_DOUBLE_MAX;;
  double cellSizeSqr = m_CellSize*m_CellSize;
  int cellStride_z = m_GridSize[0]*m_GridSize[1];

  int cellId, triangleId;
  VECTOR1 currentCellTriangleList;

  do {
    /* Get the list of cells at distance k in X Y or Z direction, which has
     * not been previously tested. Only non-empty cells are included in the
     * list. */
    cellId = gridCoords[0] + gridCoords[1]*m_GridSize[0] + gridCoords[2]*cellStride_z;
    VECTOR2 tmp;
    if (tmp.size()<= k) 
    {
      tmp.resize(k+1);
      GetCellsAtDistance(&tmp, gridCoords, k);
    }

    /* Scan each (non-empty) cell in the compiled list */
    for(int j=0;j<tmp[k].size();j++)
    {
      cellId = tmp[k][j];
      /* If minimum distance from point to cell is larger than already
       * found minimum distance we can skip all triangles in the cell */
      double pd=ComputePointToCellSqrDistance(relCoords,gridCoords,cellId, cellStride_z);
      if (distMinSqr < pd) 
      {
        continue;
      }
      // Scan all triangles (i.e. faces) in the cell
      currentCellTriangleList = m_IntersectingTrianglesForCell[cellId];

      for(int currentId =0; currentId<currentCellTriangleList.size(); currentId++)
      {
        //if(triangleId >= currentCellTriangleList.size())
        //  continue;
        triangleId = currentCellTriangleList[currentId];
        vtkGenericCell *currentTriangle = m_ListOfTriangles2[triangleId];
        distanceSqr = ComputePointToTriangleSqrDistance(point, currentTriangle);
        if (distanceSqr < distMinSqr)
        {
          distMinSqr= distanceSqr;
        }
      }
    }
    /* We loop until the minimum distance to any of the cells to come is
     * larger than the minimum distance to a face found so far; or until all
     * cells have been tested. */
    k++;

    //cleaning tmp vector2;
    for (int i=0;i<tmp.size();i++)
    {
      tmp[i].clear();
    }
    tmp.clear();
  } while (k < kmax && distMinSqr >= k*k*cellSizeSqr);
    

  return distMinSqr;
  currentCellTriangleList.clear();

}

//----------------------------------------------------------------------------
void albaComputeHausdorffDistance::GetCellsAtDistance(VECTOR2 *distanceCellsList, int cellGridCoords[3], int distance)
{
  int maxNumberoOfCells;
  int cellId;
  int cellId1,cellId2;
  int cellStride_z;
  std::vector<int> cur_cell;
  int m,n,o;
  int m1,m2,n1,n2,o1,o2;
  int min_m,max_m,min_n,max_n,min_o,max_o;
  int d;
  int tmp;
  VECTOR1 currentListOfCells;

  //assert(distance == 0 || distanceCellsList->size() <= distance);

  /* Initialize */
  cellStride_z = m_GridSize[1]*m_GridSize[0];

  /* Get the cells that are at distance k in the X, Y or Z direction from the
   * center cell. For the zero distance we also include the center cell. */
  maxNumberoOfCells = 6*(2*distance+1)*(2*distance+1)+12*(2*distance+1)+8;
  if (distance == 0) 
    maxNumberoOfCells += 1; /* add center cell */
  
  if (distance == 0) 
  { /* add center cell */
    cellId = cellGridCoords[0]+cellGridCoords[1]*m_GridSize[0]+
      cellGridCoords[2]*cellStride_z;
    if(m_EmptyCell[cellId] == 0)
    {
      currentListOfCells.push_back(cellId);
    }
  }
  /* Try to put the cells in the order of increasing distances (minimizes
   * number of cells and triangles to test). Doing a full ordering is too
   * slow, we just do what we can fast. */
  d = distance+1; /* max displacement */
  min_o = max(cellGridCoords[2]-d+1,0);
  max_o = min(cellGridCoords[2]+d-1,m_GridSize[2]-1);
  min_n = max(cellGridCoords[1]-d+1,0);
  max_n = min(cellGridCoords[1]+d-1,m_GridSize[1]-1);
  m1 = cellGridCoords[0]-d;
  m2 = cellGridCoords[0]+d;
  if (m1 >= 0) {
    if (m2 < m_GridSize[0]) 
    { 
      // left + right layer
      for (o = min_o; o <= max_o; o++)
      {
        for (n = min_n; n <= max_n; n++)
        {
          tmp = n*m_GridSize[0]+o*cellStride_z;
          cellId1 = m1+tmp;
          cellId2 = m2+tmp;
          if (m_EmptyCell[cellId1] == 0) 
          {
            currentListOfCells.push_back(cellId1);
          }
          if (m_EmptyCell[cellId2] == 0) 
          {
            currentListOfCells.push_back(cellId2);
          }
        }
      }
    } else { /* left layer */
      for (o = min_o; o <= max_o; o++)
      {
        for (n = min_n; n <= max_n; n++)
        {
          cellId = m1+n*m_GridSize[0]+o*cellStride_z;
          if(m_EmptyCell[cellId] == 0)
          {
            currentListOfCells.push_back(cellId);
          }
        }
      }
    }
  } else {
    if (m2 < m_GridSize[0]) 
    { 
      //right layer
      for (o = min_o; o <= max_o; o++)
      {
        for (n = min_n; n <= max_n; n++)
        {
          cellId = m2+n*m_GridSize[0]+o*cellStride_z;
          if(m_EmptyCell[cellId] == 0)
          {
            currentListOfCells.push_back(cellId);
          }
        }
      }
    }
  }
  
  min_m = max(cellGridCoords[0]-d,0);
  max_m = min(cellGridCoords[0]+d,m_GridSize[0]-1);
  n1 = cellGridCoords[1]-d;
  n2 = cellGridCoords[1]+d;

  if (n1 >= 0) {
    if (n2 < m_GridSize[1])
    { 
      // back + front layers
      for (o = min_o; o <= max_o; o++)
      {
        for (m = min_m; m <= max_m; m++)
        {
          tmp = m+o*cellStride_z;
          cellId1 = tmp+n1*m_GridSize[0];
          cellId2 = tmp+n2*m_GridSize[0];
          if(m_EmptyCell[cellId1] == 0)
          {
            currentListOfCells.push_back(cellId1);
          }
          if(m_EmptyCell[cellId2] == 0)
          {
            currentListOfCells.push_back(cellId2);
          }
        }
      }
    } else { 
      // back layer 
      for (o = min_o; o <= max_o; o++) 
      {
        for (m = min_m; m <= max_m; m++) 
        {
          cellId = m+n1*m_GridSize[0]+o*cellStride_z;
          if(m_EmptyCell[cellId] == 0)
          {
            currentListOfCells.push_back(cellId);
          }
        }
      }
    }
  } else {
    if (n2 < m_GridSize[1])
    { 
      // front layer 
      for (o = min_o; o <= max_o; o++) 
      {
        for (m = min_m; m <= max_m; m++) 
        {
          cellId = m+n2*m_GridSize[0]+o*cellStride_z;
          if(m_EmptyCell[cellId] == 0)
          {
            currentListOfCells.push_back(cellId);
          }
        }
      }
    }
  }
  min_n = max(cellGridCoords[1]-d,0);
  max_n = min(cellGridCoords[1]+d,m_GridSize[1]-1);
  o1 = cellGridCoords[2]-d;
  o2 = cellGridCoords[2]+d;
  if (o1 >= 0) {
    if (o2 < m_GridSize[2])
    { 
      // bottom + top layers
      for (n = min_n; n <= max_n; n++)
      {
        for (m = min_m; m <= max_m; m++)
        {
          tmp = m+n*m_GridSize[0];
          cellId1 = tmp+o1*cellStride_z;
          cellId2 = tmp+o2*cellStride_z;

          if(m_EmptyCell[cellId1] == 0)
          {
            currentListOfCells.push_back(cellId1);
          }
          if(m_EmptyCell[cellId2] == 0)
          {
            currentListOfCells.push_back(cellId2);
          }
        }
      }
    } else{
      // bottom 
      for (n = min_n; n <= max_n; n++)
      {
        for (m = min_m; m <= max_m; m++) 
        {
          cellId = m+n*m_GridSize[0]+o1*cellStride_z;
          if(m_EmptyCell[cellId] == 0)
          {
            currentListOfCells.push_back(cellId);
          }
        }
      }
    }
  } else {
    if (o2 < m_GridSize[2]) 
    { 
      //top layer
      for (n = min_n; n <= max_n; n++)
      {
        for (m = min_m; m <= max_m; m++)
        {
          cellId = m+n*m_GridSize[0]+o2*cellStride_z;
          if(m_EmptyCell[cellId] == 0)
          {
            currentListOfCells.push_back(cellId);
          }
        }
      }
    }
  }
  /* Store resulting cell list */

  distanceCellsList->at(distance) = currentListOfCells;
  currentListOfCells.clear();
}

unsigned long long counter=0;

//----------------------------------------------------------------------------
double albaComputeHausdorffDistance::ComputePointToTriangleSqrDistance(double point[3], vtkGenericCell *triangle)
{
  double minSqrDistance;

  double closestPoint[3], weights[3], pCoords[3];
  int subId=0;
  double dist2=0;

  for(int i=0;i<3;i++)
    closestPoint[i] = weights[i] = pCoords[i] = 0.0;

  triangle->EvaluatePosition(point, closestPoint, subId, pCoords, dist2, weights);

  counter++;

  minSqrDistance = dist2;

  return minSqrDistance;

}

//----------------------------------------------------------------------------
double albaComputeHausdorffDistance::ComputePointToCellSqrDistance(double point[3], int grid[3], int cellId, int cellStrideZ)
{
  double d2,tmp;
  int m,n,o,tmpi;

  //////////////////////////////////////////////////////////////////////////
  //Get 3D indices of cell cell_idx
  //////////////////////////////////////////////////////////////////////////
  o = cellId/cellStrideZ;
  tmpi = cellId%cellStrideZ;
  n = tmpi/m_GridSize[0];
  m = tmpi%m_GridSize[0];
  //////////////////////////////////////////////////////////////////////////
  

  //////////////////////////////////////////////////////////////////////////
  // Calculate distance 
  //////////////////////////////////////////////////////////////////////////
  d2 = 0;
  if (grid[0] != m) 
  { 
    // if not on same cell x wise
    tmp = (m > grid[0]) ? m*m_CellSize-point[0] : point[0]-(m+1)*m_CellSize;
    d2 += tmp*tmp;
  }
  if (grid[1] != n) 
  { 
    // if not on same cell y wise
    tmp = (n > grid[1]) ? n*m_CellSize-point[1] : point[1]-(n+1)*m_CellSize;
    d2 += tmp*tmp;
  }
  if (grid[2] != o) 
  { 
    // if not on same cell z wise 
    tmp = (o > grid[2]) ? o*m_CellSize-point[2] : point[2]-(o+1)*m_CellSize;
    d2 += tmp*tmp;
  }
  return d2;

}


//----------------------------------------------------------------------------
vtkPolyData *albaComputeHausdorffDistance::GetOutput()
{
  vtkNEW(m_OutputSurf);
  vtkNEW(m_CleanSurf1);
  vtkNEW(m_CleanSurf2);

  // glue the bridge to the surface by merging the coincident points
  vtkCleanPolyData *cleaner = vtkCleanPolyData::New();
  vtkTriangleFilter *triangulator = vtkTriangleFilter::New();
  
	albaGUIBusyInfo wait(_("albaComputeHausdorffDistance: computing Hausdorff distance..."));

	m_ProgBarHelper = new albaProgressBarHelper(m_Listener);
	m_ProgBarHelper->InitProgressBar();

  long progress = 0;

  cleaner->SetInput(m_Surf1);
  cleaner->ConvertPolysToLinesOff();
  cleaner->GetOutput()->Update();
  triangulator->SetInput(cleaner->GetOutput());
  triangulator->Update();
  m_CleanSurf1->DeepCopy(triangulator->GetOutput());
  m_CleanSurf1->Modified();
  m_CleanSurf1->Update();

  progress+=FILTER_PERC/2.0;
  
  cleaner->SetInput(m_Surf2);
  cleaner->ConvertPolysToLinesOff();
  cleaner->GetOutput()->Update();
  triangulator->SetInput(cleaner->GetOutput());
  triangulator->Update();
  m_CleanSurf2->DeepCopy(triangulator->GetOutput());
  m_CleanSurf2->Modified();
  m_CleanSurf2->Update();

  progress+=FILTER_PERC/2.0;
	m_ProgBarHelper->UpdateProgressBar(progress);

  
  m_OutputSurf->DeepCopy(m_CleanSurf1);

  albaLogMessage("albaComputeHausdorffDistance: computing Hausdorff distance...");
  ComputeHausdorffDistance();
  albaLogMessage("albaComputeHausdorffDistance: computed.");

  int nc=m_VertexErrorValues->GetNumberOfTuples();
	double sumDist=0;

  //Values are on Squared Dist
	for (int i = 0; i < nc; i++)
	{
		double dist = sqrt(m_VertexErrorValues->GetTuple1(i));
		sumDist += dist;
		m_VertexErrorValues->SetTuple1(i, dist);
	}

	double mean = sumDist / (double)nc;
	double errSq = 0;

	for (int i = 0; i < nc; i++)
	{
		double err = mean-m_VertexErrorValues->GetTuple1(i);
		errSq += err*err;
	}

	double stdDev = sqrt(errSq / (double)nc);

	albaLogMessage("Hausdorff Distances:\n Mean=%f, Standard deviation=%f", mean, stdDev);

  m_VertexErrorValues->SetName("Vertices Hausdorff Distance");

  m_OutputSurf->GetPointData()->AddArray(m_VertexErrorValues);

  m_OutputSurf->GetPointData()->SetActiveScalars("Vertices Hausdorff Distance");

	m_ProgBarHelper->CloseProgressBar();
	cppDEL(m_ProgBarHelper);

  return m_OutputSurf;
  
}


//----------------------------------------------------------------------------
void albaComputeHausdorffDistance::SetData(vtkPolyData *data1, vtkPolyData *data2)
{
  m_Surf1 = data1;
  m_Surf2 = data2;
}


//----------------------------------------------------------------------------
void albaComputeHausdorffDistance::ComputeCellAndGridSize()
{
  //get bounding box
  double bb1[6], bb2[6];

  m_CleanSurf1->GetBounds(bb1);
  m_CleanSurf2->GetBounds(bb2);

  m_BBoxMin[0] = min(bb1[0], bb2[0]);
  m_BBoxMin[1] = min(bb1[2], bb2[2]);
  m_BBoxMin[2] = min(bb1[4], bb2[4]);

  m_BBoxMax[0] = max(bb1[1], bb2[1]);
  m_BBoxMax[1] = max(bb1[3], bb2[3]);
  m_BBoxMax[2] = max(bb1[5], bb2[5]);

  m_CellSize = 0.707 * sqrt(m_TotalAreaSurf2/m_NumberOfTriangles2*4/sqrt(3.0));

  //Avoid values that can overflow or underflow
  if (m_CellSize < DBL_MIN*DMARGIN)
  { 
    m_CellSize = DBL_MIN*DMARGIN;
  } 
  else if (m_CellSize >= DBL_MAX/DMARGIN)
  {
    exit(1);
  }

  // Limit to maximum number of cells
  double gridSize[3];

  gridSize[0] = ceil((m_BBoxMax[0]-m_BBoxMin[0])/m_CellSize);
  if (gridSize[0] <= 0)
    gridSize[0] = 1;
  
  gridSize[1] = ceil((m_BBoxMax[1]-m_BBoxMin[1])/m_CellSize);
  if (gridSize[1] <= 0) 
    gridSize[1] = 1;

  gridSize[2] = ceil((m_BBoxMax[2]-m_BBoxMin[2])/m_CellSize);
  if (gridSize[2] <= 0) 
    gridSize[2] = 1;

  if (gridSize[0]*gridSize[1]*gridSize[2] > GRID_CELLS_MAX) 
  {
    m_CellSize *= pow(gridSize[0]*gridSize[1]*gridSize[02]/GRID_CELLS_MAX,1.0/3);

    gridSize[0] = ceil((m_BBoxMax[0]-m_BBoxMin[0])/m_CellSize);
    if (gridSize[0] <= 0)
      gridSize[0] = 1;
    
    gridSize[1] = ceil((m_BBoxMax[1]-m_BBoxMin[1])/m_CellSize);
    if (gridSize[1] <= 0)
      gridSize[1] = 1;

    gridSize[2] = ceil((m_BBoxMax[2]-m_BBoxMin[2])/m_CellSize);
    if (gridSize[2] <= 0)
      gridSize[2] = 1;
  }

  // Final grid values
  m_GridSize[0] = (int) gridSize[0];
  m_GridSize[1] = (int) gridSize[1];
  m_GridSize[2] = (int) gridSize[2];
};

