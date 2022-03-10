/*=========================================================================
Program:   LHP
Module:    $RCSfile: albaComputeHausdorffDistance.h,v $
Language:  C++
Date:      $Date: 2011-08-05 09:11:04 $
Version:   $Revision: 1.1.2.4 $
Authors:   Eleonora Mambrini, Gianluigi Crimi
==========================================================================
Copyright (c) 2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#ifndef __albaComputeHausdorffDistance_H__
#define __albaComputeHausdorffDistance_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------

#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class vtkFloatArray;
class vtkGenericCell;
class vtkPoints;
class vtkPolyData;
class vtkTriangle;
class vtkUnstructuredGrid;
class albaProgressBarHelper;

/* Use a bitmap for marking empty cells. Otherwise use array of a simple
 * type. Using a bitmap uses less memory and can be faster than a simple type
 * depending on the cache system and number of cells in the grid (number of
 * cache misses). 
 */

/* --------------------------------------------------------------------------*
 *                       Local data types                                    *
 * --------------------------------------------------------------------------*/

typedef std::vector<int> VECTOR1;
typedef std::vector<std::vector<int> > VECTOR2;

/** 
class name : albaComputeHausdorffDistance
*/
class albaComputeHausdorffDistance
{
public:
  /** constructor. */
  albaComputeHausdorffDistance();
  /** destructor. */
  ~albaComputeHausdorffDistance(); 

  void SetData(vtkPolyData *data1, vtkPolyData *data2);
  void SetListener(albaObserver *listener){m_Listener=listener;};
 
  vtkPolyData *GetOutput();

private:

  void ComputeHausdorffDistance();
  double ComputePointToSurfaceDistance(double point[3], double prevDist, double prevPoint[3]);
  double ComputePointToCellSqrDistance(double point[3], int grid[3], int cellId, int cellStrideZ);
  double ComputePointToTriangleSqrDistance(double point[3], vtkGenericCell *triangle);
  void ComputeTrianglesInCell();
  void GetCellsAtDistance(VECTOR2 *distanceCellsList, int cellGridCoords[3], int distance);
  

  void SampleTriangle(vtkGenericCell *triangle, int sampleFrequency, vtkPoints *sampleList);

  void ComputeCellAndGridSize();

  vtkPolyData *m_Mesh1, *m_Mesh2;
  vtkPolyData *m_CleanMesh1, *m_CleanMesh2;
  vtkPolyData *m_OutputMesh;

	albaObserver *m_Listener;

  int m_NumberOfTriangles1, m_NumberOfTriangles2;
  std::vector<vtkGenericCell*> m_ListOfTriangles2;
  
  double m_SamplingDensity;
  double m_OverallDistance;

  VECTOR2 m_IntersectingTrianglesForCell;
  VECTOR1 m_EmptyCell;
  int m_CurrentNumberOfCells;
  double m_AverageTrianglesForNonEmptyCell;

  int m_MaxDistance;

  
  double m_BBoxMin[3], m_BBoxMax[3];
  int m_GridSize[3];
  double m_CellSize;

  
  double m_MeanDistance;
  double m_RmsMean;
  double m_MinDist;
  double m_MaxDist;

  double m_TotalAreaMesh2;

  vtkFloatArray *m_VertexErrorValues;
	albaProgressBarHelper *m_ProgBarHelper;

};
#endif