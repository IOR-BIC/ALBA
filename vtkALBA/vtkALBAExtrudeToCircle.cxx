/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAExtrudeToCircle
 Authors: Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkPolyDataAlgorithm.h"
#include "vtkObjectFactory.h"
#include "vtkIdList.h"
#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkALBAPastValuesList.h"
#include "vtkALBAExtrudeToCircle.h"
#include "vtkMath.h"
#include <assert.h>

#ifndef vtkMath::Pi()
  #define _USE_MATH_DEFINES
#endif

#include <cmath>
#include "vtkInformation.h"
#include "vtkInformationVector.h"

//------------------------------------------------------------------------------
// standard macros
vtkStandardNewMacro(vtkALBAExtrudeToCircle);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Constructor
vtkALBAExtrudeToCircle::vtkALBAExtrudeToCircle() : m_DefinedLength(false), m_DefinedDirection(false), m_DefinedVector(false),
m_DefinedExtrusionPoint(false), m_DefinedMinNumEndPts(false), m_BuiltCells(false),
m_Mesh(NULL), m_DefaultDirectionSign(1)
//------------------------------------------------------------------------------
{
}


//------------------------------------------------------------------------------
// Destructor
vtkALBAExtrudeToCircle::~vtkALBAExtrudeToCircle()
//------------------------------------------------------------------------------
{  
  if (m_Mesh != NULL)
    delete m_Mesh ;
}



//------------------------------------------------------------------------------
// Initialize the filter
void vtkALBAExtrudeToCircle::Initialize()
//------------------------------------------------------------------------------
{
  if (m_Mesh != NULL){
    delete m_Mesh ;
    m_Mesh = NULL ;
  }

  m_Output->Initialize() ;
}




//------------------------------------------------------------------------------
int vtkALBAExtrudeToCircle::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//------------------------------------------------------------------------------
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkPolyData  *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkDebugMacro(<< "Executing ExtrudeToCircle Filter") ;

  // pointers to input and output
  m_Input = input ;
  m_Output = output ;

  // Make sure the filter is cleared of previous data before you run it !
  Initialize() ;



  //----------------------------------------------------------------------------
  // Calculate parameters of input hole and extrusion
  //----------------------------------------------------------------------------

  // Calculate the parameters of the input hole (centre, normal and radius)
  CalcHoleParameters(m_Input) ;

  // Calculate the direction, length etc of the extrusion
  CalculateExtrusionVector() ;

  // Calculate radius of end of extrusion.
  // If the extrusion vector is not normal to the hole, need to correct radius
  // because the hole will be an elliptical slice with a greater radius than the vessel.
  // If you slice a circular tube with radius r, at an angle a to the normal,
  // the rms radius of the elliptical slice will be r * sqrt(1+sec^2(a)) / sqrt(2)
  double cosa = DotProduct(m_HoleNormal, m_Direction) ;
  m_EndRadius = m_HoleRadius * sqrt(2.0) * fabs(cosa) / sqrt(1.0 + cosa*cosa) ;


  //----------------------------------------------------------------------------
  // Calculate the mesh structure
  //----------------------------------------------------------------------------
  m_Mesh = new MeshData ;
  CalcExtrusionRings() ;
  CalcExtrusionVertices() ;


  //----------------------------------------------------------------------------
  // Transfer mesh to output polydata
  //----------------------------------------------------------------------------
  vtkPoints *points = vtkPoints::New() ;
  VerticesToVtkPoints(points) ;
  m_Output->SetPoints(points) ;
  points->Delete() ;

  vtkCellArray *triangles = vtkCellArray::New() ;
  VerticesToVtkTriangles(triangles) ;
  m_Output->SetPolys(triangles) ;
  triangles->Delete() ;

	return 1;
}



//------------------------------------------------------------------------------
// Set the length of the extrusion
void vtkALBAExtrudeToCircle::SetLength(double len)
//------------------------------------------------------------------------------
{
  // set the length
  m_Length = len ;
  m_DefinedLength = true ;

  // extrusion vector and extrusion point are no longer defined
  m_DefinedVector = false ;
  m_DefinedExtrusionPoint = false ;

  this->Modified() ;
}



//------------------------------------------------------------------------------
// Set the direction of extrusion.
// The direction does not have to be normalised.
void vtkALBAExtrudeToCircle::SetDirection(const double *direc)       
//------------------------------------------------------------------------------
{
  // copy vector and normalize
  Normalize(direc, m_Direction) ;
  m_DefinedDirection = true ;

  // extrusion vector and extrusion point are no longer defined
  m_DefinedVector = false ;
  m_DefinedExtrusionPoint = false ;

  this->Modified() ;
}


//------------------------------------------------------------------------------
// Set the direction of extrusion.
// The direction does not have to be normalised.
void vtkALBAExtrudeToCircle::SetDirection(double x, double y, double z)       
//------------------------------------------------------------------------------
{
  // copy vector and normalize
  m_Direction[0] = x ;
  m_Direction[1] = y ;
  m_Direction[2] = z ;

  Normalize(m_Direction, m_Direction) ;
  m_DefinedDirection = true ;

  // extrusion vector and extrusion point are no longer defined
  m_DefinedVector = false ;
  m_DefinedExtrusionPoint = false ;

  this->Modified() ;
}


//------------------------------------------------------------------------------
// Set the target point of extrusion.
void vtkALBAExtrudeToCircle::SetExtrusionPoint(const double *extrPoint)
//------------------------------------------------------------------------------
{
  CopyVector(extrPoint, m_ExtrusionPoint) ;
  m_DefinedExtrusionPoint = true ;

  // everything else is undefined until we know the centre of the hole
  m_DefinedLength = false ;
  m_DefinedDirection = false ;
  m_DefinedVector = false ;

  this->Modified() ;
}


//------------------------------------------------------------------------------
// Set the extrusion vector.
void vtkALBAExtrudeToCircle::SetExtrusionVector(const double *extrVector)
//------------------------------------------------------------------------------
{
  CopyVector(extrVector, m_ExtrusionVector) ;
  m_DefinedVector = true ;

  // need to calculate length and direction later
  m_DefinedLength = false ;
  m_DefinedDirection = false ;

  // extrusion point is no longer defined
  m_DefinedExtrusionPoint = false ;

  this->Modified() ;
}


//------------------------------------------------------------------------------
// set min no. of vertices at end of extrusion
void vtkALBAExtrudeToCircle::SetMinNumberofEndPoints(int minNumEndPoints)
//------------------------------------------------------------------------------
{
  m_MinNumEndPts = minNumEndPoints ;
  m_DefinedMinNumEndPts = true ;

  this->Modified() ;
}



//------------------------------------------------------------------------------
// Reset the extrusion direction to the default, which is normal to the hole.
// Use this to reverse the direction if the extrusion goes the wrong way.
// Argument is +1 or -1 to select forward or reverse direction relative to the hole.
void vtkALBAExtrudeToCircle::SetDirectionToDefault(int directionSign)      
//------------------------------------------------------------------------------
{
  // set any existing vector data to undefined
  m_DefinedDirection = false ;
  m_DefinedVector = false ;
  m_DefinedExtrusionPoint = false ;

  // set the flag for the sign of the direction
  if (directionSign == -1)
    m_DefaultDirectionSign = -1 ;
  else
    m_DefaultDirectionSign = 1 ;

  this->Modified() ;

}



//------------------------------------------------------------------------------
// Calculate the extrusion vector, once we know the centre and normal of the hole.
// This also calculates the length, direction and extrusion point.
// This pulls together all the possible routes for setting the extrusion vector.
// The default length is 5 * hole radius and the default direction is the hole normal, if they have not been set.
void vtkALBAExtrudeToCircle::CalculateExtrusionVector()
//------------------------------------------------------------------------------
{
  if (m_DefinedVector){
    // calculate length, direction and extrusion point from vector
    if (!m_DefinedLength){
      // back calculate length
      m_Length = GetNorm(m_ExtrusionVector) ;
      m_DefinedLength = true ;
    }

    if (!m_DefinedDirection){
      // back-calculate direction
      Normalize(m_ExtrusionVector, m_Direction) ;
      m_DefinedDirection = true ;
    }

    if (!m_DefinedExtrusionPoint){
      // calculate extrusion point
      AddVector(m_HoleCentre, m_ExtrusionVector, m_ExtrusionPoint) ;
      m_DefinedExtrusionPoint = true ;
    }
  }
  else if (m_DefinedExtrusionPoint){
    // calculate vector, length and direction from extrusion point
    SubtractVector(m_ExtrusionPoint, m_HoleCentre, m_ExtrusionVector) ;
    m_Length = GetNorm(m_ExtrusionVector) ;
    Normalize(m_ExtrusionVector, m_Direction) ;
    m_DefinedLength = true ;
    m_DefinedDirection = true ;
    m_DefinedVector = true ;
  }
  else{
    if (!m_DefinedLength){
      // if no length defined, set to 5 * hole radius
      m_Length = 5.0 * m_HoleRadius ;
      m_DefinedLength = true ;
    }

    if (!m_DefinedDirection){
      // if no direction defined, set to normal of hole
      Normalize(m_HoleNormal, m_Direction) ;

      // check if the direction needs to be reversed
      if (m_DefaultDirectionSign == -1){
        MultVectorByScalar(-1.0, m_Direction, m_Direction) ;
      }

      m_DefinedDirection = true ;
    }

    // calculate vector from length and direction
    MultVectorByScalar(m_Length, m_Direction, m_ExtrusionVector) ;
    m_DefinedVector = true ;

    // calculate extrusion point
    AddVector(m_HoleCentre, m_ExtrusionVector, m_ExtrusionPoint) ;
    m_DefinedExtrusionPoint = true ;
  }
}



//------------------------------------------------------------------------------
// Calculate Centre, normal and radius of hole, and no. of vertices
// The normal is normalised.
void vtkALBAExtrudeToCircle::CalcHoleParameters(vtkPolyData *hole)
//------------------------------------------------------------------------------
{
  int i, j ;
  double sumsq, x[3] ;
  vtkPoints *pts = hole->GetPoints() ;
  int numPts = m_Input->GetNumberOfPoints() ;

  //----------------------------------------------------------------------------
  // get the centre
  //----------------------------------------------------------------------------

  // get the mean centre
  SetVector(m_HoleCentre, 0, 0, 0) ;
  for (i = 0 ;  i < numPts ; i++){
    pts->GetPoint(i, x) ;
    AddVector(m_HoleCentre, x, m_HoleCentre) ;
  }
  m_HoleCentre[0] /= (double)numPts ;
  m_HoleCentre[1] /= (double)numPts ;
  m_HoleCentre[2] /= (double)numPts ;


  //----------------------------------------------------------------------------
  // get the radius
  //----------------------------------------------------------------------------

  double dx[3] ;
  for (i = 0, sumsq = 0.0 ;  i < numPts ; i++){
    // get point relative to hole centre
    pts->GetPoint(i, x) ;
    SubtractVector(x, m_HoleCentre, dx) ;

    // add to sum squared radius
    for (j = 0 ;  j < 3 ;  j++)
      sumsq += dx[j] * dx[j] ;
  }

  // get the rms radius
  sumsq /= (double)numPts ;
  m_HoleRadius = sqrt(sumsq) ;


  //----------------------------------------------------------------------------
  // get the normal
  //----------------------------------------------------------------------------

  // Find the normal to the plane of the hole.
  // In this example data, the polydata circulates around the hole centre c, such that for each polyline cell,
  // with endpoints p0 and p1, the cross product (p1-c)^(p0-c) points in the direction of the outward normal.
  // Therefore we use the median cross product to define the normal.
	vtkIdType numPtsInCell ;
  vtkIdType *ptlist ;
  double pt0[3], pt1[3] ;
  vtkALBAPastValuesList vecProd0(1000), vecProd1(1000), vecProd2(1000) ;

  // Build cell links if not already done
  if (!m_BuiltCells){
    m_Input->BuildCells() ;
    m_BuiltCells = true ;
  }

  for (int i = 0 ;  i < m_Input->GetNumberOfCells() ;  i++){
    m_Input->GetCellPoints(i, numPtsInCell, ptlist) ;
    if (numPtsInCell == 2){
      m_Input->GetPoint(ptlist[0], pt0) ;
      m_Input->GetPoint(ptlist[1], pt1) ;
      SubtractVector(pt0, m_HoleCentre, pt0) ;
      SubtractVector(pt1, m_HoleCentre, pt1) ;

      // get cross product and add to list
      double cp[3] ;
      CrossProduct(pt1, pt0, cp) ;
      vecProd0.AddNewValue(cp[0]) ;
      vecProd1.AddNewValue(cp[1]) ;
      vecProd2.AddNewValue(cp[2]) ;
    }
  }

  // set normal to median cross product
  m_HoleNormal[0] = vecProd0.GetMedian() ;
  m_HoleNormal[1] = vecProd1.GetMedian() ;
  m_HoleNormal[2] = vecProd2.GetMedian() ;
  Normalize(m_HoleNormal, m_HoleNormal) ;



  //----------------------------------------------------------------------------
  // get the number of points around the hole
  //----------------------------------------------------------------------------
  m_HoleNumVerts = pts->GetNumberOfPoints() ;
}




//------------------------------------------------------------------------------
// Calculate "up" vector which defines phi = 0 in cylindrical coords
// It is the direction from the centre of the hole to the first vertex.
// This is not necessarily normal to the cylinder axis.
void vtkALBAExtrudeToCircle::CalcUpVector(vtkIdList *holepts)
//------------------------------------------------------------------------------
{
  double x[3] ;
  int ptId ;
  ptId = holepts->GetId(0) ;
  m_Input->GetPoint(ptId, x) ;
  SubtractVector(x, m_HoleCentre, m_UpVector) ;
}



//------------------------------------------------------------------------------
// Calculate no. of rings required on extrusion, their positions and no. of points in each
// Needs length of extrusion and corrected end radius
// Allocates memory for the rings and the vertices in the mesh structure.
void vtkALBAExtrudeToCircle::CalcExtrusionRings()
//------------------------------------------------------------------------------
{
  int i, k ;

  // set the requested min. value for the no. of end points to something sensible.
  // default is the same as the no. of vertices on the hole.
  if (!m_DefinedMinNumEndPts || (m_MinNumEndPts < m_HoleNumVerts)){
    // set min no. of end vertices to default
    m_MinNumEndPts = m_HoleNumVerts ;
    m_DefinedMinNumEndPts = true ;
  }

  // calculate circumference of extrusion
  double circumf = 2*vtkMath::Pi() * m_EndRadius ;

  // try increasing the no. of rings k until we get a number which gives the right length.
  k = 2 ;
  double m, sumL;
  while (k < 1000){
    // calculate slope m where no. of vertices ni in ring i is given by ni = n0 + m*i
    m = (double)(m_MinNumEndPts - m_HoleNumVerts) / (double)(k-1) ;

    // calculate the total length of the extrusion
    for (i = 1, sumL = 0.0 ;  i < k ;  i++){
      int ni = m_HoleNumVerts + (int)(m*(double)i) ; 
      double Li = circumf / (double)ni ;
      sumL += Li ;
    }

    // stop when the length reaches the desired length
    if (sumL >= m_Length)
      break ;

    k++ ;
  }

  // tweak the circumference so that the total length will be the same as the requested length
  circumf *= m_Length / sumL ;


  //----------------------------------------------------------------------------
  // set the positions and no. of vertices in each ring
  //----------------------------------------------------------------------------

  // allocate the rings
  m_Mesh->AllocateRings(k) ;

  // first ring is same as start hole
  m_Mesh->Ring[0].AllocateVertices(m_HoleNumVerts) ;
  m_Mesh->Ring[0].Z = 0.0 ;

  // allocate the number of vertices in each ring
  double z = 0.0 ;
  for (i = 1 ;  i < k-1 ;  i++){
    int ni = m_HoleNumVerts + (int)(m*(double)i) ; 
    z += circumf / (double)ni ;

    m_Mesh->Ring[i].AllocateVertices(ni) ;
    m_Mesh->Ring[i].Z = z ;
  }

  // last ring as requested
  m_Mesh->Ring[k-1].AllocateVertices(m_MinNumEndPts) ;
  m_Mesh->Ring[k-1].Z = m_Length ;
}





//------------------------------------------------------------------------------
// Calculate cylindrical and cartesian coords of each mesh vertex
void vtkALBAExtrudeToCircle::CalcExtrusionVertices()
//------------------------------------------------------------------------------
{
  int i, j ;

  // get indices of points around hole, in correct order
  vtkIdList *holepts = vtkIdList::New() ;
  GetPointsAroundHole(m_Input, holepts) ;

  // Calculate the up vector for the cylindrical coord system
  CalcUpVector(holepts) ;

  // Reverse order of points if the list of points contains decreasing phi
  int sense = CalcSenseOfPointsAroundHole(holepts, m_HoleCentre, m_ExtrusionVector, m_UpVector) ;
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
    m_Input->GetPoint(ptId, x) ;  // cartesian coords of hole point
    CalcCylinderCoords(x, m_HoleCentre, m_ExtrusionVector, m_UpVector, &r, &phi, &z) ;
    m_Mesh->Ring[0].Vertex[j].SetCylCoords(r, phi, z) ;
  }

  // Remove unwanted jumps of 2pi from the data
  Remove2PiArtefactsFromFirstRing() ;

  // calculate the cylindrical coords of the end ring, which is of course a 
  // perfect circle, normal to the z axis
  int iend = m_Mesh->NumRings - 1 ;
  double dphi = 2*vtkMath::Pi() / (double)m_Mesh->Ring[iend].NumVerts ;
  for (j = 0 ;  j < m_Mesh->Ring[iend].NumVerts ;  j++){
    r = m_EndRadius ;
    phi = (double)j * dphi ;
    z = m_Mesh->Ring[iend].Z ;
    m_Mesh->Ring[iend].Vertex[j].SetCylCoords(r, phi, z) ;
  }

  // Tricky bit: now we have to interpolate the vertices on the middle rings
  for (i = 1 ;  i < m_Mesh->NumRings-1 ;  i++){
    for (j = 0 ;  j < m_Mesh->Ring[i].NumVerts ;  j++){
      // find the point on the end ring which corresponds to point j
      double factor1 = (double)m_Mesh->Ring[iend].NumVerts / (double)m_Mesh->Ring[i].NumVerts ;
      int jend = (int)((double)j * factor1 + 0.5) ;

      // find the points on the start ring which correspond to point j
      // and the relative weighting for each
      double factor2 = (double)m_Mesh->Ring[0].NumVerts / (double)m_Mesh->Ring[i].NumVerts ;
      double jdouble = (double)j * (double)factor2 ;
      int jstart0 = (int)jdouble ;
      int jstart1 = (jstart0 + 1) ;
      double rmdr = jdouble - (double)jstart0 ;
      double w0 = 1.0 - rmdr ;
      double w1 = rmdr ;

      // make sure indices are in correct range
      jend = jend % m_Mesh->Ring[iend].NumVerts ;
      jstart0 = jstart0 % m_Mesh->Ring[0].NumVerts ;
      jstart1 = jstart1 % m_Mesh->Ring[0].NumVerts ;

      // get the total weights for the three points
      double lambda = m_Mesh->Ring[i].Z / m_Mesh->Ring[iend].Z ;
      double w_end = lambda ;
      double w_start0 = (1.0 - lambda)*w0 ;
      double w_start1 = (1.0 - lambda)*w1 ;

      // get the cylindrical coords of the three points
      double r_start0, phi_start0, z_start0 ;
      double r_start1, phi_start1, z_start1 ;
      double r_end, phi_end, z_end ;
      m_Mesh->Ring[0].Vertex[jstart0].GetCylCoords(&r_start0, &phi_start0, &z_start0) ;
      m_Mesh->Ring[0].Vertex[jstart1].GetCylCoords(&r_start1, &phi_start1, &z_start1) ;
      m_Mesh->Ring[iend].Vertex[jend].GetCylCoords(&r_end, &phi_end, &z_end) ;
      if (jstart1 == 0){
        // special case: deal with phi wrapping round from 2pi to zero
        phi_start1 += + 2.0*vtkMath::Pi() ;
      }

      // interpolate the three points
      r = w_start0*r_start0 + w_start1*r_start1 + w_end*r_end ;
      phi = w_start0*phi_start0 + w_start1*phi_start1 + w_end*phi_end ;
      z = w_start0*z_start0 + w_start1*z_start1 + w_end*z_end ;
      m_Mesh->Ring[i].Vertex[j].SetCylCoords(r, phi, z) ;    
    }
  }

  // finally calculate the cartesian coords of each vertex
  for (i = 0 ;  i < m_Mesh->NumRings ;  i++){
    for (j = 0 ;  j < m_Mesh->Ring[i].NumVerts ;  j++){
      m_Mesh->Ring[i].Vertex[j].GetCylCoords(&r, &phi, &z) ;
      CalcCartesianCoords(r, phi, z, m_HoleCentre, m_ExtrusionVector, m_UpVector, x) ;
      m_Mesh->Ring[i].Vertex[j].SetCartCoords(x) ;
    }
  }

  holepts->Delete() ;
}




//------------------------------------------------------------------------------
// Transfer vertices to vtkPoints
// This allocates memory for points.
void vtkALBAExtrudeToCircle::VerticesToVtkPoints(vtkPoints *points) const
//------------------------------------------------------------------------------
{
  int i, j, k, totalpts ;

  // pre-allocate memory for total no. of vertices
  for (i = 0, totalpts = 0 ;  i < m_Mesh->NumRings ;  i++)
    totalpts += m_Mesh->Ring[i].NumVerts ;
  points->Allocate(totalpts) ;       // allocate space for n points (ie 3 * n floats)

  for (i = 0, k = 0 ;  i < m_Mesh->NumRings ;  i++){
    for (j = 0 ;  j < m_Mesh->Ring[i].NumVerts ;  j++){
      // insert vertex into points array
      double x[3] ;
      m_Mesh->Ring[i].Vertex[j].GetCartCoords(x) ;
      points->InsertPoint(k, x) ; 

      // make a note of the polydata index so we can find it again
      m_Mesh->Ring[i].Vertex[j].SetId(k++) ;     
    }
  }

  points->Squeeze() ;
}



//------------------------------------------------------------------------------
//Sort vertices into vtk triangles
//This allocates memory for triangles
void vtkALBAExtrudeToCircle::VerticesToVtkTriangles(vtkCellArray *triangles) const
//------------------------------------------------------------------------------
{
  int i, j, totalTriangles ;
  vtkIdType id0, id1, id2, thisTriangle[3] ;

  // pre-allocate memory for total no. of vertices
  for (i = 0, totalTriangles = 0 ;  i < m_Mesh->NumRings-1 ;  i++)
    totalTriangles += m_Mesh->Ring[i].NumVerts + m_Mesh->Ring[i+1].NumVerts ;
  triangles->Allocate(3*totalTriangles) ;       // allocate space for indices 

  for (i = 0 ;  i < m_Mesh->NumRings-1 ;  i++){
    int nthis = m_Mesh->Ring[i].NumVerts ;      // no. vertices in this ring
    int nnext = m_Mesh->Ring[i+1].NumVerts ;    // no. vertices in previous ring
    double nratio = (double)nnext / (double)nthis ;

    // loop through the vertices
    for (j = 0 ;  j < m_Mesh->Ring[i].NumVerts ;  j++){
      // get the range of vertices which will be connected to this one on the next ring
      // n.b. need to use floor() because (int) moves negative numbers towards zero.
      int jjminus = floor(nratio * (double)(j-0.5) + 0.5) ;
      int jjplus = floor(nratio * (double)(j+0.5) + 0.5) ;

      // spray triangles forward from {ring i, vertex j} to {ring i+1, vertices jjminus to jjplus}
      for (int jj = jjminus ;  jj < jjplus ;  jj++){
        int jj0 = Modulo(jj, nnext) ;
        int jj1 = Modulo(jj+1, nnext) ;

        // construct triangle.
        // this goes anticlock if ring index goes left to right and j axis points down.
        id0 = m_Mesh->Ring[i].Vertex[j].GetId() ;
        id1 = m_Mesh->Ring[i+1].Vertex[jj1].GetId() ;
        id2 = m_Mesh->Ring[i+1].Vertex[jj0].GetId() ;

        thisTriangle[0] = id0 ; thisTriangle[1] = id1 ; thisTriangle[2] = id2 ;
        triangles->InsertNextCell(3, thisTriangle) ;
      }

      // add the triangle which has its baseline on the current ring
      int j1 = Modulo(j+1, nthis) ;
      int jjp = Modulo(jjplus, nnext) ;

      id0 = m_Mesh->Ring[i].Vertex[j].GetId() ;
      id1 = m_Mesh->Ring[i].Vertex[j1].GetId() ;
      id2 = m_Mesh->Ring[i+1].Vertex[jjp].GetId() ;

      thisTriangle[0] = id0 ; thisTriangle[1] = id1 ; thisTriangle[2] = id2 ;
      triangles->InsertNextCell(3, thisTriangle) ;
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
void vtkALBAExtrudeToCircle::CalcCylinderCoords(const double *x, const double *centre, const double *cylAxis, const double *upVector, 
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
    *phi += 2*vtkMath::Pi() ;

}




//------------------------------------------------------------------------------
// Get cartesian coords x given cylindrical coords (r, phi, z)
// The cylinder system is defined by the central axis and an "up" vector.
// cylAxis and upVector do not have to be normalised.
// upVector does not have to be exactly normal to cylAxis.
void vtkALBAExtrudeToCircle::CalcCartesianCoords(double r, double phi, double z, const double *centre, const double *cylAxis, const double *upVector, double *x) const 
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
void vtkALBAExtrudeToCircle::GetMatRotArrowToAxis(vtkMatrix4x4 *mat, const double *u) const 
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
void vtkALBAExtrudeToCircle::GetPointsAroundHole(vtkPolyData *hole, vtkIdList *pts) const 
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
int vtkALBAExtrudeToCircle::CalcSenseOfPointsAroundHole(vtkIdList *holepts, const double *centre, const double *cylAxis, const double *upVector) const
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
  mplus = 2.0*vtkMath::Pi() / (double)n ;
  mneg = -mplus ;

  for (j = 0, Eplus = 0.0, Eneg = 0.0 ;  j < n ;  j++){
    vtkIdType ptId = holepts->GetId(j) ;
    x = m_Input->GetPoint(ptId) ;  // cartesian coords of hole point
    CalcCylinderCoords(x, m_HoleCentre, m_ExtrusionVector, m_UpVector, &r, &phi, &z) ;

    // Get calculated phi for each line
    double phicalc_plus = mplus*(double)j ;
    double phicalc_neg = mneg*(double)j ;

    // Make sure the calculated phi is in the range 0-2pi, the same as the input data.
    // NB if we got the slope of the line by least squares, it would be hard to account for this.
    if (phicalc_neg < 0.0)
      phicalc_neg += 2.0*vtkMath::Pi() ;

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
void vtkALBAExtrudeToCircle::Remove2PiArtefactsFromFirstRing()
//------------------------------------------------------------------------------
{
  int j ;
  int n = m_Mesh->Ring[0].NumVerts ;

  // slope of model line phi = m*j + c where c = 0
  double m = 2.0*vtkMath::Pi() / (double)n ;

  for (j = 0 ;  j < n ;  j++){
    double phi = m_Mesh->Ring[0].Vertex[j].GetCylPhi() ;
    double phicalc = m * (double)j ;
    double dphi = phi - phicalc ;

    // add or subtract 2pi to improve value
    if (dphi > vtkMath::Pi())
      phi -= 2.0*vtkMath::Pi() ;
    else if (dphi < -vtkMath::Pi())
      phi += 2.0*vtkMath::Pi() ;

    m_Mesh->Ring[0].Vertex[j].SetCylPhi(phi) ;
  }
}



//------------------------------------------------------------------------------
// Reverse id list
void vtkALBAExtrudeToCircle::ReverseIdList(vtkIdList *pts) const
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
void vtkALBAExtrudeToCircle::CircularShiftIdList(vtkIdList *pts, int shift) const
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
inline int vtkALBAExtrudeToCircle::Modulo(int n, int m) const
//------------------------------------------------------------------------------
{
  if (n >= 0)
    return n % m ;
  else
    return m - (-n % m) ;
}


//------------------------------------------------------------------------------
// Copy vector
void vtkALBAExtrudeToCircle::CopyVector(const double *a, double *b) const
//------------------------------------------------------------------------------
{
  b[0] = a[0] ;
  b[1] = a[1] ;
  b[2] = a[2] ;
}


//------------------------------------------------------------------------------
// Set vector
void vtkALBAExtrudeToCircle::SetVector(double *vec, double x, double y, double z) const
//------------------------------------------------------------------------------
{
  vec[0] = x ;
  vec[1] = y ;
  vec[2] = z ;
}

//------------------------------------------------------------------------------
// Add vector
void vtkALBAExtrudeToCircle::AddVector(const double *vin, const double *x, double *vout) const
//------------------------------------------------------------------------------
{
  vout[0] = vin[0] + x[0] ;
  vout[1] = vin[1] + x[1] ;
  vout[2] = vin[2] + x[2] ;

}


//------------------------------------------------------------------------------
// Subtract vector
void vtkALBAExtrudeToCircle::SubtractVector(const double *vin, const double *x, double *vout) const
//------------------------------------------------------------------------------
{
  vout[0] = vin[0] - x[0] ;
  vout[1] = vin[1] - x[1] ;
  vout[2] = vin[2] - x[2] ;

}

//------------------------------------------------------------------------------
// Multiply vector by scalar
void vtkALBAExtrudeToCircle::MultVectorByScalar(double s, const double *vin, double *vout) const
//------------------------------------------------------------------------------
{
  vout[0] = s*vin[0] ;
  vout[1] = s*vin[1] ;
  vout[2] = s*vin[2] ;
}



//------------------------------------------------------------------------------
// Get norm of vector
double vtkALBAExtrudeToCircle::GetNorm(const double *vec) const
//------------------------------------------------------------------------------
{
  return sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]) ;
}

//------------------------------------------------------------------------------
// Normalize vector
void vtkALBAExtrudeToCircle::Normalize(const double *vin, double *vout) const
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
void vtkALBAExtrudeToCircle::CrossProduct(const double *a, const double *b, double *c) const
//------------------------------------------------------------------------------
{
  c[0] =   a[1]*b[2] - a[2]*b[1] ;
  c[1] = -(a[0]*b[2] - a[2]*b[0]) ;
  c[2] =   a[0]*b[1] - a[1]*b[0] ;
}


//------------------------------------------------------------------------------
// Dot product of vectors a.b
double vtkALBAExtrudeToCircle::DotProduct(const double *a, const double *b) const
//------------------------------------------------------------------------------
{
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] ;
}


//------------------------------------------------------------------------------
// Inverse solution of equation y = Mx, where M = row vectors (u, v and w)
// Find x given M and y
void vtkALBAExtrudeToCircle::InverseSolveTransform(const double *u, const double *v, const double *w, const double *y, double *x) const
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
void vtkALBAExtrudeToCircle::PrintSelf(ostream& os, vtkIndent indent) const
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
  os << indent << "requested end pts: " << m_MinNumEndPts << std::endl ; 
  m_Mesh->PrintSelf(os, indent) ;
  os << std::endl ;
}



//------------------------------------------------------------------------------
// Print self
void vtkALBAExtrudeToCircle::MeshData::PrintSelf(ostream& os, vtkIndent indent) const
//------------------------------------------------------------------------------
{
  os << indent << "no. rings = " << NumRings << std::endl ;

  for (int i = 0 ;  i < NumRings ;  i++){
    os << indent << "ring " << i << "\t" ;
    Ring[i].PrintSelf(os, vtkIndent(0)) ;
    os << std::endl ;
  }
}


//------------------------------------------------------------------------------
// Print self
void vtkALBAExtrudeToCircle::RingData::PrintSelf(ostream& os, vtkIndent indent) const
//------------------------------------------------------------------------------
{
  os << indent << "no. vertices = " << NumVerts << "\t" << "z = " << Z << std::endl ;
  for (int j = 0 ;  j < NumVerts ;  j++){
    os << indent << "vertex " << j << "\t" ;
    Vertex[j].PrintSelf(os, vtkIndent(0)) ;
  }
}


//------------------------------------------------------------------------------
// Print self
void vtkALBAExtrudeToCircle::VertexData::PrintSelf(ostream& os, vtkIndent indent) const
//------------------------------------------------------------------------------
{
  os << indent << "cyl " << Cylcoord[0] << "\t" << Cylcoord[1] << "\t" << Cylcoord[2] << "\t"
    << "cart " << Cartcoord[0] << "\t" << Cartcoord[1] << "\t" << Cartcoord[2] << std::endl ;
}
