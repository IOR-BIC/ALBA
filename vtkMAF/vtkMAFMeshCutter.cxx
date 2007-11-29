/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFMeshCutter.cxx,v $
Language:  C++
Date:      $Date: 2007-11-29 09:18:41 $
Version:   $Revision: 1.4 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2007 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/

#include "mafDefines.h"
#include "vtkMAFMeshCutter.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "assert.h"
#include "vtkIndent.h"
#include <ostream>
#include <vector>


//------------------------------------------------------------------------------
// standard macros
vtkCxxRevisionMacro(vtkMAFMeshCutter, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkMAFMeshCutter);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Constructor
vtkMAFMeshCutter::vtkMAFMeshCutter()
//------------------------------------------------------------------------------
{
  m_cutFunction = NULL ;
  m_unstructGrid = vtkUnstructuredGrid::New() ;
}

//------------------------------------------------------------------------------
// Destructor
vtkMAFMeshCutter::~vtkMAFMeshCutter()
//------------------------------------------------------------------------------
{
  m_unstructGrid->Delete() ;
}

//------------------------------------------------------------------------------
// Overload standard modified time function. If cut function is modified,
// then this object is modified as well.
unsigned long vtkMAFMeshCutter::GetMTime()
//------------------------------------------------------------------------------
{
  unsigned long mTime = this->vtkUnstructuredGridToPolyDataFilter::GetMTime();
  unsigned long time;

  if (m_cutFunction != NULL )
  {
    time = m_cutFunction->GetMTime();
    mTime = ( time > mTime ? time : mTime );
  }

  return mTime;
}

//------------------------------------------------------------------------------
// Execute method
void vtkMAFMeshCutter::Execute()
//------------------------------------------------------------------------------
{
  // Make a copy of the input data and buil links
  // Can't just set a pointer because BuildLinks() would change the input.
  m_unstructGrid->Initialize() ;
  m_unstructGrid->DeepCopy(this->GetInput()) ;
  m_unstructGrid->BuildLinks() ;

  // Set pointer to output
  m_polydata = this->GetOutput() ;

  // Make sure the cutter is cleared of previous data before you run it !
  Initialize() ;

  // make sure the output is empty
  m_polydata->Initialize() ;

  // Run the cutter
  CreateSlice() ;
}


//------------------------------------------------------------------------------
// Set cutting plane
void vtkMAFMeshCutter::SetCutFunction(vtkPlane *P)
//------------------------------------------------------------------------------
{
  m_cutFunction = P ;
}

//------------------------------------------------------------------------------
// Set cutting plane
vtkPlane* vtkMAFMeshCutter::GetCutFunction()
//------------------------------------------------------------------------------
{
  return m_cutFunction ;
}

//------------------------------------------------------------------------------
// Find the output point idout corresponding to the input edge (id0,id1)
// This returns true if the the input edge is in the table
bool vtkMAFMeshCutter::GetOutputPointWhichCutsEdge(vtkIdType id0, vtkIdType id1, vtkIdType *idout, double *lambda) const
//------------------------------------------------------------------------------
{
  unsigned int i ;
  bool found ;

  // search for (id0, id1)
  for (i = 0, found = false ;  i < m_edgeMapping.size() && !found ;  i++){
    if (m_edgeMapping[i].mtype == POINT_TO_EDGE){
      vtkIdType e0 = m_edgeMapping[i].id0 ;
      vtkIdType e1 = m_edgeMapping[i].id1 ;
      if (((id0 == e0) && (id1 == e1)) || ((id0 == e1) && (id1 == e0))){
        *idout = m_edgeMapping[i].idout ;
        *lambda = m_edgeMapping[i].lambda ;
        found = true ;
      }
    }
  }

  return found ;
}

//------------------------------------------------------------------------------
// Find the output point idout corresponding to the single input point id0
// This returns true if the the input point is in the table
bool vtkMAFMeshCutter::GetOutputPointWhichCutsPoint(vtkIdType id0, vtkIdType *idout) const
//------------------------------------------------------------------------------
{
  unsigned int i ;
  bool found ;

  // search for id0
  for (i = 0, found = false ;  i < m_edgeMapping.size() && !found ;  i++){
    if (m_edgeMapping[i].mtype == POINT_TO_POINT){
      vtkIdType e0 = m_edgeMapping[i].id0 ;
      if (id0 == e0){
        *idout = m_edgeMapping[i].idout ;
        found = true ;
      }
    }
  }

  return found ;
}

//------------------------------------------------------------------------------
// Find the input edge (id0,id1) corresponding to the output point idout
// This returns true if the output point is in the table
bool vtkMAFMeshCutter::GetInputEdgeCutByPoint(vtkIdType idout, vtkIdType *id0, vtkIdType *id1, double *lambda) const
//------------------------------------------------------------------------------
{
  unsigned int i ;
  bool found ;
  for (i = 0, found = false ;  i < m_edgeMapping.size() && !found ;  i++){
    if (m_edgeMapping[i].mtype == POINT_TO_EDGE){
      if (idout == m_edgeMapping[i].idout ){
        *id0 = m_edgeMapping[i].id0 ;
        *id1 = m_edgeMapping[i].id1 ;
        *lambda = m_edgeMapping[i].lambda ;
        found = true ;
      }
    }
  }

  return found ;
}

//------------------------------------------------------------------------------
// Find the single input point id0 corresponding to the output point idout
// This returns true if the the input point is in the table
bool vtkMAFMeshCutter::GetInputPointCutByPoint(vtkIdType idout, vtkIdType *id0) const
//------------------------------------------------------------------------------
{
  unsigned int i ;
  bool found ;
  for (i = 0, found = false ;  i < m_edgeMapping.size() && !found ;  i++){
    if (m_edgeMapping[i].mtype == POINT_TO_POINT){
      if (idout == m_edgeMapping[i].idout ){
        *id0 = m_edgeMapping[i].id0 ;
        found = true ;
      }
    }
  }

  return found ;
}

//------------------------------------------------------------------------------
// Get the input cell corresponding to the output cell
vtkIdType vtkMAFMeshCutter::GetInputCellCutByOutputCell(vtkIdType idout)
//------------------------------------------------------------------------------
{
  if ((idout < 0) || (idout >= (int)m_cellMapping.size())){
    std::cout << "GetInputCellCutByOutputCell(): output cell index " << idout << "out of range" << std::endl ;
    assert(false) ;
  }

  vtkIdType thing = m_cellMapping[idout] ;
  return m_cellMapping[idout] ;
}

//------------------------------------------------------------------------------
// Get the output cell corresponding to the input cell
// returns true if the input id was found
bool vtkMAFMeshCutter::GetOutputCellWhichCutsInputCell(vtkIdType idin, vtkIdType *idout)
//------------------------------------------------------------------------------
{
  unsigned int i ;
  bool found ;

  for (i = 0, found = false ;  i < m_cellMapping.size() ;  i++){
    if (m_cellMapping[i] == idin){
      found = true ;
      *idout = i ;
    }
  }

  return found ;
}

//------------------------------------------------------------------------------
// Add set of id's to the edge mapping table
// This is a POINT_TO_EDGE mapping
// This maps output point idout to input edge (id0,id1) 
void vtkMAFMeshCutter::AddMapping(vtkIdType idout, const Edge& edge, double lambda)
//------------------------------------------------------------------------------
{
  EdgeMapping em = {idout, edge.id0, edge.id1, lambda, POINT_TO_EDGE} ;
  m_edgeMapping.push_back(em) ;
}

//------------------------------------------------------------------------------
// Add set of id's to the edge mapping table
// this is a POINT_TO_POINT mapping
// This maps output point idout to single input point id0
void vtkMAFMeshCutter::AddMapping(vtkIdType idout, vtkIdType id0, double lambda)
//------------------------------------------------------------------------------
{
  EdgeMapping em = {idout, id0, undefinedId, lambda, POINT_TO_POINT} ;
  m_edgeMapping.push_back(em) ;
}

//------------------------------------------------------------------------------
// Get intersection of line with plane
// Edge is defined by coords of endpoints (p0, p1).
// Plane is defined by origin and normal.
// Returns type of intersection.
// lambda returns fractional distance of interpolated point along edge (0 <= lambda <= 1).
int vtkMAFMeshCutter::GetIntersectionOfLineWithPlane(const double *p0, const double *p1, const double *origin, const double *norm, double *coords, double *lambda) const
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

//------------------------------------------------------------------------------
// Get intersection of edge with plane
// Edge is defined by id's of endpoints (id0, id1).
// Plane is defined by origin and normal.
// Returns type of intersection.
// lambda returns fractional distance of interpolated point along edge (0 <= lambda <= 1).
int vtkMAFMeshCutter::GetIntersectionOfEdgeWithPlane(const Edge& edge, double *coords, double *lambda) const
//------------------------------------------------------------------------------
{
  double p0[3], p1[3] ;

  // get coords of end points of edge
  m_unstructGrid->GetPoint(edge.id0, p0) ;
  m_unstructGrid->GetPoint(edge.id1, p1) ;

  return GetIntersectionOfLineWithPlane(p0, p1, m_cutFunction->GetOrigin(), m_cutFunction->GetNormal(), coords, lambda) ;
}

//-----------------------------------------------------------------------------
// Get list of cells which are adjoined to point
void vtkMAFMeshCutter::GetCellNeighboursOfPoint(vtkIdType idpt, vtkIdList *idlist) const 
//-----------------------------------------------------------------------------
{
  // get cell neighbours of point
  int ncells = m_unstructGrid->GetCellLinks()->GetNcells(idpt) ;
  vtkIdType* id0 = m_unstructGrid->GetCellLinks()->GetCells(idpt) ;

  // copy cell id's to idlist
  for (int i = 0 ;  i < ncells ;  i++)
    idlist->InsertNextId(id0[i]) ;
}

//-----------------------------------------------------------------------------
// Get list of cells which are adjoined to edge
void vtkMAFMeshCutter::GetCellNeighboursOfEdge(const Edge& edge, vtkIdList *idlist) const
//-----------------------------------------------------------------------------
{
  vtkIdList *idlist0 = vtkIdList::New() ;
  vtkIdList *idlist1 = vtkIdList::New() ;

  // list cell neighbours of both endpoints of edge 
  GetCellNeighboursOfPoint(edge.id0, idlist0) ;
  GetCellNeighboursOfPoint(edge.id1, idlist1) ;

  // copy cell id's to idlist if they are in both lists
  for (int i = 0 ;  i < idlist0->GetNumberOfIds() ;  i++){
    bool found = false ;
    for (int j = 0, found = false ;  j < idlist1->GetNumberOfIds() && !found ;  j++){
      if (idlist1->GetId(j) == idlist0->GetId(i)){
        idlist->InsertNextId(idlist0->GetId(i)) ;
        found = true ;
      }
    }
  }

  idlist0->Delete() ;
  idlist1->Delete() ;
}

//------------------------------------------------------------------------------
// Find where the edges cross the plane and create polydata points
// It creates the table m_edgeMapping which maps output points to the input mesh.
// It also creates m_intersectedCells, which is a list of the input cells which were intersected.
void vtkMAFMeshCutter::FindPointsInPlane()
//------------------------------------------------------------------------------
{
  int i, j ;
  bool found ;

  vtkPoints *points = vtkPoints::New() ;

  // test all the edges in the input mesh
  int nc = m_unstructGrid->GetNumberOfCells() ;
  for (i = 0 ;  i < nc ;  i++){
    vtkCell* cell = m_unstructGrid->GetCell(i) ;

    int ne = cell->GetNumberOfEdges() ;
    for (j = 0, found = false ;  j < ne ;  j++){
      vtkCell* edgecell = cell->GetEdge(j) ;

      vtkIdType id0 =  edgecell->GetPointId(0) ;
      vtkIdType id1 =  edgecell->GetPointId(1) ;
      Edge edge = {id0, id1} ;

      // find if and where edge crosses plane
      double coords[3], lambda ;
      vtkIdType idout, idtemp ;
      double lamtemp ;
      int itype = GetIntersectionOfEdgeWithPlane(edge, coords, &lambda) ;
      switch(itype){
        case NO_INTERSECTION:
          break ;
        case INTERSECTS_LINE:
          if (!GetOutputPointWhichCutsEdge(id0, id1, &idtemp, &lamtemp)){
            // if edge has not been visited before, add point to the array and map it to the edge
            idout = points->InsertNextPoint(coords) ;
            AddMapping(idout, edge, lambda) ;
          }
          found = true ;
          break ;
        case INTERSECTS_POINT0:
          if (!GetOutputPointWhichCutsPoint(id0, &idtemp)){
            // if input point has not been visited before, add point to the array and map it to the single point id0
            idout = points->InsertNextPoint(coords) ;
            AddMapping(idout, id0, 0.0) ;
          }
          found = true ;
          break ;
        case INTERSECTS_POINT1:
          if (!GetOutputPointWhichCutsPoint(id1, &idtemp)){
            // if input point has not been visited before, add point to the array and map it to the single point id1
            idout = points->InsertNextPoint(coords) ;
            AddMapping(idout, id1, 0.0) ;
          }
          found = true ;
          break ;
        case LINE_IN_PLANE:
          // When the input edge {id0,id1} is exactly in the plane, we just list the endpoints as separate points,
          // without noting that they are joined by an edge.  (Otherwise things get very complicated).
          // This means that the edge only touches the plane at the endpoints.
          // Here we add both the endpoints to the array and map them to the ends of the edge.
          if (!GetOutputPointWhichCutsPoint(id0, &idtemp)){
            idout = points->InsertNextPoint(m_unstructGrid->GetPoint(id0)) ;
            AddMapping(idout, id0, 1.0) ;
          }
          if (!GetOutputPointWhichCutsPoint(id1, &idtemp)){
            idout = points->InsertNextPoint(m_unstructGrid->GetPoint(id1)) ;
            AddMapping(idout, id1, 1.0) ;
          }
          found = true ;
          break ;
      }
    }

    if (found){
      // note that cell i has been intersected
      m_intersectedCells.push_back(i) ;
    }
  }

  m_polydata->SetPoints(points) ;
  points->Delete() ;
}

//------------------------------------------------------------------------------
// Get the list of input cells which are attached to the output point
void vtkMAFMeshCutter::GetInputCellsOnOutputPoint(vtkIdType idout, vtkIdList *cellids) const
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
// Create and fill table of cells, m_pointsInCells.
// There is an entry for every input cell.
// Entry i is the list of output points in input cell i.
void vtkMAFMeshCutter::AssignPointsToCells()
//------------------------------------------------------------------------------
{
  int i, j ;
  vtkIdList *cellids = vtkIdList::New() ;

  // allocate the table with empty lists
  int ncells = m_unstructGrid->GetNumberOfCells() ;
  std::vector<vtkIdType> emptylist ;
  m_pointsInCells.resize(ncells, emptylist) ;

  // loop through all the points in the output polydata
  for (i = 0 ;  i < m_polydata->GetNumberOfPoints() ;  i++){
    // get the input cells associated with this point
    cellids->Initialize() ;
    GetInputCellsOnOutputPoint(i, cellids) ;

    for (j = 0 ;  j < cellids->GetNumberOfIds() ;  j++){
      // get the cell id and its list of points
      vtkIdType cellid = cellids->GetId(j) ;

      // add point i to the list of points
      std::vector<vtkIdType>& pointslistref = m_pointsInCells[cellid] ;
      pointslistref.push_back(i) ;
    }
  }

  cellids->Delete() ;
}

//------------------------------------------------------------------------------
// Are all the point ids members of this face
// Undefined id's are ignored, eg if the list is {1, 3, -1} and the face is {1, 3, 10, 11}, the result is true
bool vtkMAFMeshCutter::AllIdsInFace(vtkCell *face, vtkIdList *idlist)
//------------------------------------------------------------------------------
{
  int i, j ;
  bool found ;

  for (i = 0, found = true ;  i < idlist->GetNumberOfIds() && found ;  i++){
    if (idlist->GetId(i) >= 0){
      for (j = 0, found = false ;  j < face->GetNumberOfPoints() && !found ;  j++)
        found = (idlist->GetId(i) == face->GetPointId(j)) ;
    }
  }

  return found ;
}

//------------------------------------------------------------------------------
// Find a face containing all id's in the list
// Undefined id's are ignored, ie the result can still be true if some of the id's are undefined
bool vtkMAFMeshCutter::FindFaceContainingAllIds(vtkCell *cell, vtkIdList *idlist, vtkIdType *faceid)
//------------------------------------------------------------------------------
{
  int i ;
  bool found ;

  for (i = 0, found = false ;  i < cell->GetNumberOfFaces() && !found ;  i++){
    vtkCell *face = cell->GetFace(i) ;
    if (AllIdsInFace(face, idlist)){
      found = true ;
      *faceid = i ;
    }
  }

  return found ;
}

//------------------------------------------------------------------------------
// Construct the polygonal slice through the cell
// This involves putting the intersection points associated with this cell in the correct order to form a polygon.
// NB If the cell is cut by fewer than 3 points, no polygon is created.
bool vtkMAFMeshCutter::ConstructCellSlicePolygon(vtkIdType cellid, vtkIdList *polygon)
//------------------------------------------------------------------------------
{
  int i, j ;
  bool connected[8][8] ;
  vtkIdType edgepts[8][2] ;
  int maptype[8] ;
  vtkIdType f, ilast, inext, istart ;
  bool found, foundEdgeMapping, foundPtMapping, sstop ;
  bool sameface = false ;
  double lamtemp ;

  // clear list
  polygon->Initialize() ;

  // create a convenient reference for the list of output points on this cell
  const std::vector<vtkIdType>& pointslistref = m_pointsInCells[cellid] ;
  int npts = pointslistref.size() ;

  if (npts == 0){
    // this cell is supposed to be intersected, so it can't be empty
    std::cout << "ConstructCellSlicePolygon(): cell " << cellid << " contains no intersection points" << std::endl ;
    assert(false) ;
  }
  else if (npts < 3){
    // Ignore cells with less than 3 points.
    // NB this means that not every cell in m_intersectedCells corresponds to an output polygon !
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
    if (GetInputEdgeCutByPoint(pointslistref[i], &id0, &id1, &lamtemp)){
      edgepts[i][0] = id0 ;
      edgepts[i][1] = id1 ;
      maptype[i] = POINT_TO_EDGE ;
      foundEdgeMapping = true ;
    }
  }
  for (i = 0, foundPtMapping = false ;  i < npts ;  i++){
    if (GetInputPointCutByPoint(pointslistref[i], &id0)){
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
    vtkIdList *ptlist = vtkIdList::New() ;
    for (i = 0 ;  i < npts ;  i++){
      if (maptype[i] == POINT_TO_POINT)
        ptlist->InsertNextId(edgepts[i][0]) ;
    }

    // search for a face which contains all the points
    sameface = FindFaceContainingAllIds(m_unstructGrid->GetCell(cellid), ptlist, &f) ;

    ptlist->Delete() ;
  }

  if (sameface){
    // If the points are on the same face, we can just copy the face
    vtkCell *face = m_unstructGrid->GetCell(cellid)->GetFace(f) ;
    vtkIdType id_input, id_output ;
    for (i = 0 ;  i < face->GetNumberOfPoints() ;  i++){
      id_input = face->GetPointId(i) ;
      GetOutputPointWhichCutsPoint(id_input, &id_output) ;
      polygon->InsertNextId(id_output) ;
    }
  }
  else{
    // find which pairs of points are connected (ie that share a face)
    for (i = 0 ;  i < npts ;  i++)
      for (j = 0 ;  j < npts ;  j++)
        connected[i][j] = false ;

    vtkIdList *ptlist = vtkIdList::New() ;
    for (i = 0 ;  i < npts-1 ;  i++){
      for (j = i+1 ;  j < npts ;  j++){
        // construct list of input point id's 
        ptlist->Initialize() ;
        ptlist->InsertNextId(edgepts[i][0]) ;
        ptlist->InsertNextId(edgepts[i][1]) ;
        ptlist->InsertNextId(edgepts[j][0]) ;
        ptlist->InsertNextId(edgepts[j][1]) ;

        // points are connected if they share a face
        if (FindFaceContainingAllIds(m_unstructGrid->GetCell(cellid), ptlist, &f)){
          connected[i][j] = true ;
          connected[j][i] = true ;
        }
      }
    }
    ptlist->Delete() ;

    // If the points are on different faces, they must form a ring around the cell.
    // Pick a point and track it until it comes back to itself
    istart = 0 ;
    ilast = istart ;
    i = istart ;
    sstop = false ;
    while (!sstop){
      // add point index to polygon
      polygon->InsertNextId(pointslistref[i]) ;

      // find next point connected to i
      for (j = 0, found = false ;  j <  npts && !found ;  j++){
        if ((j != i) && (j != ilast) && connected[i][j]){
          found = true ;
          inext = j ;
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
    }

    // check whether we used up all the points
    // this is either a fatal error or the slice formed a hole, which we can't handle yet
    if (polygon->GetNumberOfIds() != pointslistref.size()){
      std::cout << "ConstructCellSlicePolygon(): failed to connect all the points in cell " << cellid << std::endl ;
      assert(false) ;
    }

  }

  // find the normal and compare it with the normal of the cutting plane
  double normply[3], normpln[3] ;
  CalculatePolygonNormal(polygon, normply) ;
  m_cutFunction->GetNormal(normpln) ;
  double dotprod = normply[0]*normpln[0] + normply[1]*normpln[1] + normply[2]*normpln[2] ;

  if (dotprod < 0.0){
    // polygon is winding the wrong way - need to reverse it
    for (i = 0, j = polygon->GetNumberOfIds()-1 ;  i < j ;  i++, j--){
      vtkIdType tempi = polygon->GetId(i) ;
      vtkIdType tempj = polygon->GetId(j) ;
      polygon->SetId(i, tempj) ;
      polygon->SetId(j, tempi) ;
    }
  }

  return true ;
}

//------------------------------------------------------------------------------
// calculate the normal of the output polygon, given the list of point id's
void vtkMAFMeshCutter::CalculatePolygonNormal(vtkIdList *idlist, double *norm)
//------------------------------------------------------------------------------
{
  int i, j ;
  double x[8][3] ;
  double center[3] = {0.0, 0.0, 0.0} ;
  double cross[3] ;

  int npts = idlist->GetNumberOfIds() ;

  // get the x of the points
  for (i = 0 ;  i < npts ;  i++)
    m_polydata->GetPoints()->GetPoint(idlist->GetId(i), x[i]) ;

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
// Interpolate two scalar tuples
// tupout = (1-lambda)*tup0 + lambda*tup1
// If the data type is not float or double, we add 0.5 to ensure correct rounding when it is converted back to int.
void vtkMAFMeshCutter::InterpolateScalars(double *tup0, double *tup1, double *tupout, double lambda, int ncomponents, int dattype)
//------------------------------------------------------------------------------
{ 
  for (int j = 0 ;  j < ncomponents ;  j++)
    tupout[j] = (1.0 - lambda)*tup0[j] + lambda*tup1[j] ;

  if (dattype != VTK_FLOAT && dattype != VTK_DOUBLE){
    // the real data type of the scalars is integer, so add 0.5 to ensure correct rounding when they are put back into the polydata
    for (int j = 0 ;  j < ncomponents ;  j++)
      tupout[j] += 0.5 ;
  }
}

//------------------------------------------------------------------------------
// Transfer the scalars by interpolation from input to output
void vtkMAFMeshCutter::TransferScalars()
//------------------------------------------------------------------------------
{
  int i, j ;
  double tuple0[100], tuple1[100], tuple_interp[100] ;
  int ncomp[100], dtype[100] ;

  // copy the structure of the point scalar arrays
  m_polydata->GetPointData()->CopyStructure(m_unstructGrid->GetPointData()) ;

  // allocate tuples for every point
  int npts = m_polydata->GetPoints()->GetNumberOfPoints() ;
  m_polydata->GetPointData()->SetNumberOfTuples(npts) ;

  // get no. of arrays, components and types
  int narrays = m_polydata->GetPointData()->GetNumberOfArrays() ;
  for (i = 0 ;  i < narrays ;  i++){
    ncomp[i] = m_polydata->GetPointData()->GetArray(i)->GetNumberOfComponents() ;
    dtype[i] = m_polydata->GetPointData()->GetArray(i)->GetDataType() ;
  }

  // interpolate the scalars for every point
  for (i = 0 ;  i < (int)m_edgeMapping.size() ;  i++){
    vtkIdType idout = m_edgeMapping[i].idout ;
    vtkIdType id0 = m_edgeMapping[i].id0 ;
    vtkIdType id1 = m_edgeMapping[i].id1 ;
    double lambda = m_edgeMapping[i].lambda ;

    // loop over all scalar arrays
    for (j = 0 ;  j < narrays ;  j++){
      // get the scalars for the input points
      if (m_edgeMapping[i].mtype == POINT_TO_POINT){
        // the output point corresponds to only one input point - just copy the scalars
        m_unstructGrid->GetPointData()->GetArray(j)->GetTuple(id0, tuple0) ;
        m_polydata->GetPointData()->GetArray(j)->SetTuple(idout, tuple0) ;
      }
      else{
        // the output point corresponds to an input edge, so get scalars at both ends and interpolate
        m_unstructGrid->GetPointData()->GetArray(j)->GetTuple(id0, tuple0) ;
        m_unstructGrid->GetPointData()->GetArray(j)->GetTuple(id1, tuple1) ;
        InterpolateScalars(tuple0, tuple1, tuple_interp, lambda, ncomp[j], dtype[j]) ;
        m_polydata->GetPointData()->GetArray(j)->SetTuple(idout, tuple_interp) ;
      }
    }
  }

  // copy the structure of the cell scalar arrays
  m_polydata->GetCellData()->CopyStructure(m_unstructGrid->GetCellData()) ;

  // allocate tuples for every cell
  int ncells = m_polydata->GetNumberOfCells() ;
  m_polydata->GetCellData()->SetNumberOfTuples(ncells) ;

  // get no. of arrays, components and types
  int ncellarrays = m_polydata->GetCellData()->GetNumberOfArrays() ;

  // copy the scalars for every output cell
  for (i = 0 ;  i < (int)m_cellMapping.size() ;  i++){
    vtkIdType idin = GetInputCellCutByOutputCell(i) ;

    // loop over all scalar arrays
    for (j = 0 ;  j < ncellarrays ;  j++){
      m_unstructGrid->GetCellData()->GetArray(j)->GetTuple(idin, tuple0) ;
      m_polydata->GetCellData()->GetArray(j)->SetTuple(i, tuple0) ;
    }
  }
}



//------------------------------------------------------------------------------
// Initialize the cutter
void vtkMAFMeshCutter::Initialize()
//------------------------------------------------------------------------------
{
  m_edgeMapping.clear() ;
  m_intersectedCells.clear() ;
  m_pointsInCells.clear() ;
  m_cellMapping.clear() ;

}


//------------------------------------------------------------------------------
// Create the polydata slice
// There must have been a call to m_unstructGrid->BuildLinks() for this to work
void vtkMAFMeshCutter::CreateSlice()
//------------------------------------------------------------------------------
{
  int i ;
  vtkIdList *polygon = vtkIdList::New() ;
  vtkCellArray *cells = vtkCellArray::New() ;

  FindPointsInPlane() ;           // this finds the points where the mesh intersects the plane

  AssignPointsToCells() ;         // this lists the intersection points for each mesh cell

  // construct the polygons for all the intersected cells
  int ncells = m_intersectedCells.size() ;
  cells->Allocate(4*ncells) ;
  for (i = 0 ;  i < ncells ;  i++){
    if (ConstructCellSlicePolygon(m_intersectedCells[i], polygon)){
      // if the polygon was created, insert it as a new output cell
      cells->InsertNextCell(polygon) ;

      // note mapping from new output cell to input cell
      m_cellMapping.push_back(m_intersectedCells[i]) ;
    }
  }

  // squeeze the memory
  cells->Squeeze() ;

  // add to polydata
  m_polydata->SetPolys(cells) ;

  // interpolate the scalars
  TransferScalars() ;

  // free memory
  cells->Delete()  ;
  polygon->Delete() ;
}

//------------------------------------------------------------------------------
// print self
void vtkMAFMeshCutter::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  int i, j, ni ;

  // print cutting plane
  os << indent << "cutting plane..." << std::endl ;
  double *po = m_cutFunction->GetOrigin() ;
  double *pn = m_cutFunction->GetNormal() ;
  os << indent << "origin: " << po[0] << " " << po[1] << " " << po[2] << std::endl ;
  os << indent << "normal: " << pn[0] << " " << pn[1] << " " << pn[2] << std::endl ;
  os << indent << std::endl ;

  // print table mapping output points to input mesh
  os << indent << "mapping output points to input mesh..." << std::endl ;
  os << indent << "no. of output points = " << m_edgeMapping.size() << std::endl ;
  for (i = 0 ;  i < (int)m_edgeMapping.size() ;  i++){
    EdgeMapping em = m_edgeMapping.at(i) ;
    if (em.mtype == POINT_TO_EDGE)
      os << indent << "mapping " << i << " point " << em.idout << " = input edge " << em.id0 << " " << em.id1 << " lambda " << em.lambda << std::endl ;
    else
      os << indent << "mapping " << i << " point " << em.idout << " = input point " << em.id0 << std::endl ;
  }
  os << indent << std::endl ;

  // print id's of intersected cells
  os <<  indent << "list of intersected cells..." << std::endl ;
  os << indent << "no. of intersected cells = " << m_intersectedCells.size() << std::endl ;
  for (i = 0 ;  i < (int)m_intersectedCells.size() ;  i++)
    os << indent <<"cut cell " << i << " cell id " << m_intersectedCells.at(i) << std::endl ;
  os <<  indent <<std::endl ;

  // print list of output points in each input cell
  os << indent << "list of output points created in each input cell..." << std::endl ;
  for (i = 0, ni = 0 ;  i < (int)m_pointsInCells.size()  ;  i++){
    // get the list for cell i
    std::vector<vtkIdType> &v = m_pointsInCells.at(i) ;

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
  m_polydata->PrintSelf(os, indent) ;

}
