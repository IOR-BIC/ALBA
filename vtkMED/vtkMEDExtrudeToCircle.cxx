/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMEDExtrudeToCircle.cxx,v $
Language:  C++
Date:      $Date: 2008-06-16 14:41:22 $
Version:   $Revision: 1.3 $
Authors:   Nigel McFarlane

================================================================================
Copyright (c) 2008 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/


#include "mafDefines.h"
#include "vtkPolyDataToPolyDataFilter.h"
#include "vtkObjectFactory.h"
#include "vtkIdList.h"
#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkMEDPastValuesList.h"
#include "vtkMEDExtrudeToCircle.h"
#include <assert.h>

#ifndef M_PI
  #define _USE_MATH_DEFINES
#endif

#include <cmath>




//------------------------------------------------------------------------------
// standard macros
vtkCxxRevisionMacro(vtkMEDExtrudeToCircle, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkMEDExtrudeToCircle);
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Constructor
vtkMEDExtrudeToCircle::vtkMEDExtrudeToCircle() : m_definedLength(false), m_definedDirection(false), m_definedVector(false),
m_definedExtrusionPoint(false), m_definedMinNumEndPts(false), m_builtCells(false),
m_mesh(NULL), m_defaultDirectionSign(1)
//------------------------------------------------------------------------------
{
}


//------------------------------------------------------------------------------
// Destructor
vtkMEDExtrudeToCircle::~vtkMEDExtrudeToCircle()
//------------------------------------------------------------------------------
{  
  if (m_mesh != NULL)
    delete m_mesh ;
}



//------------------------------------------------------------------------------
// Initialize the filter
void vtkMEDExtrudeToCircle::Initialize()
//------------------------------------------------------------------------------
{
  if (m_mesh != NULL){
    delete m_mesh ;
    m_mesh = NULL ;
  }

  m_output->Initialize() ;
}




//------------------------------------------------------------------------------
// Execute method
void vtkMEDExtrudeToCircle::Execute()
//------------------------------------------------------------------------------
{
  vtkDebugMacro(<< "Executing ExtrudeToCircle Filter") ;

  // pointers to input and output
  m_input = this->GetInput() ;
  m_output = this->GetOutput() ;

  // Make sure the filter is cleared of previous data before you run it !
  Initialize() ;



  //----------------------------------------------------------------------------
  // Calculate parameters of input hole and extrusion
  //----------------------------------------------------------------------------

  // Calculate the parameters of the input hole (centre, normal and radius)
  CalcHoleParameters(m_input) ;

  // Calculate the direction, length etc of the extrusion
  CalculateExtrusionVector() ;

  // Calculate radius of end of extrusion.
  // If the extrusion vector is not normal to the hole, need to correct radius
  // because the hole will be an elliptical slice with a greater radius than the vessel.
  // If you slice a circular tube with radius r, at an angle a to the normal,
  // the rms radius of the elliptical slice will be r * sqrt(1+sec^2(a)) / sqrt(2)
  double cosa = DotProduct(m_holeNormal, m_direction) ;
  m_endRadius = m_holeRadius * sqrt(2.0) * fabs(cosa) / sqrt(1.0 + cosa*cosa) ;


  //----------------------------------------------------------------------------
  // Calculate the mesh structure
  //----------------------------------------------------------------------------
  m_mesh = new MeshData ;
  CalcExtrusionRings() ;
  CalcExtrusionVertices() ;


  //----------------------------------------------------------------------------
  // Transfer mesh to output polydata
  //----------------------------------------------------------------------------
  vtkPoints *points = vtkPoints::New() ;
  VerticesToVtkPoints(points) ;
  m_output->SetPoints(points) ;
  points->Delete() ;

  vtkCellArray *triangles = vtkCellArray::New() ;
  VerticesToVtkTriangles(triangles) ;
  m_output->SetPolys(triangles) ;
  triangles->Delete() ;
}



//------------------------------------------------------------------------------
// Set the length of the extrusion
void vtkMEDExtrudeToCircle::SetLength(double len)
//------------------------------------------------------------------------------
{
  // set the length
  m_length = len ;
  m_definedLength = true ;

  // extrusion vector and extrusion point are no longer defined
  m_definedVector = false ;
  m_definedExtrusionPoint = false ;

  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set the direction of extrusion.
// The direction does not have to be normalised.
void vtkMEDExtrudeToCircle::SetDirection(const double *direc)       
//------------------------------------------------------------------------------
{
  // copy vector and normalize
  Normalize(direc, m_direction) ;
  m_definedDirection = true ;

  // extrusion vector and extrusion point are no longer defined
  m_definedVector = false ;
  m_definedExtrusionPoint = false ;

  this->Modified() ;
}


//------------------------------------------------------------------------------
// Set the direction of extrusion.
// The direction does not have to be normalised.
void vtkMEDExtrudeToCircle::SetDirection(double x, double y, double z)       
//------------------------------------------------------------------------------
{
  // copy vector and normalize
  m_direction[0] = x ;
  m_direction[1] = y ;
  m_direction[2] = z ;

  Normalize(m_direction, m_direction) ;
  m_definedDirection = true ;

  // extrusion vector and extrusion point are no longer defined
  m_definedVector = false ;
  m_definedExtrusionPoint = false ;

  this->Modified() ;
}


//------------------------------------------------------------------------------
// Set the target point of extrusion.
void vtkMEDExtrudeToCircle::SetExtrusionPoint(const double *extrPoint)
//------------------------------------------------------------------------------
{
  CopyVector(extrPoint, m_extrusionPoint) ;
  m_definedExtrusionPoint = true ;

  // everything else is undefined until we know the centre of the hole
  m_definedLength = false ;
  m_definedDirection = false ;
  m_definedVector = false ;

  this->Modified() ;
}


//------------------------------------------------------------------------------
// Set the extrusion vector.
void vtkMEDExtrudeToCircle::SetExtrusionVector(const double *extrVector)
//------------------------------------------------------------------------------
{
  CopyVector(extrVector, m_extrusionVector) ;
  m_definedVector = true ;

  // need to calculate length and direction later
  m_definedLength = false ;
  m_definedDirection = false ;

  // extrusion point is no longer defined
  m_definedExtrusionPoint = false ;

  this->Modified() ;
}


//------------------------------------------------------------------------------
// set min no. of vertices at end of extrusion
void vtkMEDExtrudeToCircle::SetMinNumberofEndPoints(int minNumEndPoints)
//------------------------------------------------------------------------------
{
  m_minNumEndPts = minNumEndPoints ;
  m_definedMinNumEndPts = true ;

  this->Modified() ;
}



//------------------------------------------------------------------------------
// Reset the extrusion direction to the default, which is normal to the hole.
// Use this to reverse the direction if the extrusion goes the wrong way.
// Argument is +1 or -1 to select forward or reverse direction relative to the hole.
void vtkMEDExtrudeToCircle::SetDirectionToDefault(int directionSign)      
//------------------------------------------------------------------------------
{
  // set any existing vector data to undefined
  m_definedDirection = false ;
  m_definedVector = false ;
  m_definedExtrusionPoint = false ;

  // set the flag for the sign of the direction
  if (directionSign == -1)
    m_defaultDirectionSign = -1 ;
  else
    m_defaultDirectionSign = 1 ;

  this->Modified() ;

}



//------------------------------------------------------------------------------
// Calculate the extrusion vector, once we know the centre and normal of the hole.
// This also calculates the length, direction and extrusion point.
// This pulls together all the possible routes for setting the extrusion vector.
// The default length is 5 * hole radius and the default direction is the hole normal, if they have not been set.
void vtkMEDExtrudeToCircle::CalculateExtrusionVector()
//------------------------------------------------------------------------------
{
  if (m_definedVector){
    // calculate length, direction and extrusion point from vector
    if (!m_definedLength){
      // back calculate length
      m_length = GetNorm(m_extrusionVector) ;
      m_definedLength = true ;
    }

    if (!m_definedDirection){
      // back-calculate direction
      Normalize(m_extrusionVector, m_direction) ;
      m_definedDirection = true ;
    }

    if (!m_definedExtrusionPoint){
      // calculate extrusion point
      AddVector(m_holeCentre, m_extrusionVector, m_extrusionPoint) ;
      m_definedExtrusionPoint = true ;
    }
  }
  else if (m_definedExtrusionPoint){
    // calculate vector, length and direction from extrusion point
    SubtractVector(m_extrusionPoint, m_holeCentre, m_extrusionVector) ;
    m_length = GetNorm(m_extrusionVector) ;
    Normalize(m_extrusionVector, m_direction) ;
    m_definedLength = true ;
    m_definedDirection = true ;
    m_definedVector = true ;
  }
  else{
    if (!m_definedLength){
      // if no length defined, set to 5 * hole radius
      m_length = 5.0 * m_holeRadius ;
      m_definedLength = true ;
    }

    if (!m_definedDirection){
      // if no direction defined, set to normal of hole
      Normalize(m_holeNormal, m_direction) ;

      // check if the direction needs to be reversed
      if (m_defaultDirectionSign == -1){
        MultVectorByScalar(-1.0, m_direction, m_direction) ;
      }

      m_definedDirection = true ;
    }

    // calculate vector from length and direction
    MultVectorByScalar(m_length, m_direction, m_extrusionVector) ;
    m_definedVector = true ;

    // calculate extrusion point
    AddVector(m_holeCentre, m_extrusionVector, m_extrusionPoint) ;
    m_definedExtrusionPoint = true ;
  }
}



//------------------------------------------------------------------------------
// Calculate Centre, normal and radius of hole, and no. of vertices
// The normal is normalised.
void vtkMEDExtrudeToCircle::CalcHoleParameters(vtkPolyData *hole)
//------------------------------------------------------------------------------
{
  int i, j ;
  double sumsq, x[3] ;
  vtkPoints *pts = hole->GetPoints() ;
  int numPts = m_input->GetNumberOfPoints() ;

  //----------------------------------------------------------------------------
  // get the centre
  //----------------------------------------------------------------------------

  // get the mean centre
  SetVector(m_holeCentre, 0, 0, 0) ;
  for (i = 0 ;  i < numPts ; i++){
    pts->GetPoint(i, x) ;
    AddVector(m_holeCentre, x, m_holeCentre) ;
  }
  m_holeCentre[0] /= (double)numPts ;
  m_holeCentre[1] /= (double)numPts ;
  m_holeCentre[2] /= (double)numPts ;


  //----------------------------------------------------------------------------
  // get the radius
  //----------------------------------------------------------------------------

  double dx[3] ;
  for (i = 0, sumsq = 0.0 ;  i < numPts ; i++){
    // get point relative to hole centre
    pts->GetPoint(i, x) ;
    SubtractVector(x, m_holeCentre, dx) ;

    // add to sum squared radius
    for (j = 0 ;  j < 3 ;  j++)
      sumsq += dx[j] * dx[j] ;
  }

  // get the rms radius
  sumsq /= (double)numPts ;
  m_holeRadius = sqrt(sumsq) ;


  //----------------------------------------------------------------------------
  // get the normal
  //----------------------------------------------------------------------------

  // Find the normal to the plane of the hole.
  // In this example data, the polydata circulates around the hole centre c, such that for each polyline cell,
  // with endpoints p0 and p1, the cross product (p1-c)^(p0-c) points in the direction of the outward normal.
  // Therefore we use the median cross product to define the normal.
  int numPtsInCell ;
  vtkIdType *ptlist ;
  double pt0[3], pt1[3] ;
  vtkMEDPastValuesList vecProd0(1000), vecProd1(1000), vecProd2(1000) ;

  // Build cell links if not already done
  if (!m_builtCells){
    m_input->BuildCells() ;
    m_builtCells = true ;
  }

  for (int i = 0 ;  i < m_input->GetNumberOfCells() ;  i++){
    m_input->GetCellPoints(i, numPtsInCell, ptlist) ;
    if (numPtsInCell == 2){
      m_input->GetPoint(ptlist[0], pt0) ;
      m_input->GetPoint(ptlist[1], pt1) ;
      SubtractVector(pt0, m_holeCentre, pt0) ;
      SubtractVector(pt1, m_holeCentre, pt1) ;

      // get cross product and add to list
      double cp[3] ;
      CrossProduct(pt1, pt0, cp) ;
      vecProd0.AddNewValue(cp[0]) ;
      vecProd1.AddNewValue(cp[1]) ;
      vecProd2.AddNewValue(cp[2]) ;
    }
  }

  // set normal to median cross product
  m_holeNormal[0] = vecProd0.GetMedian() ;
  m_holeNormal[1] = vecProd1.GetMedian() ;
  m_holeNormal[2] = vecProd2.GetMedian() ;
  Normalize(m_holeNormal, m_holeNormal) ;



  //----------------------------------------------------------------------------
  // get the number of points around the hole
  //----------------------------------------------------------------------------
  m_holeNumVerts = pts->GetNumberOfPoints() ;
}




//------------------------------------------------------------------------------
// Calculate "up" vector which defines phi = 0 in cylindrical coords
// It is the direction from the centre of the hole to the first vertex.
// This is not necessarily normal to the cylinder axis.
void vtkMEDExtrudeToCircle::CalcUpVector(vtkIdList *holepts)
//------------------------------------------------------------------------------
{
  double x[3] ;
  int ptId ;
  ptId = holepts->GetId(0) ;
  m_input->GetPoint(ptId, x) ;
  SubtractVector(x, m_holeCentre, m_upVector) ;
}



//------------------------------------------------------------------------------
// Calculate no. of rings required on extrusion, their positions and no. of points in each
// Needs length of extrusion and corrected end radius
// Allocates memory for the rings and the vertices in the mesh structure.
void vtkMEDExtrudeToCircle::CalcExtrusionRings()
//------------------------------------------------------------------------------
{
  int i ;


  // calculate no. of end points from requested min. value
  // default is the same as the no. of vertices on the hole.
  if (!m_definedMinNumEndPts || (m_minNumEndPts < m_holeNumVerts)){
    // set min no. of end vertices to default
    m_minNumEndPts = m_holeNumVerts ;
    m_definedMinNumEndPts = true ;
  }
  m_mesh->CalcNumberOfEndPoints(m_holeNumVerts, m_minNumEndPts) ;



  // calculate distance between vertices at start end = 2pi*r / no. of pts
  double triangle_size = 2*M_PI * m_endRadius / (double)m_holeNumVerts ;


  // calculate total length of doubling sections
  double lsec = triangle_size ;
  double lDoubles = 0.0 ;
  for (i = 0 ;  i < m_mesh->numDoublings ;  i++){
    lsec /= 2.0 ;
    lDoubles += lsec ;
  }


  // calculate no. of singles sections
  if (lDoubles < m_length){
    // not enough length from doubling sections - how many singles sections do we need to make it up
    // this will give a length less than or equal to the required length
    m_mesh->numSingles = (int)((m_length - lDoubles) / triangle_size) ;
  }
  else{
    // no need for any singles sections
    m_mesh->numSingles = 0 ;
  }

  // make sure that we have at least one section !
  if ((m_mesh->numSingles == 0) && (m_mesh->numDoublings == 0))
    m_mesh->numSingles = 1 ;


  // calculate length of singles sections
  double lSingles = (double)m_mesh->numSingles*triangle_size ;

  // adjust triangle size so that sections add up to exact length of tube
  triangle_size *= m_length / (lSingles + lDoubles) ;



  //----------------------------------------------------------------------------
  // set the positions and no. of vertices in each ring
  //----------------------------------------------------------------------------
  m_mesh->AllocateRings(m_mesh->numSingles + m_mesh->numDoublings + 1) ;

  // first ring is same as start hole
  m_mesh->ring[0].AllocateVertices(m_holeNumVerts) ;
  m_mesh->ring[0].z = 0.0 ;

  // rings which do not double no. of vertices
  double z = 0.0 ;
  for (i = 1 ;  i <= m_mesh->numSingles ;  i++){
    z += triangle_size ;
    m_mesh->ring[i].AllocateVertices(m_holeNumVerts) ;
    m_mesh->ring[i].z = z ;
  }

  // doubling rings
  lsec = triangle_size ;
  int verts = m_holeNumVerts ;
  for (i = m_mesh->numSingles+1 ;  i < m_mesh->numRings ;  i++){
    lsec /= 2.0 ;
    verts *= 2.0 ;
    z += lsec ;
    m_mesh->ring[i].AllocateVertices(verts) ;
    m_mesh->ring[i].z = z ;
  }
}





//------------------------------------------------------------------------------
// Calculate cylindrical and cartesian coords of each mesh vertex
void vtkMEDExtrudeToCircle::CalcExtrusionVertices()
//------------------------------------------------------------------------------
{
  int i, j ;

  // get indices of points around hole, in correct order
  vtkIdList *holepts = vtkIdList::New() ;
  GetPointsAroundHole(m_input, holepts) ;

  // Calculate the up vector for the cylindrical coord system
  CalcUpVector(holepts) ;

  // Reverse order of points if the list of points contains decreasing phi
  int sense = CalcSenseOfPointsAroundHole(holepts, m_holeCentre, m_extrusionVector, m_upVector) ;
  if (sense < 0){
    ReverseIdList(holepts) ;
    CircularShiftIdList(holepts, 1) ; // rotate list to put zero phi back at first entry
  }

  // Calculate the up vector for the cylindrical coord system
  CalcUpVector(holepts) ;

  // get the cylindrical coords of the hole points and copy them to the first ring
  double r, phi, z ;
  int ptId ;
  double x[3] ;
  for (j = 0 ;  j < holepts->GetNumberOfIds() ;  j++){
    ptId = holepts->GetId(j) ;
    m_input->GetPoint(ptId, x) ;  // cartesian coords of hole point
    CalcCylinderCoords(x, m_holeCentre, m_extrusionVector, m_upVector, &r, &phi, &z) ;
    m_mesh->ring[0].vertex[j].SetCylCoords(r, phi, z) ;
  }

  // Remove unwanted jumps of 2pi from the data
  Remove2PiArtefactsFromFirstRing() ;

  // calculate the cylindrical coords of the end ring, which is of course a 
  // perfect circle, normal to the z axis
  int iend = m_mesh->numRings - 1 ;
  double dphi = 2*M_PI / (double)m_mesh->ring[iend].numVerts ;
  for (j = 0 ;  j < m_mesh->ring[iend].numVerts ;  j++){
    r = m_endRadius ;
    phi = (double)j * dphi ;
    z = m_mesh->ring[iend].z ;
    m_mesh->ring[iend].vertex[j].SetCylCoords(r, phi, z) ;
  }

  // Tricky bit: now we have to interpolate the vertices on the middle rings
  int factor ;
  for (i = 1 ;  i < m_mesh->numRings-1 ;  i++){
    for (j = 0 ;  j < m_mesh->ring[i].numVerts ;  j++){
      // find the point on the end ring which corresponds to point j
      factor = m_mesh->ring[iend].numVerts / m_mesh->ring[i].numVerts ;
      int jend = j * factor ;

      // find the points on the start ring which correspond to point j
      // and the relative weighting for each
      factor = m_mesh->ring[i].numVerts / m_mesh->ring[0].numVerts ;
      double jdouble = (double)j / (double)factor ;
      int jstart0 = (int)jdouble ;
      int jstart1 = (jstart0 + 1) % m_mesh->ring[0].numVerts ;
      double rmdr = jdouble - jstart0 ;
      double w0 = 1.0 - rmdr ;
      double w1 = rmdr ;

      // get the total weights for the three points
      double lambda = m_mesh->ring[i].z / m_mesh->ring[iend].z ;
      double w_end = lambda ;
      double w_start0 = (1.0 - lambda)*w0 ;
      double w_start1 = (1.0 - lambda)*w1 ;

      // get the cylindrical coords of the three points
      double r_start0, phi_start0, z_start0 ;
      double r_start1, phi_start1, z_start1 ;
      double r_end, phi_end, z_end ;
      m_mesh->ring[0].vertex[jstart0].GetCylCoords(&r_start0, &phi_start0, &z_start0) ;
      m_mesh->ring[0].vertex[jstart1].GetCylCoords(&r_start1, &phi_start1, &z_start1) ;
      m_mesh->ring[iend].vertex[jend].GetCylCoords(&r_end, &phi_end, &z_end) ;
      if (jstart1 == 0){
        // special case: deal with phi wrapping round from 2pi to zero
        phi_start1 += + 2.0*M_PI ;
      }

      // interpolate the three points
      r = w_start0*r_start0 + w_start1*r_start1 + w_end*r_end ;
      phi = w_start0*phi_start0 + w_start1*phi_start1 + w_end*phi_end ;
      z = w_start0*z_start0 + w_start1*z_start1 + w_end*z_end ;
      m_mesh->ring[i].vertex[j].SetCylCoords(r, phi, z) ;    
    }
  }

  // finally calculate the cartesian coords of each vertex
  for (i = 0 ;  i < m_mesh->numRings ;  i++){
    for (j = 0 ;  j < m_mesh->ring[i].numVerts ;  j++){
      m_mesh->ring[i].vertex[j].GetCylCoords(&r, &phi, &z) ;
      CalcCartesianCoords(r, phi, z, m_holeCentre, m_extrusionVector, m_upVector, x) ;
      m_mesh->ring[i].vertex[j].SetCartCoords(x) ;
    }
  }

  holepts->Delete() ;
}




//------------------------------------------------------------------------------
// Transfer vertices to vtkPoints
// This allocates memory for points.
void vtkMEDExtrudeToCircle::VerticesToVtkPoints(vtkPoints *points) const
//------------------------------------------------------------------------------
{
  int i, j, k, totalpts ;

  // pre-allocate memory for total no. of vertices
  for (i = 0, totalpts = 0 ;  i < m_mesh->numRings ;  i++)
    totalpts += m_mesh->ring[i].numVerts ;
  points->Allocate(totalpts) ;       // allocate space for n points (ie 3 * n floats)

  for (i = 0, k = 0 ;  i < m_mesh->numRings ;  i++){
    for (j = 0 ;  j < m_mesh->ring[i].numVerts ;  j++){
      // insert vertex into points array
      double x[3] ;
      m_mesh->ring[i].vertex[j].GetCartCoords(x) ;
      points->InsertPoint(k, x) ; 

      // make a note of the polydata index so we can find it again
      m_mesh->ring[i].vertex[j].SetId(k++) ;     
    }
  }

  points->Squeeze() ;
}



//------------------------------------------------------------------------------
/*
Sort vertices into vtk triangles
This allocates memory for triangles

The triangles are arranged like this:

 --------------------------------> z
|
|      j = 0 --- 0 --- 0 - 0
|          |   / |   / | \ |
|          |  /  |  /  |   1
|          | /   | /   | / |
|          1 --- 1 --- 1 - 2
|          |   / |   / | \ |
|          |  /  |  /  |   3
|          | /   | /   | / |
|          2 --- 2 --- 2 - 4
|          |   / |   / | \ |
|          |  /  |  /  |   5
|          | /   | /   | / |
|          3 --- 3 --- 3 - 6
|          |   / |   / | \ |
|          |  /  |  /  |   7
|          | /   | /   | / |
|          0 --- 0 --- 0 - 0
|
V
phi

*/
void vtkMEDExtrudeToCircle::VerticesToVtkTriangles(vtkCellArray *triangles) const
//------------------------------------------------------------------------------
{
  int i, j, totalpts ;
  vtkIdType thisTriangle[3] ;

  // pre-allocate memory for total no. of vertices
  for (i = 0, totalpts = 0 ;  i < m_mesh->numRings ;  i++)
    totalpts += m_mesh->ring[i].numVerts ;
  triangles->Allocate(7*totalpts) ;       // allocate space for indices (each vertex appears in no more than 7 triangles)

  for (i = 1 ;  i < m_mesh->numRings ;  i++){
    int nthis = m_mesh->ring[i].numVerts ;      // no. vertices in this ring
    int nprev = m_mesh->ring[i-1].numVerts ;    // no. vertices in previous ring

    if (nthis == nprev){
      // calc triangles between ring i and i-1 where no. of vertices is the same
      vtkIdType id0, id1, id2, id3 ;
      for (j = 0 ;  j < m_mesh->ring[i].numVerts ;  j++){
        id0 = m_mesh->ring[i-1].vertex[j].GetId() ;
        id1 = m_mesh->ring[i-1].vertex[(j+1) % nprev].GetId() ;
        id2 = m_mesh->ring[i].vertex[(j+1) % nthis].GetId() ;
        id3 = m_mesh->ring[i].vertex[j].GetId() ;


        // triangle 0, 1, 3
        thisTriangle[0] = id0 ; thisTriangle[1] = id1 ; thisTriangle[2] = id3 ;
        triangles->InsertNextCell(3, thisTriangle) ;

        // triangle 1, 2, 3
        thisTriangle[0] = id1 ; thisTriangle[1] = id2 ; thisTriangle[2] = id3 ;
        triangles->InsertNextCell(3, thisTriangle) ;
      }
    }
    else if (nthis == 2*nprev){
      // calc triangles between ring i and i-1 where no. of vertices is doubled
      vtkIdType id0, id1, id2, id3, id4 ;
      for (j = 0 ;  j < m_mesh->ring[i].numVerts ;  j+=2){
        id0 = m_mesh->ring[i-1].vertex[j/2].GetId() ;
        id1 = m_mesh->ring[i-1].vertex[(j/2+1) % nprev].GetId() ;
        id2 = m_mesh->ring[i].vertex[(j+2) % nthis].GetId() ;
        id3 = m_mesh->ring[i].vertex[j+1].GetId() ;
        id4 = m_mesh->ring[i].vertex[j].GetId() ;

        // triangle 0, 3, 4
        thisTriangle[0] = id0 ; thisTriangle[1] = id3 ; thisTriangle[2] = id4 ;
        triangles->InsertNextCell(3, thisTriangle) ;

        // triangle 0, 1, 3
        thisTriangle[0] = id0 ; thisTriangle[1] = id1 ; thisTriangle[2] = id3 ;
        triangles->InsertNextCell(3, thisTriangle) ;

        // triangle 1, 2, 3
        thisTriangle[0] = id1 ; thisTriangle[1] = id2 ; thisTriangle[2] = id3 ;
        triangles->InsertNextCell(3, thisTriangle) ;
      }
    }
    else{
      // something badly wrong if you get here
      std::cout << "error in vtkMEDExtrudeToCircle::VerticesToVtkTriangles()" ;
      assert(false) ;
    }
  }

  triangles->Squeeze() ;
}


//------------------------------------------------------------------------------
// Get cylindrical coords (r, phi, z) of point x.
// The cylinder system is defined by the central axis and an "up" vector, which defines phi = 0
// cylAxis and upVector do not have to be normalised.
// upVector does not have to be exactly normal to cylAxis.
// phi is in the range 0 to 2pi.
void vtkMEDExtrudeToCircle::CalcCylinderCoords(const double *x, const double *centre, const double *cylAxis, const double *upVector, 
                                         double *r, double *phi, double *z) const 
//------------------------------------------------------------------------------
{
  // Get normalised central axis
  double u[3], v[3], w[3] ;
  Normalize(cylAxis, w) ;

  // Get orthonormal up vector: u = upvec - (upvec.w)w
  double dp = DotProduct(upVector, w) ;
  u[0] = upVector[0] - dp*w[0] ;
  u[1] = upVector[1] - dp*w[1] ;
  u[2] = upVector[2] - dp*w[2] ;
  Normalize(u, u) ;

  // make sure u has same sense as input up vector
  dp = DotProduct(u, upVector) ;
  if (dp < 0)
    MultVectorByScalar(-1.0, u, u) ;

  // Get third axis v = w^u
  CrossProduct(w,u,v) ;

  // Get x vector relative to centre
  double dx[3] ;
  SetVector(dx, x[0]-centre[0], x[1]-centre[1], x[2]-centre[2]) ;

  // Get cartesian coords in cylinder system
  double ucoord = DotProduct(dx, u) ;
  double vcoord = DotProduct(dx, v) ;
  double wcoord = DotProduct(dx, w) ;

  // calculate cylindrical coords
  *z = wcoord ;
  *r = sqrt(ucoord*ucoord + vcoord*vcoord) ;
  *phi = atan2(vcoord, ucoord) ;

  // put phi into range 0 to 2pi
  if (*phi < 0.0)
    *phi += 2*M_PI ;

}




//------------------------------------------------------------------------------
// Get cartesian coords x given cylindrical coords (r, phi, z)
// The cylinder system is defined by the central axis and an "up" vector.
// cylAxis and upVector do not have to be normalised.
// upVector does not have to be exactly normal to cylAxis.
void vtkMEDExtrudeToCircle::CalcCartesianCoords(double r, double phi, double z, const double *centre, const double *cylAxis, const double *upVector, double *x) const 
//------------------------------------------------------------------------------
{
  // Get normalised central axis
  double u[3], v[3], w[3] ;
  Normalize(cylAxis, w) ;

  // Get orthonormal up vector: u = upvec - (upvec.w)w
  double dp = DotProduct(upVector, w) ;
  u[0] = upVector[0] - dp*w[0] ;
  u[1] = upVector[1] - dp*w[1] ;
  u[2] = upVector[2] - dp*w[2] ;
  Normalize(u, u) ;

  // make sure u has same sense as input up vector
  dp = DotProduct(u, upVector) ;
  if (dp < 0)
    MultVectorByScalar(-1.0, u, u) ;

  // Get third axis v = w^u
  CrossProduct(w,u,v) ;

  // convert cylinder coords to cartesian coords (ucoord,vcoord,wcoord) in cylinder frame
  double ucoord = r*cos(phi) ;
  double vcoord = r*sin(phi) ;
  double wcoord = z ;

  // Get x vector relative to centre
  double dx[3], ucoords[3] ;
  ucoords[0] = ucoord ;
  ucoords[1] = vcoord ;
  ucoords[2] = wcoord ;
  InverseSolveTransform(u, v, w, ucoords, dx) ;

  // add centre to get world x
  SetVector(x, dx[0]+centre[0], dx[1]+centre[1], dx[2]+centre[2]) ;
}


//------------------------------------------------------------------------------
// Get matrix to rotate arrow (x axis) to vector direction u
void vtkMEDExtrudeToCircle::GetMatRotArrowToAxis(vtkMatrix4x4 *mat, const double *u) const 
//------------------------------------------------------------------------------
{
  double xnew[3], ynew[3], znew[3] ;

  // calculate new x axis to be normal along u
  Normalize(u, xnew) ;


  // calculate new y and z axes normal to u
  double a[3], b[3], c[3] ; // 3 possible vectors, at least 2 of which must be non-zero
  SetVector(a, u[2], 0.0, -u[0]) ;
  SetVector(b, 0.0, u[2], -u[1]) ;
  SetVector(c, -u[1], u[0], 0.0) ;

  // y = weighted sum of vectors
  double anorm = GetNorm(a) ;
  double bnorm = GetNorm(b) ;
  double cnorm = GetNorm(c) ;
  for (int i = 0 ;  i < 3 ;  i++)
    ynew[i] = anorm*a[i] + bnorm*b[i] + cnorm*c[i] ;
  Normalize(ynew, ynew) ;

  // point ynew in direction of y if possible
  if (ynew[1] < 0){
    ynew[0] *= -1.0 ;
    ynew[1] *= -1.0 ;
    ynew[2] *= -1.0 ;
  }


  // calculate new z axis z = x ^ y
  CrossProduct(xnew, ynew, znew) ;
  Normalize(znew, znew) ;


  // The rotation matrix should be R = xnew0 ynew0 znew0
  //                                   xnew1 ynew1 znew1
  //                                   xnew2 ynew2 znew2
  mat->Identity() ;
  for (int i = 0 ;  i < 3 ;  i++){
    mat->SetElement(i,0,xnew[i]) ;
    mat->SetElement(i,1,ynew[i]) ;
    mat->SetElement(i,2,znew[i]) ;
  }
}


//------------------------------------------------------------------------------
// List points around hole in order
void vtkMEDExtrudeToCircle::GetPointsAroundHole(vtkPolyData *hole, vtkIdList *pts) const 
//------------------------------------------------------------------------------
{
  int i ;
  vtkIdType numPtsInCell ;
  unsigned short numCellsOnPt ;
  vtkIdType *ptlist ;
  vtkIdType *cellList ;

  // check that all the cells have two points
  hole->BuildCells() ;
  hole->BuildLinks() ;
  for (i = 0 ;  i < hole->GetNumberOfCells() ;  i++){
    hole->GetCellPoints(i, numPtsInCell, ptlist) ;
    if (numPtsInCell != 2){
      std::cout << "cell " << i << " found with " << numPtsInCell << " pts" << std::endl ;
      assert(false) ;
    }
  }

  // check that all the points have two cells
  for (i = 0 ;  i < hole->GetNumberOfPoints() ;  i++){
    hole->GetPointCells(i, numCellsOnPt, cellList) ;
    if (numCellsOnPt != 2){
      std::cout << "point " << i << " found with " << numCellsOnPt << " cells" << std::endl ;
      assert(false) ;
    }
  }

  // trace points in order around the hole
  int icell = 0 ;
  int lastPt = -1 ;
  bool istop = false ;
  int ilo ;
  int ihi ;
  pts->Initialize() ;
  while (pts->GetNumberOfIds() < hole->GetNumberOfPoints() && !istop){
    // get points in icell
    hole->GetCellPoints(icell, numPtsInCell, ptlist) ;

    if (lastPt == -1){
      // first cell so put both points in list
      ilo = 0 ;
      ihi = 1 ;
      pts->InsertNextId(ptlist[ilo]) ;
      pts->InsertNextId(ptlist[ihi]) ;
      lastPt = ptlist[ihi] ;
    }
    else{
      // set ihi such that ptlist[ilo] is the previous point and ptlist[ihi] is the new point
      if ((ptlist[0] == lastPt) && (ptlist[1] != lastPt)){
        ilo = 0 ;
        ihi = 1 ;
      }
      else if ((ptlist[1] == lastPt) && (ptlist[0] != lastPt)){
        ilo = 1 ;
        ihi = 0 ;
      }
      else{
        std::cout << "problem with pt indices in GetPointsAroundHole()" << std::endl ;
        assert(false) ;
      }

      if (ptlist[ihi] != pts->GetId(0)){
        // add point to list
        pts->InsertNextId(ptlist[ihi]) ;
        lastPt = ptlist[ihi] ;
      }
      else{
        // stop when we get back to the first point
        istop = true ;
      }
    }


    // get cells attached to new point
    hole->GetPointCells(ptlist[ihi], numCellsOnPt, cellList) ;

    // set next cell to the one which is not the current cell
    if ((cellList[0] == icell) && (cellList[1] != icell))
      icell = cellList[1] ;
    else if ((cellList[1] == icell) && (cellList[0] != icell))
      icell = cellList[0] ;
    else{
      std::cout << "problem with cell indices in GetPointsAroundHole()" << std::endl ;
      assert(false) ;
    }
  }
}


//------------------------------------------------------------------------------
// Checks sense of points as calculated by GetPointsAroundHole()
// Returns 1 if phi increases with index
// Returns -1 if phi decreases with index
int vtkMEDExtrudeToCircle::CalcSenseOfPointsAroundHole(vtkIdList *holepts, const double *centre, const double *cylAxis, const double *upVector) const
//------------------------------------------------------------------------------
{
  int j ;
  double r, phi, z ;
  double *x ;

  int n ;
  double mplus, mneg, Eplus, Eneg ;
  n = holepts->GetNumberOfIds() ;

  // We define two models of the line phi = m*j = c
  // with m = +/- 2pi/n 
  // and c = 0
  mplus = 2.0*M_PI / (double)n ;
  mneg = -mplus ;

  for (j = 0, Eplus = 0.0, Eneg = 0.0 ;  j < n ;  j++){
    vtkIdType ptId = holepts->GetId(j) ;
    x = m_input->GetPoint(ptId) ;  // cartesian coords of hole point
    CalcCylinderCoords(x, m_holeCentre, m_extrusionVector, m_upVector, &r, &phi, &z) ;

    // Get calculated phi for each line
    double phicalc_plus = mplus*(double)j ;
    double phicalc_neg = mneg*(double)j ;

    // Make sure the calculated phi is in the range 0-2pi, the same as the input data.
    // NB if we got the slope of the line by least squares, it would be hard to account for this.
    if (phicalc_neg < 0.0)
      phicalc_neg += 2.0*M_PI ;

    // calculate deviations from lines
    double dphi_plus = phi - phicalc_plus ;
    double dphi_neg = phi - phicalc_neg ;

    // add to sum squared deviations for each line
    Eplus += dphi_plus*dphi_plus ;
    Eneg += dphi_neg*dphi_neg ;
  }

  if (Eplus < Eneg)
    return 1 ;
  else
    return -1 ;

}



//------------------------------------------------------------------------------
// This removes wrap around jumps of 2pi from phi in the first ring
void vtkMEDExtrudeToCircle::Remove2PiArtefactsFromFirstRing()
//------------------------------------------------------------------------------
{
  int j ;
  int n = m_mesh->ring[0].numVerts ;

  // slope of model line phi = m*j + c where c = 0
  double m = 2.0*M_PI / (double)n ;

  for (j = 0 ;  j < n ;  j++){
    double phi = m_mesh->ring[0].vertex[j].GetCylPhi() ;
    double phicalc = m * (double)j ;
    double dphi = phi - phicalc ;

    // add or subtract 2pi to improve value
    if (dphi > M_PI)
      phi -= 2.0*M_PI ;
    else if (dphi < -M_PI)
      phi += 2.0*M_PI ;

    m_mesh->ring[0].vertex[j].SetCylPhi(phi) ;
  }
}


 
//------------------------------------------------------------------------------
// Reverse id list
void vtkMEDExtrudeToCircle::ReverseIdList(vtkIdList *pts) const
//------------------------------------------------------------------------------
{
  int j, jj ;
  int n = pts->GetNumberOfIds() ;

  for (j = 0, jj = n-1 ;  j < n/2 ;  j++, jj--){
    vtkIdType id1 = pts->GetId(j) ;
    vtkIdType id2 = pts->GetId(jj) ;
    pts->InsertId(j, id2) ;
    pts->InsertId(jj, id1) ;
  }
}


//------------------------------------------------------------------------------
// Circular shift id list
void vtkMEDExtrudeToCircle::CircularShiftIdList(vtkIdList *pts, int shift) const
//------------------------------------------------------------------------------
{
  int j, jj ;
  int n = pts->GetNumberOfIds() ;

  // copy to temp list
  vtkIdList *temp = vtkIdList::New() ;
  temp->Initialize() ;
  for (j = 0 ;  j < n ;  j++)
    temp->InsertNextId(pts->GetId(j)) ;

  for (j = 0 ;  j < n ;  j++){
    jj = Modulo(j-shift, n) ;
    pts->InsertId(j, temp->GetId(jj)) ;
  }

  temp->Delete() ;
}


//------------------------------------------------------------------------------
// Modulo operator, same as % but works correctly on negative values of n as well
inline int vtkMEDExtrudeToCircle::Modulo(int n, int m) const
//------------------------------------------------------------------------------
{
  if (n >= 0)
    return n % m ;
  else
    return m - (-n % m) ;
}


//------------------------------------------------------------------------------
// Copy vector
void vtkMEDExtrudeToCircle::CopyVector(const double *a, double *b) const
//------------------------------------------------------------------------------
{
  b[0] = a[0] ;
  b[1] = a[1] ;
  b[2] = a[2] ;
}


//------------------------------------------------------------------------------
// Set vector
void vtkMEDExtrudeToCircle::SetVector(double *vec, double x, double y, double z) const
//------------------------------------------------------------------------------
{
  vec[0] = x ;
  vec[1] = y ;
  vec[2] = z ;
}

//------------------------------------------------------------------------------
// Add vector
void vtkMEDExtrudeToCircle::AddVector(const double *vin, const double *x, double *vout) const
//------------------------------------------------------------------------------
{
  vout[0] = vin[0] + x[0] ;
  vout[1] = vin[1] + x[1] ;
  vout[2] = vin[2] + x[2] ;

}


//------------------------------------------------------------------------------
// Subtract vector
void vtkMEDExtrudeToCircle::SubtractVector(const double *vin, const double *x, double *vout) const
//------------------------------------------------------------------------------
{
  vout[0] = vin[0] - x[0] ;
  vout[1] = vin[1] - x[1] ;
  vout[2] = vin[2] - x[2] ;

}

//------------------------------------------------------------------------------
// Multiply vector by scalar
void vtkMEDExtrudeToCircle::MultVectorByScalar(double s, const double *vin, double *vout) const
//------------------------------------------------------------------------------
{
  vout[0] = s*vin[0] ;
  vout[1] = s*vin[1] ;
  vout[2] = s*vin[2] ;
}



//------------------------------------------------------------------------------
// Get norm of vector
double vtkMEDExtrudeToCircle::GetNorm(const double *vec) const
//------------------------------------------------------------------------------
{
  return sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]) ;
}

//------------------------------------------------------------------------------
// Normalize vector
void vtkMEDExtrudeToCircle::Normalize(const double *vin, double *vout) const
//------------------------------------------------------------------------------
{
  double norm = GetNorm(vin) ;
  if (norm == 0.0){
    std::cout << "vector has zero magnitude" << std::endl ;
    assert(false) ;
  }

  vout[0] = vin[0] / norm ;
  vout[1] = vin[1] / norm ;
  vout[2] = vin[2] / norm ;
}

//------------------------------------------------------------------------------
// Cross product of vectors a ^ b = c
void vtkMEDExtrudeToCircle::CrossProduct(const double *a, const double *b, double *c) const
//------------------------------------------------------------------------------
{
  c[0] =   a[1]*b[2] - a[2]*b[1] ;
  c[1] = -(a[0]*b[2] - a[2]*b[0]) ;
  c[2] =   a[0]*b[1] - a[1]*b[0] ;
}


//------------------------------------------------------------------------------
// Dot product of vectors a.b
double vtkMEDExtrudeToCircle::DotProduct(const double *a, const double *b) const
//------------------------------------------------------------------------------
{
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] ;
}


//------------------------------------------------------------------------------
// Inverse solution of equation y = Mx, where M = row vectors (u, v and w)
// Find x given M and y
void vtkMEDExtrudeToCircle::InverseSolveTransform(const double *u, const double *v, const double *w, const double *y, double *x) const
//------------------------------------------------------------------------------
{
  double det, det0, det1, det2 ;

  det = u[0]*(v[1]*w[2] - v[2]*w[1]) - u[1]*(v[0]*w[2] - v[2]*w[0]) + u[2]*(v[0]*w[1] - v[1]*w[0]) ;
  det0 = y[0]*(v[1]*w[2] - v[2]*w[1]) - u[1]*(y[1]*w[2] - v[2]*y[2]) + u[2]*(y[1]*w[1] - v[1]*y[2]) ;
  det1 = u[0]*(y[1]*w[2] - v[2]*y[2]) - y[0]*(v[0]*w[2] - v[2]*w[0]) + u[2]*(v[0]*y[2] - y[1]*w[0]) ;
  det2 = u[0]*(v[1]*y[2] - y[1]*w[1]) - u[1]*(v[0]*y[2] - y[1]*w[0]) + y[0]*(v[0]*w[1] - v[1]*w[0]) ;

  x[0] = det0 / det ;
  x[1] = det1 / det ;
  x[2] = det2 / det ;
}


//------------------------------------------------------------------------------
// Print self
void vtkMEDExtrudeToCircle::PrintSelf(ostream& os, vtkIndent indent) const
//------------------------------------------------------------------------------
{
  double holeCentre[3], holeNormal[3] ;

  GetHoleCentre(holeCentre) ;
  GetHoleNormal(holeNormal) ;

  os << indent << "hole parameters..." << std::endl ;
  os << indent << "centre: " << holeCentre[0] << " " << holeCentre[1] << " " << holeCentre[2] << std::endl ;
  os << indent << "normal: " << holeNormal[0] << " " << holeNormal[1] << " " << holeNormal[2] << std::endl ;
  os << indent << "radius: " << GetHoleRadius() << std::endl ;
  os << indent << "no. pts" << GetHoleNumVerts() << std::endl ;
  os << std::endl ;


  double dir[3], vec[3], extrPt[3] ;

  GetDirection(dir) ;
  GetExtrusionVector(vec) ;
  GetExtrusionPoint(extrPt) ;

  os << indent << "extrusion parameters..." << std::endl ;
  os << indent << "direction: " << dir[0] << " " << dir[1] << " " << dir[2] << std::endl ;
  os << indent << "vector: " << vec[0] << " " << vec[1] << " " << vec[2] << std::endl ;
  os << indent << "extrusion point: " << extrPt[0] << " " << extrPt[1] << " " << extrPt[2] << std::endl ;
  os << indent << "length: " << GetLength() << std::endl ;
  os << indent << "end radius: " << GetEndRadius() << std::endl ;
  os << std::endl ;

  os << indent << "mesh structure..." << std::endl ;
  os << indent << "requested end pts: " << m_minNumEndPts << std::endl ; 
  m_mesh->PrintSelf(os, indent) ;
  os << std::endl ;
}



//------------------------------------------------------------------------------
// Print self
void vtkMEDExtrudeToCircle::MeshData::PrintSelf(ostream& os, vtkIndent indent) const
//------------------------------------------------------------------------------
{
  os << indent << "no. rings = " << numRings << std::endl ;

  os << indent << "no. endpts = " << numEndPts << "\t" 
    << "no. doublings = " << numDoublings << "\t" 
    << "no. singles = " << numSingles << std::endl ;
  os << std::endl ;

  for (int i = 0 ;  i < numRings ;  i++){
    os << indent << "ring " << i << "\t" ;
    ring[i].PrintSelf(os, 0) ;
    os << std::endl ;
  }
}


//------------------------------------------------------------------------------
// Print self
void vtkMEDExtrudeToCircle::RingData::PrintSelf(ostream& os, vtkIndent indent) const
//------------------------------------------------------------------------------
{
  os << indent << "no. vertices = " << numVerts << "\t" << "z = " << z << std::endl ;
  for (int j = 0 ;  j < numVerts ;  j++){
    os << indent << "vertex " << j << "\t" ;
    vertex[j].PrintSelf(os, 0) ;
  }
}


//------------------------------------------------------------------------------
// Print self
void vtkMEDExtrudeToCircle::VertexData::PrintSelf(ostream& os, vtkIndent indent) const
//------------------------------------------------------------------------------
{
  os << indent << "cyl " << m_cylcoord[0] << "\t" << m_cylcoord[1] << "\t" << m_cylcoord[2] << "\t"
    << "cart " << m_cartcoord[0] << "\t" << m_cartcoord[1] << "\t" << m_cartcoord[2] << std::endl ;
}



//------------------------------------------------------------------------------
// Calculate required no. of end vertices, given number on hole
// This is always 2^m no. of hole vertices
// Default is that no. end verts = no. of start verts.
// Sets no. of end points and no. of doublings reqd to get there.
void vtkMEDExtrudeToCircle::MeshData::CalcNumberOfEndPoints(int numHolePts, int minNumEndPts)
//------------------------------------------------------------------------------
{
  numDoublings = 0 ;
  numEndPts = numHolePts ;

  // calculate no. of end points and no. of doublings
  while (numEndPts < minNumEndPts){
    numEndPts *= 2 ;
    numDoublings++ ;
  }
}
